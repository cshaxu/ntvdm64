/* Independent Win95 loader diagnostic. No input, hooks or guest patches. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
static HANDLE logFile;
static char buf[1024],self[MAX_PATH],directory[MAX_PATH];
static void text(const char *key,const char *s){DWORD n=0,w;while(*key&&n<900)buf[n++]=*key++;buf[n++]='=';while(*s&&n<1000)buf[n++]=*s++;buf[n++]='\r';buf[n++]='\n';WriteFile(logFile,buf,n,&w,0);FlushFileBuffers(logFile);}
static void number(const char *key,DWORD x){char hex[9];int i;for(i=7;i>=0;i--){hex[i]="0123456789ABCDEF"[x&15];x>>=4;}hex[8]=0;text(key,hex);}
static void memory(void){MEMORYSTATUS s;LPVOID p;HANDLE heap=GetProcessHeap();s.dwLength=sizeof(s);GlobalMemoryStatus(&s);number("TOTAL_PHYS",s.dwTotalPhys);number("AVAIL_PHYS",s.dwAvailPhys);number("AVAIL_VIRTUAL",s.dwAvailVirtual);
 SetLastError(0);p=VirtualAlloc(0,1024*1024,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);number("ALLOC_1MB",!!p);number("ALLOC_ERROR",GetLastError());if(p){((volatile BYTE*)p)[0]=0x55;((volatile BYTE*)p)[1048575]=0xaa;VirtualFree(p,0,MEM_RELEASE);}
 p=HeapAlloc(heap,0,262144);number("HEAP_256KB",!!p);if(p)HeapFree(heap,0,p);
}
static void matrix(void){DWORD sizes[]={4096,65536,262144,524288,1048576,2097152},i;LPVOID p,q;
 for(i=0;i<6;i++){number("MATRIX_SIZE",sizes[i]);SetLastError(0);p=VirtualAlloc(0,sizes[i],MEM_RESERVE,PAGE_READWRITE);number("RESERVE",(DWORD)p);number("RESERVE_ERROR",GetLastError());if(p){SetLastError(0);q=VirtualAlloc(p,4096,MEM_COMMIT,PAGE_READWRITE);number("COMMIT_FIRST_PAGE",!!q);number("FIRST_ERROR",GetLastError());SetLastError(0);q=VirtualAlloc(p,sizes[i],MEM_COMMIT,PAGE_READWRITE);number("COMMIT_FULL",!!q);number("FULL_ERROR",GetLastError());VirtualFree(p,0,MEM_RELEASE);}}
}
static void file(const char *path){HANDLE f,m;LPVOID p;BYTE h[64];DWORD got;text("FILE",path);SetLastError(0);f=CreateFileA(path,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,0,OPEN_EXISTING,0,0);number("OPEN",f!=INVALID_HANDLE_VALUE);number("OPEN_ERROR",GetLastError());if(f==INVALID_HANDLE_VALUE)return;number("SIZE",GetFileSize(f,0));number("READ",ReadFile(f,h,sizeof(h),&got,0));number("READ_BYTES",got);number("MZ",h[0]|(h[1]<<8));SetLastError(0);m=CreateFileMappingA(f,0,PAGE_READONLY,0,0,0);number("FILE_MAPPING",!!m);number("MAPPING_ERROR",GetLastError());if(m){p=MapViewOfFile(m,FILE_MAP_READ,0,0,0);number("MAP_VIEW",!!p);number("MAP_ERROR",GetLastError());if(p)UnmapViewOfFile(p);CloseHandle(m);}CloseHandle(f);}
static void library(const char *name){HMODULE m,prior=GetModuleHandleA(name);char path[MAX_PATH];text("DLL",name);number("ALREADY_LOADED",!!prior);SetLastError(0);m=LoadLibraryA(name);number("LOAD",(DWORD)m);number("LOAD_ERROR",GetLastError());if(m){GetModuleFileNameA(m,path,sizeof(path));text("DLL_PATH",path);FreeLibrary(m);}}
static void process(const char *path,BOOL child){STARTUPINFOA si;PROCESS_INFORMATION pi;DWORD i,error,exitCode;char command[2*MAX_PATH];volatile BYTE *p=(BYTE*)&si;for(i=0;i<sizeof(si);i++)p[i]=0;p=(BYTE*)&pi;for(i=0;i<sizeof(pi);i++)p[i]=0;si.cb=sizeof(si);lstrcpyA(command,path);if(child)lstrcatA(command," child");text("PROCESS",command);SetLastError(0);i=CreateProcessA(path,command,0,0,FALSE,0,0,directory,&si,&pi);error=GetLastError();number("CREATE_PROCESS",i);number("PROCESS_ERROR",error);if(i){number("PROCESS_ID",pi.dwProcessId);number("PROCESS_WAIT",WaitForSingleObject(pi.hProcess,child?5000:1500));if(GetExitCodeProcess(pi.hProcess,&exitCode))number("PROCESS_EXIT",exitCode);CloseHandle(pi.hThread);CloseHandle(pi.hProcess);}}
__declspec(noreturn) void entry(void){char path[MAX_PATH];DWORD i;const char *cmd=GetCommandLineA();BOOL child=FALSE;
 GetModuleFileNameA(0,self,sizeof(self));lstrcpyA(directory,self);for(i=(DWORD)lstrlenA(directory);i;i--)if(directory[i-1]=='\\'){directory[i]=0;break;}
 for(;*cmd;cmd++)if(cmd[0]=='c'&&cmd[1]=='h'&&cmd[2]=='i'&&cmd[3]=='l'&&cmd[4]=='d'){child=TRUE;break;}
 if(child){lstrcpyA(path,directory);lstrcatA(path,"CHILD95.LOG");logFile=CreateFileA(path,GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_NEW,0,0);if(logFile!=INVALID_HANDLE_VALUE){text("CHILD_ENTRY","OK");CloseHandle(logFile);}ExitProcess(0);}
 logFile=CreateFileA("O:\\WINDOWS\\PATCH\\TESTS\\MENU95.LOG",GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,0,CREATE_NEW,0,0);if(logFile==INVALID_HANDLE_VALUE)ExitProcess(91);
 number("VERSION",GetVersion());text("SELF",self);GetWindowsDirectoryA(path,sizeof(path));text("WINDOWS_DIR",path);GetSystemDirectoryA(path,sizeof(path));text("SYSTEM_DIR",path);GetEnvironmentVariableA("COMSPEC",path,sizeof(path));text("COMSPEC",path);
 memory();matrix();file("O:\\WINDOWS\\NOTEPAD.EXE");file("O:\\WINDOWS\\SYSTEM\\OLE32.DLL");
 library("KERNEL32.DLL");library("USER32.DLL");library("GDI32.DLL");library("ADVAPI32.DLL");library("COMDLG32.DLL");library("COMCTL32.DLL");library("VERSION.DLL");library("IMM32.DLL");library("WINMM.DLL");library("OLE32.DLL");
 process(self,TRUE);process("O:\\WINDOWS\\NOTEPAD.EXE",FALSE);memory();text("LOADER_DIAGNOSTIC_COMPLETE","1");CloseHandle(logFile);ExitProcess(0);
}
