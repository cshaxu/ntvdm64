/* Test-only output-boundary injection on the private frontend UI thread.
 * Does not test physical Raw Input/capture. Downstream is production code. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "window_frame_observation.h"
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
    BOOL console_key=argc==10 && !strcmp(argv[4],"--console-key");
    BOOL keyboard=argc==10 && (!strcmp(argv[4],"--key") || console_key);
    BOOL frame=argc==5 && !strcmp(argv[3],"--frame");
    if(argc!=8 && !keyboard && !frame)return 64;
    wanted=strtoul(argv[1],NULL,10);
    if(strncmp(argv[2],"NTVDMConsoleTest-",17))return 65;
    report=fopen(argv[frame?4:keyboard?9:7],"w");if(!report)return 66;
    desktop=OpenDesktopA(argv[2],0,FALSE,DESKTOP_READOBJECTS|DESKTOP_ENUMERATE|DESKTOP_HOOKCONTROL|DESKTOP_WRITEOBJECTS);
    if(!desktop){fclose(report);return 67;}
    if(!SetThreadDesktop(desktop)){fprintf(report,"set-desktop-error=%lu\n",GetLastError());CloseDesktop(desktop);fclose(report);return 68;}
    if(!EnumDesktopWindows(desktop,find,0) && !window && !console_key){
        fprintf(report,"enum-desktop-error=%lu\n",GetLastError());
        CloseDesktop(desktop);fclose(report);return 68;
    }
    if(!window && !console_key){
        fprintf(report,"frontend=%lu awaiting-window=1\n",wanted);
        CloseDesktop(desktop);fclose(report);return 71;
    }
    thread=window?GetWindowThreadProcessId(window,&pid):0;pid=wanted;
    target=OpenProcess(SYNCHRONIZE|PROCESS_QUERY_LIMITED_INFORMATION|(frame?PROCESS_VM_READ:0),FALSE,pid);
    if(!target){fclose(report);return 69;}
    if(console_key){
        INPUT_RECORD input={0};DWORD written;HANDLE con;
        FreeConsole();
        if(!AttachConsole(pid)){CloseHandle(target);fclose(report);return 72;}
        con=CreateFileA("CONIN$",GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
        input.EventType=KEY_EVENT;input.Event.KeyEvent.wRepeatCount=1;
        input.Event.KeyEvent.wVirtualScanCode=(WORD)strtoul(argv[5],NULL,0);
        input.Event.KeyEvent.wVirtualKeyCode=(WORD)strtoul(argv[6],NULL,0);
        input.Event.KeyEvent.bKeyDown=!!strtoul(argv[7],NULL,0);
        unsigned mods=strtoul(argv[8],NULL,0);
        input.Event.KeyEvent.dwControlKeyState=((mods&1)?LEFT_CTRL_PRESSED:0)|((mods&2)?LEFT_ALT_PRESSED:0)|((mods&4)?SHIFT_PRESSED:0);
        result=con!=INVALID_HANDLE_VALUE && WriteConsoleInputW(con,&input,1,&written) && written==1?0:73;
        fprintf(report,"console-key-delivered=%d scan=%u key=%u\n",!result,input.Event.KeyEvent.wVirtualScanCode,input.Event.KeyEvent.wVirtualKeyCode);
        if(con!=INVALID_HANDLE_VALUE)CloseHandle(con);FreeConsole();
        CloseHandle(target);CloseDesktop(desktop);fclose(report);return result;
    }
    if(frame){
        observed_window_frame sample;
        if(observer_window_frame_read(target,window,&sample)){
            if(sample.frame.graphics){fprintf(report,"graphics %u %u\n",sample.graphics.width,sample.graphics.height);result=0;}
            else if(sample.text.text_columns<=1024 && sample.text.text_rows<=1024 && sample.text.stride<=1024 && sample.text.stride>=sample.text.text_columns){
                SIZE_T count=(SIZE_T)sample.text.stride*sample.text.text_rows;
                kvm_text_cell *cells=malloc(count*sizeof(*cells));
                if(cells && count<=sample.text.capacity && observer_read_bytes(target,sample.text.cells,cells,count*sizeof(*cells))){
                    fprintf(report,"text %u %u\n",sample.text.text_columns,sample.text.text_rows);
                    for(unsigned y=0;y<sample.text.text_rows;y++){for(unsigned x=0;x<sample.text.text_columns;x++){
                        unsigned c=cells[y*sample.text.stride+x].glyph_index;fputc(c>=32 && c<127?c:'.',report);
                    }fputc('\n',report);}result=0;
                }free(cells);
            }
        }
        CloseHandle(target);CloseDesktop(desktop);fclose(report);return result;
    }
    dll=LoadLibraryA(argv[3]);if(!dll){CloseHandle(target);fclose(report);return 70;}
    proc=(HOOKPROC)GetProcAddress(dll,"MouseInputHook");
    hook=proc?SetWindowsHookExW(WH_GETMESSAGE,proc,dll,thread):NULL;
    if(!hook){fprintf(report,"hook-error=%lu\n",GetLastError());goto done;}
    swprintf_s(event,96,L"Local\\NTVDM-Mouse-Probe-%lu-%lu-Input",pid,thread);
    ack=CreateEventW(NULL,TRUE,FALSE,event);
    if(!ack || GetLastError()==ERROR_ALREADY_EXISTS) {if(ack)CloseHandle(ack);goto done;}
    UINT message=keyboard?WM_APP+0x5f2:WM_APP+0x5f0;
    WPARAM value=keyboard?strtoul(argv[6],NULL,0):strtoul(argv[6],NULL,10);
    LPARAM position=keyboard?(LPARAM)(strtoul(argv[5],NULL,0) |
        (strtoul(argv[7],NULL,0)<<16) | (strtoul(argv[8],NULL,0)<<24)):
        MAKELPARAM((SHORT)strtol(argv[4],NULL,10),(SHORT)strtol(argv[5],NULL,10));
    if(PostMessageW(window,message,value,position) &&
       WaitForSingleObject(ack,10000)==WAIT_OBJECT_0)result=0;
    if(keyboard)fprintf(report,"frontend=%lu thread=%lu sink-accepted=%d scan=%s key=%s down=%s modifiers=%s\n",pid,thread,!result,argv[5],argv[6],argv[7],argv[8]);
    else fprintf(report,"frontend=%lu thread=%lu sink-accepted=%d dx=%s dy=%s buttons=%s\n",pid,thread,!result,argv[4],argv[5],argv[6]);
    CloseHandle(ack);
done:
    if(hook)UnhookWindowsHookEx(hook);FreeLibrary(dll);CloseHandle(target);CloseDesktop(desktop);fclose(report);return result;
}
