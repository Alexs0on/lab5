#include "WindowsProject11.h"
#include "framework.h"
#include <set>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
HWND htext;
HWND hEditSize;
HWND hEditRelation;
HWND hButton;
HWND hOutput;
HFONT hFontOutput;

HWND hEditStart;
HWND hEditEnd;
HWND hButtonFindPath;

vector<vector<int>> graphMatrix;





///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


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
    LoadStringW(hInstance, IDC_WINDOWSPROJECT11, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT11));

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT11));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT11);
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

   ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   hFontOutput = CreateFontW(
       26,
       0,
       0,
       0,
       FW_NORMAL,
       FALSE,
       FALSE,
       FALSE,
       DEFAULT_CHARSET,
       OUT_DEFAULT_PRECIS,
       CLIP_DEFAULT_PRECIS,
       DEFAULT_QUALITY,
       DEFAULT_PITCH | FF_DONTCARE,
       L"Segoe UI"
   );

   htext = CreateWindowW(L"STATIC", L"Матрица смежности (nxm):", WS_CHILD | WS_VISIBLE | WS_BORDER,
       10, 10, 200, 30, hWnd, NULL, hInstance, NULL);

   hEditSize = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER,
       210, 10, 150, 30, hWnd, NULL, hInstance, NULL);

   hEditRelation = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
       10, 50, 350, 250, hWnd, NULL, hInstance, NULL);

   hButton = CreateWindowW(L"BUTTON", L"Загрузить матрицу", WS_CHILD | WS_VISIBLE,
       380, 10, 150, 30, hWnd, (HMENU)1, hInstance, NULL);

   hButtonFindPath = CreateWindowW(L"BUTTON", L"Матрица достижимости", WS_CHILD | WS_VISIBLE,
       430, 310, 100, 25, hWnd, (HMENU)2, hInstance, NULL);

   hOutput = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
       10, 370, 520, 150, hWnd, NULL, hInstance, NULL);


   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   SendMessage(hOutput, WM_SETFONT, (WPARAM)hFontOutput, TRUE);
   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


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
    switch (message)
    {
    case WM_COMMAND:
        {
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if (LOWORD(wParam) == 1)
        {
            WCHAR bufferSize[64];
            GetWindowTextW(hEditSize, bufferSize, 64);

            int n = 0, m = 0;
            if (swscanf_s(bufferSize, L"%d %d", &n, &m) != 2 || n <= 0 || m <= 0) {
                SetWindowTextW(hOutput, L"Ошибка: введите корректные размеры n и m");
                break;
            }

            WCHAR bufferRelation[8192];
            GetWindowTextW(hEditRelation, bufferRelation, 8192);

            wstringstream ss(bufferRelation);
            wstring line;
            graphMatrix.clear();

            bool error = false;
            while (getline(ss, line)) {
                if (line.empty()) continue;
                wstringstream lineStream(line);
                vector<int> row;
                int val;
                while (lineStream >> val) {
                    row.push_back(val);
                }
                if ((int)row.size() != m) {
                    SetWindowTextW(hOutput, L"Ошибка: количество столбцов в строке не совпадает с m");
                    graphMatrix.clear();
                    error = true;
                    break;
                }
                graphMatrix.push_back(row);
            }
            if (error) break;

            if ((int)graphMatrix.size() != n) {
                SetWindowTextW(hOutput, L"Ошибка: количество строк матрицы не совпадает с n");
                graphMatrix.clear();
                break;
            }

            for (int i = 0; i < n; ++i) {
                for (int j = 0; j < m; ++j) {
                    if (i != j && graphMatrix[i][j] == 0) {
                        graphMatrix[i][j] = 1e9;
                    }
                }
            }

            SetWindowTextW(hOutput, L"Матрица загружена. Отображение графа обновлено.");
            InvalidateRect(hWnd, NULL, TRUE);
        }
        else if (LOWORD(wParam) == 2) 
        {
            if (graphMatrix.empty()) {
                SetWindowTextW(hOutput, L"Сначала загрузите матрицу");
                break;
            }

            int n = (int)graphMatrix.size();
            vector<vector<int>> R(n, vector<int>(n, 0));


            for (int i = 0; i < n; ++i)
                for (int j = 0; j < n; ++j)
                    R[i][j] = (graphMatrix[i][j] < 1e9) ? 1 : 0;

            for (int i = 0; i < n; ++i)
                R[i][i] = 1;  

            for (int k = 0; k < n; ++k)
                for (int i = 0; i < n; ++i)
                    for (int j = 0; j < n; ++j)
                        R[i][j] = R[i][j] || (R[i][k] && R[k][j]);



            wstring result = L"Матрица достижимости:\r\n";
            for (int i = 0; i < n; ++i) {
                for (int j = 0; j < n; ++j) {
                    result += to_wstring(R[i][j]) + L" ";
                }
                result += L"\r\n";
            }

            SetWindowTextW(hOutput, result.c_str());
        }
        break;
    }
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            SelectObject(hdc, hFontOutput);
            TextOutW(hdc, 10, 50, L"Размер nxm", lstrlenW(L"Размер nxm"));
            TextOutW(hdc, 40, 90, L"nxm", lstrlenW(L"nxm"));

            
            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
