#include <windows.h>
#include "framework.h"
#include "sprite.h"

framework* g_Engine = nullptr;

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
        0, CLASS_NAME, L"My New Game",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720,
        nullptr, nullptr, hInstance, nullptr
    );

    ShowWindow(hwnd, nCmdShow);

    // Create engine
    g_Engine = new framework(hwnd);

    // IMPORTANT: allow WindowProc to forward messages to engine
    SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(g_Engine));

    // Setup callback BEFORE initialize
    g_Engine->on_initialized = [&](framework& eng)
        {
            OutputDebugStringW(L"[Game] on_initialized lambda fired\n");

            sprite* s = eng.create_sprite(
                L"R:\\projects\\C++\\EngineTest\\EngineTestAgain\\Data\\sprite\\ko-kyaw-asset.jpg"
            );

            // initialize sprite values (avoid garbage)
            s->movex = 100.0f;
            s->movey = 100.0f;
            s->angel = 0.0f;
            s->r = 1.0f; s->g = 1.0f; s->b = 1.0f; s->a = 1.0f;
            s->visible = true;

            // engine takes ownership of sprite pointer
            eng.SetSprite(0, s);

            OutputDebugStringW(L"[Game] Player sprite created and set into engine.\n");
        };

    g_Engine->initialize();
    int result = g_Engine->run();

    // Engine destructor will cleanup sprites/device/etc.
    delete g_Engine;
    g_Engine = nullptr;

    return result;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    framework* pEngine = reinterpret_cast<framework*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    if (pEngine)
        return pEngine->handle_message(hwnd, uMsg, wParam, lParam);

    if (uMsg == WM_DESTROY) PostQuitMessage(0);
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
