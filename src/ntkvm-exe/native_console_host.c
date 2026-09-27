#include "native_console_host.h"
#include <stddef.h>
#include <string.h>
#include <wchar.h>

static DWORD transfer(HANDLE pipe,void *data,DWORD bytes,BOOL write)
{
    BYTE *cursor=data;
    while(bytes) {
        DWORD count=0;
        if(!(write ? WriteFile(pipe,cursor,bytes,&count,NULL) : ReadFile(pipe,cursor,bytes,&count,NULL))) return GetLastError();
        if(!count || count>bytes) return ERROR_BROKEN_PIPE;
        cursor+=count;bytes-=count;
    }
    return ERROR_SUCCESS;
}

static BOOL WINAPI host_control(DWORD event)
{
    /* Protect this I/O provider, not its independently executing targets.
     * Custom handlers are not inherited; do not set the inheritable ignore bit. */
    return event==CTRL_C_EVENT || event==CTRL_BREAK_EVENT;
}

static DWORD apply_font(HANDLE output,CONSOLE_FONT_INFOEX *font)
{
    CONSOLE_FONT_INFOEX previous={sizeof(previous)};
    /* New Consoles can report width zero. Preserve that font request for
     * Windows to resolve; do not reject it or invent a character width. */
    if(font->cbSize!=sizeof(*font) || font->dwFontSize.X<0 || font->dwFontSize.Y<=0 ||
        !wmemchr(font->FaceName,L'\0',LF_FACESIZE))return ERROR_INVALID_DATA;
    if(!GetCurrentConsoleFontEx(output,FALSE,&previous))return GetLastError();
    /* nFont indexes this Console's font table, not the root's table. */
    if((previous.dwFontSize.X!=font->dwFontSize.X || previous.dwFontSize.Y!=font->dwFontSize.Y ||
        previous.FontFamily!=font->FontFamily || previous.FontWeight!=font->FontWeight ||
        _wcsicmp(previous.FaceName,font->FaceName)) &&
        !SetCurrentConsoleFontEx(output,FALSE,font))return GetLastError();
    return ERROR_SUCCESS;
}

static DWORD input_records(HANDLE input,const INPUT_RECORD *records,DWORD count,DWORD *written)
{
    DWORD mode,done;
    *written=0;
    while(*written<count) {
        const INPUT_RECORD *record=records+*written;
        const KEY_EVENT_RECORD *key=&record->Event.KeyEvent;
        BOOL control=record->EventType==KEY_EVENT && key->bKeyDown &&
            (key->dwControlKeyState&(LEFT_CTRL_PRESSED|RIGHT_CTRL_PRESSED)) &&
            !(key->dwControlKeyState&(LEFT_ALT_PRESSED|RIGHT_ALT_PRESSED));
        if(!GetConsoleMode(input,&mode))return GetLastError();
        /* Original ntcon/server/input.c processes these before queue insertion.
         * WriteConsoleInput alone does not perform that UI-side dispatch. The
         * hidden native Console still owns signal delivery and target handlers. */
        if(control && ((key->wVirtualKeyCode=='C' && (mode&ENABLE_PROCESSED_INPUT)) ||
            key->wVirtualKeyCode==VK_CANCEL)) {
            DWORD event=key->wVirtualKeyCode==VK_CANCEL ? CTRL_BREAK_EVENT : CTRL_C_EVENT;
            if(event==CTRL_BREAK_EVENT && !FlushConsoleInputBuffer(input))return GetLastError();
            if(!GenerateConsoleCtrlEvent(event,0))return GetLastError();
        } else {
            if(!WriteConsoleInputW(input,record,1,&done))return GetLastError();
            if(done!=1)return ERROR_WRITE_FAULT;
        }
        ++*written;
    }
    return ERROR_SUCCESS;
}

DWORD run16_native_console_host(void)
{
    HANDLE input=GetStdHandle(STD_INPUT_HANDLE),output=GetStdHandle(STD_OUTPUT_HANDLE);
    HANDLE console_input=INVALID_HANDLE_VALUE,console_output=INVALID_HANDLE_VALUE;
    SECURITY_ATTRIBUTES security={sizeof(security),NULL,TRUE};
    PROCESS_INFORMATION pending={0};
    run16_native_capture capture={0};
    DWORD error=ERROR_SUCCESS;
    BOOL attached=FALSE;
    /* Retain the transport before AllocConsole replaces standard handles.
     * Targets must not inherit the root/helper protocol pipe endpoints. */
    if(GetFileType(input)!=FILE_TYPE_PIPE || GetFileType(output)!=FILE_TYPE_PIPE)
        return ERROR_INVALID_HANDLE;
    if(!SetHandleInformation(input,HANDLE_FLAG_INHERIT,0) ||
        !SetHandleInformation(output,HANDLE_FLAG_INHERIT,0)) return GetLastError();
    {
        DWORD member;
        if(GetConsoleProcessList(&member,1) && !FreeConsole()) return GetLastError();
    }
    if(!AllocConsole()) return GetLastError();
    attached=TRUE;
    if(!SetConsoleCtrlHandler(host_control,TRUE)) { error=GetLastError();goto done; }
    if(GetConsoleWindow()) ShowWindow(GetConsoleWindow(),SW_HIDE);
    console_input=CreateFileW(L"CONIN$",GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_READ|FILE_SHARE_WRITE,&security,OPEN_EXISTING,0,NULL);
    console_output=CreateFileW(L"CONOUT$",GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_READ|FILE_SHARE_WRITE,&security,OPEN_EXISTING,0,NULL);
    if(console_input==INVALID_HANDLE_VALUE || console_output==INVALID_HANDLE_VALUE ||
        !SetStdHandle(STD_INPUT_HANDLE,console_input) ||
        !SetStdHandle(STD_OUTPUT_HANDLE,console_output) ||
        !SetStdHandle(STD_ERROR_HANDLE,console_output)) { error=GetLastError();goto done; }
    for(;;) {
        run16_native_host_request request;
        run16_native_host_reply reply={RUN16_NATIVE_HOST_VERSION,0,0,0,0,0,{0}};
        BYTE *payload=NULL;
        union {
            CHAR_INFO cells[RUN16_NATIVE_HOST_CELLS];
            run16_native_frame_info frame;
            INPUT_RECORD input[RUN16_NATIVE_HOST_INPUTS];
        } data;
        error=transfer(input,&request,sizeof(request),FALSE);
        if(error) break;
        if(request.version!=RUN16_NATIVE_HOST_VERSION) { error=ERROR_REVISION_MISMATCH;break; }
        if(request.bytes) {
            if((request.operation!=RUN16_NATIVE_LAUNCH && request.operation!=RUN16_NATIVE_INPUT &&
                request.operation!=RUN16_NATIVE_SCREEN_APPLY && request.operation!=RUN16_NATIVE_CELLS_WRITE &&
                request.operation!=RUN16_NATIVE_GEOMETRY) ||
                (request.operation!=RUN16_NATIVE_LAUNCH && request.bytes>65536)) { error=ERROR_INVALID_DATA;break; }
            payload=HeapAlloc(GetProcessHeap(),0,request.bytes);
            if(!payload) { error=ERROR_NOT_ENOUGH_MEMORY;break; }
            error=transfer(input,payload,request.bytes,FALSE);
            if(error) { HeapFree(GetProcessHeap(),0,payload);break; }
        }
        switch(request.operation) {
        case RUN16_NATIVE_MEMBERS: {
            DWORD member,count;
            if(request.bytes || request.offset || request.count) {
                reply.status=ERROR_INVALID_PARAMETER;break;
            }
            /* Windows owns native Console membership, including descendants.
             * A short buffer still returns the total required count. This
             * helper is attached for the entire dispatch loop; exclude only
             * itself. Failure is not an empty Console or target completion. */
            count=GetConsoleProcessList(&member,1);
            if(!count)reply.status=GetLastError();
            else reply.count=count-1;
            break;
        }
        case RUN16_NATIVE_LAUNCH: {
            if(pending.hProcess) { reply.status=ERROR_BUSY;break; }
            reply.status=run16_native_launch_start(payload,request.bytes,&pending);
            if(!reply.status) { reply.process=(uint64_t)(ULONG_PTR)pending.hProcess;reply.thread=(uint64_t)(ULONG_PTR)pending.hThread; }
            break;
        }
        case RUN16_NATIVE_RELEASE:
            if(!pending.hProcess) reply.status=ERROR_INVALID_STATE;
            else {
                /* Release only the export copy, never control target execution. */
                CloseHandle(pending.hThread);CloseHandle(pending.hProcess);
                ZeroMemory(&pending,sizeof(pending));
            }
            break;
        case RUN16_NATIVE_FRAME_BEGIN:
            run16_native_capture_end(&capture);
            reply.status=run16_native_capture_begin(&capture);
            if(!reply.status) {
                data.frame.screen=capture.info;data.frame.cursor=capture.cursor;
                data.frame.output_mode=capture.output_mode;
                data.frame.input_codepage=capture.input_codepage;
                data.frame.output_codepage=capture.output_codepage;
                reply.bytes=sizeof(data.frame);
            }
            break;
        case RUN16_NATIVE_FRAME_READ:
            if(!request.count || request.count>RUN16_NATIVE_HOST_CELLS) reply.status=ERROR_INVALID_PARAMETER;
            else {
                DWORD cells=0;
                reply.status=run16_native_capture_read(&capture,request.offset,data.cells,
                    request.count,&reply.region,&cells);
                reply.count=cells;
                if(!reply.status) reply.bytes=reply.count*sizeof(CHAR_INFO);
            }
            break;
        case RUN16_NATIVE_FRAME_END:
            run16_native_capture_end(&capture);break;
        case RUN16_NATIVE_INPUT:
            if(!request.bytes || request.bytes%sizeof(INPUT_RECORD)) reply.status=ERROR_INVALID_DATA;
            else {
                DWORD count=0;
                reply.status=input_records(console_input,(INPUT_RECORD *)payload,
                    request.bytes/sizeof(INPUT_RECORD),&count);
                reply.count=count;
            }
            break;
        case RUN16_NATIVE_STOP:
            break;
        case RUN16_NATIVE_CONTROL:
            if(request.bytes || request.offset || request.count>CTRL_BREAK_EVENT)
                reply.status=ERROR_INVALID_PARAMETER;
            else if(!GenerateConsoleCtrlEvent(request.count,0))reply.status=GetLastError();
            break;
        case RUN16_NATIVE_INPUT_READ: {
            typedef BOOL (WINAPI *read_input_ex)(HANDLE,PINPUT_RECORD,DWORD,LPDWORD,USHORT);
            read_input_ex read_nowait=(read_input_ex)GetProcAddress(GetModuleHandleW(L"kernel32.dll"),
                "ReadConsoleInputExW");
            DWORD count=0;
            if(!request.count || request.count>RUN16_NATIVE_HOST_INPUTS || request.offset)
                reply.status=ERROR_INVALID_PARAMETER;
            else if(!read_nowait)reply.status=ERROR_CALL_NOT_IMPLEMENTED;
            /* Original CONSOLE_READ_NOWAIT: another attached reader may take
             * input between a peek and a read. Never block this handoff on it. */
            else if(!read_nowait(console_input,data.input,request.count,&count,2))
                reply.status=GetLastError();
            else { reply.count=count;reply.bytes=count*sizeof(INPUT_RECORD); }
            break;
        }
        case RUN16_NATIVE_SCREEN_APPLY:
            if(request.bytes!=sizeof(run16_native_screen_seed)) reply.status=ERROR_INVALID_DATA;
            else {
                run16_native_screen_seed *seed=(run16_native_screen_seed *)payload;
                run16_native_frame_info *frame=&seed->frame;
                HANDLE active=CreateFileW(L"CONOUT$",GENERIC_READ|GENERIC_WRITE,
                    FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
                if(active==INVALID_HANDLE_VALUE) reply.status=GetLastError();
                else {
                    reply.status=apply_font(active,&seed->font);
                    if(!reply.status)reply.status=run16_native_screen_apply(active,&frame->screen,&frame->cursor);
                    if(!reply.status && (!SetConsoleMode(active,frame->output_mode) ||
                        !SetConsoleCP(frame->input_codepage) || !SetConsoleOutputCP(frame->output_codepage)))
                        reply.status=GetLastError();
                    CloseHandle(active);
                }
            }
            break;
        case RUN16_NATIVE_GEOMETRY:
            if(request.bytes!=sizeof(run16_native_geometry) || request.offset || request.count)
                reply.status=ERROR_INVALID_DATA;
            else {
                run16_native_geometry *geometry=(run16_native_geometry *)payload;
                CONSOLE_SCREEN_BUFFER_INFOEX info={sizeof(info)};
                CONSOLE_CURSOR_INFO cursor;
                HANDLE active=CreateFileW(L"CONOUT$",GENERIC_READ|GENERIC_WRITE,
                    FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
                if(active==INVALID_HANDLE_VALUE)reply.status=GetLastError();
                else {
                    if(!GetConsoleScreenBufferInfoEx(active,&info) || !GetConsoleCursorInfo(active,&cursor))
                        reply.status=GetLastError();
                    else {
                        info.dwSize=geometry->size;info.srWindow=geometry->window;
                        info.dwCursorPosition.X=min(info.dwCursorPosition.X,info.dwSize.X-1);
                        info.dwCursorPosition.Y=min(info.dwCursorPosition.Y,info.dwSize.Y-1);
                        reply.status=apply_font(active,&geometry->font);
                        if(!reply.status)reply.status=run16_native_screen_apply(active,&info,&cursor);
                    }
                    CloseHandle(active);
                }
            }
            break;
        case RUN16_NATIVE_CELLS_WRITE:
            if(!request.count || request.count>RUN16_NATIVE_HOST_CELLS ||
                request.bytes!=request.count*sizeof(CHAR_INFO)) reply.status=ERROR_INVALID_DATA;
            else {
                HANDLE active=CreateFileW(L"CONOUT$",GENERIC_READ|GENERIC_WRITE,
                    FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
                if(active==INVALID_HANDLE_VALUE) reply.status=GetLastError();
                else {
                    reply.status=run16_native_cells_write(active,request.offset,(CHAR_INFO *)payload,request.count);
                    CloseHandle(active);
                }
            }
            break;
        default: reply.status=ERROR_INVALID_FUNCTION;break;
        }
        if(payload) HeapFree(GetProcessHeap(),0,payload);
        error=transfer(output,&reply,sizeof(reply),TRUE);
        if(!error && reply.bytes) error=transfer(output,&data,reply.bytes,TRUE);
        if(error || request.operation==RUN16_NATIVE_STOP) break;
    }
done:
    /* A successfully created native process already owns its execution.
     * Even loss before the export acknowledgement must not kill it. */
    if(pending.hProcess) {
        CloseHandle(pending.hThread);CloseHandle(pending.hProcess);
    }
    run16_native_capture_end(&capture);
    if(console_input!=INVALID_HANDLE_VALUE) CloseHandle(console_input);
    if(console_output!=INVALID_HANDLE_VALUE) CloseHandle(console_output);
    if(attached) FreeConsole();
    return error;
}
