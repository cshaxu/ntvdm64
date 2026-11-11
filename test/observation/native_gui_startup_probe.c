#include <windows.h>
#include <stdio.h>
#include <wchar.h>

/* The metadata-only classifier never executes this target. Runtime tests
 * additionally create a real, unshown window on their private desktop. */
static int run_probe(PWSTR command)
{
    if(command && !wcsncmp(command,L"--mark ",7)) {
        static const char marker[]="S9-GUI-TARGET-EXECUTED";
        HANDLE file=CreateFileW(command+7,GENERIC_WRITE,0,NULL,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,NULL);
        DWORD written=0;BOOL ok;
        if(file==INVALID_HANDLE_VALUE)return 101;
        ok=WriteFile(file,marker,sizeof(marker)-1,&written,NULL) && written==sizeof(marker)-1;
        CloseHandle(file);return ok ? 37 : 102;
    }
    if(command && !lstrcmpW(command,L"--spawn-text")) {
        WCHAR image[MAX_PATH],line[MAX_PATH+100],*slash;
        STARTUPINFOW startup={sizeof(startup)};PROCESS_INFORMATION process={0};
        DWORD code=0;BOOL completed;
        if(!GetModuleFileNameW(NULL,image,ARRAYSIZE(image)))return 96;
        slash=wcsrchr(image,L'\\');if(!slash)return 97;*slash=0;
        if(swprintf_s(line,ARRAYSIZE(line),L"\"%ls\\run16.exe\" cmd /d /c exit /b 29",image)<0)return 98;
        if(!CreateProcessW(NULL,line,NULL,NULL,FALSE,0,NULL,NULL,&startup,&process))return 99;
        CloseHandle(process.hThread);
        completed=WaitForSingleObject(process.hProcess,10000)==WAIT_OBJECT_0 &&
            GetExitCodeProcess(process.hProcess,&code) && code==29;
        CloseHandle(process.hProcess);
        return completed ? 37 : 100;
    }
    if(command && *command) {
        WCHAR name[256];HANDLE ready,release;DWORD wait;
        if(swprintf_s(name,ARRAYSIZE(name),L"Local\\%ls-ready",command)<0)return 91;
        ready=OpenEventW(EVENT_MODIFY_STATE,FALSE,name);
        if(swprintf_s(name,ARRAYSIZE(name),L"Local\\%ls-release",command)<0){if(ready)CloseHandle(ready);return 92;}
        release=OpenEventW(SYNCHRONIZE,FALSE,name);
        if(!ready || !release){if(ready)CloseHandle(ready);if(release)CloseHandle(release);return 93;}
        if(!SetEvent(ready)){CloseHandle(ready);CloseHandle(release);return 94;}
        wait=WaitForSingleObject(release,30000);
        CloseHandle(ready);CloseHandle(release);
        if(wait!=WAIT_OBJECT_0)return 95;
    }
    return 37;
}
int WINAPI wWinMain(HINSTANCE instance,HINSTANCE previous,PWSTR command,int show)
{
    WNDCLASSEXW type={sizeof(type)};HWND window;int result;
    (void)previous;(void)show;
    type.hInstance=instance;type.lpfnWndProc=DefWindowProcW;
    type.lpszClassName=L"S9NativeGuiProbe";
    if(!RegisterClassExW(&type))return 103;
    window=CreateWindowExW(0,type.lpszClassName,L"S9 native GUI target",
        WS_OVERLAPPEDWINDOW,0,0,160,80,NULL,NULL,instance,NULL);
    if(!window)return 104;
    result=run_probe(command);
    if(!DestroyWindow(window))return 105;
    return result;
}
