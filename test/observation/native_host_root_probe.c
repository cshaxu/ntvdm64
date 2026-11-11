/* Authored native text witness; does not alter host or guest environment. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wchar.h>
#include <stdio.h>

int wmain(void)
{
    WCHAR root[MAX_PATH],windir[MAX_PATH],windows[MAX_PATH],system[MAX_PATH];
    WCHAR report[MAX_PATH],text[MAX_PATH*4];
    DWORD length,written,bytes;
    HANDLE file;
    if(!GetEnvironmentVariableW(L"SYSTEMROOT",root,ARRAYSIZE(root)))wcscpy_s(root,ARRAYSIZE(root),L"<missing>");
    if(!GetEnvironmentVariableW(L"WINDIR",windir,ARRAYSIZE(windir)))wcscpy_s(windir,ARRAYSIZE(windir),L"<missing>");
    if(!GetWindowsDirectoryW(windows,ARRAYSIZE(windows)) ||
       !GetSystemDirectoryW(system,ARRAYSIZE(system))){fprintf(stderr,"HOST-ROOT-API-ERROR %lu\n",GetLastError());return 1;}
    length=GetEnvironmentVariableW(L"HOST_ROOT_REPORT",report,ARRAYSIZE(report));
    if(!length || length>=ARRAYSIZE(report)){fprintf(stderr,"HOST-ROOT-REPORT-ERROR %lu\n",GetLastError());return 3;}
    if(swprintf_s(text,ARRAYSIZE(text),L"SYSTEMROOT=%ls\nWINDIR=%ls\nWindows=%ls\nSystem=%ls\n",
        root,windir,windows,system)<0)return 4;
    file=CreateFileW(report,GENERIC_WRITE,0,NULL,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,NULL);
    if(file==INVALID_HANDLE_VALUE)return 5;
    bytes=(DWORD)wcslen(text)*sizeof(WCHAR);
    if(!WriteFile(file,text,bytes,&written,NULL) || written!=bytes){CloseHandle(file);return 6;}
    if(!CloseHandle(file))return 7;
    /* Original cmdGetInitEnvironment deliberately removes WINDIR from DOS;
     * its shell-out transform must not be mistaken for a root leak. */
    if(_wcsicmp(root,windows) ||
       (_wcsicmp(windir,L"<missing>") && _wcsicmp(windir,windows)))return 2;
    puts("NATIVE-HOST-ROOT-PASS");
    return 0;
}
