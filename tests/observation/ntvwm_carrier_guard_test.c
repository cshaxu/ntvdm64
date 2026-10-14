/* Actual production guard, private desktop only; no foreground interaction. */
#define _WIN32_WINNT 0x0A00
#include "ntvwm-exe/console_state.h"
#include <stdio.h>

static int wait_hidden(HWND window)
{
    ULONGLONG until=GetTickCount64()+3000;
    while(IsWindowVisible(window) && GetTickCount64()<until)Sleep(5);
    return !IsWindowVisible(window);
}

static int exercise(FILE *report,BOOL target)
{
    HWND carrier=GetConsoleWindow(),other;
    ntvwm_console_guard *guard=NULL;
    DWORD before,after,pid=0;unsigned round;
    if(!carrier)return 2;
    GetWindowThreadProcessId(carrier,&pid);
    fprintf(report,"carrier=%p window-owner=%lu target=%lu\n",carrier,pid,GetCurrentProcessId());
    other=CreateWindowExW(0,L"STATIC",L"Unrelated negative control",WS_OVERLAPPEDWINDOW,
        0,0,120,80,NULL,NULL,GetModuleHandleW(NULL),NULL);
    if(!other)return 3;
    ShowWindow(other,SW_SHOWNOACTIVATE);
    if(!target) {
        if(ntvwm_console_guard_start(NULL,&guard) || guard)return 4;
        if(ntvwm_console_hide(NULL))return 5;
        ntvwm_console_guard_stop(NULL);
    }
    GetProcessHandleCount(GetCurrentProcess(),&before);
    for(round=0;round<(target ? 3u : 30u);++round) {
        if(!target && ntvwm_console_guard_start(carrier,&guard))return 6;
        /* Synchronous show ensures the subsequent hidden assertion cannot
         * pass merely because the show request has not yet been processed. */
        ShowWindow(carrier,SW_SHOWNOACTIVATE);
        if(!wait_hidden(carrier)){fprintf(report,"FAIL remained visible round=%u\n",round);return 7;}
        if(!IsWindowVisible(other))return 8;
        if(!target) {
            /* Queue another show immediately before stop: join/unhook must
             * finish before caller detaches, without use-after-free. */
            ShowWindow(carrier,SW_SHOWNOACTIVATE);
            ntvwm_console_guard_stop(guard);guard=NULL;
            Sleep(30); /* Drain already posted asynchronous hide requests. */
            ShowWindow(carrier,SW_SHOWNOACTIVATE);
            Sleep(30);
            if(!IsWindowVisible(carrier))return 9;
            ShowWindow(carrier,SW_HIDE);
        }
    }
    GetProcessHandleCount(GetCurrentProcess(),&after);
    fprintf(report,"PASS exact HWND; unrelated visible; cycles=%u handles=%lu/%lu\n",round,before,after);
    DestroyWindow(other);
    if(!target && after!=before)return 10;
    /* The final target show intentionally races its completion. The worker
     * end_io hide must run before its completion receipt is returned. */
    if(target)ShowWindow(carrier,SW_SHOWNOACTIVATE);
    if(target){printf("CARRIER-TARGET-COMPLETE-37\n");fflush(stdout);}
    return target ? 37 : 0;
}

static int product(const WCHAR *launcher,const WCHAR *root,BOOL negative)
{
    WCHAR image[MAX_PATH],command[2048],path[MAX_PATH];unsigned i;
    HWND previous=NULL,outer=GetConsoleWindow();
    if(!GetModuleFileNameW(NULL,image,MAX_PATH))return 20;
    for(i=0;i<2;++i) {
        STARTUPINFOW startup={sizeof(startup)};PROCESS_INFORMATION child={0};
        DWORD code=99;FILE *report;HWND carrier=NULL;
        swprintf_s(path,MAX_PATH,L"%s-%u.txt",root,i);
        swprintf_s(command,2048,L"\"%s\" \"%s\" target \"%s\"",launcher,image,path);
        if(!CreateProcessW(launcher,command,NULL,NULL,FALSE,0,NULL,NULL,&startup,&child))return 21;
        CloseHandle(child.hThread);
        if(WaitForSingleObject(child.hProcess,20000)==WAIT_OBJECT_0)GetExitCodeProcess(child.hProcess,&code);
        else {TerminateProcess(child.hProcess,99);WaitForSingleObject(child.hProcess,5000);}
        CloseHandle(child.hProcess);
        if(code!=(negative ? 7u : 37u))return 22;
        if(_wfopen_s(&report,path,L"r"))return 23;
        if(fscanf_s(report,"carrier=%p",&carrier)!=1){fclose(report);return 24;}
        fclose(report);
        if(!carrier || carrier==outer)return 25;
        if(negative) {if(!IsWindowVisible(carrier))return 26;}
        else {
            if(IsWindowVisible(carrier))return 27;
            if(previous && carrier!=previous)return 28;
        }
        previous=carrier;
    }
    swprintf_s(path,MAX_PATH,L"%s-summary.txt",root);
    { FILE *report;if(_wfopen_s(&report,path,L"w"))return 29;
      fprintf(report,"PASS product %s: two direct requests, exit=%u, outer=%p carrier=%p\n",
        negative ? "negative" : "guard/reuse",negative ? 7u : 37u,outer,previous);fclose(report); }
    return 0;
}

int wmain(int argc,WCHAR **argv)
{
    FILE *report;int result;
    if(argc==5 && !wcscmp(argv[1],L"product-child"))
        return product(argv[2],argv[3],!wcscmp(argv[4],L"negative"));
    if(argc==3 && (!wcscmp(argv[1],L"child") || !wcscmp(argv[1],L"target"))) {
        if(_wfopen_s(&report,argv[2],L"w"))return 11;
        result=exercise(report,!wcscmp(argv[1],L"target"));
        fprintf(report,"result=%d\n",result);fclose(report);return result;
    }
    if(argc==2 || (argc==5 && !wcscmp(argv[1],L"product"))) {
        WCHAR name[80],image[MAX_PATH],command[2048];
        STARTUPINFOW startup={sizeof(startup)};PROCESS_INFORMATION child={0};
        HDESK desktop;DWORD code=99;
        swprintf_s(name,80,L"NTVWM-Guard-%lu",GetCurrentProcessId());
        desktop=CreateDesktopW(name,NULL,NULL,0,GENERIC_ALL,NULL);
        if(!desktop || !GetModuleFileNameW(NULL,image,MAX_PATH))return 12;
        if(argc==2)swprintf_s(command,2048,L"\"%s\" child \"%s\"",image,argv[1]);
        else swprintf_s(command,2048,L"\"%s\" product-child \"%s\" \"%s\" %s",image,argv[2],argv[3],argv[4]);
        startup.lpDesktop=name;startup.dwFlags=STARTF_USESHOWWINDOW;startup.wShowWindow=SW_HIDE;
        if(!CreateProcessW(image,command,NULL,NULL,FALSE,CREATE_NEW_CONSOLE,NULL,NULL,&startup,&child))return 13;
        CloseHandle(child.hThread);
        if(WaitForSingleObject(child.hProcess,60000)==WAIT_OBJECT_0)GetExitCodeProcess(child.hProcess,&code);
        else {TerminateProcess(child.hProcess,99);WaitForSingleObject(child.hProcess,5000);}
        CloseHandle(child.hProcess);CloseDesktop(desktop);return (int)code;
    }
    return 14;
}
