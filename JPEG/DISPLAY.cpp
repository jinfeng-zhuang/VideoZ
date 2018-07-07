// DISPLAY.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "DISPLAY.h"

#include <stdio.h>
#include <assert.h>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

#define MCU_NUMBER 64

unsigned char rgb24[16 * 16 * 3 * MCU_NUMBER];

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	FILE *fp;
	int ret;

	ret = fopen_s(&fp, "output.rgb24", "rb");
	assert(0 == ret);

	//ret = fseek(fp, width * height * 3, SEEK_SET);
	//assert(0 == ret);

	ret = fread(rgb24, 1, 16 * 16 * 3 * MCU_NUMBER, fp);
	assert(ret == 16 * 16 * 3 * MCU_NUMBER);

	ret = fclose(fp);
	assert(0 == ret);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_DISPLAY, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DISPLAY));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DISPLAY));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_DISPLAY);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

void display(HDC hdc, unsigned char *rgb, int x, int y)
{
	COLORREF *arr = (COLORREF*)calloc(16 * 16 * 2, sizeof(COLORREF));

	memset(arr, 0, 16 * 16 * 2 * sizeof(COLORREF));

	for (int i = 0; i < 16 * 16; i++) {
		arr[i] = 0x00 << 24 | rgb[i * 3 + 0] << 16 | rgb[i * 3 + 1] << 8 | rgb[i * 3 + 2] << 0;
	}

	// Creating temp bitmap
	HBITMAP map = CreateBitmap(
		16,				// width
		16,				// height
		1,				// Color Planes, unfortanutelly don't know what is it actually. Let it be 1
		8 * 4,			// Size of memory for one pixel in bits (in win32 4 bytes = 4*8 bits)
		(void*)arr);	// pointer to array

						// Temp HDC to copy picture
	HDC src = CreateCompatibleDC(hdc);

	// Inserting picture into our temp HDC
	SelectObject(src, map);

	// Copy image from temp HDC to window
	BitBlt(
		hdc,		// Destination
		x,			// x and
		y,			// y - upper-left corner of place, where we'd like to copy
		16,			// width of the region
		16,			// height
		src,		// source
		0,			// x and
		0,			// y of upper left corner  of part of the source, from where we'd like to copy
		SRCCOPY);	// Defined DWORD to juct copy pixels. Watch more on msdn;

	DeleteObject(map);
	DeleteObject(src);
	free(arr);
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            
			for (int i = 0; i < MCU_NUMBER / 8; i++) {
				for (int j = 0; j < 8; j++) {
					display(hdc, &rgb24[16 * 16 * 3 * (i*8+j)], 16 * j, 16 * i);
				}
			}

            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
