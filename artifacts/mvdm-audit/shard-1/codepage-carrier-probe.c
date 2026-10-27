#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static DWORD find_ntcon(void){PROCESSENTRY32 e={0}; HANDLE s; DWORD pid=0,n=0;e.dwSize=sizeof e;s=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);if(s==INVALID_HANDLE_VALUE)return 0;if(Process32First(s,&e))do{if(!_stricmp(e.szExeFile,"ntcon.exe")){pid=e.th32ProcessID;++n;}}while(Process32Next(s,&e));CloseHandle(s);return n==1?pid:0;}
int main(int argc,char **argv){char *path=getenv("CP_PROBE_FILE"); FILE *f; UINT prior=GetConsoleOutputCP(),next=prior==850?437:850,hidden,front=0; DWORD pid=find_ntcon(),err=0; BOOL changed=FALSE,attached=FALSE; const char *mode=argc>1?argv[1]:"control"; if(!path || !(f=fopen(path,"a")))return 3; if(!strcmp(mode,"set"))changed=SetConsoleOutputCP(next); hidden=GetConsoleOutputCP(); printf("CPPROBE-%s\n",mode); fflush(stdout); if(pid && FreeConsole()){attached=AttachConsole(pid);if(attached)front=GetConsoleOutputCP();else err=GetLastError();}else err=GetLastError(); fprintf(f,"mode=%s prior=%u requested=%u changed=%d hidden=%u ntcon_pid=%lu attached=%d frontend=%u error=%lu\n",mode,prior,next,changed,hidden,(unsigned long)pid,attached,front,(unsigned long)err); fclose(f);return !prior || !attached || !front || (!strcmp(mode,"set")&&!changed);}
