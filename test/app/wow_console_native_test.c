/* Native boundary test, not a real WOW/guest acceptance test. */
#include <windows.h>
#include <stdio.h>
#include <string.h>

static int child(void)
{
    HANDLE output;
    HWND window, console=GetConsoleWindow();
    DWORD count;
    CHAR cell=0;
    COORD origin={0,0};
    if (console && IsWindowVisible(console)) return 10;
    output=CreateFileW(L"CONOUT$",GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
    if (output==INVALID_HANDLE_VALUE) return 11;
    if (!SetConsoleTitleW(L"Hidden Console of WOW VDM") ||
        !WriteConsoleOutputCharacterA(output,"W",1,origin,&count) || count!=1 ||
        !ReadConsoleOutputCharacterA(output,&cell,1,origin,&count) || count!=1 || cell!='W') {
        CloseHandle(output);return 12;
    }
    CloseHandle(output);
    window=CreateWindowExW(0,L"STATIC",L"WOW Console boundary test",WS_OVERLAPPEDWINDOW,
        0,0,120,80,NULL,NULL,GetModuleHandleW(NULL),NULL);
    if (!window) return 13;
    ShowWindow(window,SW_SHOW);
    if (!IsWindowVisible(window)) {DestroyWindow(window);return 14;}
    DestroyWindow(window);
    return 0;
}

int main(int argc,char **argv)
{
    WCHAR image[MAX_PATH],command[MAX_PATH+32],name[64];
    STARTUPINFOW startup={sizeof(startup)};
    PROCESS_INFORMATION process={0};
    HDESK desktop;
    DWORD result=1,wait;
    if (argc==2 && !strcmp(argv[1],"--child")) return child();
    if (!GetModuleFileNameW(NULL,image,MAX_PATH)) return 1;
    swprintf_s(name,64,L"ntvdm-wow-console-test-%lu",GetCurrentProcessId());
    desktop=CreateDesktopW(name,NULL,NULL,0,GENERIC_ALL,NULL);
    if (!desktop) return 2;
    startup.lpDesktop=name; /* Never switch the user's desktop or focus. */
    swprintf_s(command,MAX_PATH+32,L"\"%s\" --child",image);
    if (!CreateProcessW(image,command,NULL,NULL,FALSE,CREATE_NO_WINDOW,
        NULL,NULL,&startup,&process)) {CloseDesktop(desktop);return 3;}
    wait=WaitForSingleObject(process.hProcess,10000);
    if (wait!=WAIT_OBJECT_0) {
        TerminateProcess(process.hProcess,15);
        WaitForSingleObject(process.hProcess,INFINITE);
        result=15;
    } else GetExitCodeProcess(process.hProcess,&result);
    CloseHandle(process.hThread);CloseHandle(process.hProcess);CloseDesktop(desktop);
    printf("%s: no-window Console I/O and independent GUI visibility (%lu)\n",
        result ? "FAIL" : "PASS",result);
    return (int)result;
}
