
// PathEditorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PathEditor.h"
#include "PathEditorDlg.h"
#include "afxdialogex.h"

#include "Util.h"

#include <iterator>
#include <sstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CPathEditorDlg dialog

CPathEditorDlg::CPathEditorDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(CPathEditorDlg::IDD, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPathEditorDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_USER, m_usrListCtrl);
    DDX_Control(pDX, IDC_LIST_SYSTEM, m_sysListCtrl);
}

BEGIN_MESSAGE_MAP(CPathEditorDlg, CDialogEx)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BUTTON_USER_ADD, &CPathEditorDlg::OnBnClickedButtonUserAdd)
    ON_BN_CLICKED(IDC_BUTTON_USER_REMOVE, &CPathEditorDlg::OnBnClickedButtonUserRemove)
    ON_BN_CLICKED(IDC_BUTTON_SYSTEM_ADD, &CPathEditorDlg::OnBnClickedButtonSystemAdd)
    ON_BN_CLICKED(IDC_BUTTON_SYSTEM_REMOVE, &CPathEditorDlg::OnBnClickedButtonSystemRemove)
    ON_BN_CLICKED(IDC_BUTTON_USER_EDIT, &CPathEditorDlg::OnBnClickedButtonUserEdit)
    ON_BN_CLICKED(IDC_BUTTON_SYSTEM_EDIT, &CPathEditorDlg::OnBnClickedButtonSystemEdit)
    ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_USER, &CPathEditorDlg::OnGetdispinfoListUser)
    ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_SYSTEM, &CPathEditorDlg::OnGetdispinfoListSystem)
    ON_BN_CLICKED(IDC_BUTTON_USER_UP, &CPathEditorDlg::OnBnClickedButtonUserUp)
    ON_BN_CLICKED(IDC_BUTTON_USER_DOWN, &CPathEditorDlg::OnBnClickedButtonUserDown)
    ON_BN_CLICKED(IDC_BUTTON_SYSTEM_UP, &CPathEditorDlg::OnBnClickedButtonSystemUp)
    ON_BN_CLICKED(IDC_BUTTON_SYSTEM_DOWN, &CPathEditorDlg::OnBnClickedButtonSystemDown)
    ON_NOTIFY(NM_DBLCLK, IDC_LIST_USER, &CPathEditorDlg::OnNMDblclkListUser)
    ON_NOTIFY(NM_DBLCLK, IDC_LIST_SYSTEM, &CPathEditorDlg::OnNMDblclkListSystem)
END_MESSAGE_MAP()

// CPathEditorDlg message handlers

BOOL CPathEditorDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);			// Set big icon
    SetIcon(m_hIcon, FALSE);		// Set small icon

    m_bIsAdmin = GetAdminStatus(::GetCurrentProcess());
    m_usrListCtrl.Init( HKEY_CURRENT_USER, _T("Environment"), _T("Path"));
    if(m_bIsAdmin.first)
        m_sysListCtrl.Init( HKEY_LOCAL_MACHINE, _T("SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment"), _T("Path"));

    m_pImageList = new CImageList();
    m_pImageList->Create( 16, 16, ILC_MASK | ILC_COLOR32, 0, 2);
    m_pImageList->Add( AfxGetApp()->LoadIcon( IDI_ICON_EXIST));
    m_pImageList->Add( AfxGetApp()->LoadIcon( IDI_ICON_NON_EXIST));

    m_usrListCtrl.SetImageList( m_pImageList, LVSIL_SMALL);
    if(m_bIsAdmin.first)
    {
        m_sysListCtrl.SetImageList( m_pImageList, LVSIL_SMALL);
    }
    else
    {
        GetDlgItem(IDC_BUTTON_SYSTEM_UP)->EnableWindow(FALSE);
        GetDlgItem(IDC_BUTTON_SYSTEM_DOWN)->EnableWindow(FALSE);
        GetDlgItem(IDC_BUTTON_SYSTEM_ADD)->EnableWindow(FALSE);
        GetDlgItem(IDC_BUTTON_SYSTEM_REMOVE)->EnableWindow(FALSE);
        GetDlgItem(IDC_BUTTON_SYSTEM_EDIT)->EnableWindow(FALSE);
    }

    return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPathEditorDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this); // device context for painting

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // Draw the icon
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialogEx::OnPaint();
    }
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPathEditorDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CPathEditorDlg::OnOK()
{
    if( !m_usrListCtrl.Commit())
    {
        MessageBox( L"Failed to save User PATH");
        return;
    }

    if( m_bIsAdmin.first && !m_sysListCtrl.Commit())
    {
        MessageBox( L"Failed to save System PATH");
        return;
    }

    // broadcast path change messages to interested parties
    DWORD_PTR dwResult = 0;
    SendMessageTimeout( HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)_T("Environment"), SMTO_ABORTIFHUNG, 1, &dwResult);

    m_pImageList->DeleteImageList();
    delete m_pImageList;

    CDialogEx::OnOK();
}

void CPathEditorDlg::OnBnClickedButtonUserAdd()
{
    m_usrListCtrl.AddPath();
}

void CPathEditorDlg::OnBnClickedButtonUserEdit()
{
    m_usrListCtrl.EditPath();
}

void CPathEditorDlg::OnBnClickedButtonUserRemove()
{
    m_usrListCtrl.RemovePath();
}

void CPathEditorDlg::OnBnClickedButtonSystemAdd()
{
    m_sysListCtrl.AddPath();
}

void CPathEditorDlg::OnBnClickedButtonSystemEdit()
{
    m_sysListCtrl.EditPath();
}

void CPathEditorDlg::OnBnClickedButtonSystemRemove()
{
    m_sysListCtrl.RemovePath();
}

void CPathEditorDlg::OnGetdispinfoListUser(NMHDR *pNMHDR, LRESULT *pResult)
{
    m_usrListCtrl.OnGetdispinfo( pNMHDR, pResult);
}

void CPathEditorDlg::OnGetdispinfoListSystem(NMHDR *pNMHDR, LRESULT *pResult)
{
    m_sysListCtrl.OnGetdispinfo( pNMHDR, pResult);
}

void CPathEditorDlg::OnBnClickedButtonUserUp()
{
    m_usrListCtrl.MoveUp();
}

void CPathEditorDlg::OnBnClickedButtonUserDown()
{
    m_usrListCtrl.MoveDown();
}

void CPathEditorDlg::OnBnClickedButtonSystemUp()
{
    m_sysListCtrl.MoveUp();
}

void CPathEditorDlg::OnBnClickedButtonSystemDown()
{
    m_sysListCtrl.MoveDown();
}

void CPathEditorDlg::OnNMDblclkListUser(NMHDR *pNMHDR, LRESULT *pResult)
{
    m_usrListCtrl.OnNMDblclkList(pNMHDR, pResult);
}

void CPathEditorDlg::OnNMDblclkListSystem(NMHDR *pNMHDR, LRESULT *pResult)
{
    m_sysListCtrl.OnNMDblclkList(pNMHDR, pResult);
}
