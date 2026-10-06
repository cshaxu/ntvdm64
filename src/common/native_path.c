/* File identity for cross-width creation; no search/classifier or native RTL
 * dependency. Kept separate from the image-section metadata object. */
#include "native_image.h"
#include <string.h>
#include <wchar.h>

DWORD common_native_application_path(PCWSTR application,PWSTR path,DWORD capacity)
{
    DWORD count,error;
    HANDLE file;
    if(!path || !capacity)return ERROR_INVALID_PARAMETER;
    path[0]=0;
    if(!application || !*application)return ERROR_INVALID_PARAMETER;
    file=CreateFileW(application,FILE_READ_ATTRIBUTES,FILE_SHARE_READ|FILE_SHARE_DELETE,
        NULL,OPEN_EXISTING,0,NULL);
    if(file==INVALID_HANDLE_VALUE)return GetLastError();
    /* CreateProcess needs a DOS/UNC application path, not the native device
     * spelling used by read-only process metadata. The selected x64 worker
     * has no WOW64 file view; this final path preserves the caller's file. */
    count=GetFinalPathNameByHandleW(file,path,capacity,
        FILE_NAME_NORMALIZED|VOLUME_NAME_DOS);
    error=count ? (count>=capacity ? ERROR_INSUFFICIENT_BUFFER : 0) : GetLastError();
    CloseHandle(file);
    if(error){path[0]=0;return error;}
    /* Keep ordinary short DOS/UNC spelling for native applications' module
     * and MUI resource lookup. The prefix is a Win32 namespace wrapper, not
     * part of the selected file identity. Long-path support is unchanged. */
    if(count<MAX_PATH && !wcsncmp(path,L"\\\\?\\UNC\\",8)) {
        memmove(path+2,path+8,(count-8+1)*sizeof(WCHAR));
        path[0]=path[1]=L'\\';
    }else if(count<MAX_PATH && !wcsncmp(path,L"\\\\?\\",4) && path[5]==L':') {
        memmove(path,path+4,(count-4+1)*sizeof(WCHAR));
    }
    return ERROR_SUCCESS;
}
