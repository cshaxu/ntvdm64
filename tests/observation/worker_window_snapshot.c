/* Read-only failure evidence for a test-owned worker, including private
 * desktops. No foreground change, input, close message or guest access. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <stdlib.h>

static BOOL CALLBACK control(HWND window,LPARAM unused)
{
    WCHAR text[1024]={0},kind[128]={0};
    DWORD_PTR copied=0;
    (void)unused;
    GetClassNameW(window,kind,128);
    SendMessageTimeoutW(window,WM_GETTEXT,1024,(LPARAM)text,
        SMTO_ABORTIFHUNG|SMTO_BLOCK,200,&copied);
    wprintf(L"window=%p class=%s visible=%d text=%s\n",window,kind,
        IsWindowVisible(window),text);
    return TRUE;
}
static BOOL CALLBACK window(HWND handle,LPARAM unused)
{
    DWORD pid;
    GetWindowThreadProcessId(handle,&pid);
    if(pid!=(DWORD)unused) return TRUE;
    control(handle,unused);EnumChildWindows(handle,control,unused);return TRUE;
}
int main(int argc,char **argv)
{
    DWORD pid,size=MAX_PATH;
    WCHAR image[MAX_PATH];
    HANDLE process,snapshot;
    THREADENTRY32 thread={sizeof(thread)};
    if((argc!=2 && argc!=3) || !(pid=strtoul(argv[1],NULL,10))) return 2;
    process=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION|SYNCHRONIZE,FALSE,pid);
    if(!process) return 3;
    if(!QueryFullProcessImageNameW(process,0,image,&size)) return 4;
    wprintf(L"pid=%lu image=%s alive=%d\n",pid,image,
        WaitForSingleObject(process,0)==WAIT_TIMEOUT);
    if(argc==3) {
        HDESK desktop=OpenDesktopA(argv[2],0,FALSE,DESKTOP_READOBJECTS|DESKTOP_ENUMERATE);
        if(!desktop) { printf("open-desktop-error=%lu\n",GetLastError());return 6; }
        if(!EnumDesktopWindows(desktop,window,(LPARAM)pid))
            printf("named-desktop-enumeration-error=%lu\n",GetLastError());
        CloseDesktop(desktop);CloseHandle(process);return 0;
    }
    snapshot=CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD,0);
    if(snapshot==INVALID_HANDLE_VALUE) return 5;
    if(Thread32First(snapshot,&thread)) do {
        if(thread.th32OwnerProcessID==pid) {
            HDESK desktop=GetThreadDesktop(thread.th32ThreadID);
            printf("thread=%lu\n",thread.th32ThreadID);
            if(!desktop || !EnumDesktopWindows(desktop,window,(LPARAM)pid))
                printf("desktop-enumeration-error=%lu\n",GetLastError());
        }
    } while(Thread32Next(snapshot,&thread));
    CloseHandle(snapshot);CloseHandle(process);return 0;
}
