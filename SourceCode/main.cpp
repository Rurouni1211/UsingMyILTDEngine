#include <windows.h>
#include <memory>

#include "framework.h"
#include "sprite.h"
#include "Input.h"   // your Input.h

framework* g_Engine = nullptr;
sprite* myPlayerSprite = nullptr;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow)
{
    const wchar_t CLASS_NAME[] = L"MyGameWindow";

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClassExW(&wc);

    HWND hwnd = CreateWindowExW(
        0, CLASS_NAME, L"My New Game", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720,
        NULL, NULL, hInstance, NULL
    );

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Engine
    g_Engine = new framework(hwnd);
    SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)g_Engine);

    // Create sprite AFTER engine device exists (use callback you already have)
    g_Engine->on_initialized = [&](framework& eng)
        {
            myPlayerSprite = eng.create_sprite(
                L"R:\\projects\\C++\\EngineTest\\EngineTestAgain\\Data\\sprite\\ko-kyaw-asset.jpg"
            );
            myPlayerSprite->movex = 100.0f;
            myPlayerSprite->movey = 100.0f;
            myPlayerSprite->angel = 0.0f;

            myPlayerSprite->r = 1.0f;
            myPlayerSprite->g = 1.0f;
            myPlayerSprite->b = 1.0f;
            myPlayerSprite->a = 1.0f;

            myPlayerSprite->visible = true;

            eng.SetSprite(0, myPlayerSprite);
        };

    // Initialize engine ONCE
    if (!g_Engine->initialize())
        return 0;

    // Input (main owns input now)
    Input input;
    // if you upgraded Input to have Init() mapping, call it
    input.Init();

    MSG msg{};
    bool running = true;

    while (running)
    {
        // Process all pending messages first
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                running = false;
                break;
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (!running)
            break;

        // -------- GAME LOGIC (main.cpp owns it) --------
        input.Update();

        // Move sprite (use arrow keys)
        if (myPlayerSprite)
        {
            const float speed = 0.4f; // pixels per frame (simple)
            if (input.Down(Input::PAD_LEFT))  myPlayerSprite->movex -= speed;
            if (input.Down(Input::PAD_RIGHT)) myPlayerSprite->movex += speed;
            if (input.Down(Input::PAD_UP))    myPlayerSprite->movey -= speed;
            if (input.Down(Input::PAD_DOWN))  myPlayerSprite->movey += speed;
        }

        // -------- ENGINE FRAME --------
        g_Engine->tick();   // calls update/render with dt internally
    }

    // Shutdown
    g_Engine->uninitialize();
    delete g_Engine;
    g_Engine = nullptr;

    return (int)msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    framework* pEngine = reinterpret_cast<framework*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

    if (pEngine)
        return pEngine->handle_message(hwnd, uMsg, wParam, lParam);

    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
