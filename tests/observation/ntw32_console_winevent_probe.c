/* S24 research probe: the WinEvent source must actually cover the native
 * worker's hidden Console before it replaces any production refresh wake. */
#define _WIN32_WINNT 0x0A00
#include <windows.h>
#include <stdio.h>

static HWND console_window;
static LONG seen[8];
static LONG all_seen[8];
static HWND last_window;

static void CALLBACK on_console_event(HWINEVENTHOOK hook,DWORD event,HWND window,
    LONG object,LONG child,DWORD thread,DWORD time)
{
    (void)hook;(void)object;(void)child;(void)thread;(void)time;
    if(event>=EVENT_CONSOLE_CARET && event<=EVENT_CONSOLE_END_APPLICATION){
        InterlockedIncrement(&all_seen[event-EVENT_CONSOLE_CARET]);
        last_window=window;
        if(window==console_window)InterlockedIncrement(&seen[event-EVENT_CONSOLE_CARET]);
    }
}

int wmain(int argc,WCHAR **argv)
{
    HWINEVENTHOOK hook;
    HANDLE output;
    FILE *report;
    DWORD written;
    LONG synthetic_simple;
    COORD position={2,2};
    MSG message;
    DWORD until;
    if(argc!=2 || _wfopen_s(&report,argv[1],L"w"))return 2;
    console_window=GetConsoleWindow();
    output=CreateFileW(L"CONOUT$",GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
    if(!console_window || output==INVALID_HANDLE_VALUE){fprintf(report,"no-console\n");fclose(report);return 3;}
    /* Calling thread owns the message loop required for out-of-context
     * Console WinEvents, including the 32-bit worker / 64-bit conhost case. */
    PeekMessageW(&message,NULL,0,0,PM_NOREMOVE);
    hook=SetWinEventHook(EVENT_CONSOLE_CARET,EVENT_CONSOLE_END_APPLICATION,NULL,
        on_console_event,0,0,WINEVENT_OUTOFCONTEXT);
    if(!hook){fprintf(report,"hook-error=%lu\n",GetLastError());fclose(report);return 4;}
    /* Prove the callback/message loop works independently of conhost's
     * willingness to announce its own hidden-buffer changes. */
    NotifyWinEvent(EVENT_CONSOLE_UPDATE_SIMPLE,console_window,0,0);
    until=GetTickCount()+500;
    do {
        while(PeekMessageW(&message,NULL,0,0,PM_REMOVE)){
            TranslateMessage(&message);DispatchMessageW(&message);
        }
        if(seen[EVENT_CONSOLE_UPDATE_SIMPLE-EVENT_CONSOLE_CARET])break;
        MsgWaitForMultipleObjects(0,NULL,FALSE,50,QS_ALLINPUT);
    }while((LONG)(until-GetTickCount())>0);
    synthetic_simple=seen[EVENT_CONSOLE_UPDATE_SIMPLE-EVENT_CONSOLE_CARET];
    WriteConsoleOutputCharacterW(output,L"NTW32-EVENT-PROBE",17,position,&written);
    SetConsoleCursorPosition(output,position);
    until=GetTickCount()+2000;
    do {
        while(PeekMessageW(&message,NULL,0,0,PM_REMOVE)){
            TranslateMessage(&message);DispatchMessageW(&message);
        }
        MsgWaitForMultipleObjects(0,NULL,FALSE,50,QS_ALLINPUT);
    }while((LONG)(until-GetTickCount())>0);
    fprintf(report,"window=%p caret=%ld region=%ld simple=%ld scroll=%ld layout=%ld start=%ld end=%ld\n",
        console_window,seen[0],seen[1],seen[2],seen[3],seen[4],seen[5],seen[6]);
    fprintf(report,"all-window=%p caret=%ld region=%ld simple=%ld scroll=%ld layout=%ld start=%ld end=%ld\n",
        last_window,all_seen[0],all_seen[1],all_seen[2],all_seen[3],all_seen[4],all_seen[5],all_seen[6]);
    fprintf(report,"synthetic-simple=%ld output-wrote=%lu real-update=%ld\n",
        synthetic_simple,written,seen[EVENT_CONSOLE_UPDATE_SIMPLE-EVENT_CONSOLE_CARET]-synthetic_simple);
    UnhookWinEvent(hook);CloseHandle(output);fclose(report);
    return seen[EVENT_CONSOLE_UPDATE_REGION-EVENT_CONSOLE_CARET] ||
        seen[EVENT_CONSOLE_UPDATE_SIMPLE-EVENT_CONSOLE_CARET]>synthetic_simple ? 0 : 5;
}
