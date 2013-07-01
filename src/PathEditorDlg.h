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

	std::pair<bool, bool> m_bIsAdmin;
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

	void OnListGetDispInfo(NMLVDISPINFO *pDispInfo);
	void OnListDoubleClick(LPNMITEMACTIVATE lpNMItemActivate);
};
