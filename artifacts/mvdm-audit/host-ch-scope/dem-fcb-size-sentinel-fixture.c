#include <windows.h>
#include <stdio.h>
#include <winioctl.h>
BOOL demGetMiscInfo(HANDLE,WORD*,WORD*,DWORD*);
static int errs;static DWORD error_seen;
void demClientError(HANDLE h,char d){(void)h;(void)d;errs++;error_seen=GetLastError();}
int main(void){char tmp[MAX_PATH],path[MAX_PATH];HANDLE h;DWORD got,high,low;WORD a=0,b=0;DWORD size=0;LARGE_INTEGER len;BY_HANDLE_FILE_INFORMATION info;BOOL ok,live;
 if(!GetTempPathA(MAX_PATH,tmp)||!GetTempFileNameA(tmp,"fc2",0,path))return 2;
 h=CreateFileA(path,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,0,CREATE_ALWAYS,FILE_ATTRIBUTE_TEMPORARY|FILE_FLAG_DELETE_ON_CLOSE,0);if(h==INVALID_HANDLE_VALUE)return 3;
 if(!DeviceIoControl(h,FSCTL_SET_SPARSE,0,0,0,0,&got,0)){CloseHandle(h);return 6;}len.QuadPart=0xffffffffLL;if(!SetFilePointerEx(h,len,0,FILE_BEGIN)||!SetEndOfFile(h)){CloseHandle(h);return 4;}
 SetLastError(0);high=0;low=GetFileSize(h,&high);got=GetLastError();fprintf(stderr,"NATIVE low=%08lx high=%08lx error=%lu\n",(unsigned long)low,(unsigned long)high,(unsigned long)got);if(low!=0xffffffffUL||high!=0||got!=0){CloseHandle(h);return 7;}
 SetLastError(0);ok=demGetMiscInfo(h,&a,&b,&size);live=GetFileInformationByHandle(h,&info);fprintf(stderr,"DEM result=%d errors=%d error_seen=%lu size=%08lx handle_live=%d\n",ok,errs,(unsigned long)error_seen,(unsigned long)size,live);CloseHandle(h);return (ok==0 && errs==1 && error_seen==0 && size==0xffffffffUL && live)?0:8;}
