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

#include "Util.h"
#include <memory>

std::pair<bool, bool> GetAdminStatus( HANDLE hProcess)
{
    std::pair<bool, bool> retValue = std::make_pair( false, false);

    HANDLE hToken;
    if( FALSE == ::OpenProcessToken( hProcess, TOKEN_QUERY, &hToken))
        return retValue;
    std::shared_ptr<void> afHandle( hToken, ::CloseHandle);

    DWORD dwSize = 0;
    if( FALSE == ::GetTokenInformation( hToken, TokenGroups, NULL, dwSize, &dwSize))
    {
        if( GetLastError() != ERROR_INSUFFICIENT_BUFFER)
            return retValue;
    }

    PTOKEN_GROUPS pGroupInfo = (PTOKEN_GROUPS)::GlobalAlloc( GPTR, dwSize);
    if( pGroupInfo == 0)
        return retValue;
    std::shared_ptr<TOKEN_GROUPS> afGlobalFree( pGroupInfo, ::GlobalFree);

    if( FALSE == ::GetTokenInformation( hToken, TokenGroups, pGroupInfo, dwSize, &dwSize)) 
        return retValue;

    PSID pAdminSID;
    SID_IDENTIFIER_AUTHORITY sidAuthority = SECURITY_NT_AUTHORITY;
    if( FALSE == ::AllocateAndInitializeSid( &sidAuthority, 2,
                SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS,
                0, 0, 0, 0, 0, 0, &pAdminSID))
                return retValue;
    std::shared_ptr<void> afFreeSID( pAdminSID, ::FreeSid);

    for( DWORD i = 0; i < pGroupInfo->GroupCount; i++)
    {
        if(::EqualSid(pAdminSID, pGroupInfo->Groups[i].Sid))
        {      
            if (pGroupInfo->Groups[i].Attributes & SE_GROUP_ENABLED)
                retValue.first = true;
            else
                retValue.second = true;
            break;
        }
    }
    return retValue;
}