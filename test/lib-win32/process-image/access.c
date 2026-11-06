#include "lib-win32/process-image/file_handle.h"
#include "lib-win32/process-image/remote.h"
#include <stdio.h>
#include <stdint.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"IMAGE_ACCESS_FAIL line=%d error=%lu\n",__LINE__,GetLastError()); result=1; goto cleanup; } } while (0)
int main(void)
{
    IMAGE_NT_HEADERS header;
    IMAGE_DOS_HEADER dos, invalid={0};
    HANDLE self=GetCurrentProcess(),file=INVALID_HANDLE_VALUE;
    BYTE *base=(BYTE *)GetModuleHandleW(NULL);
    WCHAR path[MAX_PATH];
    int result=0;
    CHECK(win32_process_image_remote_header(self,base,&header)==ERROR_SUCCESS);
    CHECK(win32_process_image_remote_header(self,&invalid,&header)==ERROR_BAD_EXE_FORMAT);
    CHECK(win32_process_image_remote_header(self,base,&header)==ERROR_SUCCESS);
    CHECK(win32_process_image_remote_read(self,base,0,&dos,sizeof(dos),header.OptionalHeader.SizeOfImage)==ERROR_SUCCESS);
    CHECK(dos.e_magic==IMAGE_DOS_SIGNATURE);
    CHECK(!win32_process_image_remote_range(UINT32_MAX,2,header.OptionalHeader.SizeOfImage));
    CHECK(win32_process_image_remote_range(header.OptionalHeader.SizeOfImage,0,header.OptionalHeader.SizeOfImage));
    CHECK(win32_process_image_remote_read(self,base,header.OptionalHeader.SizeOfImage,&dos,1,header.OptionalHeader.SizeOfImage)==ERROR_BAD_EXE_FORMAT);
    CHECK(win32_process_image_remote_executable(self,(const void *)main,base));
    CHECK(!win32_process_image_remote_executable(self,&invalid,base));
    CHECK(win32_process_image_validate_dll_handle(INVALID_HANDLE_VALUE)==ERROR_INVALID_HANDLE);
    CHECK(GetModuleFileNameW(GetModuleHandleW(L"kernel32.dll"),path,MAX_PATH));
    file=CreateFileW(path,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_DELETE,NULL,OPEN_EXISTING,0,NULL);
    CHECK(file!=INVALID_HANDLE_VALUE);
    CHECK(win32_process_image_validate_dll_handle(file)==ERROR_SUCCESS);
    CloseHandle(file);file=INVALID_HANDLE_VALUE;
    CHECK(GetModuleFileNameW(NULL,path,MAX_PATH));
    file=CreateFileW(path,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_DELETE,NULL,OPEN_EXISTING,0,NULL);
    CHECK(file!=INVALID_HANDLE_VALUE);
    CHECK(win32_process_image_validate_dll_handle(file)==ERROR_BAD_EXE_FORMAT);
cleanup:
    if(file!=INVALID_HANDLE_VALUE)CloseHandle(file);
    if(!result)puts("IMAGE_ACCESS_PASS remote=1 bounds=1 executable=1 dll_handle=1");
    return result;
}
