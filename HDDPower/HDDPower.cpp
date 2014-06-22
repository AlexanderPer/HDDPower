#include "stdafx.h"
#include <Shellapi.h>
#include "HDDPower.h"
#include "PowerMode.h"

#pragma region define
#ifdef UNICODE
#define stringcopy wcscpy
#else
#define stringcopy strcpy
#endif

#define MAX_LOADSTRING 100
#define ID_TRAY_APP_ICON                5000
#define ID_TRAY_EXIT_CONTEXT_MENU_ITEM  3000
#define ID_TRAY_HIGH_PERF_CONTEXT_MENU_ITEM	3001
#define ID_TRAY_NO_STANDBY_CONTEXT_MENU_ITEM	3002
#define ID_TRAY_STANDBY_CONTEXT_MENU_ITEM	3003
#define ID_TRAY_LOW_POWER_CONTEXT_MENU_ITEM	3004
#define ID_TRAY_REFRESH_CONTEXT_MENU_ITEM	3005
#define ID_TRAY_ABOUT_CONTEXT_MENU_ITEM	3006
#define WM_TRAYICON ( WM_USER + 1 )
#pragma endregion

#pragma region constants and globals
HINSTANCE hInst;
TCHAR szTitle[MAX_LOADSTRING];
TCHAR szWindowClass[MAX_LOADSTRING];


UINT WM_TASKBARCREATED = 0;

HWND g_hwnd;
HMENU g_menu;

NOTIFYICONDATA g_notifyIconData;
#pragma endregion

ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

void InitNotifyIconData()
{
	memset( &g_notifyIconData, 0, sizeof( NOTIFYICONDATA ) ) ;	
	g_notifyIconData.cbSize = sizeof(NOTIFYICONDATA);
	g_notifyIconData.hWnd = g_hwnd;
	//g_notifyIconData.uID = ID_TRAY_APP_ICON;
	g_notifyIconData.uID = 100;
	g_notifyIconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;      	
	g_notifyIconData.uCallbackMessage = WM_TRAYICON;
	g_notifyIconData.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_HDDICON));
	stringcopy(g_notifyIconData.szTip, TEXT("HDD Power Manager"));
	g_notifyIconData.uVersion = NOTIFYICON_VERSION;
}

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;
	HACCEL hAccelTable;

	InitPowerMode();
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_HDDPOWER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_HDDPOWER));

	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}


ATOM MyRegisterClass(HINSTANCE hInstance)
{	
	WNDCLASSEX wcex = { 0 };

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_HDDICON));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_HDDPOWER);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_HDDICON));

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{   
	hInst = hInstance;
	g_hwnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
	if (!g_hwnd)
	{
	   return FALSE;
	}  

	CreateWindow( TEXT("static"), TEXT("right click the system tray icon to close"), WS_CHILD | WS_VISIBLE | SS_CENTER,
		0, 0, 400, 400, g_hwnd, 0, hInstance, NULL ) ;

	InitNotifyIconData();

	Shell_NotifyIcon(NIM_ADD, &g_notifyIconData);	
	
	ShowWindow(g_hwnd, SW_HIDE);
	UpdateWindow(g_hwnd);

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;

	switch (message)
	{
	case WM_POWERBROADCAST:
		if (wParam == PBT_APMRESUMEAUTOMATIC) // PBT_APMRESUMESUSPEND - If the system wakes due to user activity
		{
			Refresh();
		}
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);

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
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	
	case WM_TRAYICON:
	{		
		if ((lParam == WM_RBUTTONDOWN) || (lParam == WM_LBUTTONDOWN))
		{			
			HMENU hPopupMenu = CreatePopupMenu();
			
			if (PowerModeIs(HighPerf))
				AppendMenu(hPopupMenu,  MF_CHECKED, ID_TRAY_HIGH_PERF_CONTEXT_MENU_ITEM, TEXT("High Performance"));
			else
				AppendMenu(hPopupMenu, MF_STRING, ID_TRAY_HIGH_PERF_CONTEXT_MENU_ITEM, TEXT("High Performance"));
			
			if (PowerModeIs(NoStandby))
				AppendMenu(hPopupMenu, MF_CHECKED, ID_TRAY_NO_STANDBY_CONTEXT_MENU_ITEM, TEXT("Standby not allowed"));
			else
				AppendMenu(hPopupMenu, MF_STRING, ID_TRAY_NO_STANDBY_CONTEXT_MENU_ITEM, TEXT("Standby not allowed"));
			
			if (PowerModeIs(Standby))
				AppendMenu(hPopupMenu, MF_CHECKED, ID_TRAY_STANDBY_CONTEXT_MENU_ITEM, TEXT("Standby allowed"));
			else
				AppendMenu(hPopupMenu, MF_STRING, ID_TRAY_STANDBY_CONTEXT_MENU_ITEM, TEXT("Standby allowed"));

			if (PowerModeIs(LowPower))
				AppendMenu(hPopupMenu, MF_CHECKED, ID_TRAY_LOW_POWER_CONTEXT_MENU_ITEM, TEXT("Low Power"));
			else
				AppendMenu(hPopupMenu, MF_STRING, ID_TRAY_LOW_POWER_CONTEXT_MENU_ITEM, TEXT("Low Power"));

			AppendMenu(hPopupMenu, MF_SEPARATOR, NULL, NULL);
			AppendMenu(hPopupMenu, MF_STRING, ID_TRAY_REFRESH_CONTEXT_MENU_ITEM, TEXT("Refresh"));
			
			AppendMenu(hPopupMenu, MF_SEPARATOR, NULL, NULL);
			AppendMenu(hPopupMenu, MF_STRING, ID_TRAY_ABOUT_CONTEXT_MENU_ITEM, TEXT("About"));
			AppendMenu(hPopupMenu, MF_STRING, ID_TRAY_EXIT_CONTEXT_MENU_ITEM, TEXT("Exit"));
			
			POINT curPoint ;
			GetCursorPos( &curPoint ) ;
			
			SetForegroundWindow(hWnd);			
			
			UINT clicked = TrackPopupMenu(			  
				hPopupMenu,
				TPM_RETURNCMD | TPM_NONOTIFY,
				curPoint.x,
				curPoint.y,
				0,
				hWnd,
				NULL
			);

			switch (clicked)
			{
				case ID_TRAY_HIGH_PERF_CONTEXT_MENU_ITEM:					
					SetPowerMode(HighPerf);
					break;

				case ID_TRAY_NO_STANDBY_CONTEXT_MENU_ITEM:
					SetPowerMode(NoStandby);
					break;
					
				case ID_TRAY_STANDBY_CONTEXT_MENU_ITEM:
					SetPowerMode(Standby);
					break;

				case ID_TRAY_LOW_POWER_CONTEXT_MENU_ITEM:
					SetPowerMode(LowPower);
					break;

				case ID_TRAY_REFRESH_CONTEXT_MENU_ITEM:
					Refresh();
					break;

				case ID_TRAY_ABOUT_CONTEXT_MENU_ITEM:
					DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
					break;
				
				case ID_TRAY_EXIT_CONTEXT_MENU_ITEM:
					PostQuitMessage( 0 ) ;
					break;
			}
		}
	}
	break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
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
