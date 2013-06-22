#pragma once

#include <string>
#include <vector>

typedef std::vector<std::wstring> StringListT;

class CPathReader
{
private:
    HKEY hKey_;
    LPCTSTR lpszKeyName_, lpszValueName_;

public:
    bool Read( StringListT& strList);
    bool Write( const StringListT& strList);

    CPathReader(void);
    CPathReader( HKEY hKey, LPCTSTR lpszKeyName, LPCTSTR lpszValueName);
    ~CPathReader(void);
};
