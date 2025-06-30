#include <windows.h>
#include "..//Engine_Core//Engine.h"

Engine_Core::Engine engine;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CLOSE:
        // X 버튼 누르면 여기로 옴
        PostQuitMessage(0); // 메시지 루프 종료
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"DirectXEngine";
    RegisterClassW(&wc);

    UINT width = 1280;
    UINT height = 720;
    HWND hwnd = CreateWindowExW(
        0,
        L"DirectXEngine",
        L"DirectXEngine",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, width, height,
        nullptr, nullptr, hInstance, nullptr
    );

    if (!hwnd)
        return -1;


    engine.Init(hwnd, width, height);
    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (true)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                break;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            engine.Run();
        }
    }

    return static_cast<int>(msg.wParam);
}
