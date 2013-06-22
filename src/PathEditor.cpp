
// PathEditor.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "PathEditor.h"
#include "PathEditorDlg.h"
#include "afxshellmanager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPathEditorApp

BEGIN_MESSAGE_MAP(CPathEditorApp, CWinApp)
    ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CPathEditorApp construction

CPathEditorApp::CPathEditorApp()
{
}


// The one and only CPathEditorApp object

CPathEditorApp theApp;


// CPathEditorApp initialization

BOOL CPathEditorApp::InitInstance()
{
    CWinApp::InitInstance();

    // Required for SHAutoComplete
    CoInitialize(0);

    // Create the shell manager, in case the dialog contains
    // any shell tree view or shell list view controls.
    CShellManager *pShellManager = new CShellManager;

    CPathEditorDlg dlg;
    m_pMainWnd = &dlg;
    INT_PTR nResponse = dlg.DoModal();
    if (nResponse == IDOK)
    {
    }
    else if (nResponse == IDCANCEL)
    {
    }

    // Delete the shell manager created above.
    if (pShellManager != NULL)
    {
        delete pShellManager;
    }

    CoUninitialize();

    // Since the dialog has been closed, return FALSE so that we exit the
    //  application, rather than start the application's message pump.
    return FALSE;
}
