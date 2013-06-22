
// PathEditorDlg.h : header file
//

#pragma once
#include "afxdialogex.h"
#include "PathReader.h"
#include "PathListCtrl.h"


// CPathEditorDlg dialog
class CPathEditorDlg : public CDialogEx
{
// Construction
public:
    CPathEditorDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
    enum { IDD = IDD_PATHEDITOR_DIALOG };

    protected:
    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
    HICON m_hIcon;

    // Generated message map functions
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()

    CImageList* m_pImageList;
	std::pair<bool, bool> m_bIsAdmin;
public:
    CPathListCtrl m_usrListCtrl;
    CPathListCtrl m_sysListCtrl;

    virtual void OnOK();
    afx_msg void OnBnClickedButtonUserAdd();
    afx_msg void OnBnClickedButtonUserRemove();
    afx_msg void OnBnClickedButtonSystemAdd();
    afx_msg void OnBnClickedButtonSystemRemove();
    afx_msg void OnBnClickedButtonUserEdit();
    afx_msg void OnBnClickedButtonSystemEdit();
    afx_msg void OnGetdispinfoListUser(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnGetdispinfoListSystem(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedButtonUserUp();
    afx_msg void OnBnClickedButtonUserDown();
    afx_msg void OnBnClickedButtonSystemUp();
    afx_msg void OnBnClickedButtonSystemDown();
    afx_msg void OnNMDblclkListUser(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMDblclkListSystem(NMHDR *pNMHDR, LRESULT *pResult);
};
