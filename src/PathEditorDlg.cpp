#include "PathEditorDlg.h"
#include "resource.h"
#include "Util.h"

#include <algorithm>

CPathEditorDlg::CPathEditorDlg()
{
}

CPathEditorDlg::~CPathEditorDlg()
{
	DestroyIcon(m_hIcon);
	std::for_each( m_ButtonIcons.begin(), m_ButtonIcons.end(), DestroyIcon);
	ImageList_Destroy(m_hImageList);
}

// copied whole stock from atlwin.h
// trimmed ATL dependent macros, mostly asserts.
BOOL CPathEditorDlg::_CenterWindow()
{
	// determine owner window to center against
	DWORD dwStyle = (DWORD)::GetWindowLong(m_hWnd, GWL_STYLE);
	HWND hWndCenter = NULL;
	if(dwStyle & WS_CHILD)
		hWndCenter = ::GetParent(m_hWnd);
	else
		hWndCenter = ::GetWindow(m_hWnd, GW_OWNER);

	// get coordinates of the window relative to its parent
	RECT rcDlg;
	::GetWindowRect(m_hWnd, &rcDlg);
	RECT rcArea;
	RECT rcCenter;
	HWND hWndParent;
	if(!(dwStyle & WS_CHILD))
	{
		// don't center against invisible or minimized windows
		if(hWndCenter != NULL)
		{
			DWORD dwStyleCenter = ::GetWindowLong(hWndCenter, GWL_STYLE);
			if(!(dwStyleCenter & WS_VISIBLE) || (dwStyleCenter & WS_MINIMIZE))
				hWndCenter = NULL;
		}

		HMONITOR hMonitor = NULL;
		if(hWndCenter != NULL)
		{
			hMonitor = ::MonitorFromWindow(hWndCenter, MONITOR_DEFAULTTONEAREST);
		}
		else
		{
			hMonitor = ::MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
		}

		MONITORINFO minfo;
		minfo.cbSize = sizeof(MONITORINFO);
		BOOL bResult = ::GetMonitorInfo(hMonitor, &minfo);

		rcArea = minfo.rcWork;
		if(hWndCenter == NULL)
			rcCenter = rcArea;
		else
			::GetWindowRect(hWndCenter, &rcCenter);
	}
	else
	{
		// center within parent client coordinates
		hWndParent = ::GetParent(m_hWnd);

		::GetClientRect(hWndParent, &rcArea);
		::GetClientRect(hWndCenter, &rcCenter);
		::MapWindowPoints(hWndCenter, hWndParent, (POINT*)&rcCenter, 2);
	}

	int DlgWidth = rcDlg.right - rcDlg.left;
	int DlgHeight = rcDlg.bottom - rcDlg.top;

	// find dialog's upper left based on rcCenter
	int xLeft = (rcCenter.left + rcCenter.right) / 2 - DlgWidth / 2;
	int yTop = (rcCenter.top + rcCenter.bottom) / 2 - DlgHeight / 2;

	// if the dialog is outside the screen, move it inside
	if(xLeft + DlgWidth > rcArea.right)
		xLeft = rcArea.right - DlgWidth;
	if(xLeft < rcArea.left)
		xLeft = rcArea.left;

	if(yTop + DlgHeight > rcArea.bottom)
		yTop = rcArea.bottom - DlgHeight;
	if(yTop < rcArea.top)
		yTop = rcArea.top;

	// map screen coordinates to child coordinates
	return ::SetWindowPos(m_hWnd, NULL, xLeft, yTop, -1, -1,
		SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

BOOL CPathEditorDlg::_CreateImageList()
{
	m_hImageList = ImageList_Create(16, 16, ILC_MASK | ILC_COLOR32, 2, 2);
	if( m_hImageList == NULL)
		return FALSE;

    HICON hIconExist = LoadIconW( m_hInstance, MAKEINTRESOURCE(IDI_ICON_EXIST));
    ImageList_ReplaceIcon( m_hImageList, -1, hIconExist);
    if( DeleteObject(hIconExist))
		return FALSE;

    HICON hIconNonExist = LoadIconW( m_hInstance, MAKEINTRESOURCE(IDI_ICON_NON_EXIST));
	ImageList_ReplaceIcon( m_hImageList, -1, hIconNonExist);
	if( DeleteObject(hIconNonExist))
		return FALSE;
	return TRUE;
}

BOOL CPathEditorDlg::_SetButtonIcons()
{
	HICON hAddIcon = (HICON)LoadImage( m_hInstance, MAKEINTRESOURCE(IDI_ICON_DIR_ADD), IMAGE_ICON, 16, 16, LR_SHARED);
	SendMessage( ::GetDlgItem( m_hWnd, IDC_BUTTON_USER_ADD), BM_SETIMAGE, IMAGE_ICON, (LPARAM)hAddIcon);
	SendMessage( ::GetDlgItem( m_hWnd, IDC_BUTTON_SYSTEM_ADD), BM_SETIMAGE, IMAGE_ICON, (LPARAM)hAddIcon);
	m_ButtonIcons.push_back(hAddIcon);

	HICON hDelIcon = (HICON)LoadImage( m_hInstance, MAKEINTRESOURCE(IDI_ICON_DIR_DEL), IMAGE_ICON, 16, 16, LR_SHARED);
	SendMessage( ::GetDlgItem( m_hWnd, IDC_BUTTON_USER_REMOVE), BM_SETIMAGE, IMAGE_ICON, (LPARAM)hDelIcon);
	SendMessage( ::GetDlgItem( m_hWnd, IDC_BUTTON_SYSTEM_REMOVE), BM_SETIMAGE, IMAGE_ICON, (LPARAM)hDelIcon);
	m_ButtonIcons.push_back(hDelIcon);

	HICON hEditIcon = (HICON)LoadImage( m_hInstance, MAKEINTRESOURCE(IDI_ICON_DIR_EDIT), IMAGE_ICON, 16, 16, LR_SHARED);
	SendMessage( ::GetDlgItem( m_hWnd, IDC_BUTTON_USER_EDIT), BM_SETIMAGE, IMAGE_ICON, (LPARAM)hEditIcon);
	SendMessage( ::GetDlgItem( m_hWnd, IDC_BUTTON_SYSTEM_EDIT), BM_SETIMAGE, IMAGE_ICON, (LPARAM)hEditIcon);
	m_ButtonIcons.push_back(hEditIcon);

	HICON hUpIcon = (HICON)LoadImage( m_hInstance, MAKEINTRESOURCE(IDI_ICON_DIR_UP), IMAGE_ICON, 16, 16, LR_SHARED);
	SendMessage( ::GetDlgItem( m_hWnd, IDC_BUTTON_USER_UP), BM_SETIMAGE, IMAGE_ICON, (LPARAM)hUpIcon);
	SendMessage( ::GetDlgItem( m_hWnd, IDC_BUTTON_SYSTEM_UP), BM_SETIMAGE, IMAGE_ICON, (LPARAM)hUpIcon);
	m_ButtonIcons.push_back(hUpIcon);

	HICON hDownIcon = (HICON)LoadImage( m_hInstance, MAKEINTRESOURCE(IDI_ICON_DIR_DOWN), IMAGE_ICON, 16, 16, LR_SHARED);
	SendMessage( ::GetDlgItem( m_hWnd, IDC_BUTTON_USER_DOWN), BM_SETIMAGE, IMAGE_ICON, (LPARAM)hDownIcon);
	SendMessage( ::GetDlgItem( m_hWnd, IDC_BUTTON_SYSTEM_DOWN), BM_SETIMAGE, IMAGE_ICON, (LPARAM)hDownIcon);
	m_ButtonIcons.push_back(hDownIcon);

	return TRUE;
}

BOOL CPathEditorDlg::OnInitDialog( HINSTANCE hInstance, HWND hWnd)
{
	// save interesting handles
	m_hWnd = hWnd;
	m_hInstance = hInstance;

	// set application icon
	m_hIcon = LoadIconW(hInstance, MAKEINTRESOURCE(IDR_MAINFRAME));
	SendMessage(m_hWnd, WM_SETICON, TRUE, (LPARAM)m_hIcon);
	SendMessage(m_hWnd, WM_SETICON, FALSE, (LPARAM)m_hIcon);

	if( _SetButtonIcons() == FALSE)
		return FALSE;
	if( _CreateImageList() == FALSE)
		return FALSE;
	if( _CenterWindow() == FALSE)
		return FALSE;

	m_bIsAdmin = GetAdminStatus(::GetCurrentProcess());
	m_usrListCtrl.Init( ::GetDlgItem(m_hWnd, IDC_LIST_USER), m_hImageList,
		HKEY_CURRENT_USER, L"Environment", L"Path");
    if(m_bIsAdmin.first)
	{
		::ShowWindow( ::GetDlgItem( m_hWnd, IDC_BUTTON_GAIN_PRIVILEGE), SW_HIDE);
        m_sysListCtrl.Init( ::GetDlgItem( m_hWnd, IDC_LIST_SYSTEM), m_hImageList,
			HKEY_LOCAL_MACHINE,
			L"SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment",
			L"Path");
	}
	else
	{
		::ShowWindow( ::GetDlgItem( m_hWnd, IDC_LIST_SYSTEM), SW_HIDE);
		::ShowWindow( ::GetDlgItem( m_hWnd, IDC_BUTTON_GAIN_PRIVILEGE), SW_SHOW);
		Button_SetElevationRequiredState( ::GetDlgItem( m_hWnd, IDC_BUTTON_GAIN_PRIVILEGE), TRUE);

		::EnableWindow( ::GetDlgItem( m_hWnd, IDC_BUTTON_SYSTEM_UP),     FALSE);
		::EnableWindow( ::GetDlgItem( m_hWnd, IDC_BUTTON_SYSTEM_DOWN),   FALSE);
		::EnableWindow( ::GetDlgItem( m_hWnd, IDC_BUTTON_SYSTEM_ADD),    FALSE);
		::EnableWindow( ::GetDlgItem( m_hWnd, IDC_BUTTON_SYSTEM_REMOVE), FALSE);
		::EnableWindow( ::GetDlgItem( m_hWnd, IDC_BUTTON_SYSTEM_EDIT),   FALSE);
	}
	return TRUE;
}

void CPathEditorDlg::OnButtonGainPrivilege()
{
	WCHAR szBuffer[MAX_PATH];
	std::fill_n( szBuffer, MAX_PATH, L'\0');
	if( 0 == GetModuleFileName( 0, szBuffer, MAX_PATH))
		return;

	SHELLEXECUTEINFO exInfo = { 0 };
	exInfo.cbSize = sizeof(exInfo);
	exInfo.lpVerb = L"runas";
	exInfo.lpFile = szBuffer;
	exInfo.nShow = SW_SHOW;
	if( TRUE == ShellExecuteEx( &exInfo))
		SendMessage( m_hWnd, WM_CLOSE, 0, 0);
}

BOOL CPathEditorDlg::OnCommand( UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
	case IDC_BUTTON_USER_UP:
		m_usrListCtrl.MoveUp();
		break;
	case IDC_BUTTON_USER_DOWN:
		m_usrListCtrl.MoveDown();
		break;
	case IDC_BUTTON_USER_ADD:
		m_usrListCtrl.AddPath();
		break;
	case IDC_BUTTON_USER_REMOVE:
		m_usrListCtrl.RemovePath();
		break;
	case IDC_BUTTON_USER_EDIT:
		m_usrListCtrl.EditPath();
		break;
	case IDC_BUTTON_SYSTEM_UP:
		m_sysListCtrl.MoveUp();
		break;
	case IDC_BUTTON_SYSTEM_DOWN:
		m_sysListCtrl.MoveDown();
		break;
	case IDC_BUTTON_SYSTEM_ADD:
		m_sysListCtrl.AddPath();
		break;
	case IDC_BUTTON_SYSTEM_REMOVE:
		m_sysListCtrl.RemovePath();
		break;
	case IDC_BUTTON_SYSTEM_EDIT:
		m_sysListCtrl.EditPath();
		break;
	case IDC_BUTTON_GAIN_PRIVILEGE:
		OnButtonGainPrivilege();
		break;
	case IDOK:
		OnOK();
	case IDCANCEL:
		SendMessage( m_hWnd, WM_CLOSE, 0, 0);
		return TRUE;
	}
	return FALSE;
}

BOOL CPathEditorDlg::OnNotify( LPNMHDR lpNMHDR)
{
	switch(lpNMHDR->code)
	{
	case NM_DBLCLK:
		OnListDoubleClick(reinterpret_cast<LPNMITEMACTIVATE>(lpNMHDR));
		break;
	case LVN_GETDISPINFO:
		OnListGetDispInfo(reinterpret_cast<NMLVDISPINFO*>(lpNMHDR));
		break;
	}
	return TRUE;
}

void CPathEditorDlg::OnListGetDispInfo(NMLVDISPINFO *pDispInfo)
{
	switch(pDispInfo->hdr.idFrom)
	{
	case IDC_LIST_USER:
		m_usrListCtrl.OnGetdispinfo(pDispInfo);
		break;
	case IDC_LIST_SYSTEM:
		m_sysListCtrl.OnGetdispinfo(pDispInfo);
		break;
	}
}

void CPathEditorDlg::OnListDoubleClick(LPNMITEMACTIVATE lpNMItemActivate)
{
	switch(lpNMItemActivate->hdr.idFrom)
	{
	case IDC_LIST_USER:
		m_usrListCtrl.OnDoubleClick(lpNMItemActivate);
		break;
	case IDC_LIST_SYSTEM:
		m_sysListCtrl.OnDoubleClick(lpNMItemActivate);
		break;
	}
}

BOOL CPathEditorDlg::OnOK()
{
	if( !m_usrListCtrl.Commit())
	{
		MessageBox( m_hWnd, L"Failed to save User PATH", L"Path Editor", MB_OK);
		return FALSE;
	}

	if( m_bIsAdmin.first && !m_sysListCtrl.Commit())
	{
		MessageBox( m_hWnd, L"Failed to save System PATH", L"Path Editor", MB_OK);
		return FALSE;
	}

	// broadcast path change messages to interested parties
	DWORD_PTR dwResult = 0;
	SendMessageTimeout( HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)L"Environment", SMTO_ABORTIFHUNG, 1, &dwResult);
	return TRUE;
}
