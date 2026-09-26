/* DTASKMGR is a native Console observer for this product's BaseSrv only.
 * It owns presentation/selection, never task identity or process control. */
#include <windows.h>
#include <rpc.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "service.h"
#include "basesrv-exe/transport/rpc_security.h"
#include "product-abi/version.h"

#define DTMGR_COLUMNS 80
#define DTMGR_ROWS 25
#define DTMGR_INTERIOR (DTMGR_COLUMNS-2)
#define DTMGR_STATUS_COLUMN 55
#define DTMGR_BODY_TOP 4
#define DTMGR_BODY_ROWS 19
#define DTMGR_SCROLL_ROW 23
#define DTMGR_SCROLL_ATTRIBUTE (BACKGROUND_RED|BACKGROUND_GREEN|BACKGROUND_BLUE)
#define DTMGR_THUMB_ATTRIBUTE 0
/* QBasic's isaEditWindow: CaMake(coWhite, coBlue), or VGA 17h. */
#define DTMGR_NORMAL_ATTRIBUTE (BACKGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE)
#define DTMGR_ACCENT_ATTRIBUTE DTMGR_NORMAL_ATTRIBUTE
/* Keep title and selection blue identical to the standard blue edit surface. */
#define DTMGR_SELECTED_ATTRIBUTE (BACKGROUND_RED|BACKGROUND_GREEN|BACKGROUND_BLUE|FOREGROUND_BLUE)
#define DTMGR_TAB_ATTRIBUTE DTMGR_SELECTED_ATTRIBUTE
/* QBasic's bright status treatment: CaMake(coBrightWhite, coCyan), or VGA 3Fh. */
#define DTMGR_STATUS_ATTRIBUTE (BACKGROUND_BLUE|BACKGROUND_GREEN|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY)

void *__RPC_USER MIDL_user_allocate(size_t bytes) { return malloc(bytes); }
void __RPC_USER MIDL_user_free(void *value) { free(value); }

typedef struct DTASKMGR_STATE {
    RPC_BINDING_HANDLE binding;
    broker_rpc_scope scope;
    HANDLE process;
    uint64_t selected_epoch;
    ULONG selected_sequence;
    ULONG confirm_sequence;
    ULONG confirm_task_count;
    DWORD status;
    DWORD action_error;
    ULONG rendered_rows;
    ULONG first_visible;
    DWORD horizontal_offset;
    DWORD horizontal_limit;
    CONSOLE_CURSOR_INFO cursor;
    BOOL cursor_saved;
    CONSOLE_SCREEN_BUFFER_INFO console;
    BOOL console_saved;
} DTASKMGR_STATE;
static const unsigned char app_version[APP_VERSION_BYTES]=APP_VERSION;

static BOOL bind_basesrv(DTASKMGR_STATE *state)
{
    WCHAR endpoint[128];
    RPC_WSTR text=NULL;
    RPC_STATUS rpc;
    if (!broker_rpc_capture_scope(&state->scope)) return FALSE;
    wsprintfW(endpoint,L"ntvdm-basesrv-%lu-%08lx-%08lx",state->scope.session,
        (ULONG)state->scope.logon.HighPart,(ULONG)state->scope.logon.LowPart);
    rpc=RpcStringBindingComposeW(NULL,(RPC_WSTR)L"ncalrpc",NULL,(RPC_WSTR)endpoint,NULL,&text);
    if (rpc!=RPC_S_OK) { SetLastError(rpc); return FALSE; }
    rpc=RpcBindingFromStringBindingW(text,&state->binding);
    RpcStringFreeW(&text);
    if (rpc!=RPC_S_OK) { SetLastError(rpc); return FALSE; }
    rpc=RpcBindingSetAuthInfoW(state->binding,NULL,RPC_C_AUTHN_LEVEL_PKT_PRIVACY,
        RPC_C_AUTHN_WINNT,NULL,RPC_C_AUTHZ_NONE);
    if (rpc!=RPC_S_OK) { RpcBindingFree(&state->binding); SetLastError(rpc); return FALSE; }
    /* A system_handle must be a real, duplicable client handle.  The BaseSrv
     * peer check compares its server-side PID with the authenticated RPC
     * caller.  This is deliberately the same shape as dtaskmgr_rpc_test,
     * rather than the process pseudo-handle. */
    state->process=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION|SYNCHRONIZE,
        FALSE,GetCurrentProcessId());
    if (!state->process) { DWORD error=GetLastError(); RpcBindingFree(&state->binding); SetLastError(error); return FALSE; }
    return TRUE;
}
static const WCHAR *kind_name(ULONG kind)
{
    return kind==3u ? L"WOW16" : kind==2u ? L"Win16" : L"DOS";
}
static void elapsed_text(const FILETIME *started,const FILETIME *now,WCHAR output[16])
{
    ULARGE_INTEGER then_value,now_value;
    ULONGLONG seconds;
    if (!started->dwLowDateTime && !started->dwHighDateTime) {
        lstrcpyW(output,L"-"); return;
    }
    then_value.LowPart=started->dwLowDateTime;then_value.HighPart=started->dwHighDateTime;
    now_value.LowPart=now->dwLowDateTime;now_value.HighPart=now->dwHighDateTime;
    seconds=now_value.QuadPart>then_value.QuadPart ?
        (now_value.QuadPart-then_value.QuadPart)/10000000ULL : 0;
    swprintf_s(output,16,L"%02llu:%02llu:%02llu",seconds/3600ULL,
        (seconds/60ULL)%60ULL,seconds%60ULL);
}
static void framed_text(WCHAR output[DTMGR_COLUMNS+1],WCHAR left,PCWSTR text,WCHAR right)
{
    DWORD index,length=text?(DWORD)lstrlenW(text):0;
    if (length>DTMGR_INTERIOR) length=DTMGR_INTERIOR;
    output[0]=left;
    for (index=0;index<DTMGR_INTERIOR;++index) output[index+1]=L' ';
    if (length) CopyMemory(output+1,text,length*sizeof(*output));
    output[DTMGR_COLUMNS-1]=right;
    output[DTMGR_COLUMNS]=L'\0';
}
static void framed_rule(WCHAR output[DTMGR_COLUMNS+1],WCHAR left,WCHAR middle,WCHAR right)
{
    DWORD index;
    output[0]=left;
    for (index=1;index<DTMGR_COLUMNS-1;++index) output[index]=middle;
    output[DTMGR_COLUMNS-1]=right;
    output[DTMGR_COLUMNS]=L'\0';
}
static void render_line(HANDLE output,SHORT row,PCWSTR text,WORD attributes)
{
    CONSOLE_SCREEN_BUFFER_INFO info;
    DWORD written,length,width;
    COORD origin={0,row};
    if (!GetConsoleScreenBufferInfo(output,&info)) return;
    if (row<0 || row>=info.dwSize.Y) return;
    width=(DWORD)info.dwSize.X;
    length=(DWORD)lstrlenW(text);
    if (length>width) length=width;
    (void)FillConsoleOutputAttribute(output,attributes,width,origin,&written);
    if (length && !WriteConsoleOutputCharacterW(output,text,length,origin,&written)) return;
    if (length<width) {
        origin.X=(SHORT)length;
        (void)FillConsoleOutputCharacterW(output,L' ',width-length,origin,&written);
    }
}
static void render_attributes(HANDLE output,SHORT row,SHORT column,DWORD count,WORD attributes)
{
    DWORD written;
    if (count) (void)FillConsoleOutputAttribute(output,attributes,count,(COORD){column,row},&written);
}
static void render_framed_line(HANDLE output,SHORT row,PCWSTR text,WORD interior_attributes)
{
    CHAR_INFO cells[DTMGR_COLUMNS];
    CONSOLE_SCREEN_BUFFER_INFO info;
    SMALL_RECT target={0,row,DTMGR_COLUMNS-1,row};
    DWORD index;
    if (!GetConsoleScreenBufferInfo(output,&info) || row<0 || row>=info.dwSize.Y ||
        info.dwSize.X<DTMGR_COLUMNS) {
        render_line(output,row,text,interior_attributes);
        return;
    }
    for (index=0;index<DTMGR_COLUMNS;++index) {
        cells[index].Char.UnicodeChar=text[index];
        cells[index].Attributes=(index==0 || index==DTMGR_COLUMNS-1) ?
            DTMGR_ACCENT_ATTRIBUTE : interior_attributes;
    }
    (void)WriteConsoleOutputW(output,cells,(COORD){DTMGR_COLUMNS,1},(COORD){0,0},&target);
}
/* EDIT's open-bottom border style has vertical lower ends, not a bottom
 * rule. See docs/etc/evidence/dtmgr-edit-style.md for the byte-table audit. */
static void render_cell(HANDLE output,SHORT column,SHORT row,WCHAR character,WORD attribute)
{
    DWORD written;
    COORD origin={column,row};
    (void)WriteConsoleOutputCharacterW(output,&character,1,origin,&written);
    render_attributes(output,row,column,1,attribute);
}
static DWORD thumb_position(DWORD position,DWORD limit,DWORD track_cells)
{
    return limit ? (DWORD)((ULONGLONG)position*(track_cells-1)/limit) : 0;
}
static void render_scrollbars(HANDLE output,const DTASKMGR_STATE *state,ULONG selected_index)
{
    WCHAR frame[DTMGR_COLUMNS+1];
    DWORD index,vertical_thumb=selected_index>=state->first_visible ?
        selected_index-state->first_visible : 0;
    DWORD horizontal_thumb=thumb_position(state->horizontal_offset,state->horizontal_limit,DTMGR_INTERIOR-2);
    /* Follow the selected visible row even before the viewport scrolls.
     * Reserve the final cell for the down arrow. */
    if (vertical_thumb>DTMGR_BODY_ROWS-2) vertical_thumb=DTMGR_BODY_ROWS-2;
    /* The up arrow occupies the header separator, leaving the first body
     * row available for the thumb at the start of the scroll range. */
    render_cell(output,DTMGR_COLUMNS-1,DTMGR_BODY_TOP-1,L'\x2191',DTMGR_SCROLL_ATTRIBUTE);
    for (index=0;index<DTMGR_BODY_ROWS;++index) {
        WCHAR character=index==DTMGR_BODY_ROWS-1 ? L'\x2193' : L'\x2591';
        WORD attribute=DTMGR_SCROLL_ATTRIBUTE;
        if (index==vertical_thumb) { character=L' '; attribute=DTMGR_THUMB_ATTRIBUTE; }
        render_cell(output,DTMGR_COLUMNS-1,(SHORT)(DTMGR_BODY_TOP+index),character,attribute);
    }
    framed_rule(frame,L'\x2502',L'\x2591',L'\x2502');
    frame[1]=L'\x2190';frame[DTMGR_COLUMNS-2]=L'\x2192';
    render_framed_line(output,DTMGR_SCROLL_ROW,frame,DTMGR_SCROLL_ATTRIBUTE);
    render_cell(output,(SHORT)(2+horizontal_thumb),DTMGR_SCROLL_ROW,L' ',DTMGR_THUMB_ATTRIBUTE);
}
static void task_line(WCHAR *line,DWORD capacity,const DTASKMGR_STATE *state,
    const DTASKMGR_WORKER *item,const FILETIME *now)
{
    FILETIME started; WCHAR elapsed[16];
    started.dwLowDateTime=(DWORD)item->started_filetime;
    started.dwHighDateTime=(DWORD)(item->started_filetime>>32);
    elapsed_text(&started,now,elapsed);
    swprintf_s(line,capacity,L"%c  %-8lu %-7s %-10s %-5lu %s",
        item->sequence==state->selected_sequence ? L'>' : L' ',(unsigned long)item->sequence,
        kind_name(item->kind),elapsed,(unsigned long)item->stack_depth,
        item->image[0] ? item->image : L"Unknown");
}
static PCWSTR scrolled_text(PCWSTR text,DWORD offset)
{
    DWORD length=(DWORD)lstrlenW(text);
    return text+(offset<length ? offset : length);
}
static void footer_text(WCHAR output[DTMGR_COLUMNS+1],DWORD status,DWORD action_error,
    PCWSTR left_text)
{
    WCHAR connection[DTMGR_COLUMNS-DTMGR_STATUS_COLUMN];
    DWORD index,left_length=(DWORD)lstrlenW(left_text);
    DWORD connection_length;
    for (index=0;index<DTMGR_COLUMNS;++index) output[index]=L' ';
    /* The bottom-left pane has the same one-cell inset as the framed body. */
    if (left_length>DTMGR_STATUS_COLUMN-2) left_length=DTMGR_STATUS_COLUMN-2;
    CopyMemory(output+1,left_text,left_length*sizeof(*output));
    if (status!=ERROR_SUCCESS) lstrcpyW(connection,L"BaseSrv not connected");
    else if (action_error) swprintf_s(connection,ARRAYSIZE(connection),L"Kill failed %lu",(unsigned long)action_error);
    else lstrcpyW(connection,L"Ready");
    connection_length=(DWORD)lstrlenW(connection);
    if (connection_length>DTMGR_COLUMNS-DTMGR_STATUS_COLUMN-3)
        connection_length=DTMGR_COLUMNS-DTMGR_STATUS_COLUMN-3;
    output[DTMGR_STATUS_COLUMN]=L'\x2502'; output[DTMGR_STATUS_COLUMN+1]=L' ';
    CopyMemory(output+DTMGR_STATUS_COLUMN+2,connection,connection_length*sizeof(*output));
    output[DTMGR_COLUMNS]=L'\0';
}
static void configure_presentation(HANDLE output,DTASKMGR_STATE *state)
{
    CONSOLE_SCREEN_BUFFER_INFO info;
    COORD target={DTMGR_COLUMNS,DTMGR_ROWS};
    SMALL_RECT window={0,0,DTMGR_COLUMNS-1,DTMGR_ROWS-1};
    DWORD written;
    if (!GetConsoleScreenBufferInfo(output,&info)) return;
    state->console=info;state->console_saved=TRUE;
    if (info.srWindow.Right-info.srWindow.Left+1>DTMGR_COLUMNS ||
        info.srWindow.Bottom-info.srWindow.Top+1>DTMGR_ROWS) {
        SMALL_RECT smaller={0,0,
            (SHORT)((info.srWindow.Right-info.srWindow.Left+1>DTMGR_COLUMNS)?DTMGR_COLUMNS-1:info.srWindow.Right-info.srWindow.Left),
            (SHORT)((info.srWindow.Bottom-info.srWindow.Top+1>DTMGR_ROWS)?DTMGR_ROWS-1:info.srWindow.Bottom-info.srWindow.Top)};
        (void)SetConsoleWindowInfo(output,TRUE,&smaller);
    }
    (void)SetConsoleScreenBufferSize(output,target);
    (void)SetConsoleWindowInfo(output,TRUE,&window);
    (void)FillConsoleOutputAttribute(output,DTMGR_NORMAL_ATTRIBUTE,DTMGR_COLUMNS*DTMGR_ROWS,(COORD){0,0},&written);
    (void)FillConsoleOutputCharacterW(output,L' ',DTMGR_COLUMNS*DTMGR_ROWS,(COORD){0,0},&written);
    (void)SetConsoleCursorPosition(output,(COORD){0,0});
}
static void restore_presentation(HANDLE output,const DTASKMGR_STATE *state)
{
    if (!state->console_saved) return;
    (void)SetConsoleWindowInfo(output,TRUE,&(SMALL_RECT){0,0,0,0});
    (void)SetConsoleScreenBufferSize(output,state->console.dwSize);
    (void)SetConsoleWindowInfo(output,TRUE,&state->console.srWindow);
    (void)SetConsoleTextAttribute(output,state->console.wAttributes);
}
static void render(HANDLE output,DTASKMGR_STATE *state,DTASKMGR_WORKER *items,ULONG count)
{
    ULONG index,row=0,visible,selected_index=0;
    static const WCHAR title[]=L"NTVDM Task Manager";
    FILETIME now;
    WCHAR line[512],frame[DTMGR_COLUMNS+1];
    GetSystemTimeAsFileTime(&now);
    state->horizontal_limit=0;
    for (index=0;index<count;++index) {
        DWORD length;
        task_line(line,ARRAYSIZE(line),state,&items[index],&now);
        length=(DWORD)lstrlenW(line);
        if (length>DTMGR_INTERIOR && length-DTMGR_INTERIOR>state->horizontal_limit)
            state->horizontal_limit=length-DTMGR_INTERIOR;
        if (items[index].sequence==state->selected_sequence) {
            selected_index=index;
            if (index<state->first_visible) state->first_visible=index;
            else if (index-state->first_visible>=DTMGR_BODY_ROWS)
                state->first_visible=index-DTMGR_BODY_ROWS+1;
        }
    }
    if (state->horizontal_offset>state->horizontal_limit) state->horizontal_offset=state->horizontal_limit;
    if (count<=DTMGR_BODY_ROWS) state->first_visible=0;
    else if (state->first_visible>count-DTMGR_BODY_ROWS) state->first_visible=count-DTMGR_BODY_ROWS;
    visible=count-state->first_visible;
    if (visible>DTMGR_BODY_ROWS) visible=DTMGR_BODY_ROWS;
    framed_rule(frame,L' ',L' ',L' ');
    CopyMemory(frame+(DTMGR_COLUMNS-(ARRAYSIZE(title)-1))/2,title,sizeof(title)-sizeof(WCHAR));
    render_line(output,(SHORT)row++,frame,DTMGR_TAB_ATTRIBUTE);
    framed_rule(frame,L'\x250C',L'\x2500',L'\x2510');
    render_framed_line(output,(SHORT)row++,frame,DTMGR_ACCENT_ATTRIBUTE);
    swprintf_s(line,ARRAYSIZE(line),L"   %-8s %-7s %-10s %-5s %s",
        L"WORKER",L"KIND",L"ELAPSED",L"STACK",L"TASK");
    framed_text(frame,L'\x2502',scrolled_text(line,state->horizontal_offset),L'\x2502');render_framed_line(output,(SHORT)row++,frame,DTMGR_ACCENT_ATTRIBUTE);
    framed_rule(frame,L'\x251C',L'\x2500',L'\x2524');render_framed_line(output,(SHORT)row++,frame,DTMGR_ACCENT_ATTRIBUTE);
    for (index=0;index<visible;++index) {
        const DTASKMGR_WORKER *item=&items[state->first_visible+index];
        task_line(line,ARRAYSIZE(line),state,item,&now);
        framed_text(frame,L'\x2502',scrolled_text(line,state->horizontal_offset),L'\x2502');
        render_framed_line(output,(SHORT)row++,frame,
            item->sequence==state->selected_sequence ? DTMGR_SELECTED_ATTRIBUTE : DTMGR_NORMAL_ATTRIBUTE);
    }
    if (!count) {
        framed_text(frame,L'\x2502',state->status==ERROR_SUCCESS ? L"  No active tasks." : L"",L'\x2502');
        render_framed_line(output,(SHORT)row++,frame,DTMGR_NORMAL_ATTRIBUTE);
    }
    while (row<DTMGR_SCROLL_ROW) { framed_text(frame,L'\x2502',L"",L'\x2502');render_framed_line(output,(SHORT)row++,frame,DTMGR_NORMAL_ATTRIBUTE); }
    render_scrollbars(output,state,selected_index);++row;
    if (state->confirm_sequence) {
        swprintf_s(line,ARRAYSIZE(line),L"End worker %lu and all its %lu tasks [Y/N]?",
            (unsigned long)state->confirm_sequence,(unsigned long)state->confirm_task_count);
        footer_text(frame,state->status,state->action_error,line);
    } else footer_text(frame,state->status,state->action_error,L"UP/DOWN=Select   DEL=Kill   F3=Exit");
    render_line(output,(SHORT)row++,frame,DTMGR_STATUS_ATTRIBUTE);
    state->rendered_rows=row;
}
static DWORD refresh(DTASKMGR_STATE *state,DTASKMGR_WORKER **items,ULONG *count)
{
    hyper epoch=0;
    ULONG result_count=0;
    DTASKMGR_WORKER *result=NULL;
    DWORD error=ERROR_SUCCESS;
    if (!state->binding && !bind_basesrv(state)) return GetLastError();
    RpcTryExcept {
        error=Client_TaskSnapshot(state->binding,state->process,APP_PROTOCOL_VERSION,
            (unsigned char *)app_version,&epoch,&result_count,&result);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    if (error) {
        /* With no authoritative snapshot, a queued kill cannot remain valid. */
        state->confirm_sequence=0;
        state->confirm_task_count=0;
        if (result) MIDL_user_free(result); return error;
    }
    *items=result; *count=result_count;
    /* A restarted broker may recycle a local worker sequence.  A visual
     * selection is valid only for the epoch in which it was observed. */
    if (state->selected_epoch && state->selected_epoch!=(uint64_t)epoch)
        state->selected_sequence=0;
    state->selected_epoch=(uint64_t)epoch;
    if (result_count && !state->selected_sequence) state->selected_sequence=result[0].sequence;
    if (state->selected_sequence) {
        ULONG index; BOOL found=FALSE;
        for (index=0;index<result_count;++index) if (result[index].sequence==state->selected_sequence) found=TRUE;
        if (!found) state->selected_sequence=result_count ? result[0].sequence : 0;
    }
    if (state->confirm_sequence) {
        ULONG index;
        BOOL live=FALSE;
        for (index=0;index<result_count;++index)
            if (result[index].sequence==state->confirm_sequence) {
                live=TRUE;
                state->confirm_task_count=result[index].reserved;
                break;
            }
        if (!live) { state->confirm_sequence=0; state->confirm_task_count=0; }
    }
    return ERROR_SUCCESS;
}
static DWORD terminate_worker(DTASKMGR_STATE *state)
{
    DWORD error=ERROR_SUCCESS;
    if (!state->confirm_sequence) return ERROR_NOT_FOUND;
    RpcTryExcept {
        error=Client_TerminateWorker(state->binding,state->process,APP_PROTOCOL_VERSION,
            (unsigned char *)app_version,(hyper)state->selected_epoch,state->confirm_sequence);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    return error;
}
int wmain(void)
{
    DTASKMGR_STATE state;
    HANDLE input=GetStdHandle(STD_INPUT_HANDLE),output=GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode;
    ZeroMemory(&state,sizeof(state));
    if (!GetConsoleMode(input,&mode) || !SetConsoleMode(input,mode|ENABLE_WINDOW_INPUT)) return (int)GetLastError();
    configure_presentation(output,&state);
    if (GetConsoleCursorInfo(output,&state.cursor)) {
        CONSOLE_CURSOR_INFO hidden=state.cursor;
        hidden.bVisible=FALSE;
        state.cursor_saved=SetConsoleCursorInfo(output,&hidden);
    }
    for (;;) {
        DTASKMGR_WORKER *items=NULL; ULONG count=0; DWORD wait,error;
        error=refresh(&state,&items,&count); state.status=error;
        render(output,&state,items,count);
        wait=WaitForSingleObject(input,750);
        if (wait==WAIT_OBJECT_0) {
            INPUT_RECORD record; DWORD read=0;
            if (ReadConsoleInputW(input,&record,1,&read) && record.EventType==KEY_EVENT && record.Event.KeyEvent.bKeyDown) {
                WORD key=record.Event.KeyEvent.wVirtualKeyCode;
                ULONG index;
                if (state.confirm_sequence) {
                    WCHAR character=record.Event.KeyEvent.uChar.UnicodeChar;
                    if (character==L'n' || character==L'N' || key==VK_ESCAPE)
                        { state.confirm_sequence=0; state.confirm_task_count=0; }
                    else if (character==L'y' || character==L'Y') {
                        state.action_error=terminate_worker(&state);
                        state.confirm_sequence=0;
                        state.confirm_task_count=0;
                    }
                    if (items) MIDL_user_free(items);
                    continue;
                }
                if (key==VK_F3) { if (items) MIDL_user_free(items); break; }
                for (index=0;index<count;++index) if (items[index].sequence==state.selected_sequence) break;
                if (key==VK_UP && count) state.selected_sequence=items[index ? index-1 : 0].sequence;
                if (key==VK_DOWN && count) state.selected_sequence=items[index+1<count ? index+1 : count-1].sequence;
                if (key==VK_LEFT && state.horizontal_offset) --state.horizontal_offset;
                if (key==VK_RIGHT && state.horizontal_offset<state.horizontal_limit) ++state.horizontal_offset;
                if (key==VK_DELETE) {
                    state.action_error=ERROR_SUCCESS;
                    state.confirm_sequence=state.selected_sequence;
                    state.confirm_task_count=index<count ? items[index].reserved : 0;
                }
            }
        }
        if (items) MIDL_user_free(items);
    }
    if (state.binding) RpcBindingFree(&state.binding);
    if (state.process) CloseHandle(state.process);
    if (state.cursor_saved) (void)SetConsoleCursorInfo(output,&state.cursor);
    restore_presentation(output,&state);
    return 0;
}
