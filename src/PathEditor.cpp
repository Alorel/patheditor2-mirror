#include <windows.h>
#include <commctrl.h>
#include "resource.h"
#include "PathEditorDlg.h"

#pragma comment(linker, \
	"\"/manifestdependency:type='Win32' "\
	"name='Microsoft.Windows.Common-Controls' "\
	"version='6.0.0.0' "\
	"processorArchitecture='*' "\
	"publicKeyToken='6595b64144ccf1df' "\
	"language='*'\"")

#pragma comment(lib, "comctl32.lib")

INT_PTR CALLBACK DialogProc(HWND hWnd, UINT, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd)
{
	InitCommonControls();
	HWND hWnd = CreateDialogParam( hInstance, MAKEINTRESOURCE(IDD_PATHEDITOR_DIALOG), 0,
		DialogProc, reinterpret_cast<LPARAM>(hInstance));
	ShowWindow( hWnd, nShowCmd);

	MSG msg;
	while(GetMessage(&msg, 0, 0, 0) == TRUE)
	{
		if(IsDialogMessage(hWnd, &msg) == FALSE)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return 0;
}

CPathEditorDlg theDialog;
INT_PTR CALLBACK DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		return theDialog.OnInitDialog( reinterpret_cast<HINSTANCE>(lParam), hWnd);
	case WM_NOTIFY:
		return theDialog.OnNotify( reinterpret_cast<LPNMHDR>(lParam));
	case WM_COMMAND:
		return theDialog.OnCommand(uMsg, wParam, lParam);
	case WM_CLOSE:
		DestroyWindow(hWnd);
		return TRUE;
	case WM_DESTROY:
		PostQuitMessage(0);
	}
	return FALSE;
}
