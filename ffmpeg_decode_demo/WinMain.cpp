#include <Windows.h>
#include <stdio.h>
#include <process.h>

extern int TVBoardRegister(HINSTANCE hInstance);
extern int FrameGenerator(void* arg);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    MSG msg;
    HWND hwnd;
    HANDLE hThread;

    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    //freopen("log.txt", "w", stdout);

    TVBoardRegister(hInstance);

    hThread = (HANDLE)_beginthreadex(NULL, 0, (_beginthreadex_proc_type)FrameGenerator, NULL, 0, NULL);
    if (NULL == hThread)
        return -1;

    hwnd = CreateWindowEx(
        0, // dwExStyle
        TEXT("TVBoard"),
        TEXT("TV"), // Title
        WS_SIZEBOX,
        100, 100, 800, 600,
        NULL, // hWndParent
        NULL, // hMenu
        hInstance,
        NULL);

    ShowWindow(hwnd, nShowCmd);
    UpdateWindow(hwnd);

    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)(msg.wParam);
}

