/*
 * Observe the system EDIT.EXE in the same kind of hidden ordinary Console
 * owned by NTVWM.  This is a test probe: it neither changes the product
 * Console mode nor drives the application's UI.
 */
#define _WIN32_WINNT 0x0A00
#include <windows.h>
#include <stdio.h>
#include <wchar.h>
#include "ntvwm-exe/text_frame.h"

static int host(WCHAR *ready_name,WCHAR *finish_name,WCHAR *reply_name,BOOL terminal_profile)
{
    HANDLE ready=OpenEventW(EVENT_MODIFY_STATE,FALSE,ready_name);
    HANDLE finish=OpenEventW(SYNCHRONIZE,FALSE,finish_name);
    HANDLE reply=OpenEventW(EVENT_MODIFY_STATE,FALSE,reply_name);
    HANDLE output=CreateFileW(L"CONOUT$",GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
    STARTUPINFOW startup={sizeof(startup)};PROCESS_INFORMATION child={0};
    DWORD mode;
    if(!ready || !finish || !reply || output==INVALID_HANDLE_VALUE)return 90;
    if(!GetConsoleMode(output,&mode) ||
        !SetConsoleMode(output,mode|ENABLE_VIRTUAL_TERMINAL_PROCESSING))return 91;
    if(terminal_profile) {
        SetEnvironmentVariableW(L"TERM",L"xterm-256color");
        SetEnvironmentVariableW(L"COLORTERM",L"truecolor");
        SetEnvironmentVariableW(L"WT_SESSION",L"ntvwm-colour-probe");
    }
    /* Modern EDIT asks this question during terminal setup.  An ordinary
     * Console API surface parses OSC output but does not answer it on CONIN$. */
    {
        static const char query[]="\x1b]4;0;?\x07";DWORD written,read,index,chars=0;
        HANDLE input=GetStdHandle(STD_INPUT_HANDLE);
        INPUT_RECORD records[64];WCHAR text[64]={0};
        if(FlushConsoleInputBuffer(input) && WriteFile(output,query,sizeof(query)-1,&written,NULL) &&
            written==sizeof(query)-1 && WaitForSingleObject(input,250)==WAIT_OBJECT_0 &&
            ReadConsoleInputW(input,records,64,&read)) {
            for(index=0;index<read && chars<63;++index)
                if(records[index].EventType==KEY_EVENT && records[index].Event.KeyEvent.bKeyDown)
                    text[chars++]=records[index].Event.KeyEvent.uChar.UnicodeChar;
            if(chars>=5 && text[0]==0x1b && text[1]==L']' && text[2]==L'4' && text[3]==L';' && text[4]==L'0')
                SetEvent(reply);
        }
    }
    if(!CreateProcessW(L"C:\\Windows\\System32\\edit.exe",L"edit.exe",NULL,NULL,FALSE,
        CREATE_NEW_PROCESS_GROUP,NULL,NULL,&startup,&child))return 92;
    SetEvent(ready);
    WaitForSingleObject(finish,INFINITE);
    if(WaitForSingleObject(child.hProcess,0)==WAIT_TIMEOUT)TerminateProcess(child.hProcess,93);
    WaitForSingleObject(child.hProcess,5000);
    CloseHandle(child.hThread);CloseHandle(child.hProcess);CloseHandle(output);
    CloseHandle(reply);CloseHandle(finish);CloseHandle(ready);return 0;
}

static int sample(BOOL terminal_profile,const WCHAR *self,const WCHAR *report)
{
    WCHAR ready_name[96],finish_name[96],reply_name[96],command[1024];
    HANDLE ready=NULL,finish=NULL,reply=NULL,output=INVALID_HANDLE_VALUE;
    STARTUPINFOW startup={sizeof(startup)};PROCESS_INFORMATION host_process={0};
    CONSOLE_SCREEN_BUFFER_INFOEX info={sizeof(info)};CHAR_INFO *cells=NULL;
    SMALL_RECT region;COORD size,origin={0,0};DWORD count=0,mode=0,attributes[16]={0};
    console_text_style font={0};console_video_description frame={0};BYTE *payload=NULL;
    DWORD frame_attributes[16]={0};DWORD error=0,index;BOOL query_reply=FALSE;FILE *file=NULL;
    swprintf_s(ready_name,96,L"Global\\ntvwm-colour-ready-%lu-%u",GetCurrentProcessId(),terminal_profile);
    swprintf_s(finish_name,96,L"Global\\ntvwm-colour-finish-%lu-%u",GetCurrentProcessId(),terminal_profile);
    swprintf_s(reply_name,96,L"Global\\ntvwm-colour-reply-%lu-%u",GetCurrentProcessId(),terminal_profile);
    ready=CreateEventW(NULL,TRUE,FALSE,ready_name);finish=CreateEventW(NULL,TRUE,FALSE,finish_name);
    reply=CreateEventW(NULL,TRUE,FALSE,reply_name);
    if(!ready || !finish || !reply){error=GetLastError();goto done;}
    swprintf_s(command,1024,L"\"%ls\" --host \"%ls\" \"%ls\" \"%ls\" %u",self,ready_name,finish_name,reply_name,terminal_profile);
    startup.dwFlags=STARTF_USESHOWWINDOW;startup.wShowWindow=SW_HIDE;
    if(!CreateProcessW(self,command,NULL,NULL,FALSE,CREATE_NEW_CONSOLE,NULL,NULL,&startup,&host_process))
        {error=GetLastError();goto done;}
    if(WaitForSingleObject(ready,10000)!=WAIT_OBJECT_0){error=ERROR_TIMEOUT;goto done;}
    query_reply=WaitForSingleObject(reply,0)==WAIT_OBJECT_0;
    Sleep(1800); /* Allow the target to draw; never used by product code. */
    /* This probe inherits the invoking shell's Console.  Detach that shell
     * before attaching the hidden target Console; otherwise AttachConsole
     * correctly rejects the second attachment with ERROR_ACCESS_DENIED. */
    if(!FreeConsole()){error=GetLastError();goto done;}
    if(!AttachConsole(host_process.dwProcessId)){error=GetLastError();goto done;}
    output=CreateFileW(L"CONOUT$",GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
    if(output==INVALID_HANDLE_VALUE){error=GetLastError();goto done;}
    if(!GetConsoleScreenBufferInfoEx(output,&info) || !GetConsoleMode(output,&mode))
        {error=GetLastError();goto done;}
    count=(DWORD)info.dwSize.X*(DWORD)info.dwSize.Y;
    cells=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,(SIZE_T)count*sizeof(*cells));
    if(!cells){error=ERROR_NOT_ENOUGH_MEMORY;goto done;}
    size=info.dwSize;region=(SMALL_RECT){0,0,(SHORT)(size.X-1),(SHORT)(size.Y-1)};
    if(!ReadConsoleOutputW(output,cells,size,origin,&region)){error=GetLastError();goto done;}
    for(index=0;index<count;++index)++attributes[cells[index].Attributes&15];
    /* Apply the production NTVWM packer to these exact target cells.  This
     * distinguishes target-side monochrome output from a frame conversion
     * fault without touching the target or the frontend. */
    font.font_height=16;
    {
        CONSOLE_CURSOR_INFO cursor={25,TRUE};
        if(!GetConsoleCursorInfo(output,&cursor) ||
            (error=ntvwm_text_frame_pack(&info,&cursor,cells,count,&font,&frame,&payload)))goto done;
    }
    for(index=0;index<frame.width*frame.height;++index)
        ++frame_attributes[payload[sizeof(console_text_style)+index*2+1]&15];
done:
    _wfopen_s(&file,report,L"ab");
    if(file) {
        fprintf(file,"terminal_profile=%u error=%lu query_reply=%u mode=%08lx size=%dx%d attrs",terminal_profile,error,query_reply,mode,info.dwSize.X,info.dwSize.Y);
        for(index=0;index<16;++index)fprintf(file," %lu",attributes[index]);
        fprintf(file," frame_attrs");
        for(index=0;index<16;++index)fprintf(file," %lu",frame_attributes[index]);
        fprintf(file," palette");
        for(index=0;index<16;++index)fprintf(file," %06lx",(unsigned long)(info.ColorTable[index]&0xffffff));
        fprintf(file," frame_palette");
        for(index=0;index<16;++index)fprintf(file," %06lx",(unsigned long)frame.palette[index]);
        fputc('\n',file);fclose(file);
    }
    if(output!=INVALID_HANDLE_VALUE)CloseHandle(output);
    if(payload)HeapFree(GetProcessHeap(),0,payload);
    if(cells)HeapFree(GetProcessHeap(),0,cells);
    FreeConsole();
    if(finish)SetEvent(finish);
    if(host_process.hProcess) {
        WaitForSingleObject(host_process.hProcess,6000);
        CloseHandle(host_process.hThread);CloseHandle(host_process.hProcess);
    }
    if(ready)CloseHandle(ready);if(finish)CloseHandle(finish);if(reply)CloseHandle(reply);
    return error ? 1 : 0;
}

int wmain(int argc,WCHAR **argv)
{
    WCHAR self[MAX_PATH],report[MAX_PATH];
    if(argc==6 && !wcscmp(argv[1],L"--host"))return host(argv[2],argv[3],argv[4],wcstoul(argv[5],NULL,10)!=0);
    if(argc!=2)return 2;
    if(!GetModuleFileNameW(NULL,self,MAX_PATH))return 3;
    if(!GetFullPathNameW(argv[1],MAX_PATH,report,NULL))return 4;
    DeleteFileW(report);
    return sample(FALSE,self,report) || sample(TRUE,self,report);
}
