// schematic_engine.cpp : Defines the entry point for the application.
#include <windows.h>
#include <windowsx.h>
#include <objidl.h>
#include <gdiplus.h>
#include <ShObjIdl.h>
#include <thread>
#include "framework.h"
#include "schematic_engine.h"
#include "schem.h"
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
LPWSTR *cmdtext;
char boardfn[64];
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    ComponentDialog(HWND, UINT, WPARAM, LPARAM);
HWND hWnd;
HDC hdc;
HDC memdc;

int selectedcomponent = UNDEFCOMP;
int globalres = 220;
int globalcapacity = 500;
int globaltorque = 20;
int globalluminosity = 100;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    int argc, i;
    LPWSTR* argv;
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR           gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SCHEMATICENGINE, szWindowClass, MAX_LOADSTRING);

    //UNCOMMENT FOR CONSOLE DEBUGGING
    FILE* fp;

    AllocConsole();
    freopen_s(&fp, "CONIN$", "r", stdin);
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);

    MyRegisterClass(hInstance);
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SCHEMATICENGINE));
    CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOGBAR), hWnd, ComponentDialog);
    MSG msg;
    initparametermapping();
    initcomponentmapping();
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        SetCursor(LoadCursor(NULL, IDC_CROSS));
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    GdiplusShutdown(gdiplusToken);
    return (int) msg.wParam;
}

PWSTR fileOpenFunction(void) {
    wchar_t tmp[] = L"Temp";
    PWSTR returnFileName = tmp;
    int succeeded = 0;
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
        COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(hr))
    {
        IFileOpenDialog* pFileOpen;
        hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
            IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

        if (SUCCEEDED(hr))
        {
            hr = pFileOpen->Show(NULL);
            if (SUCCEEDED(hr))
            {
                IShellItem* pItem;
                hr = pFileOpen->GetResult(&pItem);
                if (SUCCEEDED(hr))
                {
                    PWSTR pszFilePath;
                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                    if (SUCCEEDED(hr)) {
                        succeeded = 1;
                        returnFileName = pszFilePath;
                        CoTaskMemFree(pszFilePath);
                    }
                    pItem->Release();
                }
            }
            pFileOpen->Release();
        }
        CoUninitialize();
    }
    return(returnFileName);
}

PWSTR fileSaveAsFunction(void) {
    wchar_t tmp[] = L"Temp";
    LPCWSTR paintExt;
    paintExt = L"txt";
    PWSTR returnFileName = tmp;
    int succeeded = 0;
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
        COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(hr)) {
        IFileSaveDialog* pFileSaveAs;
        hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSaveAs));
        if (SUCCEEDED(hr)) {
            pFileSaveAs->SetDefaultExtension(paintExt);
            pFileSaveAs->SetTitle(L"Save painting as...");
            hr = pFileSaveAs->Show(NULL);
            if (SUCCEEDED(hr)) {
                IShellItem* pItem;
                hr = pFileSaveAs->GetResult(&pItem);
                if (SUCCEEDED(hr)) {
                    PWSTR pszFilePath;
                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                    if (SUCCEEDED(hr)) {
                        returnFileName = pszFilePath;
                        CoTaskMemFree(pszFilePath);
                    }
                    pItem->Release();
                }
            }
            pFileSaveAs->Release();
        }
        CoUninitialize();

    }
    return (returnFileName);
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SCHEMATICENGINE));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_SCHEMATICENGINE);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

void INITDBLBUFFER(HDC hdc) {
    memdc = CreateCompatibleDC(hdc);
    INT width, height;
    RECT dimension;
    GetWindowRect(hWnd, &dimension);
    width = dimension.right - dimension.left;
    height = dimension.bottom - dimension.top;
    HBITMAP hBM = CreateCompatibleBitmap(hdc, width, height);
    SelectObject(memdc, hBM);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static int callbacks = 0;
    PAINTSTRUCT ps;
    int index = 0;
    char msg[32];
    switch (message) {
    case WM_KEYDOWN:
        sprintf(msg, "WM_KEYDOWN: 0x%x\n", wParam);
        OutputDebugStringA(msg);
        break;
    case WM_RBUTTONDOWN:
        //undoaction();
        mouserbutton(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        break;
    case WM_LBUTTONDOWN:
        mouselbutton(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        //pushdata(line);
        break;
    case WM_LBUTTONUP:
        mouseup(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        
        break;
    case WM_MOUSEMOVE:
        
        if (wParam & MK_LBUTTON) {
            mouselmove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        }
        else if (wParam & MK_RBUTTON) {
            mousermove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        }
        else {
            mousemove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        }
    case WM_CREATE:
    {
        std::thread cfthread(checkfile);
        cfthread.detach();
    }
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            RECT* windowrect = {};
            GetWindowRect(hWnd, windowrect);
            switch (wmId)
            {
                case IDM_ABOUT:
                    DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                    break;
                case IDM_EXIT:
                    DestroyWindow(hWnd);
                    break;
                case ID_FILE_OPEN:
                    if(fp)
                        fclose(fp);
                    invokestate = false;
                    sprintf(boardfn, "\0");
                    sprintf(boardfn, "%ws", fileOpenFunction());
                    while (boardfn[index] != '\0') {
                        if(boardfn[index] == '\\')
                            boardfn[index] = '/';
                        index++;
                    }
                    //MainRender(hdc);
                    RedrawWindow(hWnd, windowrect, NULL, RDW_ERASE | RDW_INVALIDATE);
                    break;
                case ID_FILE_SAVE:
                {
                    char savefn[64];
                    invokestate = false;
                    sprintf(savefn, "\0");
                    sprintf(savefn, "%ws", fileSaveAsFunction());
                    while (boardfn[index] != '\0') {
                        if (boardfn[index] == '\\')
                            boardfn[index] = '/';
                        index++;
                    }
                    saveboard(savefn);
                }
                break;
                case ID_ZOOM_25:
                    gridstep = 5;
                    RedrawWindow(hWnd, windowrect, NULL, RDW_ERASE | RDW_INVALIDATE);
                    break;
                case ID_ZOOM_50:
                    gridstep = 10;
                    RedrawWindow(hWnd, windowrect, NULL, RDW_ERASE | RDW_INVALIDATE);
                    break;
                case ID_ZOOM_75:
                    gridstep = 15;
                    RedrawWindow(hWnd, windowrect, NULL, RDW_ERASE | RDW_INVALIDATE);
                    break;
                case ID_ZOOM_100:
                    gridstep = 20;
                    RedrawWindow(hWnd, windowrect, NULL, RDW_ERASE | RDW_INVALIDATE);
                    break;
                case ID_ZOOM_125:
                    gridstep = 25;
                    RedrawWindow(hWnd, windowrect, NULL, RDW_ERASE | RDW_INVALIDATE);
                    break;
                case ID_ZOOM_150:
                    gridstep = 30;
                    RedrawWindow(hWnd, windowrect, NULL, RDW_ERASE | RDW_INVALIDATE);
                    break;
                case ID_ZOOM_175:
                    gridstep = 35;
                    RedrawWindow(hWnd, windowrect, NULL, RDW_ERASE | RDW_INVALIDATE);
                    break;
                case ID_ZOOM_200:
                    gridstep = 40;
                    RedrawWindow(hWnd, windowrect, NULL, RDW_ERASE | RDW_INVALIDATE);
                    break;
                default:
                    return DefWindowProc(hWnd, message, wParam, lParam);
            }
            free(windowrect);
        }
        break;
    case WM_PAINT:
        {
            hdc = BeginPaint(hWnd, &ps);
            if (callbacks == 0) {
                INITDBLBUFFER(hdc);
                MainRender(hdc);
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

INT_PTR CALLBACK ComponentDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        SendDlgItemMessage(hDlg, IDC_BUTTON5, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)LoadBitmap(hInst, MAKEINTRESOURCE(IDB_RESISTORHZ)));
        SendDlgItemMessage(hDlg, IDC_BUTTON2, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)LoadBitmap(hInst, MAKEINTRESOURCE(IDB_CAPACITORHZ)));
        SendDlgItemMessage(hDlg, IDC_BUTTON3, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)LoadBitmap(hInst, MAKEINTRESOURCE(IDB_DCMOTORHZ)));
        SendDlgItemMessage(hDlg, IDC_BUTTON4, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)LoadBitmap(hInst, MAKEINTRESOURCE(IDB_WIRE)));
        SendDlgItemMessage(hDlg, IDC_BUTTON6, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)LoadBitmap(hInst, MAKEINTRESOURCE(IDB_LED)));
        SendDlgItemMessage(hDlg, IDC_BUTTON7, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)LoadBitmap(hInst, MAKEINTRESOURCE(IDB_SWITCH)));
        SendDlgItemMessage(hDlg, IDC_BUTTON8, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)LoadBitmap(hInst, MAKEINTRESOURCE(IDB_INDUCTOR)));
        SendDlgItemMessage(hDlg, IDC_BUTTON9, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)LoadBitmap(hInst, MAKEINTRESOURCE(IDB_ARDMEGA)));
        SendDlgItemMessage(hDlg, IDC_BUTTON10, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)LoadBitmap(hInst, MAKEINTRESOURCE(IDB_ULTRASONIC)));
        SendDlgItemMessage(hDlg, IDC_BUTTON11, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)LoadBitmap(hInst, MAKEINTRESOURCE(IDB_INFRARED)));
        SendDlgItemMessage(hDlg, IDC_BUTTON12, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)LoadBitmap(hInst, MAKEINTRESOURCE(IDB_NRF)));
        SendDlgItemMessage(hDlg, IDC_BUTTON13, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)LoadBitmap(hInst, MAKEINTRESOURCE(IDB_SERVO)));
        SendDlgItemMessage(hDlg, IDC_BUTTON14, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)LoadBitmap(hInst, MAKEINTRESOURCE(IDB_MICRO)));
        SendDlgItemMessage(hDlg, IDC_BUTTON15, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BUZZER)));
        SendDlgItemMessage(hDlg, IDC_BUTTON16, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)LoadBitmap(hInst, MAKEINTRESOURCE(IDB_NANO)));
        SendDlgItemMessage(hDlg, IDC_BUTTON17, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)LoadBitmap(hInst, MAKEINTRESOURCE(IDB_DHT11)));
        SendDlgItemMessage(hDlg, IDC_BUTTON18, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)LoadBitmap(hInst, MAKEINTRESOURCE(IDB_NODEMCU)));
        SendDlgItemMessage(hDlg, IDC_BUTTON19, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)LoadBitmap(hInst, MAKEINTRESOURCE(IDB_GY521)));
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        switch (LOWORD(wParam)) {
            case IDC_BUTTON1:
                if(GetDlgItemInt(hDlg, IDC_EDIT2, NULL, TRUE) != 0)
                    globalres = GetDlgItemInt(hDlg, IDC_EDIT2, NULL, TRUE);
                if (GetDlgItemInt(hDlg, IDC_EDIT3, NULL, TRUE) != 0)
                    globalcapacity = GetDlgItemInt(hDlg, IDC_EDIT3, NULL, TRUE);
                if (GetDlgItemInt(hDlg, IDC_EDIT4, NULL, TRUE) != 0)
                    globaltorque = GetDlgItemInt(hDlg, IDC_EDIT4, NULL, TRUE);
                break;
            case IDC_BUTTON5:
                selectedcomponent = RESISTOR;
                break;
            case IDC_BUTTON4:
                selectedcomponent = WIRECOMP;
                break;
            case IDC_BUTTON2:
                selectedcomponent = CAPACITOR;
                break;
            case IDC_BUTTON3:
                selectedcomponent = DCMOTOR;
                break;
            case IDC_BUTTON6:
                selectedcomponent = LEDLIGHT;
                break;
            case IDC_BUTTON7:
                selectedcomponent = SWITCHCOMP;
                break;
            case IDC_BUTTON8:
                selectedcomponent = INDUCTOR;
                break;
            case IDC_BUTTON9:
                selectedcomponent = MEGA;
                break;
            case IDC_BUTTON10:
                selectedcomponent = ULTRASONIC;
                break;
            case IDC_BUTTON11:
                selectedcomponent = INFRARED;
                break;
            case IDC_BUTTON12:
                selectedcomponent = NRF;
                break;
            case IDC_BUTTON13:
                selectedcomponent = SERVO;
                break;
            case IDC_BUTTON14:
                selectedcomponent = MICRO;
                break;
            case IDC_BUTTON15:
                selectedcomponent = BUZZER;
                break;
            case IDC_BUTTON16:
                selectedcomponent = NANO;
                break;
            case IDC_BUTTON17:
                selectedcomponent = DHT11;
                break;
            case IDC_BUTTON18:
                selectedcomponent = NODEMCU;
                break;
            case IDC_BUTTON19:
                selectedcomponent = ACCELEROMETER;
                break;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

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
