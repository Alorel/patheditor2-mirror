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

#include "PathListCtrl.h"
#include <shlobj.h>
#include <algorithm>
#include <iterator>
#include "resource.h"

int CALLBACK BrowseCallbackProc( HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
    if( uMsg == BFFM_INITIALIZED)
        SendMessage( hWnd, BFFM_SETSELECTION, TRUE, lpData);
    return 0;
}

std::wstring CPathListCtrl::_ExpandEnvironmentStrings(const std::wstring& sVar)
{
	if (std::wstring::npos == sVar.find(L'%'))
		return sVar;

	DWORD dwLen = 0;
	dwLen = ExpandEnvironmentStrings(sVar.c_str(), nullptr, dwLen);
	if (dwLen == 0)
		return L"";

	std::wstring strValue(dwLen, 0);
	dwLen = ExpandEnvironmentStrings(sVar.c_str(), &strValue[0], dwLen);
	if (dwLen == 0)
		return L"";

	strValue.resize(strValue.find_first_of(L'\0'));
	return strValue;
}

int CPathListCtrl::_GetImageIndex( std::wstring fname)
{
    std::wstring pathName = _ExpandEnvironmentStrings(fname);
    return GetFileAttributes(pathName.c_str()) == INVALID_FILE_ATTRIBUTES ? 1 : 0;
}

void CPathListCtrl::Init( HWND hWnd, HIMAGELIST hImageList, HKEY hKey, LPCTSTR lpszKeyName, LPCTSTR lpszValueName)
{
    m_hWnd = hWnd;
    ListView_SetImageList( m_hWnd, hImageList, LVSIL_SMALL);

    RECT reg;
    ::GetWindowRect( ListView_GetHeader( m_hWnd), &reg);

    LVCOLUMN lvColumn = { 0 };
    lvColumn.mask = LVCF_WIDTH;
    lvColumn.cx = reg.right - reg.left;
    ListView_InsertColumn( m_hWnd, 0, &lvColumn);

    DWORD dwStyle = LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
    ListView_SetExtendedListViewStyle( m_hWnd, dwStyle);

    // load data
    m_reader = CPathReader( hKey, lpszKeyName, lpszValueName);
    m_reader.Read( m_str_list);
    for( std::size_t count = 0; count < m_str_list.size(); ++count)
    {
        LVITEM lvItem = { 0 };
        lvItem.mask = LVIF_TEXT | LVIF_STATE | LVIF_IMAGE;
        lvItem.iItem = static_cast<int>(count);
        lvItem.iImage = I_IMAGECALLBACK;
        lvItem.pszText = LPSTR_TEXTCALLBACK;
        ListView_InsertItem( m_hWnd, &lvItem);
    }
}

bool CPathListCtrl::Commit()
{
    StringListT strList;
    for( int i = 0; i < ListView_GetItemCount( m_hWnd); ++i)
    {
        int cchTextMax = MAX_PATH;
        std::wstring strValue(cchTextMax, 0);
        ListView_GetItemText(m_hWnd, i, 0, &strValue[0], cchTextMax);

        strValue.resize(strValue.find_first_of(L'\0'));
        strList.push_back(strValue);
    }
    return m_reader.Write( strList);
}

void CPathListCtrl::AddPath()
{
    BROWSEINFO bi = { 0 };
    bi.hwndOwner = GetParent(m_hWnd);
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_EDITBOX | BIF_USENEWUI | BIF_NEWDIALOGSTYLE;
    PIDLIST_ABSOLUTE strList = SHBrowseForFolder( &bi);
    if( strList)
    {
        std::wstring strPath(MAX_PATH, 0);
        SHGetPathFromIDList(strList, &strPath[0]);
        strPath.resize(strPath.find_first_of(L'\0'));
        m_str_list.push_back(strPath);

        LVITEM lvItem = { 0 };
        lvItem.mask = LVIF_TEXT | LVIF_STATE;
        lvItem.iItem = ListView_GetItemCount( m_hWnd);
        lvItem.pszText = LPSTR_TEXTCALLBACK;
        ListView_InsertItem( m_hWnd, &lvItem);
    }
}

void CPathListCtrl::EditPath()
{
    int iItem = ListView_GetNextItem( m_hWnd, -1, LVNI_SELECTED);
    if( iItem == -1)
        return;

    BROWSEINFO bi = { 0 };
    bi.hwndOwner = GetParent(m_hWnd);
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_EDITBOX | BIF_USENEWUI | BIF_NEWDIALOGSTYLE;
    bi.lpfn = BrowseCallbackProc;

    std::wstring pathName = _ExpandEnvironmentStrings(m_str_list[iItem]);
    bi.lParam = reinterpret_cast<LPARAM>(pathName.c_str());

    PIDLIST_ABSOLUTE strList = SHBrowseForFolder( &bi);
    if( strList)
    {
        std::wstring pathName(MAX_PATH, 0);
        SHGetPathFromIDList(strList, &pathName[0]);
        pathName.resize(pathName.find_first_of(L'\0'));

        m_str_list[iItem] = pathName;
        ListView_Update( m_hWnd, iItem);
    }
}

void CPathListCtrl::OnDoubleClick( LPNMITEMACTIVATE lpNMItemActivate)
{
    int iItem = lpNMItemActivate->iItem;
    if( iItem == -1)
        return;

    std::wstring pathName = _ExpandEnvironmentStrings(m_str_list[iItem]);
    if (GetFileAttributes(pathName.c_str()) == INVALID_FILE_ATTRIBUTES)
        return;
    ShellExecute(0, L"open", pathName.c_str(), 0, 0, SW_NORMAL);
}

void CPathListCtrl::RemovePath()
{
    int iItem = ListView_GetNextItem( m_hWnd, -1, LVNI_SELECTED);
    if( iItem == -1)
        return;

    m_str_list.erase( std::find( m_str_list.begin(), m_str_list.end(), m_str_list[iItem]));
    ListView_DeleteItem( m_hWnd, iItem);
    ListView_Update( m_hWnd, iItem);

    if( iItem == m_str_list.size())
        iItem = iItem - 1;
    ListView_SetItemState( m_hWnd, iItem, LVNI_SELECTED, LVNI_SELECTED);
}

void CPathListCtrl::OnGetdispinfo( NMLVDISPINFO *pDispInfo)
{
    if( pDispInfo->item.mask & LVIF_TEXT)
    {
        if( pDispInfo->item.iSubItem == 0)
        {
            std::wstring& sItem = m_str_list[pDispInfo->item.iItem];
            pDispInfo->item.pszText = &sItem[0];
        }
    }

    if( pDispInfo->item.mask & LVIF_IMAGE)
        pDispInfo->item.iImage = _GetImageIndex( m_str_list[pDispInfo->item.iItem]);
}

void CPathListCtrl::MoveUp()
{
    int iItem = ListView_GetNextItem( m_hWnd, -1, LVNI_SELECTED);
    if( iItem == -1 || iItem == 0)
        return;

    m_str_list[iItem].swap( m_str_list[iItem - 1]);
    ListView_Update( m_hWnd, iItem);
    ListView_Update( m_hWnd, iItem - 1);
    ListView_SetItemState( m_hWnd, iItem - 1, LVNI_SELECTED, LVNI_SELECTED);
    ListView_EnsureVisible( m_hWnd, iItem - 1, FALSE);
}

void CPathListCtrl::MoveDown()
{
    int iItem = ListView_GetNextItem( m_hWnd, -1, LVNI_SELECTED);
    if( iItem == -1 || ( iItem == ( m_str_list.size() - 1)))
        return;

    m_str_list[iItem].swap( m_str_list[iItem + 1]);
    ListView_Update( m_hWnd, iItem);
    ListView_Update( m_hWnd, iItem + 1);
    ListView_SetItemState( m_hWnd, iItem + 1, LVNI_SELECTED, LVNI_SELECTED);
    ListView_EnsureVisible( m_hWnd, iItem + 1, FALSE);
}
