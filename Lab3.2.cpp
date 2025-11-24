// Lab3.2.cpp : Defines the entry point for the application.
//

#include <string>
#include <iostream>
#include <vector>
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


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
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
	int r=0; // Red component (0-255)
	int g=0; // Green component (0-255)
	int b=0; // Blue component (0-255)

	// Default constructor
	Color() : r(0), g(0), b(0) {}

	// Parameterized constructor
	Color(int red, int green, int blue)
		: r(red), g(green), b(blue) {
	}

	void set(int red, int green, int blue)
	{
		r = red;
		g = green;
		b = blue;
	}

	COLORREF rgb()
	{
		return RGB(r, g, b);
	}

	int& operator[](size_t idx)
	{
		switch (idx)
		{
		case 0: return r;
		case 1: return g;
		case 2: return b;
		default: throw std::out_of_range("Color index out of range (valid: 0..2)");
		}
	}

	// Const version for reads: int v = col[1];
	const int& operator[](size_t idx) const
	{
		switch (idx)
		{
		case 0: return r;
		case 1: return g;
		case 2: return b;
		default: throw std::out_of_range("Color index out of range (valid: 0..2)");
		}
	}
};

int brush[] = { PS_DASH, PS_DOT, PS_DASHDOT };

class shape_info
{
public:
	int scale=0;
	Color border;
	Color filling;
	vector<int> brush;

	shape_info() = default;

	// Return a deep copy of this shape_info
	shape_info clone() const
	{
		shape_info copy;
		copy.scale = scale;
		copy.border = border;
		copy.filling = filling;
		copy.brush = brush; // vector copy performs deep copy of elements
		return copy;
	}

	shape_info& operator=(const shape_info& other)
	{
		if (this == &other) return *this;
		scale = other.scale;
		border = other.border;
		filling = other.filling;
		brush = other.brush; // vector copy performs deep copy of elements
		return *this;
	}
};

shape_info global_shape_info;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static POINT default_coord = { 200, 200 };
	static POINT mouse_coord = { 0, 0 };
	static POINT mouse_coord_correction = { 0, 0 };
	static bool lcm = false;
	int scale = global_shape_info.scale +1;

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
		int wmId = LOWORD(wParam);
		switch (wmId)
		{
		case ID_EXIT: PostQuitMessage(0); break;
		case ID_SETTING: DialogBox(hInst, MAKEINTRESOURCE(IDD_SETTING), hWnd, SettingsDlgProc); break;
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

		HPEN hOutlinePen = CreatePen(PS_SOLID, 2, global_shape_info.border.rgb());
		HPEN hOldPen = (HPEN)SelectObject(hdcMem, hOutlinePen);

		HBRUSH hBrush = CreateSolidBrush(global_shape_info.filling.rgb());
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

	InvalidateRect(hWnd, NULL, FALSE);
	return 0;
}

void scrollbarHandler(WPARAM wParam, LPARAM lParam, int scrollbarId, int &value)
{
	HWND hScroll = (HWND)lParam;
	if (GetDlgCtrlID(hScroll) != scrollbarId) return;

	const int MAX_VALUE = 255;

	switch (LOWORD(wParam))
	{
		case SB_LINELEFT: value--; break; // click left arrow
		case SB_LINERIGHT: value++; break; // click right arrow
		case SB_PAGELEFT: value -= MAX_VALUE / 4; break; // click page left
		case SB_PAGERIGHT: value += MAX_VALUE / 4; break; // click page right
		case SB_THUMBTRACK: value = HIWORD(wParam); break; // dragging thumb
	}

	value = max(0, min(value, MAX_VALUE));

	SetScrollRange(hScroll, SB_CTL, 0, MAX_VALUE, TRUE);
	SetScrollPos(hScroll, SB_CTL, value, TRUE);
}

void initScrollbar(HWND hScrollBar, int max, int value)
{
	SetScrollRange(hScrollBar, SB_CTL, 0, max, TRUE);
	SetScrollPos(hScrollBar, SB_CTL, value, TRUE);
}

void setInputValue(HWND hDlg, int IDC_EDIT_HSCR, int value)
{
	SetDlgItemInt(hDlg, IDC_EDIT_HSCR, value, FALSE);
}

int indexOf(const int arr[], int seek)
{
	for (int i = 0; i < sizeof(arr[0]); ++i)
	{
		if (arr[i] == seek) return i;
	}
	return -1;
}

const int IDD_SETTING_BORDER[] = { IDD_SETTING_BORDER_R, IDD_SETTING_BORDER_G, IDD_SETTING_BORDER_B };
const int IDD_SETTING_BORDER_BAR[] = { IDD_SETTING_BORDER_BAR_R, IDD_SETTING_BORDER_BAR_G, IDD_SETTING_BORDER_BAR_B };

const int IDD_SETTING_FILLING[] = { IDD_SETTING_FILLING_R, IDD_SETTING_FILLING_G, IDD_SETTING_FILLING_B };
const int IDD_SETTING_FILLING_BAR[] = { IDD_SETTING_FILLING_BAR_R, IDD_SETTING_FILLING_BAR_G, IDD_SETTING_FILLING_BAR_B };

const int IDD_SETTING_SCALE_X[] = { IDD_SETTING_SCALE_X1, IDD_SETTING_SCALE_X2, IDD_SETTING_SCALE_X3 };


INT_PTR CALLBACK SettingsDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{	
	static shape_info local_shape_info;


	switch (message)
	{
	case WM_INITDIALOG:
	{
		for (int i = 0; i < sizeof(IDD_SETTING_BORDER_BAR)/sizeof(IDD_SETTING_BORDER_BAR[0]); i++)
			initScrollbar(GetDlgItem(hDlg, IDD_SETTING_BORDER_BAR[i]), 255, global_shape_info.border[i]);
		for (int i = 0; i < sizeof(IDD_SETTING_FILLING_BAR) / sizeof(IDD_SETTING_FILLING_BAR[0]); i++)
			initScrollbar(GetDlgItem(hDlg, IDD_SETTING_FILLING_BAR[i]), 255, global_shape_info.filling[i]);

		for (int i = 0; i < sizeof(IDD_SETTING_BORDER) / sizeof(IDD_SETTING_BORDER[0]); i++)
			setInputValue(hDlg, IDD_SETTING_BORDER[i], global_shape_info.border[i]);
		for (int i = 0; i < sizeof(IDD_SETTING_FILLING) / sizeof(IDD_SETTING_BORDER[0]); i++)
			setInputValue(hDlg, IDD_SETTING_FILLING[i], global_shape_info.filling[i]);

		CheckRadioButton(hDlg, IDD_SETTING_SCALE_X[0], IDD_SETTING_SCALE_X[(sizeof(IDD_SETTING_SCALE_X) / sizeof(IDD_SETTING_SCALE_X[0])) - 1], IDD_SETTING_SCALE_X[global_shape_info.scale]);

		break;
	}

	case WM_HSCROLL:
	{
		for (int i = 0; i < sizeof(IDD_SETTING_BORDER_BAR) / sizeof(int); i++)
			scrollbarHandler(wParam, lParam, IDD_SETTING_BORDER_BAR[i], local_shape_info.border[i]);
		for (int i = 0; i < sizeof(IDD_SETTING_FILLING_BAR) / sizeof(int); i++)
			scrollbarHandler(wParam, lParam, IDD_SETTING_FILLING_BAR[i], local_shape_info.filling[i]);

		for (int i = 0; i < sizeof(IDD_SETTING_BORDER) / sizeof(int); i++)
			setInputValue(hDlg, IDD_SETTING_BORDER[i], local_shape_info.border[i]);
		for (int i = 0; i < sizeof(IDD_SETTING_FILLING) / sizeof(int); i++)
			setInputValue(hDlg, IDD_SETTING_FILLING[i], local_shape_info.filling[i]);

		break;
	}


	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);

		int inputID = indexOf(IDD_SETTING_BORDER, wmId);
		if (inputID != -1)
		{
			int value = GetDlgItemInt(hDlg, IDD_SETTING_BORDER[inputID], NULL, FALSE);

			if (value < 0) value = 0, setInputValue(hDlg, IDD_SETTING_BORDER[inputID], 0);
			if (value > 255) value = 255, setInputValue(hDlg, IDD_SETTING_BORDER[inputID], 255);

			local_shape_info.border[inputID] = value;
			initScrollbar(GetDlgItem(hDlg, IDD_SETTING_BORDER_BAR[inputID]), 255, local_shape_info.border[inputID]);
		}

		inputID = indexOf(IDD_SETTING_FILLING, wmId);
		if (inputID != -1)
		{
			int value = GetDlgItemInt(hDlg, IDD_SETTING_FILLING[inputID], NULL, FALSE);

			if (value < 0) value = 0, setInputValue(hDlg, IDD_SETTING_FILLING[inputID], 0);
			if (value > 255) value = 255, setInputValue(hDlg, IDD_SETTING_FILLING[inputID], 255);

			local_shape_info.filling[inputID] = value;
			initScrollbar(GetDlgItem(hDlg, IDD_SETTING_FILLING_BAR[inputID]), 255, local_shape_info.filling[inputID]);
		}


		int radio = indexOf(IDD_SETTING_SCALE_X, wmId);
		if (radio != -1)
		{
			local_shape_info.scale = radio;
		}

		switch (wmId)
		{
		case IDOK:
			global_shape_info = local_shape_info;
			EndDialog(hDlg, IDOK);
			return 0;

		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			return 0;
		}
		break;
	}

	}
	return 0;
}