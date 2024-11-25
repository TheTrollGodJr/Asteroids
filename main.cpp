#include <algorithm>
#include <iostream>
#include <windows.h>
#include "Asteroid.h"

using namespace std;

void gameUpdate();
void drawGameElements(const HDC& hdc);

vector<Bullet> bullets;
vector<Asteroid*> asteroids;
Player player(400, 300, 90, 0, bullets);

int level = 0; // there are 4 levels of increasing difficulty

// W, A, S, D, Space
bool keyboardInputs[5] = {false};

//Asteroid asteroid(100, 100, 0, 5, 1);

//int large[][2] = {{0,0},{20,-20},{40,0},{60,-20},{80,0},{60,20},{80,50},{40,70},{20,70},{0,50},{0,0}};
//int medium[][2] = {{0,0},{10,-10},{20,0},{30,-10},{40,0},{30,10},{40,25},{20,35},{10,35},{0,25},{0,0}};
//int small[][2] = {}

//TODO: the medium size is good, add the small size, make the large size a little smaller
int asteroidSizes[3][11][2] = {{{0,0},{20,-20},{40,0},{60,-20},{80,0},{60,20},{80,50},{40,70},{20,70},{0,50},{0,0}},
    {{0,0},{10,-10},{20,0},{30,-10},{40,0},{30,10},{40,25},{20,35},{10,35},{0,25},{0,0}},
    {}}; //index 0 is large, index 1 is medium, index 2 is small

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
            //cout << "Paint called" << endl;

            int size;
            coords temp;

            hdc = BeginPaint(hwnd, &ps);

            hPen = CreatePen(PS_SOLID, 1, RGB(255,255,255));
            hOldPen = (HPEN)SelectObject(hdc, hPen);

            for (const auto item : asteroids) {
                temp = item->getCoords();
                size = item->getSize();
                MoveToEx(hdc, temp.x, temp.y, nullptr);
                for (int i = 0; i < 11; i++) {
                    LineTo(hdc, temp.x+asteroidSizes[size][i][0], temp.y+asteroidSizes[size][i][1]);
                }
            }

            /*
             temp = asteroid.getCoords();
             size = asteroid.getSize();
             //if (size > 2 || size < 0) {cerr << "Invalid size while getting item.size()" << endl;}
             MoveToEx(hdc, temp.x, temp.y, nullptr);
             for (int i = 0; i < 11; i++) {
                 LineTo(hdc, temp.x+asteroidSizes[size][i][0], temp.y+asteroidSizes[size][i][1]);
             }
             */

            //gameUpdate();
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
                default: break;
            }
            return 0;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
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

void gameUpdate() {
    // Move all bullets
    for (auto &item : bullets) {
        item.move();
    }
    //Remove bullets if they hit an asteroid or went out of bounds
    bullets.erase(
        ranges::remove_if(bullets,
                          [](Bullet& b) {
                              return b.collisions(800, 600);
                          }).begin(),
        bullets.end()
    );
    for (const auto item : asteroids) {
        item->move();
    }
    //TODO: Add keyboard functionality -- this includes functionality for moving the player around/shooting
    //TODO: check the asteroids vector -- if it is empty start the next level/level 1 at the start
    //TODO: somewhere in here use collision functions from the classes to check if something's been hit
    //TODO:     - if a asteroid has been hit, save its coords and size, delete it from the vector, then add a new asteroid with the same corods and a size 1 smaller; if the size can't go smaller the asteroid is perm removed until the next round.
    //TODO:     - add collsion logic for when the player is hit -- sub a life/game over

}

/*
void drawGameElements(const HDC& hdc) {
    for (auto& item : asteroids) {
        auto [x, y] = item.getCoords();
        const int size = item.getSize();
        MoveToEx(hdc, x, y, nullptr);
        for (int i = 0; i < 11; i++) {
            LineTo(hdc, asteroidSizes[size][i][0], asteroidSizes[size][i][1]);
        }
    }
}
*/