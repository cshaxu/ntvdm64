#include <windows.h>
#include <stdio.h>
#include <winioctl.h>
BOOL demGetMiscInfo(HANDLE, WORD *, WORD *, DWORD *);
void demClientError(HANDLE h, char x) {(void)h; (void)x;}
int main(void) {
 char tmp[MAX_PATH], path[MAX_PATH]; WORD a=0,b=0; DWORD size=0; LARGE_INTEGER target; BY_HANDLE_FILE_INFORMATION info; HANDLE h; BOOL result,live;
 if(!GetTempPathA(MAX_PATH,tmp) || !GetTempFileNameA(tmp,"fcb",0,path)) return 2;
 h=CreateFileA(path,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,0,CREATE_ALWAYS,FILE_ATTRIBUTE_TEMPORARY|FILE_FLAG_DELETE_ON_CLOSE,0);
 if(h==INVALID_HANDLE_VALUE)return 3;
 DWORD done=0; if(!DeviceIoControl(h,FSCTL_SET_SPARSE,0,0,0,0,&done,0)){CloseHandle(h);return 6;} target.QuadPart=0x100000000LL;
 if(!SetFilePointerEx(h,target,0,FILE_BEGIN) || !SetEndOfFile(h)){CloseHandle(h);return 4;}
 result=demGetMiscInfo(h,&a,&b,&size); live=GetFileInformationByHandle(h,&info);
 printf("highsize result=%d handle_live=%d\n",result,live);
 CloseHandle(h); return (result==0 && live!=0)?0:5;
}
