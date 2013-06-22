#pragma once

#include "PathReader.h"

// CPathListCtrl

class CPathListCtrl : public CListCtrl
{
    DECLARE_DYNAMIC(CPathListCtrl)

    CPathReader m_reader;
    StringListT m_str_list;
    // CEdit* m_pEdit;
    // CButton* m_pButton;

protected:
    DECLARE_MESSAGE_MAP()

private:
	void _ExpandEnvironmentStrings( wchar_t* pszDst, int nLen, const std::wstring& sVar);
    int _GetImageIndex( std::wstring fname);

public:
    afx_msg void OnGetdispinfo(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBeginlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult);

    void Init( HKEY hKey, LPCTSTR lpszKeyName, LPCTSTR lpszValueName);
    bool Commit();
    void AddPath();
    void EditPath();
    void RemovePath();

    void MoveUp();
    void MoveDown();
};
