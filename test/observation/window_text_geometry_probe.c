/* Read-only x86 observer for the selected kvm-window context layout. This is
 * a test, not an IPC contract; include the actual frame declaration so changes
 * to its layout invalidate/rebuild this observer. No focus/input operations. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "window_frame_observation.h"
typedef struct observation {
    FILE *log;
    ULONGLONG started;
    char previous[256];
    unsigned samples;
} observation;

static BOOL CALLBACK window_sample(HWND window,LPARAM parameter)
{
    observation *state=(observation *)parameter;
    WCHAR name[128];RECT client,outer;DWORD pid;
    HANDLE process;observed_window_frame sample;
    char current[256];
    if(!GetClassNameW(window,name,ARRAYSIZE(name)) || wcscmp(name,L"LibKvmWindow"))return TRUE;
    if(!GetClientRect(window,&client) || !GetWindowRect(window,&outer))return TRUE;
    GetWindowThreadProcessId(window,&pid);
    process=OpenProcess(PROCESS_VM_READ,FALSE,pid);
    if(!process)return TRUE;
    if(observer_window_frame_read(process,window,&sample)) {
        if(sample.frame.graphics)
            sprintf_s(current,sizeof(current),"hwnd=%p client=%ldx%ld graphics=%ux%u",window,
                client.right,client.bottom,sample.graphics.width,sample.graphics.height);
        else
            sprintf_s(current,sizeof(current),"hwnd=%p client=%ldx%ld text=%ux%u font=%u",window,
                client.right,client.bottom,sample.text.text_columns,sample.text.text_rows,sample.text.font_height);
        ++state->samples;
        if(strcmp(current,state->previous)) {
            fprintf(state->log,"%llu ms %s\n",GetTickCount64()-state->started,current);
            fflush(state->log);strcpy_s(state->previous,sizeof(state->previous),current);
        }
    }
    CloseHandle(process);return TRUE;
}

int main(int argc,char **argv)
{
    observation state={0};HDESK desktop=NULL;DWORD duration;
    if(argc!=4)return 2;
    duration=strtoul(argv[2],NULL,10);
    if(!duration || duration>60000 || fopen_s(&state.log,argv[3],"w"))return 2;
    state.started=GetTickCount64();
    while(GetTickCount64()-state.started<duration) {
        if(!desktop)desktop=OpenDesktopA(argv[1],0,FALSE,DESKTOP_READOBJECTS|DESKTOP_ENUMERATE);
        if(desktop)EnumDesktopWindows(desktop,window_sample,(LPARAM)&state);
        Sleep(25);
    }
    if(desktop)CloseDesktop(desktop);
    fprintf(state.log,"samples=%u\n",state.samples);fclose(state.log);
    return state.samples ? 0 : 1;
}
