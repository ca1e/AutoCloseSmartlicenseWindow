#include <stdio.h>
#include <windows.h>
#include<tlhelp32.h>

BOOL CALLBACK EnumChildProc(HWND hwndChild, LPARAM lParam)
{
	char className[MAXCHAR] = "";
	char winText[MAX_PATH] = "";
	GetClassName(hwndChild, className, MAXCHAR);
	if (strcmp(className, "Button") == 0)
	{
		EnableWindow(hwndChild, TRUE);
		SendMessage(hwndChild, BM_CLICK, NULL, NULL);
	}
	return TRUE;
}

int count = 0;

int main()
{
	SetConsoleTitle("AutoCloseSmartlicenseWindow");
	printf("waiting");
	while (TRUE)
	{
		if (count == 30)
		{
			printf("\n"); count = 0;
		}
		Sleep(1500);
		HWND hwnd1 = FindWindowEx(NULL, NULL, "#32770", "Non-Commercial License");
		HWND hwnd2 = FindWindowEx(NULL, NULL, "#32770", "非商业许可证");
		if (hwnd1 == 0 && hwnd2 == 0)
		{
			printf("."); count++;
			continue;
		}
		HWND hwnd = (HWND)((int)hwnd1 | (int)hwnd2);
		EnumChildWindows(hwnd, EnumChildProc, NULL);
		printf("*");
	}

	return 0;
}
