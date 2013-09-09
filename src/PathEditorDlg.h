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

#include <utility>
#include "PathListCtrl.h"
#include <vector>

class CPathEditorDlg
{
private:
	HWND m_hWnd;
	HINSTANCE m_hInstance;
	HICON m_hIcon;
	HIMAGELIST m_hImageList;

	bool m_bIsAdmin;
	CPathListCtrl m_usrListCtrl;
	CPathListCtrl m_sysListCtrl;

	std::vector<HICON> m_ButtonIcons;

private:
	BOOL _CenterWindow();
	BOOL _CreateImageList();
	BOOL _SetButtonIcons();

public:
	CPathEditorDlg();
	~CPathEditorDlg();
	BOOL OnInitDialog( HINSTANCE hInstance, HWND hWnd);
	BOOL OnCommand(UINT nMsg, WPARAM wParam, LPARAM lParam);
	BOOL OnNotify(LPNMHDR lpNMHDR);
	BOOL OnOK();
	void OnButtonGainPrivilege();
	void OnListGetDispInfo(NMLVDISPINFO *pDispInfo);
	void OnListDoubleClick(LPNMITEMACTIVATE lpNMItemActivate);
};
