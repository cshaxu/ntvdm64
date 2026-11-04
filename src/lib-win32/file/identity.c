#include "identity.h"
DWORD win32_file_same(PCWSTR left,PCWSTR right,BOOL *same)
{
    HANDLE files[2]={INVALID_HANDLE_VALUE,INVALID_HANDLE_VALUE};
    BY_HANDLE_FILE_INFORMATION info[2];PCWSTR names[2]={left,right};
    DWORD error=ERROR_SUCCESS;
    if(!same)return ERROR_INVALID_PARAMETER;
    *same=FALSE;
    if(!left || !right)return ERROR_INVALID_PARAMETER;
    for(unsigned i=0;i<2;++i) {
        files[i]=CreateFileW(names[i],FILE_READ_ATTRIBUTES,FILE_SHARE_READ|FILE_SHARE_DELETE,
            NULL,OPEN_EXISTING,0,NULL);
        if(files[i]==INVALID_HANDLE_VALUE || !GetFileInformationByHandle(files[i],&info[i])) {
            error=GetLastError();break;
        }
    }
    if(!error)*same=info[0].dwVolumeSerialNumber==info[1].dwVolumeSerialNumber &&
        info[0].nFileIndexHigh==info[1].nFileIndexHigh && info[0].nFileIndexLow==info[1].nFileIndexLow;
    for(unsigned i=0;i<2;++i)if(files[i]!=INVALID_HANDLE_VALUE)CloseHandle(files[i]);
    return error;
}
