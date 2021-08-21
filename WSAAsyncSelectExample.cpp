// WSAAsyncSelectExample.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "WSAAsyncSelectExample.h"
#include<WinSock2.h>

#define MAX_LOADSTRING 100
#define WM_ACCEPT WM_USER + 1
#define WM_SOCKET WM_USER + 2
#define MAX_CLIENT 1024

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

HWND g_hWnd = 0;
SOCKET g_clients[MAX_CLIENT];
int g_count = 0;

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

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WSAASYNCSELECTEXAMPLE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WSAASYNCSELECTEXAMPLE));

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WSAASYNCSELECTEXAMPLE));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WSAASYNCSELECTEXAMPLE);
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

   for (int i = 0;i < MAX_CLIENT;i++) {
       g_clients[i] = 0;
   }

   g_hWnd = hWnd;
   //tao socket
   WSADATA data;
   WSAStartup(MAKEWORD(2, 2), &data);
   SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
   SOCKADDR_IN saddr;
   saddr.sin_family = AF_INET;
   saddr.sin_port = htons(8888);
   saddr.sin_addr.S_un.S_addr = INADDR_ANY;
   bind(s, (sockaddr*)&saddr, sizeof(saddr));
   listen(s, 10);
   WSAAsyncSelect(s, hWnd, WM_ACCEPT, FD_ACCEPT);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    SOCKET s = 0;
    SOCKET c = 0;
    SOCKADDR_IN caddr;
    int clen = sizeof(caddr);
    char buffer[1024];
    int i;

    switch (message)
    { 
    case WM_ACCEPT:
        s = (SOCKET)wParam;
        c = accept(s, (sockaddr*)&caddr, &clen);
        for (i = 0; i < g_count; i++) {
            if (g_clients[i] == 0) {
                g_clients[i] = c;
                break;
            }
        }
        if (i == g_count) {
            g_clients[g_count++] = c;
        }
        WSAAsyncSelect(c, hWnd, WM_SOCKET, FD_READ | FD_CLOSE);
        break;
    case WM_SOCKET:
        c = (SOCKET)wParam;

        if (LOWORD(lParam) & FD_READ) {
            memset(buffer, 0, sizeof(buffer));
            recv(c, buffer, sizeof(buffer), 0);
            for (int i = 0;i < g_count;i++) {
                send(g_clients[i], buffer, strlen(buffer), 0);
            }
        }

        if (LOWORD(lParam) & FD_CLOSE) {
            for (int i = 0;i < g_count;i++) {
                if (g_clients[i] == c) {
                    g_clients[i] = 0;
                    break;
                }
            }
            MessageBoxA(hWnd, "A client has disconnected", "Warning", MB_OK);
        }
        
        break;
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
            // TODO: Add any drawing code that uses hdc here...
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
