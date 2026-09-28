#include "native_console_backend.h"
#include "native_conpty.h"
#include "native_terminal.h"
#include "product-abi/console_mouse.h"
#include <stdio.h>

/* Frontend-local ConPTY and terminal ownership; never target completion. */
struct run16_native_backend {
    ntkvm_conpty *pty;
    ntkvm_terminal *terminal;
    HANDLE stop,ended;
    COORD size;
    unsigned history;
    DWORD buttons;
    int wheel_vertical,wheel_horizontal;
    BOOL launched,inherited;
    CRITICAL_SECTION lock;
};
static DWORD create_console(run16_native_backend *backend)
{
    DWORD error=0;
    if(!backend->terminal)error=ntkvm_terminal_open(backend->size.Y,backend->size.X,&backend->terminal);
    if(!error)error=ntkvm_terminal_history_limit(backend->terminal,backend->history);
    if(!error) {
        ResetEvent(backend->ended);
        error=ntkvm_conpty_open_events(backend->size,ntkvm_terminal_feed,backend->terminal,
            backend->stop,backend->ended,backend->inherited,&backend->pty);
    }
    if(error) {
        ntkvm_terminal_close(backend->terminal);backend->terminal=NULL;
        SetEvent(backend->ended);
    }
    return error;
}
DWORD run16_native_backend_open(run16_native_backend **output)
{
    return run16_native_backend_open_cancel(NULL,output);
}
DWORD run16_native_backend_open_cancel(HANDLE stop,run16_native_backend **output)
{
    run16_native_backend *backend;DWORD error=0;
    if(!output)return ERROR_INVALID_PARAMETER;
    *output=NULL;
    backend=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*backend));
    if(!backend)return ERROR_NOT_ENOUGH_MEMORY;
    InitializeCriticalSection(&backend->lock);
    backend->size.X=80;backend->size.Y=25;
    if(stop)DuplicateHandle(GetCurrentProcess(),stop,GetCurrentProcess(),&backend->stop,
        SYNCHRONIZE|EVENT_MODIFY_STATE,FALSE,0);
    else backend->stop=CreateEventW(NULL,TRUE,FALSE,NULL);
    backend->ended=CreateEventW(NULL,TRUE,FALSE,NULL);
    if(!backend->stop || !backend->ended)error=GetLastError();
    if(error)run16_native_backend_close(backend);else *output=backend;
    return error;
}
DWORD run16_native_backend_launch(run16_native_backend *backend,const run16_native_start *start,HANDLE *target)
{
    PROCESS_INFORMATION process={0};DWORD error=0;
    run16_native_start local;HANDLE capabilities[2]={0};unsigned i;
    if(!backend || !start || !target)return ERROR_INVALID_PARAMETER;
    *target=NULL;local=*start;
    EnterCriticalSection(&backend->lock);
    if(WaitForSingleObject(backend->stop,0)==WAIT_OBJECT_0)error=ERROR_OPERATION_ABORTED;
    if(!error && backend->pty && WaitForSingleObject(backend->ended,0)==WAIT_OBJECT_0) {
        error=ntkvm_conpty_error(backend->pty);
        if(!error)error=ERROR_BROKEN_PIPE;
    }
    if(!error && !backend->pty)error=create_console(backend);
    for(i=0;!error && i<2;++i)if(start->capabilities[i]) {
        if(!DuplicateHandle(GetCurrentProcess(),start->capabilities[i],GetCurrentProcess(),
            capabilities+i,SYNCHRONIZE,FALSE,0))error=GetLastError();
        else local.capabilities[i]=capabilities[i];
    }
    if(!error)error=ntkvm_conpty_launch(backend->pty,&local,&process);
    if(!error) {
        backend->launched=TRUE;
        *target=process.hProcess;CloseHandle(process.hThread);
        if(backend->inherited) {
            ULONGLONG deadline=GetTickCount64()+5000;
            DWORD startup_error=0;
            /* The first native client may still be in Console startup when
             * CreateProcess returns. Finish its cursor reply before the caller
             * can cancel presentation and strand that client before main.
             * This caller owns the same recursive backend lock; no extra
             * thread/helper and no synchronous write from the output reader. */
            do {
                startup_error=run16_native_backend_pump(backend);
                if(startup_error || ntkvm_terminal_startup_replied(backend->terminal) ||
                   WaitForSingleObject(process.hProcess,0)==WAIT_OBJECT_0)break;
                Sleep(1);
            }while(GetTickCount64()<deadline);
            if(startup_error || (!ntkvm_terminal_startup_replied(backend->terminal) &&
               WaitForSingleObject(process.hProcess,0)!=WAIT_OBJECT_0))SetEvent(backend->stop);
        }
        /* One frontend keeps one ConPTY, including across an empty interval
         * or DOS handoff. Direct target completion never releases admission. */
    }
    for(i=0;i<2;++i)if(capabilities[i])CloseHandle(capabilities[i]);
    LeaveCriticalSection(&backend->lock);
    return error;
}
DWORD run16_native_backend_configure(run16_native_backend *backend,COORD size,unsigned history)
{
    DWORD error=0;
    if(!backend || size.X<=0 || size.Y<=0)return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&backend->lock);
    if(backend->pty && WaitForSingleObject(backend->ended,0)!=WAIT_OBJECT_0) {
        if(size.X!=backend->size.X || size.Y!=backend->size.Y) {
            error=ntkvm_terminal_resize(backend->terminal,size.Y,size.X);
            if(!error)error=ntkvm_conpty_resize(backend->pty,size);
        }
        if(!error)error=ntkvm_terminal_history_limit(backend->terminal,history);
    }
    if(!error) {backend->size=size;backend->history=history;}
    LeaveCriticalSection(&backend->lock);
    return error;
}
DWORD run16_native_backend_capture(run16_native_backend *backend,ntkvm_terminal_frame *frame)
{
    DWORD error;
    if(!backend || !frame)return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&backend->lock);
    error=backend->terminal ? ntkvm_terminal_capture(backend->terminal,frame) : ERROR_NO_DATA;
    LeaveCriticalSection(&backend->lock);
    return error;
}
DWORD run16_native_backend_screen_enter(run16_native_backend *backend)
{
    if(!backend)return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&backend->lock);
    if(!backend->terminal || !backend->pty ||
       WaitForSingleObject(backend->ended,0)==WAIT_OBJECT_0 || ntkvm_conpty_error(backend->pty)) {
        LeaveCriticalSection(&backend->lock);return ERROR_NO_DATA;
    }
    ntkvm_terminal_screen_enter(backend->terminal);
    return 0;
}
void run16_native_backend_screen_leave(run16_native_backend *backend)
{
    ntkvm_terminal_screen_leave(backend->terminal);
    LeaveCriticalSection(&backend->lock);
}
DWORD run16_native_backend_seed(run16_native_backend *backend,
    const CONSOLE_SCREEN_BUFFER_INFOEX *info,const CHAR_INFO *cells,unsigned top,ULONGLONG *revision)
{
    ntkvm_terminal *terminal=NULL;DWORD error=0;
    if(!backend || !info || !cells)return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&backend->lock);
    if(backend->pty) {
        /* Only local state changes under a screen transaction. A later
         * configure, outside its parser lock, resizes the native ConPTY.
         * Backend death during this transaction cannot invalidate a completed
         * DOS screen write; importing cells performs no native pipe I/O. */
        if(!error)error=ntkvm_terminal_resize(backend->terminal,
            info->srWindow.Bottom-info->srWindow.Top+1,info->dwSize.X);
        if(!error)error=ntkvm_terminal_import_console(backend->terminal,info,cells,top);
        goto done; /* Never replace the terminal/ConPTY of this frontend. */
    }
    if(!error)error=ntkvm_terminal_open(backend->size.Y,backend->size.X,&terminal);
    if(!error)error=ntkvm_terminal_history_limit(terminal,backend->history);
    if(!error)error=ntkvm_terminal_seed_console(terminal,info,cells,top);
    if(!error) {
        ntkvm_terminal_close(backend->terminal);backend->terminal=terminal;terminal=NULL;
        backend->buttons=0;backend->launched=FALSE;backend->inherited=TRUE;
    }
done:
    if(!error && revision)*revision=ntkvm_terminal_revision(backend->terminal);
    ntkvm_terminal_close(terminal);LeaveCriticalSection(&backend->lock);
    return error;
}
/* Presentation drains replies even while DOS owns user input. Never write
 * replies synchronously from the ConPTY output-reader callback. */
DWORD run16_native_backend_pump(run16_native_backend *backend)
{
    BYTE *bytes=NULL;DWORD size=0,delivered=0,error=0;
    if(!backend)return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&backend->lock);
    if(backend->pty) {
        error=ntkvm_conpty_error(backend->pty);
        if(!error && WaitForSingleObject(backend->ended,0)!=WAIT_OBJECT_0) {
            error=ntkvm_terminal_take_replies(backend->terminal,&bytes,&size);
            if(!error && size)error=ntkvm_conpty_write(backend->pty,bytes,size,&delivered);
            if(error==ERROR_NO_MORE_ITEMS)error=0; /* Clean EOF: no requester remains. */
        }
    }
    if(bytes)HeapFree(GetProcessHeap(),0,bytes);
    LeaveCriticalSection(&backend->lock);
    return error;
}
DWORD run16_native_backend_members(run16_native_backend *backend,DWORD *members)
{
    DWORD error=0;
    if(!backend || !members)return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&backend->lock);
    if(WaitForSingleObject(backend->stop,0)==WAIT_OBJECT_0)error=ERROR_OPERATION_ABORTED;
    else if(backend->pty)error=ntkvm_conpty_error(backend->pty);
    /* Conservative retention, NOT an attached-client count. Keeping HPCON
     * permits future admissions; only explicit frontend close retires it. */
    if(!error)*members=backend->launched && WaitForSingleObject(backend->ended,0)!=WAIT_OBJECT_0;
    LeaveCriticalSection(&backend->lock);
    return error;
}
DWORD run16_native_backend_input(run16_native_backend *backend,const INPUT_RECORD *records,DWORD count)
{
    DWORD consumed;
    return run16_native_backend_input_some(backend,records,count,&consumed);
}
DWORD run16_native_backend_input_some(run16_native_backend *backend,const INPUT_RECORD *records,DWORD count,DWORD *consumed)
{
    DWORD i,error=0,delivered;
    if(!consumed)return ERROR_INVALID_PARAMETER;
    *consumed=0;
    if(!backend || (!records && count))return ERROR_INVALID_PARAMETER;
    for(i=0;i<count;++i)if(records[i].EventType==CONSOLE_INPUT_RELATIVE_MOUSE)return ERROR_INVALID_DATA;
    EnterCriticalSection(&backend->lock);
    if(count && !backend->pty)error=ERROR_NO_DATA;
    for(i=0;!error && i<count;++i) {
        const INPUT_RECORD *record=records+i;char bytes[128];int size=0;
        if(record->EventType==KEY_EVENT) {
            const KEY_EVENT_RECORD *key=&record->Event.KeyEvent;
            size=sprintf_s(bytes,sizeof(bytes),"\033[%u;%u;%u;%u;%lu;%u_",
                key->wVirtualKeyCode,key->wVirtualScanCode,key->uChar.UnicodeChar,
                key->bKeyDown!=FALSE,key->dwControlKeyState,key->wRepeatCount);
        } else if(record->EventType==FOCUS_EVENT) {
            size=sprintf_s(bytes,sizeof(bytes),"\033[%c",record->Event.FocusEvent.bSetFocus ? 'I' : 'O');
        } else if(record->EventType==MOUSE_EVENT) {
            const MOUSE_EVENT_RECORD *mouse=&record->Event.MouseEvent;
            DWORD buttons=mouse->dwButtonState&0x1f,changed=buttons^backend->buttons;
            unsigned code=3,modifiers=0;char final='M';
            if(mouse->dwControlKeyState&SHIFT_PRESSED)modifiers|=4;
            if(mouse->dwControlKeyState&(LEFT_ALT_PRESSED|RIGHT_ALT_PRESSED))modifiers|=8;
            if(mouse->dwControlKeyState&(LEFT_CTRL_PRESSED|RIGHT_CTRL_PRESSED))modifiers|=16;
            if(mouse->dwEventFlags&(MOUSE_WHEELED|MOUSE_HWHEELED)) {
                int *remainder=(mouse->dwEventFlags&MOUSE_HWHEELED) ?
                    &backend->wheel_horizontal : &backend->wheel_vertical;
                int pending=*remainder+(SHORT)HIWORD(mouse->dwButtonState);
                DWORD sent=0;
                code=(mouse->dwEventFlags&MOUSE_HWHEELED) ? 66 : 64;
                /* SGR has whole wheel steps, whereas Console records may
                 * combine steps or contain a fraction of WHEEL_DELTA. Keep
                 * independent axis remainders; never multiply small events. */
                while(!error && (pending>=WHEEL_DELTA || pending<=-WHEEL_DELTA)) {
                    int step=pending<0 ? -WHEEL_DELTA : WHEEL_DELTA;
                    size=sprintf_s(bytes,sizeof(bytes),"\033[<%u;%u;%uM",
                        (code+((mouse->dwEventFlags&MOUSE_HWHEELED) ? step>0 : step<0))|modifiers,
                        (unsigned)max(0,mouse->dwMousePosition.X)+1,
                        (unsigned)max(0,mouse->dwMousePosition.Y)+1);
                    delivered=0;
                    if(size<0)error=ERROR_INVALID_DATA;
                    else error=ntkvm_conpty_write(backend->pty,bytes,(DWORD)size,&delivered);
                    sent+=delivered;
                    if(!error)pending-=step;
                }
                if(!error) {*remainder=pending;backend->buttons=buttons;}
                if(!error || sent)*consumed=i+1;
                continue;
            } else if(changed) {
                static const DWORD masks[]={FROM_LEFT_1ST_BUTTON_PRESSED,
                    FROM_LEFT_2ND_BUTTON_PRESSED,RIGHTMOST_BUTTON_PRESSED};
                unsigned button;
                /* Console reports a button bitmap; SGR reports one edge.
                 * Serialize every changed primary button, not just the first. */
                for(button=0;button<3;++button)if(changed&masks[button]) {
                    int part=sprintf_s(bytes+size,sizeof(bytes)-(size_t)size,
                        "\033[<%u;%u;%u%c",button|modifiers,
                        (unsigned)max(0,mouse->dwMousePosition.X)+1,
                        (unsigned)max(0,mouse->dwMousePosition.Y)+1,
                        (buttons&masks[button]) ? 'M' : 'm');
                    if(part<0) {size=-1;break;}
                    size+=part;
                }
            } else {
                if(buttons&FROM_LEFT_1ST_BUTTON_PRESSED)code=0;
                else if(buttons&FROM_LEFT_2ND_BUTTON_PRESSED)code=1;
                else if(buttons&RIGHTMOST_BUTTON_PRESSED)code=2;
                if(mouse->dwEventFlags&MOUSE_MOVED)code|=32;
            }
            if(!size)size=sprintf_s(bytes,sizeof(bytes),"\033[<%u;%u;%u%c",code|modifiers,
                (unsigned)max(0,mouse->dwMousePosition.X)+1,
                (unsigned)max(0,mouse->dwMousePosition.Y)+1,final);
            backend->buttons=buttons;
        }
        delivered=0;
        if(size<0)error=ERROR_INVALID_DATA;
        else if(size)error=ntkvm_conpty_write(backend->pty,bytes,(DWORD)size,&delivered);
        if(!error || delivered)*consumed=i+1;
    }
    LeaveCriticalSection(&backend->lock);
    return error;
}
DWORD run16_native_backend_control(run16_native_backend *backend,DWORD control)
{
    INPUT_RECORD records[2]={0};
    if(control!=CTRL_C_EVENT && control!=CTRL_BREAK_EVENT)return ERROR_INVALID_PARAMETER;
    records[0].EventType=KEY_EVENT;
    records[0].Event.KeyEvent.bKeyDown=TRUE;
    records[0].Event.KeyEvent.wRepeatCount=1;
    records[0].Event.KeyEvent.wVirtualKeyCode=control==CTRL_C_EVENT ? 'C' : VK_CANCEL;
    records[0].Event.KeyEvent.wVirtualScanCode=control==CTRL_C_EVENT ? 0x2e : 0x46;
    records[0].Event.KeyEvent.uChar.UnicodeChar=3;
    records[0].Event.KeyEvent.dwControlKeyState=LEFT_CTRL_PRESSED;
    records[1]=records[0];records[1].Event.KeyEvent.bKeyDown=FALSE;
    return run16_native_backend_input(backend,records,2);
}
void run16_native_backend_cancel(run16_native_backend *backend)
{
    if(backend)SetEvent(backend->stop);
}
HANDLE run16_native_backend_process(run16_native_backend *backend)
{
    return backend ? backend->ended : NULL;
}
DWORD run16_native_backend_close(run16_native_backend *backend)
{
    if(!backend)return 0;
    run16_native_backend_cancel(backend);
    ntkvm_conpty_close(backend->pty);
    ntkvm_terminal_close(backend->terminal);
    if(backend->stop)CloseHandle(backend->stop);
    if(backend->ended)CloseHandle(backend->ended);
    DeleteCriticalSection(&backend->lock);
    HeapFree(GetProcessHeap(),0,backend);
    return 0;
}
