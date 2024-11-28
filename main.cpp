#include <algorithm>
#include <iostream>
#include <windows.h>
#include "Asteroid.h"
#include <cmath>
#include <random>

using namespace std;

void gameUpdate();
void drawGameElements(const HDC& hdc);
coords rotatePoint(coords p, float cx, float cy, float angleDeg);
void calculateTriangleVertices(int cx, int cy, float r, int angle, coords vertices[3]);
void checkInputs();
void startLevel();
int getRandomInt(int min, int max);
bool collisions();
void checkCollisions();
void removeAsteroid(const Asteroid* asteroid);
coords calcBulletLine(coords pos, int angle);
void displayGameObject(const HDC& hdc);
void displayGameInfo(HDC &hdc);

vector<Bullet> bullets;
vector<Asteroid*> asteroids;
Player player(400, 300, 90, 5, bullets);

//int level = 0; // there are 4 levels of increasing difficulty
int asteroidSpawnCount = 4;

// W, A, S, D, Space
bool keyboardInputs[5] = {false};
bool shot = false;
bool gameOver = false;

//Asteroid asteroid(100, 100, 0, 5, 1);

//int large[][2] = {{0,0},{20,-20},{40,0},{60,-20},{80,0},{60,20},{80,50},{40,70},{20,70},{0,50},{0,0}};
//int medium[][2] = {{0,0},{10,-10},{20,0},{30,-10},{40,0},{30,10},{40,25},{20,35},{10,35},{0,25},{0,0}};
//int small[][2] = {}

//TODO: the medium size is good, add the small size, make the large size a little smaller
int asteroidSizes[3][11][2] = {{{0,0},{20,-20},{40,0},{60,-20},{80,0},{60,20},{80,50},{40,70},{20,70},{0,50},{0,0}},
    {{0,0},{10,-10},{20,0},{30,-10},{40,0},{30,10},{40,25},{20,35},{10,35},{0,25},{0,0}},
    {{0,0},{5,-5},{10,0},{15,-5},{20,0},{15,5},{20,12},{10,17},{5,17},{0,12},{0,0}}}; //index 0 is large, index 1 is medium, index 2 is small

// each number gets a 15x20 grid
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

LRESULT CALLBACK windowsProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    //gameUpdate();
    //Sleep(100);

    PAINTSTRUCT ps;
    HDC hdc = nullptr;
    RECT bg;
    HBRUSH brush = nullptr;
    HPEN hPen = nullptr;
    HPEN hOldPen = nullptr;

    switch (uMsg) {
        case WM_DESTROY:
            for (const auto item : asteroids) {
                delete item;
            }

            PostQuitMessage(0);
            return 0;
        //black background
        case WM_ERASEBKGND:
            hdc = (HDC)wParam;
            bg.left = 0;
            bg.top = 0;
            bg.right = GetSystemMetrics(SM_CXSCREEN);
            bg.bottom = GetSystemMetrics(SM_CYSCREEN);
            brush = CreateSolidBrush(RGB(0,0,0));
            FillRect(hdc, &bg, brush);
            DeleteObject(brush);
            return 1;
        case WM_TIMER:
            gameUpdate();
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;
        case WM_PAINT:
            hdc = BeginPaint(hwnd, &ps);

            hPen = CreatePen(PS_SOLID, 1, RGB(255,255,255));
            hOldPen = (HPEN)SelectObject(hdc, hPen);

            if (gameOver) {
                SetBkMode(hdc, RGB(0,0,0));
                SetTextColor(hdc, RGB(255, 255, 255));
                TextOut(hdc, 350, 250, "GAME OVER", 9);
                TextOut(hdc, 325, 275, "Press 'Enter' to exit", 21);

                KillTimer(hwnd, 1);
            }

            displayGameObject(hdc);
            displayGameInfo(hdc);

            SelectObject(hdc, hOldPen);
            DeleteObject(brush);

            EndPaint(hwnd, &ps);
            return 0;
        case WM_KEYDOWN:
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
        case WM_KEYUP:
            switch(wParam) {
                case 'W': keyboardInputs[0] = false; break;
                case 'A': keyboardInputs[1] = false; break;
                case 'S': keyboardInputs[2] = false; break;
                case 'D': keyboardInputs[3] = false; break;
                case VK_SPACE: keyboardInputs[4] = true; shot = false; break;
                default: break;
            }
            return 0;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 1;
}

int main() {
    const char CLASS[] = "window";

    WNDCLASS wc = {};
    wc.lpfnWndProc = windowsProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = CLASS;

    if (!RegisterClass(&wc)) {
        cerr << "Window Class Registration Failed" << endl;
    }

    //HWND hwnd = CreateWindowEx(0, "Asteroids", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, NULL, GetModuleHandle(NULL), NULL);
    HWND hwnd = CreateWindowEx(
        0,
        CLASS,
        "Asteroids",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL, NULL, GetModuleHandle(NULL), NULL
    );

    if (!hwnd) {cerr << "Window Creation Failure"; return 1;}

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    //Asteroid temp(100, 100, 0, 5, 0);
    //asteroids.push_back(temp);//.emplace_back(100, 100, 0, 5, 0);
    //if (asteroids.empty()) {cout << "No Asteroids" << endl; return 1;}

    MSG msg = {};
    SetTimer(hwnd, 1, 1000 / 24, NULL);

    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}

coords calcBulletLine(const coords pos, const int angle) {
    float radians = (angle - 90) * (M_PI / 180.0f);

    coords out;
    out.x = pos.x + 5 * cos(radians);
    out.y = pos.y + 5 * sin(radians);

    return out;
}


void gameUpdate() {
    // Move all bullets
    for (auto &item : bullets) {
        item.move();
    }

    for (const auto item : asteroids) {
        item->move();
    }

    checkInputs();

    if (player.getLives() == 0) {
        gameOver = true;
    }

    bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](const Bullet& obj) {
    return obj.getRemoveStatus(); // Check if the object should be removed
    }), bullets.end());

    checkCollisions();

    //DONE: Add keyboard functionality -- this includes functionality for moving the player around/shooting
    //DONE: check the asteroids vector -- if it is empty start the next level/level 1 at the start
    //DONE: somewhere in here use collision functions from the classes to check if something's been hit
    //DONE:     - if a asteroid has been hit, save its coords and size, delete it from the vector, then add a new asteroid with the same corods and a size 1 smaller; if the size can't go smaller the asteroid is perm removed until the next round.
    //DONE:     - add collision logic for when the player is hit -- sub a life/game over

    player.cooldown();


    if (asteroids.empty()) {
        startLevel();
    }
}

coords rotatePoint(coords p, float cx, float cy, float angleDeg) {
    // Convert angle to radians
    float angleRad = angleDeg * (3.14159f / 180.0f);

    // Translate the point to the origin, rotate, then translate back
    float xTranslated = p.x - cx;
    float yTranslated = p.y - cy;

    float xRot = xTranslated * cos(angleRad) - yTranslated * sin(angleRad);
    float yRot = xTranslated * sin(angleRad) + yTranslated * cos(angleRad);

    // Translate back to the original center
    p.x = static_cast<int>(xRot + cx);
    p.y = static_cast<int>(yRot + cy);

    return p;
}

void calculateTriangleVertices(const int cx, const int cy, const float r, const int angle, coords vertices[3]) {
    //std::vector<coords> vertices(3);
    //coords vertices[3] = {};

    // First, the vertices of an unrotated equilateral triangle
    vertices[0] = coords(cx, cy - r - 4);
    vertices[1] = coords(cx - static_cast<int>(r * sin(60.0f * 3.14159f / 180.0f)), static_cast<int>(cy + r * cos(60.0f * 3.14159f / 180.0f)));
    vertices[2] = coords(cx + static_cast<int>(r * sin(60.0f * 3.14159f / 180.0f)), static_cast<int>(cy + r * cos(60.0f * 3.14159f / 180.0f)));

    // Rotate each vertex by the specified angle
    for (int i = 0; i < 3; i++) {
        vertices[i] = rotatePoint(vertices[i], static_cast<float>(cx), static_cast<float>(cy), static_cast<float>(angle));
    }
}

// W, A, S, D, Space
void checkInputs() {
    if (keyboardInputs[0]) {player.move();} // W pressed, move forward
    else if (keyboardInputs[2]) {player.moveBack();} // S pressed, move backward
    else if (keyboardInputs[1]) {player.rotate(1, 10);} // A pressed, rotate left
    else if (keyboardInputs[3]) {player.rotate(0, 10);} // D pressed, rotate right
    else if (keyboardInputs[4] && !shot) {player.shoot(); shot = true;}
}

int getRandomInt(const int min, const int max) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(min, max);

    return dis(gen);
}

void startLevel() {
    for (int i = 0; i < asteroidSpawnCount; i++) {
        auto *temp = new Asteroid(getRandomInt(0, 100), getRandomInt(0, 100), getRandomInt(0, 359), 3, 0);
        asteroids.push_back(temp);
    }
    asteroidSpawnCount += 2;
}

// Checks if a specified coordinate is inside any asteroid; if it is, destroy that asteroid
bool collisions(const coords p, const bool destroy) {
    bool inside = false;
    for (const auto item : asteroids) {
        const int size = item->getSize();
        coords pos = item->getCoords();
        for (size_t i = 0; i < 10; i++) {
            coords a = {asteroidSizes[size][i][0]+pos.x, asteroidSizes[size][i][1]+pos.y};
            coords b = {asteroidSizes[size][i+1][0]+pos.x, asteroidSizes[size][i+1][1]+pos.y};
            // Ray casting algorithm
            if (((a.y > p.y) != (b.y > p.y)) && (p.x < (b.x - a.x) * (p.y - a.y) / (b.y - a.y) + a.x)) {
                inside = !inside;
            }
        }
        if (inside) {
            if (destroy) {removeAsteroid(item);}
            break;
        }
    }
    return inside; // If true, remove the bullet. Otherwise, nothing happens
}

// Check whether a bullet/player is in an asteroid
void checkCollisions() {
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](const Bullet& obj) {
    return collisions(obj.getCoords(), true); // Check if the object should be removed
    }), bullets.end());

    if (player.canBeHit()) {
        if (collisions(player.getCoords(), false)) {
            player.hit(true);
        }
    }
    else {
        if (!collisions(player.getCoords(), false)) {
            player.hit(false);
        }
    }
}

// Destroy an asteroid and create smaller if needed
void removeAsteroid(const Asteroid* asteroid) {
    const int size = asteroid->getSize();
    const coords pos = asteroid->getCoords();
    int speed = asteroid->getSpeed();
    delete asteroid;

    switch (size) {
        case 0: player.addToScore(20); break;
        case 1: player.addToScore(50); break;
        case 2: player.addToScore(100); break;
        default: break;
    }

    asteroids.erase(std::remove(asteroids.begin(), asteroids.end(), asteroid), asteroids.end());
    if (size != 2) {
        auto *temp = new Asteroid(pos.x, pos.y, getRandomInt(0, 359), speed+=2, size+1);
        asteroids.push_back(temp);
        temp = new Asteroid(pos.x, pos.y, getRandomInt(0, 359), speed+=2, size+1);
        asteroids.push_back(temp);
    }
}

void displayGameObject(const HDC& hdc) {
    coords temp;
    coords vertices[3];
    for (const auto item : asteroids) {
        temp = item->getCoords();
        const int size = item->getSize();
        MoveToEx(hdc, temp.x, temp.y, nullptr);
        for (int i = 0; i < 11; i++) {
            LineTo(hdc, temp.x+asteroidSizes[size][i][0], temp.y+asteroidSizes[size][i][1]);
        }
    }

    for (const auto &item : bullets) {
        coords pos = item.getCoords();
        MoveToEx(hdc, pos.x, pos.y, nullptr);
        pos = calcBulletLine(pos, item.getDirection());
        LineTo(hdc, pos.x, pos.y);
    }

    temp = player.getCoords();
    calculateTriangleVertices(temp.x, temp.y, 15.0, player.getAngle(), vertices);

    MoveToEx(hdc, vertices[2].x, vertices[2].y, nullptr);
    for (const auto [x, y] : vertices) {
        LineTo(hdc, x, y);
    }
}

void displayGameInfo(HDC &hdc) {
    int xAlign = 40;
    int yAlign = 20;

    const string scoreStr = player.getScore();
    char score[scoreStr.size()];
    strcpy(score, scoreStr.c_str());

    for (const char &item : score) {
        const int scoreIndex = item - '0';
        MoveToEx(hdc, numCoords[scoreIndex][0][0]+xAlign, numCoords[scoreIndex][1][1]+yAlign, nullptr);
        for (auto &coord : numCoords[item-'0']) {
            LineTo(hdc, coord[0]+xAlign, coord[1]+yAlign);
        }
        xAlign += 20;
    }

    yAlign += 50;
    xAlign = 50;

    //calculateTriangleVertices(const int cx, const int cy, const float r, const int angle, coords vertices[3])
    for (size_t i = 0; i < player.getLives(); i++) {
        coords vertices[3];
        calculateTriangleVertices(xAlign, yAlign, 13, 0, vertices);
        MoveToEx(hdc, vertices[2].x, vertices[2].y, nullptr);
        for (const auto &[x, y] : vertices) {
            LineTo(hdc, x, y);
        }
        xAlign += 30;
    }
}