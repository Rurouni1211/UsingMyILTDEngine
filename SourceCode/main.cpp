#include <windows.h>
#include "framework.h"
#include "sprite.h"
#include "Input.h"

#include "Game.h"

framework* g_Engine = nullptr;
static Game gGame;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// High-res timer
static LARGE_INTEGER gFreq{};
static LARGE_INTEGER gPrev{};

static float GetDeltaTime()
{
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    double dt = double(now.QuadPart - gPrev.QuadPart) / double(gFreq.QuadPart);
    gPrev = now;

    // clamp dt to avoid huge jumps when dragging window etc.
    if (dt < 0.0) dt = 0.0;
    if (dt > 0.033) dt = 0.033;
    return (float)dt;
}

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
        0, CLASS_NAME, L"Basket Shooter (Stages 1-4)", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720,
        NULL, NULL, hInstance, NULL
    );

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Engine
    g_Engine = new framework(hwnd);
    SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)g_Engine);

    // Create sprites AFTER device exists
    g_Engine->on_initialized = [&](framework& eng)
        {

            // New textures – change these paths to your actual files
            const wchar_t* THROW_BALL_TEX = L"R:\\projects\\C++\\EngineTest\\EngineTestAgain\\Data\\sprite\\ball.png";      // the ball you throw / control
            const wchar_t* HOOP_TEX = L"R:\\projects\\C++\\EngineTest\\EngineTestAgain\\Data\\sprite\\hoop.png";      // the basket

            // Load throwable ball (will be player)
            sprite* throwBall = eng.create_sprite(THROW_BALL_TEX);
            if (throwBall)
            {
                throwBall->movex = 200.0f;           // starting position bottom-left-ish
                throwBall->movey = 560.0f;
                throwBall->angel = 0.0f;
                throwBall->r = throwBall->g = throwBall->b = throwBall->a = 1.0f;
                throwBall->visible = true;
                eng.SetSprite(0, throwBall);
                OutputDebugStringW(L"[Main] Throw ball (player) loaded and set\n");
            }

            // Load hoop
            sprite* hoop = eng.create_sprite(HOOP_TEX);
            if (hoop)
            {
                hoop->movex = 980.0f;                // right side
                hoop->movey = 160.0f;
                hoop->angel = 0.0f;
                hoop->r = hoop->g = hoop->b = hoop->a = 1.0f;
                hoop->visible = true;
                eng.SetSprite(1, hoop);
                OutputDebugStringW(L"[Main] Hoop loaded and set\n");
            }

            // Pass to game – now ballTexturePath is the same as throw ball
            gGame.Init(eng, throwBall, hoop, THROW_BALL_TEX);
        };

    // Initialize engine ONCE
    if (!g_Engine->initialize())
        return 0;

    // Input
    Input input;
    input.Init();

    // Timer init
    QueryPerformanceFrequency(&gFreq);
    QueryPerformanceCounter(&gPrev);

    MSG msg{};
    bool running = true;

    while (running)
    {
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
        if (!running) break;

        input.Update();
        float dt = GetDeltaTime();

        // ✅ all logic here
        gGame.Update(dt, input, *g_Engine);

        // engine frame (renders sprites)
        g_Engine->tick();
    }

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
