// PathListCtrl.cpp : implementation file
//

#include "stdafx.h"

#include "PathEditor.h"
#include "PathListCtrl.h"

#include <algorithm>
#include <iterator>

// CPathListCtrl

IMPLEMENT_DYNAMIC(CPathListCtrl, CListCtrl)

int CALLBACK BrowseCallbackProc( HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
    if( uMsg == BFFM_INITIALIZED)
        SendMessage( hwnd, BFFM_SETSELECTION, TRUE, lpData);
    return 0;
}

BEGIN_MESSAGE_MAP(CPathListCtrl, CListCtrl)
END_MESSAGE_MAP()

// CPathListCtrl message handlers

void CPathListCtrl::_ExpandEnvironmentStrings( wchar_t* pszDst, int nLen, const std::wstring& sVar)
{
	if( std::wstring::npos != sVar.find( _T('%')))
	{
		ExpandEnvironmentStrings( sVar.c_str(), pszDst, nLen);
		return;
	}

	stdext::checked_array_iterator<wchar_t*> iter(pszDst, nLen);
	std::size_t nMaxCopy = nLen;
	if( nMaxCopy >= sVar.size())
		nMaxCopy = sVar.size();
	std::copy_n( sVar.begin(), nMaxCopy, iter);
}

int CPathListCtrl::_GetImageIndex( std::wstring fname)
{
    int imgIndex = 0;
	WCHAR szDst[MAX_PATH] = { 0 };
	_ExpandEnvironmentStrings( szDst, MAX_PATH, fname);
    return GetFileAttributes( szDst) == INVALID_FILE_ATTRIBUTES ? 1 : 0;
}

void CPathListCtrl::Init( HKEY hKey, LPCTSTR lpszKeyName, LPCTSTR lpszValueName)
{
    m_reader = CPathReader( hKey, lpszKeyName, lpszValueName);
	m_reader.Read( m_str_list);

    CRect reg;
    GetHeaderCtrl()->GetWindowRect( reg);

    LVCOLUMN lvColumn = { 0 };
    lvColumn.mask = LVCF_WIDTH;
    lvColumn.cx = reg.Width();
    InsertColumn( 0, &lvColumn);

    SetExtendedStyle( LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

    for( std::size_t count = 0; count < m_str_list.size(); ++count)
    {
        LVITEM lvItem = { 0 };
        lvItem.mask = LVIF_TEXT | LVIF_STATE | LVIF_IMAGE;
        lvItem.iItem = count;
        lvItem.iImage = I_IMAGECALLBACK;
        lvItem.pszText = LPSTR_TEXTCALLBACK;
        InsertItem( &lvItem);
    }
}

bool CPathListCtrl::Commit()
{
    StringListT strList;
    for( int i = 0; i < GetItemCount(); ++i)
        strList.push_back( GetItemText( i, 0).GetString());
    return m_reader.Write( strList);
}

void CPathListCtrl::AddPath()
{
    BROWSEINFO bi = { 0 };
    bi.hwndOwner = GetParent()->GetSafeHwnd();
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_EDITBOX | BIF_USENEWUI | BIF_NEWDIALOGSTYLE;
    PIDLIST_ABSOLUTE strList = SHBrowseForFolder( &bi);
    if( strList)
    {
		TCHAR szPath[MAX_PATH] = { 0 };
        SHGetPathFromIDList( strList, szPath);
        m_str_list.push_back( szPath);

        LVITEM lvItem = { 0 };
        lvItem.mask = LVIF_TEXT | LVIF_STATE;
        lvItem.iItem = GetItemCount();
        lvItem.pszText = LPSTR_TEXTCALLBACK;
        InsertItem( &lvItem);
    }
}

void CPathListCtrl::EditPath()
{
    int iItem = GetNextItem( -1, LVNI_SELECTED);
    if( iItem == -1)
        return;

    BROWSEINFO bi = { 0 };
    bi.hwndOwner = GetParent()->GetSafeHwnd();
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_EDITBOX | BIF_USENEWUI | BIF_NEWDIALOGSTYLE;
    bi.lpfn = BrowseCallbackProc;

	WCHAR szDst[MAX_PATH] = { 0 };
	_ExpandEnvironmentStrings( szDst, MAX_PATH, m_str_list[iItem]);
    bi.lParam = reinterpret_cast<LPARAM>( szDst);

    PIDLIST_ABSOLUTE strList = SHBrowseForFolder( &bi);
    if( strList)
    {
        TCHAR szPath[MAX_PATH];
        SHGetPathFromIDList( strList, szPath);
        m_str_list[iItem] = szPath;
        Update( iItem);
    }
}

void CPathListCtrl::OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    int iItem = pNMItemActivate->iItem;

    if( iItem == -1)
        return;

	WCHAR szDst[MAX_PATH] = { 0 };
	_ExpandEnvironmentStrings( szDst, MAX_PATH, m_str_list[iItem]);
    ShellExecute(0, _T("open"), _T("explorer.exe"), szDst, 0, SW_NORMAL);
    *pResult = 0;
}

void CPathListCtrl::RemovePath()
{
    int iItem = GetNextItem( -1, LVNI_SELECTED);
    if( iItem == -1)
        return;

    m_str_list.erase( std::find( m_str_list.begin(), m_str_list.end(), m_str_list[iItem]));
    DeleteItem( iItem);
    Update( iItem);

    if( iItem == m_str_list.size())
        iItem = iItem - 1;
    SetItemState( iItem, LVNI_SELECTED, LVNI_SELECTED);
}

void CPathListCtrl::OnGetdispinfo(NMHDR *pNMHDR, LRESULT *pResult)
{
    NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
    if( pDispInfo->item.mask & LVIF_TEXT)
	{
        if( pDispInfo->item.iSubItem == 0)
		{
			std::wstring& sItem = m_str_list[pDispInfo->item.iItem];
			stdext::checked_array_iterator<wchar_t*> iter(pDispInfo->item.pszText, pDispInfo->item.cchTextMax);
			std::size_t nLen = pDispInfo->item.cchTextMax;

			if(nLen >= sItem.size())
				nLen = sItem.size();
			auto iterLast = std::copy_n( sItem.begin(), nLen, iter);
			*iterLast = L'\0';
		}
	}

    if( pDispInfo->item.mask & LVIF_IMAGE)
        pDispInfo->item.iImage = _GetImageIndex( m_str_list[pDispInfo->item.iItem]);

    *pResult = 0;
}

void CPathListCtrl::MoveUp()
{
    int iItem = GetNextItem( -1, LVNI_SELECTED);
    if( iItem == -1 || iItem == 0)
        return;

    m_str_list[iItem].swap( m_str_list[iItem - 1]);
    Update( iItem);
    Update( iItem - 1);
    SetItemState( iItem - 1, LVNI_SELECTED, LVNI_SELECTED);
    EnsureVisible( iItem - 1, FALSE);
}

void CPathListCtrl::MoveDown()
{
    int iItem = GetNextItem( -1, LVNI_SELECTED);
    if( iItem == -1 || ( iItem == ( m_str_list.size() - 1)))
        return;

    m_str_list[iItem].swap( m_str_list[iItem + 1]);
    Update( iItem);
    Update( iItem + 1);
    SetItemState( iItem + 1, LVNI_SELECTED, LVNI_SELECTED);
    EnsureVisible( iItem + 1, FALSE);
}

//void CPathListCtrl::BeginEditing()
//{
//    int iItem = GetNextItem( -1, LVNI_SELECTED);
//    if( iItem == -1)
//        return;
//
//    CRect reg;
//    GetItemRect( iItem, reg, LVIR_LABEL);
//    ClientToScreen( reg);
//    GetParent()->ScreenToClient( reg);
//
//    const int BROWSE_BUTTON_WIDTH = 25;
//    CRect rect = reg;
//    rect.left  = reg.right - BROWSE_BUTTON_WIDTH;
//    rect.right = rect.left + BROWSE_BUTTON_WIDTH;
//
//    m_pButton = new CButton();
//    BOOL retVal = m_pButton->Create( _T("..."), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, rect, GetParent(), IDC_BUTTON_BROWSE);
//
//    CRect erect = reg;
//    erect.right = reg.right - BROWSE_BUTTON_WIDTH;
//    m_pEdit = new CEdit();
//    retVal = m_pEdit->Create( WS_CHILD | WS_VISIBLE | WS_BORDER, erect, GetParent(), IDC_EDIT_DIR);
//
//    m_pEdit->SetFont( GetFont());
//    m_pEdit->SetFocus();
//    m_pEdit->SetWindowText( m_str_list[iItem].c_str());
//    m_pEdit->SetSel( 0, -1);
//
//    SHAutoComplete( m_pEdit->GetSafeHwnd(), SHACF_FILESYS_DIRS);
//}
