#include <windows.h>
#include <CommCtrl.h>
#include <Richedit.h>
#include <stdio.h>

HWND main, re;
#pragma comment( lib, "comctl32.lib" )

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow);


void setColor(HWND richEditCtrl, COLORREF textColor, COLORREF bgColor)
{
    CHARFORMAT2 cf;

    cf.cbSize = sizeof(CHARFORMAT2);
    cf.dwMask = CFM_COLOR | CFM_BACKCOLOR | CFM_EFFECTS2; // I'm setting only the style information

    cf.crTextColor = textColor;
    cf.crBackColor = bgColor;
    cf.dwEffects = CFE_BOLD;

    SendMessage(richEditCtrl, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPInst, char* line, int show)
{
    InitCommonControls();
    LoadLibrary(TEXT("Msftedit.dll"));


    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hInstance = hInstance;
    wc.lpfnWndProc = WndProc;
    wc.lpszClassName = TEXT("rt");
    wc.style = CS_HREDRAW | CS_VREDRAW;

    if (!RegisterClassEx(&wc))
        FatalAppExitA(0, "Couldn't register window class!");

    main = CreateWindowEx(0, L"rt", L"title", WS_OVERLAPPEDWINDOW, 64, 64, 640, 480, 0, 0, hInstance, 0);
    if (!main)
        FatalAppExitA(0, "Couldn't create main!");

    // http://msdn.microsoft.com/en-us/library/bb774367(VS.85).aspx
    re = CreateWindowEx(0, MSFTEDIT_CLASS, TEXT("Hi!  Click anywhere on the bar above.\n"),
        WS_VISIBLE | WS_CHILD | WS_VSCROLL |
        ES_MULTILINE | ES_LEFT | ES_NOHIDESEL | ES_AUTOVSCROLL | ES_READONLY,
        0, 0, 640, 480,
        main, NULL, hInstance, NULL);
    if (!re)
        FatalAppExitA(0, "Couldn't create your rich text box!");

    CHARFORMAT2 cf;
    cf.cbSize = sizeof(CHARFORMAT2);
    cf.dwMask = CFM_COLOR | CFM_BACKCOLOR | CFM_EFFECTS2; // I'm setting only the style information
    cf.crTextColor = RGB(255, 0, 0);
    cf.crBackColor = RGB(255, 255, 255);
    cf.dwEffects = CFE_BOLD;
    SendMessage(re, EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&cf);

    ShowWindow(main, show);
    ShowWindow(re, show);

    MSG Msg = { 0 };
    while (GetMessageA(&Msg, 0, 0, 0))
    {
        TranslateMessage(&Msg);
        DispatchMessageA(&Msg);
    }
    return 0;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    switch (message)
    {
    case WM_CREATE:
        // upon creation, let the speaker beep at 50Hz, for 10ms.
        Beep(50, 10);
        return 0;
        break;

    case WM_PAINT:
    {
        // we would place our Windows painting code here.
        HDC hdc;
        PAINTSTRUCT ps;
        hdc = BeginPaint(hwnd, &ps);

        // draw a circle and a 2 squares
        Ellipse(hdc, 20, 20, 160, 160);
        Rectangle(hdc, 50, 50, 90, 90);
        Rectangle(hdc, 100, 50, 140, 90);

        EndPaint(hwnd, &ps);
    }
    return 0;
    break;

    case WM_SIZE:
    {
        int w = LOWORD(lparam);
        int h = HIWORD(lparam);
        MoveWindow(re, 0, 0, w, h, true);
    }
    break;

    case WM_CHAR:
        switch (wparam)
        {
        case VK_ESCAPE:
            PostQuitMessage(0);
            break;
        }
        break;

    case WM_NCHITTEST:
    case WM_LBUTTONDOWN:
    {
        int x = LOWORD(lparam);
        int y = HIWORD(lparam);
        TCHAR buf[280];
        wsprintf(buf, L"Mouse is @ (%d, %d)\n", x, y);

        // Move the caret to the end
        CHARRANGE crg = { LONG_MAX, LONG_MAX };
        SendMessage(re, EM_EXSETSEL, 0, (LPARAM)&crg);

        // select random text color
        setColor(re, RGB(rand() % 256, rand() % 256, rand() % 256), RGB(rand() % 256, rand() % 256, rand() % 256));

        SendMessage(re, EM_REPLACESEL, TRUE, (LPARAM)buf);


    }
    break;

    case WM_RBUTTONDOWN:
    {
        // clear it with SETTEXTEX
        SETTEXTEX st;
        st.codepage = CP_ACP; // ansi codepage
        st.flags = ST_KEEPUNDO;

        SendMessage(re, EM_SETTEXTEX, (WPARAM)&st, (LPARAM)"Cleared"); // OVERWRITES all text
    }
    break;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
        break;

    }

    return DefWindowProc(hwnd, message, wparam, lparam);
}