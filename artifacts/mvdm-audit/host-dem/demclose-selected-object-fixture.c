/* Selected DEM object; real file handles, controlled CPU/redirection/error seams. */
#include <windows.h>
#include <stdint.h>
#include <stdio.h>
static WORD ax,bp,cx,dx,cf;static unsigned errors,removed;static DWORD error_code;static BOOL redir,closed_at_callback;
unsigned short getAX(void){return ax;} unsigned short getBP(void){return bp;}
unsigned short getCX(void){return cx;} unsigned short getDX(void){return dx;}
void setCF(unsigned short x){cf=x;}
BOOL IsVdmRedirLoaded(void){return redir;}
static BOOL on_remove(HANDLE h){DWORD flags;removed++;closed_at_callback=!GetHandleInformation(h,&flags);return TRUE;}
BOOL (*VrRemoveOpenNamedPipeInfo)(HANDLE)=on_remove;
void demClientError(HANDLE h,CHAR d){(void)h;(void)d;errors++;error_code=GetLastError();cf=1;}
extern void demClose(void);
static void args(HANDLE h,WORD high,WORD low){uintptr_t n=(uintptr_t)h;ax=(WORD)(n>>16);bp=(WORD)n;cx=high;dx=low;cf=9;}
int main(void){char dir[MAX_PATH],path[MAX_PATH];HANDLE h;DWORD flags;int rc=0;
 if(!GetTempPathA(MAX_PATH,dir)||!GetTempFileNameA(dir,"dmc",0,path))return 2;
 h=CreateFileA(path,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_TEMPORARY|FILE_FLAG_DELETE_ON_CLOSE,NULL);
 if(h==INVALID_HANDLE_VALUE){DeleteFileA(path);return 3;}
 args((HANDLE)0,0,0);demClose();if(cf!=0||errors||removed){rc=4;goto end;}
 args(h,0xffff,0xfffe);demClose();printf("seek failure cf=%u error=%lu handle_live=%d callbacks=%u\n",cf,error_code,GetHandleInformation(h,&flags)!=0,removed);
 if(cf!=1||errors!=1||!GetHandleInformation(h,&flags)||removed){rc=5;goto end;}
 redir=TRUE;args(h,0xffff,0xffff);demClose();printf("close cf=%u removed=%u closed_at_callback=%d\n",cf,removed,closed_at_callback);
 if(cf!=0||removed!=1||!closed_at_callback){rc=6;goto end;}
 args(h,0xffff,0xffff);demClose();printf("stale cf=%u error=%lu callbacks=%u\n",cf,error_code,removed);h=INVALID_HANDLE_VALUE;
 if(cf!=1||errors!=2||error_code!=ERROR_INVALID_HANDLE||removed!=1)rc=7;
 end:if(h!=INVALID_HANDLE_VALUE)CloseHandle(h);DeleteFileA(path);return rc;
}
