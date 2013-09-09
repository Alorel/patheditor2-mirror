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

#include "PathReader.h"
#include "Util.h"

#include <memory>
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
	std::shared_ptr<void> afPathKey( hPathKey, RegCloseKey);

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
	std::shared_ptr<void> afPathKey( hPathKey, RegCloseKey);

	std::wstring strValue = join_vector_by( strList, L';');
	const BYTE* lpcBuffer = reinterpret_cast<const BYTE*>( strValue.c_str());
	DWORD cbData = static_cast<DWORD>( strValue.size() * sizeof(wchar_t));
	LSTATUS lStatus = RegSetValueEx( hPathKey, lpszValueName_, 0, REG_EXPAND_SZ, lpcBuffer, cbData);
	return lStatus == ERROR_SUCCESS;
}
