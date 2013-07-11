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