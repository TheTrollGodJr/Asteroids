#include <algorithm>
#include <iostream>
#include <windows.h>
#include "Asteroid.h"

using namespace std;

void gameUpdate();

vector<Bullet> bullets;
vector<Asteroid> asteroids;
Player player(400, 300, 90, 0, bullets);

int large[][2] = {{0,0},{20,-20},{40,0},{60,-20},{80,0},{60,20},{80,50},{40,70},{20,70},{0,50},{0,0}};
int medium[][2] = {{0,0},{10,-10},{20,0},{30,-10},{40,0},{30,10},{40,25},{20,35},{10,35},{0,25},{0,0}};
int small[][2] = {};

LRESULT CALLBACK windowsProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    gameUpdate();

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
        case WM_PAINT:
            hdc = BeginPaint(hwnd, &ps);

            hPen = CreatePen(PS_SOLID, 1, RGB(255,255,255));
            hOldPen = (HPEN)SelectObject(hdc, hPen);

            MoveToEx(hdc, 100, 100, nullptr);
            LineTo(hdc, 120, 80);
            LineTo(hdc, 140, 100);
            LineTo(hdc, 160, 80);
            LineTo(hdc, 180, 100);
            LineTo(hdc, 160, 120);
            LineTo(hdc, 180, 150);
            LineTo(hdc, 140, 170);
            LineTo(hdc, 120, 170);
            LineTo(hdc, 100, 150);
            LineTo(hdc, 100, 100);
            SelectObject(hdc, hOldPen);
            DeleteObject(brush);

            EndPaint(hwnd, &ps);
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

    MSG msg = {};
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
    for (auto &item : asteroids) {
        item.move();
    }

}
