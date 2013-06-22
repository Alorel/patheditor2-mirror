#include "StdAfx.h"
#include "PathReader.h"

#include <sstream>
#include <regex>
#include <iterator>
#include <algorithm>

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

CPathReader::CPathReader(void)
{
}

CPathReader::CPathReader( HKEY hKey, LPCTSTR lpszKeyName, LPCTSTR lpszValueName) : hKey_( hKey), lpszKeyName_( lpszKeyName), lpszValueName_( lpszValueName)
{
}

CPathReader::~CPathReader(void)
{
}

bool CPathReader::Read( StringListT& strList)
{
	CRegKey read;
	if( read.Open( hKey_, lpszKeyName_) == ERROR_SUCCESS)
	{
		ULONG nChars = 10 * MAX_PATH;
		TCHAR buffer[10 * MAX_PATH] = _T("");
		if( read.QueryStringValue( lpszValueName_, buffer, &nChars) == ERROR_SUCCESS)
            strList = split_string_by( StringListT::value_type( buffer), _T(';'));
		return true;
	}
	return false;
}

bool CPathReader::Write( const StringListT& strList)
{
	if( strList.empty())
		return false;

	CRegKey writer;
	if( writer.Open( hKey_, lpszKeyName_) == ERROR_SUCCESS)
	{
		std::wstring strValue = join_vector_by( strList, L';');
		writer.SetStringValue( lpszValueName_, strValue.c_str(), REG_EXPAND_SZ);
		return true;
	}
	return false;
}
