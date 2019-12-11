
#include <windows.h>
#include "resource.h"

#define WM_TRAY (WM_USER + 100)
#define WM_TASKBAR_CREATED RegisterWindowMessage(TEXT("TaskbarCreated"))

#define APP_NAME	TEXT("AutoCloseSmartLicense")

NOTIFYICONDATA nid;
HMENU hMenu;
HWND closeBtn = 0;

void InitTray(HWND hWnd)
{
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hWnd;
	nid.uID = IDI_TRAY;
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_INFO;
	nid.uCallbackMessage = WM_TRAY;
	nid.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_TRAY));
	lstrcpy(nid.szTip, APP_NAME);

	hMenu = CreatePopupMenu();
	AppendMenu(hMenu, MF_STRING, ID_EXIT, TEXT("Exit."));

	Shell_NotifyIcon(NIM_ADD, &nid);
}

void ShowBalloonTip()
{
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.uFlags = NIF_INFO;
	nid.uVersion = NOTIFYICON_VERSION;
	nid.uTimeout = 600;
	nid.dwInfoFlags = NIIF_INFO;
	lstrcpy(nid.szInfoTitle, APP_NAME);
	lstrcpy(nid.szInfo, TEXT("Oops! c ya!"));
	Shell_NotifyIcon(NIM_MODIFY, &nid);
}

BOOL CALLBACK EnumChildProc(HWND hwndChild, LPARAM lParam)
{
	TCHAR className[MAXCHAR] = L"";
	TCHAR winText[MAX_PATH] = L"";
	GetClassName(hwndChild, className, MAXCHAR);
	if (lstrcmp(className, TEXT("Button")) == 0)
	{
		GetWindowText(hwndChild, winText, MAX_PATH);
		if (wcsstr(winText, TEXT("Close")) != 0)
		{
			closeBtn = hwndChild;
		}
		else {
			EnableWindow(hwndChild, TRUE);
			SendMessage(hwndChild, BM_CLICK, NULL, NULL);
		}
	}
	return TRUE;        
}

void CloseSmartLicenseWindow()
{
	HWND hwnd1 = FindWindowEx(NULL, NULL, TEXT("#32770"), TEXT("Non-Commercial License"));
	HWND hwnd2 = FindWindowEx(NULL, NULL, TEXT("#32770"), TEXT("非商业许可证"));
	if (hwnd1 == 0 && hwnd2 == 0)
	{
		return;
	}
	HWND hwnd = (HWND)((int)hwnd1 | (int)hwnd2);
	EnumChildWindows(hwnd, EnumChildProc, NULL);

	if (closeBtn != 0) {
		SendMessage(closeBtn, BM_CLICK, NULL, NULL);
		ShowBalloonTip();
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_TRAY:
		switch (lParam)
		{
		case WM_RBUTTONDOWN:
		{
			POINT pt; GetCursorPos(&pt);
			SetForegroundWindow(hWnd);
			int cmd = TrackPopupMenu(hMenu, TPM_RETURNCMD, pt.x, pt.y, NULL, hWnd, NULL);
			if (cmd == ID_EXIT)
				PostMessage(hWnd, WM_DESTROY, NULL, NULL);
		}
		break;
		}
		break;
	case WM_DESTROY:
		Shell_NotifyIcon(NIM_DELETE, &nid);
		PostQuitMessage(0);
		break;
	case WM_TIMER:
		CloseSmartLicenseWindow();
		break;
	}
	if (uMsg == WM_TASKBAR_CREATED)
	{
		Shell_NotifyIcon(NIM_ADD, &nid);
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/,
	LPSTR /*lpCmdLine*/, int iCmdShow)
{
	HWND hWnd;
	MSG msg;
	WNDCLASS wc = { 0 };
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = APP_NAME;

	if (!RegisterClass(&wc)) return 0;

	hWnd = CreateWindowEx(WS_EX_TOOLWINDOW, APP_NAME, APP_NAME, WS_POPUP, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
	UpdateWindow(hWnd);

	InitTray(hWnd);
	SetTimer(hWnd, 3, 1500, NULL);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}