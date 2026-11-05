#include "common/application_search.h"
#include <stdio.h>
#include <wchar.h>

#define CHECK(x) do {if(!(x)){fprintf(stderr,"FAIL line %d: %s error=%lu\n",__LINE__,#x,GetLastError());return 1;}}while(0)

static BOOL touch(PCWSTR directory,PCWSTR leaf)
{
    WCHAR path[MAX_PATH];HANDLE file;
    if(swprintf_s(path,ARRAYSIZE(path),L"%ls\\%ls",directory,leaf)<0)return FALSE;
    file=CreateFileW(path,GENERIC_WRITE,0,NULL,CREATE_NEW,0,NULL);
    if(file==INVALID_HANDLE_VALUE)return FALSE;
    return CloseHandle(file);
}

static BOOL selected(PCWSTR input,PCWSTR directory,PCWSTR leaf)
{
    WCHAR actual[MAX_PATH],expected[MAX_PATH];
    if(swprintf_s(expected,ARRAYSIZE(expected),L"%ls\\%ls",directory,leaf)<0)return FALSE;
    return !common_resolve_application(input,actual,ARRAYSIZE(actual)) &&
        !_wcsicmp(actual,expected);
}

int wmain(int argc,WCHAR **argv)
{
    WCHAR root[MAX_PATH],cwd[MAX_PATH],first[MAX_PATH],second[MAX_PATH];
    WCHAR paths[MAX_PATH*3],explicit_path[MAX_PATH],drive_relative[40],output[MAX_PATH],tiny[2];
    CHECK(argc==2);
    CHECK(GetFullPathNameW(argv[1],ARRAYSIZE(root),root,NULL));
    CHECK(wcsstr(root,L"\\build\\")!=NULL);
    CHECK(CreateDirectoryW(root,NULL));
    CHECK(swprintf_s(cwd,ARRAYSIZE(cwd),L"%ls\\cwd",root)>0);
    CHECK(swprintf_s(first,ARRAYSIZE(first),L"%ls\\path first",root)>0);
    CHECK(swprintf_s(second,ARRAYSIZE(second),L"%ls\\path-\x4e8c",root)>0);
    CHECK(CreateDirectoryW(cwd,NULL));CHECK(CreateDirectoryW(first,NULL));CHECK(CreateDirectoryW(second,NULL));
    CHECK(SetCurrentDirectoryW(cwd));
    CHECK(swprintf_s(paths,ARRAYSIZE(paths),L"%ls;%ls",first,second)>0);
    CHECK(SetEnvironmentVariableW(L"PATH",paths));
    CHECK(touch(cwd,L"cwd-first.exe"));CHECK(touch(first,L"cwd-first.com"));
    CHECK(selected(L"cwd-first",cwd,L"cwd-first.exe"));
    CHECK(touch(first,L"path-order.exe"));CHECK(touch(second,L"path-order.com"));
    CHECK(selected(L"path-order",first,L"path-order.exe"));
    CHECK(touch(cwd,L"suffix.com"));CHECK(touch(cwd,L"suffix.exe"));
    CHECK(touch(cwd,L"suffix.bat"));CHECK(touch(cwd,L"suffix.pif"));
    CHECK(selected(L"suffix",cwd,L"suffix.com"));
    CHECK(selected(L"suffix.exe",cwd,L"suffix.exe"));
    CHECK(touch(cwd,L"bat-first.bat"));CHECK(touch(cwd,L"bat-first.pif"));
    CHECK(selected(L"bat-first",cwd,L"bat-first.bat"));
    CHECK(swprintf_s(explicit_path,ARRAYSIZE(explicit_path),L"%ls\\path-order",second)>0);
    CHECK(selected(explicit_path,second,L"path-order.com"));
    CHECK(touch(first,L"missing.exe"));
    CHECK(common_resolve_application(L".\\missing.exe",output,ARRAYSIZE(output))==ERROR_FILE_NOT_FOUND && !*output);
    CHECK(swprintf_s(drive_relative,ARRAYSIZE(drive_relative),L"%lc:missing.exe",root[0])>0);
    CHECK(common_resolve_application(drive_relative,output,ARRAYSIZE(output))==ERROR_FILE_NOT_FOUND && !*output);
    CHECK(swprintf_s(drive_relative,ARRAYSIZE(drive_relative),L"%lc:suffix",root[0])>0);
    CHECK(selected(drive_relative,cwd,L"suffix.com"));
    CHECK(common_resolve_application(L"suffix",tiny,ARRAYSIZE(tiny))==ERROR_INSUFFICIENT_BUFFER && !*tiny);
    CHECK(SetEnvironmentVariableW(L"PATH",NULL));
    CHECK(selected(L"suffix",cwd,L"suffix.com"));
    CHECK(common_resolve_application(L"path-order",output,ARRAYSIZE(output))==ERROR_FILE_NOT_FOUND && !*output);
    CHECK(SetEnvironmentVariableW(L"PATH",L""));
    CHECK(common_resolve_application(L"path-order",output,ARRAYSIZE(output))==ERROR_FILE_NOT_FOUND && !*output);
    CHECK(swprintf_s(paths,ARRAYSIZE(paths),L";\"%ls\";;%ls;",second,first)>0);
    CHECK(SetEnvironmentVariableW(L"PATH",paths));
    CHECK(selected(L"path-order",second,L"path-order.com"));
    CHECK(common_resolve_application(NULL,output,ARRAYSIZE(output))==ERROR_INVALID_PARAMETER && !*output);
    puts("APPLICATION-SEARCH-PASS: directory-first suffix/explicit/drive/empty/Unicode/capacity");
    return 0;
}
