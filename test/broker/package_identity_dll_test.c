#include <windows.h>
#include <stdio.h>
#ifdef PACKAGE_ID_DLL
#include "ntsrv-exe/transport/rpc_security.h"
__declspec(dllexport) DWORD WINAPI TestPackageIdentity(void)
{
    broker_rpc_scope scope;
    return broker_rpc_capture_scope(&scope) ? ERROR_SUCCESS : GetLastError();
}
#else
typedef DWORD (WINAPI *identity_fn)(void);
int wmain(int argc,WCHAR **argv)
{
    HMODULE dll;
    identity_fn test;
    DWORD error;
    if(argc!=2)return 2;
    dll=LoadLibraryW(argv[1]);if(!dll)return 3;
    test=(identity_fn)(ULONG_PTR)GetProcAddress(dll,"TestPackageIdentity");
#ifndef _WIN64
    if(!test)test=(identity_fn)(ULONG_PTR)GetProcAddress(dll,"_TestPackageIdentity@0");
#endif
    if(!test){FreeLibrary(dll);return 4;}
    error=test();FreeLibrary(dll);
    wprintf(L"dll-identity-error=%lu\n",error);
    return error ? 5 : 0;
}
#endif
