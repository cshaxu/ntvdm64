#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <wchar.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"guard check %d error %lu\n",__LINE__,GetLastError());return 1;}}while(0)
int wmain(int argc,wchar_t **argv) {
    WCHAR inside[32768],outside[32768],missing[32768],renamed[32768],shadow[32768];HANDLE h;DWORD n;char ch='x';
    CHECK(argc==4);CHECK(swprintf_s(shadow,32768,L"%s\\shadow.txt",argv[3])>0);CHECK(swprintf_s(inside,32768,L"%s\\allowed.txt",argv[1])>0);CHECK(swprintf_s(outside,32768,L"%s\\outside.txt",argv[2])>0);CHECK(swprintf_s(missing,32768,L"%s\\missing.txt",argv[2])>0);CHECK(swprintf_s(renamed,32768,L"%s\\escaped.txt",argv[2])>0);
    h=CreateFileW(inside,GENERIC_WRITE|GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,CREATE_ALWAYS,0,NULL);CHECK(h!=INVALID_HANDLE_VALUE);CHECK(WriteFile(h,&ch,1,&n,NULL)&&n==1);CloseHandle(h);
    h=CreateFileW(outside,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);CHECK(h!=INVALID_HANDLE_VALUE);CHECK(ReadFile(h,&ch,1,&n,NULL)&&n==1);CloseHandle(h);
    h=CreateFileW(outside,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_ALWAYS,0,NULL);CHECK(h!=INVALID_HANDLE_VALUE);CloseHandle(h);
    h=CreateFileW(outside,GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);CHECK(h==INVALID_HANDLE_VALUE && GetLastError()==ERROR_ACCESS_DENIED);
    h=CreateFileW(missing,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_ALWAYS,0,NULL);CHECK(h==INVALID_HANDLE_VALUE && GetLastError()==ERROR_FILE_NOT_FOUND);
    h=CreateFileW(shadow,GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);CHECK(h==INVALID_HANDLE_VALUE && GetLastError()==ERROR_ACCESS_DENIED);
    CHECK(!DeleteFileW(outside) && GetLastError()==ERROR_ACCESS_DENIED);
    CHECK(!MoveFileW(inside,renamed) && GetLastError()==ERROR_ACCESS_DENIED);CHECK(GetFileAttributesW(inside)!=INVALID_FILE_ATTRIBUTES);CHECK(GetFileAttributesW(renamed)==INVALID_FILE_ATTRIBUTES);
    h=CreateFileW(L"NUL",GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);CHECK(h!=INVALID_HANDLE_VALUE);CHECK(WriteFile(h,&ch,1,&n,NULL));CloseHandle(h);
    puts("PASS real guard allows owned writes and devices; rejects external writes and rename");return 0;
}
