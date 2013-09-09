/* Copyright (c) 2013, Masoom Shaikh
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

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
