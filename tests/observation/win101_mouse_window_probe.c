/* Test-only output-boundary injection on the private frontend UI thread.
 * Does not test physical Raw Input/capture. Downstream is production code. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
static DWORD wanted;
static HWND window;
static BOOL CALLBACK find(HWND w,LPARAM unused){
    DWORD pid;char name[64];(void)unused;
    GetWindowThreadProcessId(w,&pid);GetClassNameA(w,name,sizeof(name));
    if(pid==wanted && !strcmp(name,"LibKvmWindow")){window=w;return FALSE;}return TRUE;
}
int main(int argc,char **argv){
    HDESK desktop;DWORD thread,pid;HMODULE dll;HOOKPROC proc;HHOOK hook;
    HANDLE target,ack;WCHAR event[96];int result=1;FILE *report;
    if(argc!=8)return 64;
    wanted=strtoul(argv[1],NULL,10);
    if(strncmp(argv[2],"NTVDMConsoleTest-",17))return 65;
    report=fopen(argv[7],"w");if(!report)return 66;
    desktop=OpenDesktopA(argv[2],0,FALSE,DESKTOP_READOBJECTS|DESKTOP_ENUMERATE|DESKTOP_HOOKCONTROL|DESKTOP_WRITEOBJECTS);
    if(!desktop){fclose(report);return 67;}
    if(!SetThreadDesktop(desktop)){fprintf(report,"set-desktop-error=%lu\n",GetLastError());CloseDesktop(desktop);fclose(report);return 68;}
    EnumDesktopWindows(desktop,find,0);
    if(!window){CloseDesktop(desktop);fclose(report);return 68;}
    thread=GetWindowThreadProcessId(window,&pid);
    target=OpenProcess(SYNCHRONIZE|PROCESS_QUERY_LIMITED_INFORMATION,FALSE,pid);
    if(!target){fclose(report);return 69;}
    dll=LoadLibraryA(argv[3]);if(!dll){CloseHandle(target);fclose(report);return 70;}
    proc=(HOOKPROC)GetProcAddress(dll,"MouseInputHook");
    hook=proc?SetWindowsHookExW(WH_GETMESSAGE,proc,dll,thread):NULL;
    if(!hook){fprintf(report,"hook-error=%lu\n",GetLastError());goto done;}
    swprintf_s(event,96,L"Local\\NTVDM-Mouse-Probe-%lu-%lu",pid,thread);
    ack=CreateEventW(NULL,TRUE,FALSE,event);
    if(!ack || GetLastError()==ERROR_ALREADY_EXISTS) {if(ack)CloseHandle(ack);goto done;}
    if(PostMessageW(window,WM_APP+0x5f0,strtoul(argv[6],NULL,10),
        MAKELPARAM((SHORT)strtol(argv[4],NULL,10),(SHORT)strtol(argv[5],NULL,10))) &&
       PostMessageW(window,WM_APP+0x5f1,1,0) && WaitForSingleObject(ack,10000)==WAIT_OBJECT_0)result=0;
    fprintf(report,"frontend=%lu thread=%lu sink-accepted=%d dx=%s dy=%s buttons=%s\n",pid,thread,!result,argv[4],argv[5],argv[6]);
    CloseHandle(ack);
done:
    if(hook)UnhookWindowsHookEx(hook);FreeLibrary(dll);CloseHandle(target);CloseDesktop(desktop);fclose(report);return result;
}
