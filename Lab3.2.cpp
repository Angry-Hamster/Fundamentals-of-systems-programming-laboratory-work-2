// Lab3.2.cpp : Defines the entry point for the application.
//

#include <string>
#include <iostream>
#include "framework.h"
#include "Resource.h"

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
INT_PTR CALLBACK    SettingsDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine,	_In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_LAB32, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB32));

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

	return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB32));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_LAB32);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

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

wstring StringToWide(const string& str) {
	if (str.empty()) return wstring();
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
	wstring wstr(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstr[0], size_needed);
	return wstr;
}

struct Color {
	uint8_t r; // Red component (0-255)
	uint8_t g; // Green component (0-255)
	uint8_t b; // Blue component (0-255)

	// Default constructor
	Color() : r(0), g(0), b(0) {}

	// Parameterized constructor
	Color(uint8_t red, uint8_t green, uint8_t blue)
		: r(red), g(green), b(blue){}

	void set(uint8_t red, uint8_t green, uint8_t blue)
	{
		r = red;
		g = green;
		b = blue;
	}

	COLORREF rgb()
	{
		return RGB(r, g, b);
	}
};

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	InvalidateRect(hWnd, NULL, FALSE);

	static POINT default_coord = { 200, 200 };
	static POINT mouse_coord = { 0, 0 };
	static POINT mouse_coord_correction = { 0, 0 };
	static int scale = 2;
	static bool lcm = false;
	static int wmId;
	static Color border(30, 122, 27);
	static Color filling(50, 142, 47);

	static bool on_shape = false;
	if (on_shape) SetCursor(LoadCursor(NULL, IDC_HAND));
	else SetCursor(LoadCursor(NULL, IDC_ARROW));


	POINT square[] = {
		{default_coord.x + scale * 100,   default_coord.y + scale * 0},
		{default_coord.x + scale * 0,     default_coord.y + scale * 100},
		{default_coord.x + scale * -100,  default_coord.y + scale * 0},
		{default_coord.x + scale * 0,     default_coord.y + scale * -100}
	};

	POINT triangle[] = {
		{default_coord.x + scale * 100,   default_coord.y + scale * 0},
		{default_coord.x + scale * -50,   default_coord.y + scale * 50},
		{default_coord.x + scale * -50,   default_coord.y + scale * -50}
	};

	switch (message)
	{
	case WM_LBUTTONDOWN:
	{
		if (on_shape)
			lcm = true;
	}
	break;
	case WM_LBUTTONUP:
	{
		lcm = false;
	}
	break;
	case WM_MOUSEMOVE:
	{
		mouse_coord.x = LOWORD(lParam);
		mouse_coord.y = HIWORD(lParam);

		int dx = default_coord.x - mouse_coord.x;
		int dy = default_coord.y - mouse_coord.y;
		on_shape = sqrt(dx * dx + dy * dy) <= 100 * abs(scale);

		if ((GetKeyState(VK_LBUTTON) & 0x8000) && lcm)
		{
			default_coord.x = mouse_coord.x - mouse_coord_correction.x;
			default_coord.y = mouse_coord.y - mouse_coord_correction.y;
		}
		else
		{
			mouse_coord_correction.x = mouse_coord.x - default_coord.x;
			mouse_coord_correction.y = mouse_coord.y - default_coord.y;
		}
	}
	break;
	case WM_CREATE:
	{
		RECT rect;
		GetClientRect(hWnd, &rect);
		default_coord = { (rect.left + rect.right) / 2, (rect.top + rect.bottom) / 2 };
	}
	break;
	case WM_COMMAND:
	{
		wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case ID_EXIT: PostQuitMessage(0); break;
		case ID_SETTING: DialogBox(hInst, MAKEINTRESOURCE(IDD_SETTING), hWnd, SettingsDlgProc); break;

		case 32771: scale = 1 * scale / abs(scale); break;
		case 32772: scale = 2 * scale / abs(scale); break;
		case 32773: scale = 3 * scale / abs(scale); break;

		case 65535: scale = -scale; break;

		case 32778: border.set(30, 122, 27); break;
		case 32779: border.set(240, 240, 14); break;
		case 32780: border.set(159, 20, 201); break; 
		case 32781: border.set(181, 37, 14); break;

		case 32782: filling.set(50, 142, 47); break;
		case 32783: filling.set(255, 255, 34); break;
		case 32784: filling.set(179, 40, 221); break;
		case 32785: filling.set(201, 47, 34); break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;

	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		int width = ps.rcPaint.right - ps.rcPaint.left;
		int height = ps.rcPaint.bottom - ps.rcPaint.top;

		HDC hdcMem = CreateCompatibleDC(hdc);
		HBITMAP hbmNew = CreateCompatibleBitmap(hdc, width, height); 
		HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbmNew); 

		RECT memRect = { 0, 0, width, height }; 
		FillRect(hdcMem, &memRect, (HBRUSH)(COLOR_WINDOW + 1));

		HPEN hOutlinePen = CreatePen(PS_SOLID, 2, border.rgb());
		HPEN hOldPen = (HPEN)SelectObject(hdcMem, hOutlinePen);

		HBRUSH hBrush = CreateSolidBrush(filling.rgb());
		HBRUSH hOldBrush = (HBRUSH)SelectObject(hdcMem, hBrush);

		Polygon(hdcMem, square, size(square));
		Polygon(hdcMem, triangle, size(triangle));

		BitBlt(hdc, ps.rcPaint.left, ps.rcPaint.top, width, height,
			hdcMem, 0, 0, SRCCOPY);

		SelectObject(hdcMem, hbmOld); 
		DeleteObject(hbmNew); 
		DeleteDC(hdcMem);

		EndPaint(hWnd, &ps);
		return 0;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

INT_PTR CALLBACK SettingsDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    // default color matches your current defaults in WndProc
    static int defR = 0;
	static int defG = 0;
	static int defB = 0;

    switch (message)
    {
    case WM_INITDIALOG:
    {
        // initialize scrollbars: range 0..255 and initial positions
        HWND hR = GetDlgItem(hDlg, IDC_SCROLLBAR4);
        HWND hG = GetDlgItem(hDlg, IDC_SCROLLBAR5);
        HWND hB = GetDlgItem(hDlg, IDC_SCROLLBAR6);

        // set range and position
        SetScrollRange(hR, SB_CTL, 0, 255, FALSE);
        SetScrollRange(hG, SB_CTL, 0, 255, FALSE);
        SetScrollRange(hB, SB_CTL, 0, 255, FALSE);

        SetScrollPos(hR, SB_CTL, defR, TRUE);
        SetScrollPos(hG, SB_CTL, defG, TRUE);
        SetScrollPos(hB, SB_CTL, defB, TRUE);

        // update numeric labels
        SetDlgItemInt(hDlg, IDC_RVALUE, defR, TRUE);
        SetDlgItemInt(hDlg, IDC_GVALUE, defG, TRUE);
        SetDlgItemInt(hDlg, IDC_BVALUE, defB, TRUE);

        return (INT_PTR)TRUE;
    }

    case WM_HSCROLL:
    {
        // lParam is the scrollbar HWND for control scrollbars
        HWND hScroll = (HWND)lParam;
        if (hScroll == NULL)
            break;

        // get current position (SBM_GETPOS is reliable)
        int pos = (int)SendMessage(hScroll, SBM_GETPOS, 0, 0);
        // ensure it's limited to 0..255
        if (pos < 0) pos = 0;
        if (pos > 255) pos = 255;
        // apply and redraw thumb
        SetScrollPos(hScroll, SB_CTL, pos, TRUE);

        // update the matching numeric label
        int id = GetDlgCtrlID(hScroll);
        if (id == IDC_SCROLLBAR4) SetDlgItemInt(hDlg, IDC_RVALUE, pos, FALSE);
        else if (id == IDC_SCROLLBAR5) SetDlgItemInt(hDlg, IDC_GVALUE, pos, FALSE);
        else if (id == IDC_SCROLLBAR6) SetDlgItemInt(hDlg, IDC_BVALUE, pos, FALSE);

        return (INT_PTR)TRUE;
    }

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            if (LOWORD(wParam) == IDOK)
            {
                // read final values
                BOOL ok;
                int r = GetDlgItemInt(hDlg, IDC_RVALUE, &ok, FALSE);
                int g = GetDlgItemInt(hDlg, IDC_GVALUE, &ok, FALSE);
                int b = GetDlgItemInt(hDlg, IDC_BVALUE, &ok, FALSE);

                // clamp just in case
                r = max(0, min(255, r));
                g = max(0, min(255, g));
                b = max(0, min(255, b));

                // Optionally communicate to parent window:
                // Parent can handle WM_APP + 1 and extract RGB from wParam:
                // wParam = (r << 16) | (g << 8) | b
                HWND parent = GetParent(hDlg);
                if (parent)
                    SendMessage(parent, WM_APP + 1, (WPARAM)((r << 16) | (g << 8) | b), 0);
            }

            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}