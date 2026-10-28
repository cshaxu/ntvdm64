/* Selected DEM object; real cross-process Win32 lock and owner termination. */
#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
static WORD ax,bx,bp,cx,dx,si,di,cf; static DWORD last_error;static unsigned errors;
unsigned short getAL(void){return (unsigned char)ax;} unsigned short getBX(void){return bx;}
unsigned short getBP(void){return bp;} unsigned short getCX(void){return cx;}
unsigned short getDX(void){return dx;} unsigned short getSI(void){return si;}
unsigned short getDI(void){return di;} void setCF(unsigned short v){cf=v;}
void demClientError(HANDLE h, CHAR drive){(void)h;(void)drive;last_error=GetLastError();++errors;cf=1;}
extern void demLockOper(void);
static void args(HANDLE h,WORD operation){uintptr_t v=(uintptr_t)h;ax=operation;bx=(WORD)(v>>16);bp=(WORD)v;cx=dx=si=0;di=1;cf=9;}
int main(int argc,char **argv){
 HANDLE file=INVALID_HANDLE_VALUE,ready=NULL; int result=0;
 if(argc==4 && !strcmp(argv[1],"--child")){
  ready=OpenEventA(EVENT_MODIFY_STATE,FALSE,argv[3]);
  file=CreateFileA(argv[2],GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_TEMPORARY,NULL);
  if(!ready||file==INVALID_HANDLE_VALUE)return 2;
  args(file,0);demLockOper();if(cf!=0)return 3;
  if(!SetEvent(ready))return 4;
  Sleep(INFINITE);return 5;
 }
 {char dir[MAX_PATH],path[MAX_PATH],exe[MAX_PATH],eventname[96],command[3*MAX_PATH+160];
 STARTUPINFOA si_info;PROCESS_INFORMATION pi; HANDLE waits[2];DWORD wait;
 ZeroMemory(&si_info,sizeof si_info);si_info.cb=sizeof si_info;ZeroMemory(&pi,sizeof pi);
 if(!GetTempPathA(MAX_PATH,dir)||!GetTempFileNameA(dir,"lck",0,path))return 10;
 file=CreateFileA(path,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_TEMPORARY,NULL);
 if(file==INVALID_HANDLE_VALUE){DeleteFileA(path);return 11;}
 snprintf(eventname,sizeof eventname,"Local\\demlock-probe-%lu",GetCurrentProcessId());
 ready=CreateEventA(NULL,TRUE,FALSE,eventname);if(!ready){result=12;goto done;}
 if(!GetModuleFileNameA(NULL,exe,MAX_PATH)){result=13;goto done;}
 if(snprintf(command,sizeof command,"\"%s\" --child \"%s\" \"%s\"",exe,path,eventname)<0){result=14;goto done;}
 if(!CreateProcessA(NULL,command,NULL,NULL,FALSE,CREATE_NO_WINDOW,NULL,NULL,&si_info,&pi)){result=15;goto done;}
 waits[0]=ready;waits[1]=pi.hProcess;wait=WaitForMultipleObjects(2,waits,FALSE,INFINITE);
 if(wait!=WAIT_OBJECT_0){result=16;goto stop;}
 args(file,0);demLockOper();printf("cross-process conflict cf=%u error=%lu errors=%u\n",cf,last_error,errors);
 if(cf!=1||last_error!=ERROR_LOCK_VIOLATION||errors!=1){result=17;goto stop;}
 if(!TerminateProcess(pi.hProcess,0)){result=18;goto stop;}WaitForSingleObject(pi.hProcess,INFINITE);
 args(file,0);demLockOper();printf("after owner exit cf=%u errors=%u\n",cf,errors);
 if(cf!=0||errors!=1){result=19;goto stop;}
 args(file,1);demLockOper();if(cf!=0){result=20;goto stop;}
 stop: if(WaitForSingleObject(pi.hProcess,0)==WAIT_TIMEOUT){TerminateProcess(pi.hProcess,1);WaitForSingleObject(pi.hProcess,INFINITE);}CloseHandle(pi.hThread);CloseHandle(pi.hProcess);
 done: if(ready)CloseHandle(ready);CloseHandle(file);DeleteFileA(path);return result;
 }
}
