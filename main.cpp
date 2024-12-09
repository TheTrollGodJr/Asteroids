#include <algorithm>
#include <iostream>
#include <windows.h>
#include "Asteroid.h"
#include <cmath>
#include <random>

using namespace std;

// DISPLAY UPDATES
void displayGameObject(const HDC& hdc);
void displayGameInfo(const HDC &hdc);

// CALCULATE COORDINATES
coords rotatePoint(coords p, float cx, float cy, float angleDeg);
coords calcBulletLine(coords pos, int angle);
void calculateTriangleVertices(int cx, int cy, float r, int angle, coords vertices[3]);

// COLLISIONS
bool collisions();
void checkCollisions();
void removeAsteroid(const Asteroid* asteroid);

// GAME UPDATES
void gameUpdate();
void checkInputs();
void startLevel();

// MISC
int getRandomInt(int min, int max);


// GLOBAL GAME OBJECTS
vector<Bullet> bullets;
vector<Asteroid*> asteroids;
Player player(400, 300, 90, 5, bullets);

// GLOBAL GAME VARIABLES
int asteroidSpawnCount = 4;
bool keyboardInputs[5] = {false}; // W, A, S, D, Space
bool shot = false;
bool gameOver = false;

// GLOBAL COORDINATE DATA
int asteroidSizes[3][11][2] = {{{0,0},{20,-20},{40,0},{60,-20},{80,0},{60,20},{80,50},{40,70},{20,70},{0,50},{0,0}},
    {{0,0},{10,-10},{20,0},{30,-10},{40,0},{30,10},{40,25},{20,35},{10,35},{0,25},{0,0}},
    {{0,0},{5,-5},{10,0},{15,-5},{20,0},{15,5},{20,12},{10,17},{5,17},{0,12},{0,0}}}; //index 0 is large, index 1 is medium, index 2 is small
vector<vector<vector<int>>> numCoords
    {{{0,0},{15,0},{15,20},{0,20},{0,0}}, //0
    {{0,0},{0,20}}, //1
    {{0,0},{15,0},{15,10},{0,10},{0,20},{15,20}}, //2
    {{0,0},{15,0},{15,10},{0,10},{15,10},{15,20},{0,20}}, //3
    {{0,0},{0,10},{15,10},{15,0},{15,20}}, //4
    {{15,0},{0,0},{0,10},{15,10},{15,20},{0,20}}, //5
    {{0,0},{0,20},{15,20},{15,10},{0,10}}, //6
    {{0,0},{15,0},{15,20}}, //7
    {{0,0},{15,0},{15,20},{0,20},{0,0},{0,10},{15,10}}, //8
    {{15,20},{15,0},{0,0},{0,10},{15,10}}}; //9


// WINDOW GAME LOOP
LRESULT CALLBACK windowsProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    // INITIALIZING VARIABLES
    PAINTSTRUCT ps;
    HDC hdc = nullptr;
    HBRUSH brush = nullptr;
    HPEN hPen = nullptr;
    HPEN hOldPen = nullptr;
    RECT bg;

    // SWITCH BETWEEN WINDOW EVENTS
    switch (uMsg) {

        // If exiting the window
        case WM_DESTROY:
            // Destroy asteroid pointers
            for (const auto item : asteroids) {
                delete item;
            }

            PostQuitMessage(0); // Close the window
            return 0; // Return success

        // Set background color
        case WM_ERASEBKGND:
            hdc = (HDC)wParam; // Device context for the window

            // Set size of background rectangle
            bg.left = 0;
            bg.top = 0;
            bg.right = GetSystemMetrics(SM_CXSCREEN);
            bg.bottom = GetSystemMetrics(SM_CYSCREEN);

            // Make the rectangle black and draw it.
            brush = CreateSolidBrush(RGB(0,0,0));
            FillRect(hdc, &bg, brush);
            DeleteObject(brush);
            return 1;

        // Set a game timer
        case WM_TIMER:
            gameUpdate(); // Update game objects
            InvalidateRect(hwnd, NULL, TRUE); // Send a request to redraw the screen
            return 0;

        // Draw game elemnts
        case WM_PAINT:
            hdc = BeginPaint(hwnd, &ps); // Device context for the window

            // Create pens to draw with
            hPen = CreatePen(PS_SOLID, 1, RGB(255,255,255));
            hOldPen = (HPEN)SelectObject(hdc, hPen);

            // Check if the player has reached a game over
            if (gameOver) {
                // Set background to black and text to white
                SetBkMode(hdc, RGB(0,0,0));
                SetTextColor(hdc, RGB(255, 255, 255));

                // Draw Game over text
                TextOut(hdc, 350, 250, "GAME OVER", 9);
                TextOut(hdc, 325, 275, "Press 'Enter' to exit", 21);

                KillTimer(hwnd, 1); // Stop the game loop
            }

            // Draw game objects
            displayGameObject(hdc);
            displayGameInfo(hdc);

            // Remove drawing variables and stop drawing
            SelectObject(hdc, hOldPen);
            DeleteObject(brush);
            EndPaint(hwnd, &ps);
            return 0;

        // Get key pressed
        case WM_KEYDOWN:

            // Save keyboard inputs
            switch(wParam) {
                case 'W': keyboardInputs[0] = true; break;
                case 'A': keyboardInputs[1] = true; break;
                case 'S': keyboardInputs[2] = true; break;
                case 'D': keyboardInputs[3] = true; break;
                case VK_SPACE: keyboardInputs[4] = true; break;
                case VK_RETURN: if (gameOver) {PostQuitMessage(0);}
                default: break;
            }
            return 0;

        // Get key releases
        case WM_KEYUP:

            // Save key releases
            switch(wParam) {
                case 'W': keyboardInputs[0] = false; break;
                case 'A': keyboardInputs[1] = false; break;
                case 'S': keyboardInputs[2] = false; break;
                case 'D': keyboardInputs[3] = false; break;
                case VK_SPACE: keyboardInputs[4] = true; shot = false; break;
                default: break;
            }
            return 0;

        // Default case
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

/*
           -------- DISPLAY UPDATES --------
*/

// DRAW GAME OBJECTS
void displayGameObject(const HDC& hdc) {

    // Initialize temporary variables
    coords temp{}; // Store coordinates of a game object
    coords vertices[3]; // Stores calculated vertices of a triangle

    // Draw all asteroids
    for (const auto item : asteroids) {

        temp = item->getCoords(); // Get asteroid coordinates
        const int size = item->getSize(); // Get asteroid size

        MoveToEx(hdc, temp.x, temp.y, nullptr); // Move drawing cursor
        for (int i = 0; i < 11; i++) { // Loop through asteroid shape coordinates
            LineTo(hdc, temp.x+asteroidSizes[size][i][0], temp.y+asteroidSizes[size][i][1]); // Draw lines to each vertice of the asteroid
        }
    }

    // Draw all bullets
    for (const auto &item : bullets) {

        coords pos = item.getCoords(); // Get bullet coordinates
        pos = calcBulletLine(pos, item.getDirection()); // Calculate second coordinate of the bullet line

        MoveToEx(hdc, pos.x, pos.y, nullptr); // Move drawing cursor
        LineTo(hdc, pos.x, pos.y); // Draw the bullet
    }

    temp = player.getCoords(); // Get player coordinates

    calculateTriangleVertices(temp.x, temp.y, 15.0, player.getAngle(), vertices); // Calculate the vertices of the player model

    MoveToEx(hdc, vertices[2].x, vertices[2].y, nullptr); // Move drawing cursor
    for (const auto [x, y] : vertices) {
        LineTo(hdc, x, y); // Draw player triangle
    }
}

// DRAW GAME INFO
void displayGameInfo(const HDC &hdc) {

    // temporary coordinate variables
    int xAlign = 40;
    int yAlign = 20;

    // Get players score
    const string scoreStr = player.getScore(); // Save score as a string
    char score[scoreStr.size()]; // Make a character array the size of the string
    strcpy(score, scoreStr.c_str()); // Save the string in the character array

    // Draw the score
    for (const char &item : score) {

        const int scoreIndex = item - '0'; // convert the character to an int

        MoveToEx(hdc, numCoords[scoreIndex][0][0]+xAlign, numCoords[scoreIndex][1][1]+yAlign, nullptr); // Move drwaing cursor
        for (auto &coord : numCoords[item-'0']) { // Loop through the coordinates for the selected number
            LineTo(hdc, coord[0]+xAlign, coord[1]+yAlign); // Draw the number
        }
        xAlign += 20; // Move drawing cursor to the right to draw the next digit
    }

    // Adjust temporary coordinate values
    yAlign += 50;
    xAlign = 50;

    // Draw the amount of lives the player has
    for (size_t i = 0; i < player.getLives(); i++) {

        coords vertices[3]; // Initialize an array to store triangle vertices
        calculateTriangleVertices(xAlign, yAlign, 13, 0, vertices); // Calculate triangle vertices and add them to the array

        MoveToEx(hdc, vertices[2].x, vertices[2].y, nullptr); // Move drawing cursor
        for (const auto &[x, y] : vertices) { // Loop through the triangles vertices
            LineTo(hdc, x, y); // Draw the triangle
        }
        xAlign += 30; // Move drawing cursor to the right to draw the next triangle
    }
}

/*
            -------- CALCULATE COORDINATES --------
*/

// ROTATE TRIANGLE VERTICES
coords rotatePoint(coords p, float cx, float cy, float angleDeg) {

    float angleRad = angleDeg * (3.14159f / 180.0f); // Convert angle to radians

    // Translate the point to the origin, rotate, then translate back
    float xTranslated = p.x - cx;
    float yTranslated = p.y - cy;
    float xRot = xTranslated * cos(angleRad) - yTranslated * sin(angleRad);
    float yRot = xTranslated * sin(angleRad) + yTranslated * cos(angleRad);

    // Translate back to the original center
    p.x = static_cast<int>(xRot + cx);
    p.y = static_cast<int>(yRot + cy);

    return p; // Return the new coordinate
}

// CALCULATE THE SECOND COORDINATE FOR BULLETS
coords calcBulletLine(const coords pos, const int angle) {

    const float radians = (angle - 90) * (M_PI / 180.0f); // Convert angle to radians
    coords out{}; // Initialize a return coordinate

    // Translate the point according to the angle
    out.x = pos.x + 5 * cos(radians);
    out.y = pos.y + 5 * sin(radians);

    return out; // Return the new coordinate
}

// CALCULATE THE VERTICES OF A TRIANGLE AND SAVE THEM
void calculateTriangleVertices(const int cx, const int cy, const float r, const int angle, coords vertices[3]) {

    // Create the vertices of an un-rotated equilateral triangle
    vertices[0] = coords(cx, cy - r - 4);
    vertices[1] = coords(cx - static_cast<int>(r * sin(60.0f * 3.14159f / 180.0f)), static_cast<int>(cy + r * cos(60.0f * 3.14159f / 180.0f)));
    vertices[2] = coords(cx + static_cast<int>(r * sin(60.0f * 3.14159f / 180.0f)), static_cast<int>(cy + r * cos(60.0f * 3.14159f / 180.0f)));

    // Rotate each vertex by the specified angle
    for (int i = 0; i < 3; i++) {
        vertices[i] = rotatePoint(vertices[i], static_cast<float>(cx), static_cast<float>(cy), static_cast<float>(angle));
    }
}

/*
            -------- COLLISIONS --------
*/

// CHECKS IF A SPECIFIED COORDINATE IS INSIDE ANY ASTEROID
bool collisions(const coords p, const bool destroy) {

    bool inside = false; // Initialize variable to check if the point is inside an asteroid

    // Loop through all asteroids
    for (const auto item : asteroids) {

        const int size = item->getSize(); // Get the size of the asteroid
        const auto [x, y] = item->getCoords(); // Get the coordinates of the asteroid

        // Use a ray-casting algorithm to check if the point is inside the asteroid
        for (size_t i = 0; i < 10; i++) {

            // Get two vertices of the asteroid
            const coords a = {asteroidSizes[size][i][0]+x, asteroidSizes[size][i][1]+y};
            const coords b = {asteroidSizes[size][i+1][0]+x, asteroidSizes[size][i+1][1]+y};

            // Ray casting algorithm
            if (((a.y > p.y) != (b.y > p.y)) && (p.x < (b.x - a.x) * (p.y - a.y) / (b.y - a.y) + a.x)) {
                inside = !inside;
            }
        }

        // Run if the point was inside the asteroid and if the asteroid needs to be removed
        if (inside && destroy) {removeAsteroid(item); break;}
    }

    return inside; // Return whether the point was inside an asteroid or not
}

// Check whether a bullet/player is in an asteroid
void checkCollisions() {

    // Remove a bullet from the bullet vector if the bullet hits an asteroid
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](const Bullet& obj) {
    return collisions(obj.getCoords(), true); // Check if the object should be removed
    }), bullets.end());

    // Player collisions
    if (player.canBeHit()) { // Check if the player is allowed to be hit
        if (collisions(player.getCoords(), false)) { // Run if the player was hit
            player.hit(true); // Set the players status to hit
        }
    }
    // If the player cannot be hit
    else {
        // Wait until they aren't colliding with an asteroid
        if (!collisions(player.getCoords(), false)) {
            player.hit(false); // Set the players status to not hit
        }
    }
}

// Destroy an asteroid and create smaller if needed
void removeAsteroid(const Asteroid* asteroid) {

    // Initialize temporary variables
    const int size = asteroid->getSize(); // Get the asteroids size
    const auto [x, y] = asteroid->getCoords(); // Get the asteroids coordinates
    int speed = asteroid->getSpeed(); // Get the asteroids speed

    delete asteroid; // delete the asteroid object

    // Increase the players score depending on the size of the asteroid
    switch (size) {
        case 0: player.addToScore(20); break; // Large asteroid
        case 1: player.addToScore(50); break; // Medium asteroid
        case 2: player.addToScore(100); break; // Small asteroid
        default: break;
    }

    // Remove the asteroid from the asteroids vector
    asteroids.erase(std::remove(asteroids.begin(), asteroids.end(), asteroid), asteroids.end());

    // Create new asteroids if needed
    if (size != 2) { // Run if the asteroid isn't the smallest size

        // Create a new asteroid and add it to the asteroid vector
        auto *temp = new Asteroid(x, y, getRandomInt(0, 359), speed+=2, size+1);
        asteroids.push_back(temp);

        // Create a new asteroid and add it to the asteroid size, again
        temp = new Asteroid(x, y, getRandomInt(0, 359), speed+=2, size+1);
        asteroids.push_back(temp);
    }
}

/*
                -------- GAME UPDATES --------
*/

// UPDATE GAME OBJECTS
void gameUpdate() {

    // Move all bullets
    for (auto &item : bullets) {
        item.move();
    }

    // Move all asteroids
    for (const auto item : asteroids) {
        item->move();
    }

    checkInputs(); // Check player inputs and move the player character

    // Check for a game over state
    if (player.getLives() == 0) {
        gameOver = true;
    }

    // remove bullets if they go off the screen
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](const Bullet& obj) {
    return obj.getRemoveStatus(); // Check if the object should be removed
    }), bullets.end());

    checkCollisions(); // Check for object collisions

    player.cooldown(); // Adjust the players shooting cooldown

    // Start a new level if there are no asteroid in play
    if (asteroids.empty()) {
        startLevel();
    }
}

// CHECK PLAYER INPUTS AND MOVE IF NECESSARY
void checkInputs() {

    if (keyboardInputs[0]) {player.move();} // W pressed, move forward
    else if (keyboardInputs[2]) {player.moveBack();} // S pressed, move backward
    else if (keyboardInputs[1]) {player.rotate(1, 10);} // A pressed, rotate left
    else if (keyboardInputs[3]) {player.rotate(0, 10);} // D pressed, rotate right
    else if (keyboardInputs[4] && !shot) {player.shoot(); shot = true;} // Space pressed, shoot a bullet
}

// SPAWN IN ASTEROIDS FOR A NEW LEVEL
void startLevel() {

    // Spawn in an i amount of asteroids
    for (int i = 0; i < asteroidSpawnCount; i++) {

        // Create new asteroids and add them to the asteroids vector
        auto *temp = new Asteroid(getRandomInt(0, 100), getRandomInt(0, 100), getRandomInt(0, 359), 3, 0);
        asteroids.push_back(temp);
    }

    asteroidSpawnCount += 2; // Increase the amount of asteroids that will be spawned in the next level
}

/*
                -------- MISC --------
*/

// Get a random integer
int getRandomInt(const int min, const int max) {
    std::random_device rd; // Initialize a random number generator source
    std::mt19937 gen(rd()); // Initialize a generator with rd as its seed
    std::uniform_int_distribution<> dis(min, max); // Define a random distribution of numbers

    return dis(gen); // return a random number between min and max
}



// Main
int main() {
    const char CLASS[] = "window"; // Initialize unique identifier for CLASS

    // Define window properties
    WNDCLASS wc = {}; // Initialize window properties structure
    wc.lpfnWndProc = windowsProc; // Sets the function to process window messages
    wc.hInstance = GetModuleHandle(NULL); // Gets the handle to the current application instance
    wc.lpszClassName = CLASS; // Identify the window class

    // Register the window
    if (!RegisterClass(&wc)) {
        cerr << "Window Class Registration Failed" << endl; // Throw an error if registration failed
    }

    // Create the window, define more window properties
    HWND hwnd = CreateWindowEx(
        0,
        CLASS,
        "Asteroids",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL, NULL, GetModuleHandle(NULL), NULL
    );

    // Throw an error if there was a problem creating the window
    if (!hwnd) {cerr << "Window Creation Failure"; return 1;}

    // Display window
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    MSG msg = {}; // Initialize a variable for window message handling

    SetTimer(hwnd, 1, 1000 / 24, NULL); // Set a timer for the application at 24 fps

    // Create a message loop
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg); // Get hardware input
        DispatchMessage(&msg); // Send a message to the windowProc function
    }
    return 0;
}
