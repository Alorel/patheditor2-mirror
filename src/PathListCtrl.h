#include <windows.h>
#include <commctrl.h>

#include "PathReader.h"
#include <vector>
#include <string>

typedef std::vector<std::wstring> StringListT;

class CPathListCtrl
{
private:
	HWND m_hWnd;

	StringListT m_str_list;
	CPathReader m_reader;

private:
	void _ExpandEnvironmentStrings( wchar_t* pszDst, int nLen, const std::wstring& sVar);
	int _GetImageIndex( std::wstring fname);

public:
	void Init( HWND hWnd, HIMAGELIST hImageList, HKEY hKey, LPCTSTR lpszKeyName, LPCTSTR lpszValueName);
	bool Commit();

	void AddPath();
	void EditPath();
	void RemovePath();

	void MoveUp();
	void MoveDown();

	void OnDoubleClick(LPNMITEMACTIVATE lpNMItemActivate);
	void OnGetdispinfo(NMLVDISPINFO *pDispInfo);
};
