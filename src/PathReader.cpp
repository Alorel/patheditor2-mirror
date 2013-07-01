#include "PathReader.h"
#include "Util.h"

#include <sstream>
#include <regex>

StringListT split_string_by( const std::wstring& sInput, wchar_t wcSeparator)
{
    // tokenize using RegExp
    StringListT strList;
    std::wostringstream ss;
    ss << L"[^" << wcSeparator << L"]+";
    std::wregex reSeparator( ss.str());

    std::wsregex_token_iterator begin( sInput.begin(), sInput.end(), reSeparator);
    std::wsregex_token_iterator end;
    std::copy( begin, end, std::back_inserter( strList));
    return strList;
}

std::wstring join_vector_by( const StringListT& strList, wchar_t wcSeparator)
{
    std::wstring sSeparator( 1, wcSeparator);
    std::wostringstream ss;
    std::copy( strList.begin(), strList.end(), std::ostream_iterator<StringListT::value_type, StringListT::value_type::value_type>( ss, sSeparator.c_str()));
    return ss.str();
}

CPathReader::CPathReader()
{
}

CPathReader::CPathReader( HKEY hKey, LPCTSTR lpszKeyName, LPCTSTR lpszValueName) : hKey_( hKey), lpszKeyName_( lpszKeyName), lpszValueName_( lpszValueName)
{
}

CPathReader::~CPathReader()
{
}

bool CPathReader::Read( StringListT& strList)
{
	HKEY hPathKey = 0;
	if( RegOpenKeyEx( hKey_, lpszKeyName_, 0, KEY_QUERY_VALUE, &hPathKey) != ERROR_SUCCESS)
		return false;
	CAutoFree<HKEY,LSTATUS> afPathKey(hPathKey, RegCloseKey);

	ULONG nChars = 0;
	if( RegQueryValueEx( hPathKey, lpszValueName_, 0, 0, 0, &nChars) != ERROR_SUCCESS)
		return false;

	// empty path is not failure
	if(nChars == 0)
		return true;

	std::vector<BYTE> vBuffer(nChars + sizeof(wchar_t), 0);
	LPBYTE lpBuffer = vBuffer.data();
	if( RegQueryValueEx( hPathKey, lpszValueName_, 0, 0, lpBuffer, &nChars) != ERROR_SUCCESS)
		return false;

	std::wstring sBuffer(reinterpret_cast<wchar_t*>(lpBuffer));
	strList = split_string_by( sBuffer, L';');
	return true;
}

bool CPathReader::Write( const StringListT& strList)
{
	HKEY hPathKey = 0;
	if( RegOpenKeyEx( hKey_, lpszKeyName_, 0, KEY_SET_VALUE, &hPathKey) != ERROR_SUCCESS)
		return false;
	CAutoFree<HKEY,LSTATUS> afPathKey(hPathKey, RegCloseKey);

	std::wstring strValue = join_vector_by( strList, L';');
	const BYTE* lpcBuffer = reinterpret_cast<const BYTE*>( strValue.c_str());
	DWORD cbData = static_cast<DWORD>( strValue.size() * sizeof(wchar_t));
	LSTATUS lStatus = RegSetValueEx( hPathKey, lpszValueName_, 0, REG_EXPAND_SZ, lpcBuffer, cbData);
	return lStatus == ERROR_SUCCESS;
}
