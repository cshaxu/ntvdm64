/* MONITOR is a native Console observer for this product's BaseSrv only.
 * It owns presentation/selection, never task identity or process control. */
#include <windows.h>
#include <rpc.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "service.h"
#include "ntsrv-exe/transport/rpc_security.h"
#include "common/protocol/version.h"
#include "common/rpc/local_binding.h"
#include "common/rpc/management.h"
#include "common/protocol/management.h"
#include "common/protocol/task_trace.h"

#define MONITOR_COLUMNS 80
#define MONITOR_ROWS 25
#define MONITOR_INTERIOR (MONITOR_COLUMNS-2)
#define MONITOR_STATUS_COLUMN 55
#define MONITOR_BODY_TOP 4
#define MONITOR_BODY_ROWS 19
#define MONITOR_SCROLL_ROW 23
#define MONITOR_SCROLL_ATTRIBUTE (BACKGROUND_RED|BACKGROUND_GREEN|BACKGROUND_BLUE)
#define MONITOR_THUMB_ATTRIBUTE 0
/* QBasic's isaEditWindow: CaMake(coWhite, coBlue), or VGA 17h. */
#define MONITOR_NORMAL_ATTRIBUTE (BACKGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE)
#define MONITOR_ACCENT_ATTRIBUTE MONITOR_NORMAL_ATTRIBUTE
/* Keep title and selection blue identical to the standard blue edit surface. */
#define MONITOR_SELECTED_ATTRIBUTE (BACKGROUND_RED|BACKGROUND_GREEN|BACKGROUND_BLUE|FOREGROUND_BLUE)
#define MONITOR_TAB_ATTRIBUTE MONITOR_SELECTED_ATTRIBUTE
/* QBasic's bright status treatment: CaMake(coBrightWhite, coCyan), or VGA 3Fh. */
#define MONITOR_STATUS_ATTRIBUTE (BACKGROUND_BLUE|BACKGROUND_GREEN|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY)

void *__RPC_USER MIDL_user_allocate(size_t bytes) { return malloc(bytes); }
void __RPC_USER MIDL_user_free(void *value) { free(value); }

typedef struct MONITOR_STATE {
    RPC_BINDING_HANDLE binding;
    broker_rpc_scope scope;
    HANDLE process;
    DTASKMGR_KEY selected_key,confirm_key;
    DTASKMGR_KEY trace_key;
    ULONG trace_first,trace_count;
    ULONG confirm_pid;
    ULONG selected_row; /* Last snapshot position, not an execution identity. */
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
} MONITOR_STATE;

static BOOL bind_basesrv(MONITOR_STATE *state)
{
    WCHAR endpoint[128];
    RPC_STATUS rpc;
    if (!broker_rpc_capture_scope(&state->scope)) return FALSE;
    swprintf_s(endpoint,_countof(endpoint),L"ntvdm-basesrv-%lu-%08lx-%08lx",state->scope.session,
        (ULONG)state->scope.logon.HighPart,(ULONG)state->scope.logon.LowPart);
    rpc=common_rpc_bind_local(endpoint,&state->binding);
    if (rpc!=RPC_S_OK) { SetLastError(rpc); return FALSE; }
    /* A system_handle must be a real, duplicable client handle.  The BaseSrv
     * peer check compares its server-side PID with the authenticated RPC
     * caller.  This is deliberately the same shape as monitor_rpc_test,
     * rather than the process pseudo-handle. */
    state->process=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION|SYNCHRONIZE,
        FALSE,GetCurrentProcessId());
    if (!state->process) { DWORD error=GetLastError(); RpcBindingFree(&state->binding); SetLastError(error); return FALSE; }
    return TRUE;
}
static const WCHAR *kind_name(ULONG kind)
{
    return kind==MANAGEMENT_KIND_WIN64 ? L"WIN64" :
        kind==MANAGEMENT_KIND_WIN32 ? L"WIN32" :
        kind==MANAGEMENT_KIND_WIN16 ? L"WIN16" : L"DOS";
}
static BOOL same_key(const DTASKMGR_KEY *left,const DTASKMGR_KEY *right)
{
    return left->instance==right->instance && left->category==right->category &&
        left->generation==right->generation && left->object==right->object;
}
static const WCHAR *state_name(ULONG state)
{
    switch(state) {
    case MANAGEMENT_IDLE:return L"IDLE";
    case MANAGEMENT_BUSY:return L"BUSY";
    case MANAGEMENT_MISSING:return L"MISSING";
    case MANAGEMENT_CLOSING:return L"CLOSING";
    default:return L"UNKNOWN";
    }
}
static void elapsed_text(const FILETIME *started,const FILETIME *now,WCHAR output[16])
{
    ULARGE_INTEGER then_value,now_value;
    ULONGLONG seconds;
    if (!started->dwLowDateTime && !started->dwHighDateTime) {
        lstrcpyW(output,L"--:--:--"); return;
    }
    then_value.LowPart=started->dwLowDateTime;then_value.HighPart=started->dwHighDateTime;
    now_value.LowPart=now->dwLowDateTime;now_value.HighPart=now->dwHighDateTime;
    seconds=now_value.QuadPart>then_value.QuadPart ?
        (now_value.QuadPart-then_value.QuadPart)/10000000ULL : 0;
    swprintf_s(output,16,L"%02llu:%02llu:%02llu",seconds/3600ULL,
        (seconds/60ULL)%60ULL,seconds%60ULL);
}
static void framed_text(WCHAR output[MONITOR_COLUMNS+1],WCHAR left,PCWSTR text,WCHAR right)
{
    DWORD index,length=text?(DWORD)lstrlenW(text):0;
    if (length>MONITOR_INTERIOR) length=MONITOR_INTERIOR;
    output[0]=left;
    for (index=0;index<MONITOR_INTERIOR;++index) output[index+1]=L' ';
    if (length) CopyMemory(output+1,text,length*sizeof(*output));
    output[MONITOR_COLUMNS-1]=right;
    output[MONITOR_COLUMNS]=L'\0';
}
static void framed_rule(WCHAR output[MONITOR_COLUMNS+1],WCHAR left,WCHAR middle,WCHAR right)
{
    DWORD index;
    output[0]=left;
    for (index=1;index<MONITOR_COLUMNS-1;++index) output[index]=middle;
    output[MONITOR_COLUMNS-1]=right;
    output[MONITOR_COLUMNS]=L'\0';
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
    CHAR_INFO cells[MONITOR_COLUMNS];
    CONSOLE_SCREEN_BUFFER_INFO info;
    SMALL_RECT target={0,row,MONITOR_COLUMNS-1,row};
    DWORD index;
    if (!GetConsoleScreenBufferInfo(output,&info) || row<0 || row>=info.dwSize.Y ||
        info.dwSize.X<MONITOR_COLUMNS) {
        render_line(output,row,text,interior_attributes);
        return;
    }
    for (index=0;index<MONITOR_COLUMNS;++index) {
        cells[index].Char.UnicodeChar=text[index];
        cells[index].Attributes=(index==0 || index==MONITOR_COLUMNS-1) ?
            MONITOR_ACCENT_ATTRIBUTE : interior_attributes;
    }
    (void)WriteConsoleOutputW(output,cells,(COORD){MONITOR_COLUMNS,1},(COORD){0,0},&target);
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
static void render_scrollbars(HANDLE output,const MONITOR_STATE *state,ULONG selected_index)
{
    WCHAR frame[MONITOR_COLUMNS+1];
    DWORD index,vertical_thumb=selected_index>=state->first_visible ?
        selected_index-state->first_visible : 0;
    DWORD horizontal_thumb=thumb_position(state->horizontal_offset,state->horizontal_limit,MONITOR_INTERIOR-2);
    /* Follow the selected visible row even before the viewport scrolls.
     * Reserve the final cell for the down arrow. */
    if (vertical_thumb>MONITOR_BODY_ROWS-2) vertical_thumb=MONITOR_BODY_ROWS-2;
    /* The up arrow occupies the header separator, leaving the first body
     * row available for the thumb at the start of the scroll range. */
    render_cell(output,MONITOR_COLUMNS-1,MONITOR_BODY_TOP-1,L'\x2191',MONITOR_SCROLL_ATTRIBUTE);
    for (index=0;index<MONITOR_BODY_ROWS;++index) {
        WCHAR character=index==MONITOR_BODY_ROWS-1 ? L'\x2193' : L'\x2591';
        WORD attribute=MONITOR_SCROLL_ATTRIBUTE;
        if (index==vertical_thumb) { character=L' '; attribute=MONITOR_THUMB_ATTRIBUTE; }
        render_cell(output,MONITOR_COLUMNS-1,(SHORT)(MONITOR_BODY_TOP+index),character,attribute);
    }
    framed_rule(frame,L'\x2502',L'\x2591',L'\x2502');
    frame[1]=L'\x2190';frame[MONITOR_COLUMNS-2]=L'\x2192';
    render_framed_line(output,MONITOR_SCROLL_ROW,frame,MONITOR_SCROLL_ATTRIBUTE);
    render_cell(output,(SHORT)(2+horizontal_thumb),MONITOR_SCROLL_ROW,L' ',MONITOR_THUMB_ATTRIBUTE);
}
static void task_line(WCHAR *line,DWORD capacity,const MONITOR_STATE *state,
    const DTASKMGR_WORKER *item,const FILETIME *now)
{
    FILETIME started; WCHAR elapsed[16],pid[16],node[16],stack[16];
    started.dwLowDateTime=(DWORD)item->started_filetime;
    started.dwHighDateTime=(DWORD)(item->started_filetime>>32);
    elapsed_text(&started,now,elapsed);
    if(item->process_id)swprintf_s(pid,ARRAYSIZE(pid),L"%lu",item->process_id);
    else lstrcpyW(pid,L"-");
    if(item->key.category==MANAGEMENT_WORKER)swprintf_s(stack,ARRAYSIZE(stack),L"%lu",item->stack_depth);
    else lstrcpyW(stack,L"-");
    /* The service contract has roots and one child level. Reserve indentation
     * inside the PID field so every other column stays aligned. */
    swprintf_s(node,ARRAYSIZE(node),L"%s%s",item->depth ? L"  " : L"",pid);
    swprintf_s(line,capacity,L"%c %-12s %-7s %-7s %-10s %-5s %s",
        same_key(&item->key,&state->selected_key) ? L'>' : L' ',node,
        item->key.category==MANAGEMENT_FRONTEND ? L"CONSOLE" : kind_name(item->kind),
        state_name(item->display_state),elapsed,stack,
        item->key.category==MANAGEMENT_FRONTEND ||
        (item->key.category==MANAGEMENT_WORKER && item->kind==1u) ? L"-" :
        item->image[0] ? item->image : L"Unknown");
}
static void confirmation_text(WCHAR *line,DWORD capacity,const MONITOR_STATE *state)
{
    swprintf_s(line,capacity,L"End %s %lu [Y/N]?",
        state->confirm_key.category==MANAGEMENT_FRONTEND ? L"Console" :
        state->confirm_key.category==MANAGEMENT_GUI_TARGET ? L"program" : L"worker",
        (unsigned long)state->confirm_pid);
}
static PCWSTR scrolled_text(PCWSTR text,DWORD offset)
{
    DWORD length=(DWORD)lstrlenW(text);
    return text+(offset<length ? offset : length);
}
static void footer_text(WCHAR output[MONITOR_COLUMNS+1],DWORD status,DWORD action_error,
    PCWSTR left_text)
{
    WCHAR connection[MONITOR_COLUMNS-MONITOR_STATUS_COLUMN];
    DWORD index,left_length=(DWORD)lstrlenW(left_text);
    DWORD connection_length;
    for (index=0;index<MONITOR_COLUMNS;++index) output[index]=L' ';
    /* The bottom-left pane has the same one-cell inset as the framed body. */
    if (left_length>MONITOR_STATUS_COLUMN-2) left_length=MONITOR_STATUS_COLUMN-2;
    CopyMemory(output+1,left_text,left_length*sizeof(*output));
    if (status!=ERROR_SUCCESS) lstrcpyW(connection,L"BaseSrv not connected");
    else if (action_error) swprintf_s(connection,ARRAYSIZE(connection),L"Kill failed %lu",(unsigned long)action_error);
    else lstrcpyW(connection,L"Ready");
    connection_length=(DWORD)lstrlenW(connection);
    if (connection_length>MONITOR_COLUMNS-MONITOR_STATUS_COLUMN-3)
        connection_length=MONITOR_COLUMNS-MONITOR_STATUS_COLUMN-3;
    output[MONITOR_STATUS_COLUMN]=L'\x2502'; output[MONITOR_STATUS_COLUMN+1]=L' ';
    CopyMemory(output+MONITOR_STATUS_COLUMN+2,connection,connection_length*sizeof(*output));
    output[MONITOR_COLUMNS]=L'\0';
}
static void configure_presentation(HANDLE output,MONITOR_STATE *state)
{
    CONSOLE_SCREEN_BUFFER_INFO info;
    COORD target={MONITOR_COLUMNS,MONITOR_ROWS};
    SMALL_RECT window={0,0,MONITOR_COLUMNS-1,MONITOR_ROWS-1};
    DWORD written;
    if (!GetConsoleScreenBufferInfo(output,&info)) return;
    state->console=info;state->console_saved=TRUE;
    if (info.srWindow.Right-info.srWindow.Left+1>MONITOR_COLUMNS ||
        info.srWindow.Bottom-info.srWindow.Top+1>MONITOR_ROWS) {
        SMALL_RECT smaller={0,0,
            (SHORT)((info.srWindow.Right-info.srWindow.Left+1>MONITOR_COLUMNS)?MONITOR_COLUMNS-1:info.srWindow.Right-info.srWindow.Left),
            (SHORT)((info.srWindow.Bottom-info.srWindow.Top+1>MONITOR_ROWS)?MONITOR_ROWS-1:info.srWindow.Bottom-info.srWindow.Top)};
        (void)SetConsoleWindowInfo(output,TRUE,&smaller);
    }
    (void)SetConsoleScreenBufferSize(output,target);
    (void)SetConsoleWindowInfo(output,TRUE,&window);
    (void)FillConsoleOutputAttribute(output,MONITOR_NORMAL_ATTRIBUTE,MONITOR_COLUMNS*MONITOR_ROWS,(COORD){0,0},&written);
    (void)FillConsoleOutputCharacterW(output,L' ',MONITOR_COLUMNS*MONITOR_ROWS,(COORD){0,0},&written);
    (void)SetConsoleCursorPosition(output,(COORD){0,0});
}
static void restore_presentation(HANDLE output,const MONITOR_STATE *state)
{
    if (!state->console_saved) return;
    (void)SetConsoleWindowInfo(output,TRUE,&(SMALL_RECT){0,0,0,0});
    (void)SetConsoleScreenBufferSize(output,state->console.dwSize);
    (void)SetConsoleWindowInfo(output,TRUE,&state->console.srWindow);
    (void)SetConsoleTextAttribute(output,state->console.wAttributes);
}
static void render(HANDLE output,MONITOR_STATE *state,DTASKMGR_WORKER *items,ULONG count)
{
    ULONG index,row=0,visible,selected_index=0;
    static const WCHAR title[]=L"NTVDM Task Monitor";
    FILETIME now;
    WCHAR line[512],frame[MONITOR_COLUMNS+1];
    GetSystemTimeAsFileTime(&now);
    state->horizontal_limit=0;
    for (index=0;index<count;++index) {
        DWORD length;
        task_line(line,ARRAYSIZE(line),state,&items[index],&now);
        length=(DWORD)lstrlenW(line);
        if (length>MONITOR_INTERIOR && length-MONITOR_INTERIOR>state->horizontal_limit)
            state->horizontal_limit=length-MONITOR_INTERIOR;
        if (same_key(&items[index].key,&state->selected_key)) {
            selected_index=index;
            if (index<state->first_visible) state->first_visible=index;
            else if (index-state->first_visible>=MONITOR_BODY_ROWS)
                state->first_visible=index-MONITOR_BODY_ROWS+1;
        }
    }
    if (state->horizontal_offset>state->horizontal_limit) state->horizontal_offset=state->horizontal_limit;
    if (count<=MONITOR_BODY_ROWS) state->first_visible=0;
    else if (state->first_visible>count-MONITOR_BODY_ROWS) state->first_visible=count-MONITOR_BODY_ROWS;
    visible=count-state->first_visible;
    if (visible>MONITOR_BODY_ROWS) visible=MONITOR_BODY_ROWS;
    framed_rule(frame,L' ',L' ',L' ');
    CopyMemory(frame+(MONITOR_COLUMNS-(ARRAYSIZE(title)-1))/2,title,sizeof(title)-sizeof(WCHAR));
    render_line(output,(SHORT)row++,frame,MONITOR_TAB_ATTRIBUTE);
    framed_rule(frame,L'\x250C',L'\x2500',L'\x2510');
    render_framed_line(output,(SHORT)row++,frame,MONITOR_ACCENT_ATTRIBUTE);
    swprintf_s(line,ARRAYSIZE(line),L"  %-12s %-7s %-7s %-10s %-5s %s",
        L"PID",L"KIND",L"STATE",L"ELAPSED",L"STACK",L"TASK");
    framed_text(frame,L'\x2502',scrolled_text(line,state->horizontal_offset),L'\x2502');render_framed_line(output,(SHORT)row++,frame,MONITOR_ACCENT_ATTRIBUTE);
    framed_rule(frame,L'\x251C',L'\x2500',L'\x2524');render_framed_line(output,(SHORT)row++,frame,MONITOR_ACCENT_ATTRIBUTE);
    for (index=0;index<visible;++index) {
        const DTASKMGR_WORKER *item=&items[state->first_visible+index];
        task_line(line,ARRAYSIZE(line),state,item,&now);
        framed_text(frame,L'\x2502',scrolled_text(line,state->horizontal_offset),L'\x2502');
        render_framed_line(output,(SHORT)row++,frame,
            same_key(&item->key,&state->selected_key) ? MONITOR_SELECTED_ATTRIBUTE : MONITOR_NORMAL_ATTRIBUTE);
    }
    if (!count) {
        framed_text(frame,L'\x2502',state->status==ERROR_SUCCESS ? L"  No active tasks." : L"",L'\x2502');
        render_framed_line(output,(SHORT)row++,frame,MONITOR_NORMAL_ATTRIBUTE);
    }
    while (row<MONITOR_SCROLL_ROW) { framed_text(frame,L'\x2502',L"",L'\x2502');render_framed_line(output,(SHORT)row++,frame,MONITOR_NORMAL_ATTRIBUTE); }
    render_scrollbars(output,state,selected_index);++row;
    if (state->confirm_key.category) {
        confirmation_text(line,ARRAYSIZE(line),state);
        footer_text(frame,state->status,state->action_error,line);
    } else footer_text(frame,state->status,state->action_error,L"UP/DOWN=Select Task DEL=End Task ESC=EXIT");
    render_line(output,(SHORT)row++,frame,MONITOR_STATUS_ATTRIBUTE);
    state->rendered_rows=row;
}
static void clear_confirmation(MONITOR_STATE *state)
{
    state->confirm_pid=0;
    ZeroMemory(&state->confirm_key,sizeof(state->confirm_key));
}

static void render_trace(HANDLE output,MONITOR_STATE *state)
{
    common_rpc_management client={state->binding,state->process};
    WORKER_TRACE_NODE *nodes=NULL;
    ULONG coverage=0,count=0,index,row=0;
    DWORD error=common_rpc_worker_task_trace(&client,&state->trace_key,&coverage,&count,&nodes);
    WCHAR frame[MONITOR_COLUMNS+1],line[640];
    state->trace_count=count;
    if(count<=MONITOR_BODY_ROWS)state->trace_first=0;
    else if(state->trace_first>count-MONITOR_BODY_ROWS)state->trace_first=count-MONITOR_BODY_ROWS;
    framed_text(frame,L' ',L"  Worker task trace - read only",L' ');
    render_line(output,(SHORT)row++,frame,MONITOR_TAB_ATTRIBUTE);
    framed_rule(frame,L'\x250C',L'\x2500',L'\x2510');
    render_framed_line(output,(SHORT)row++,frame,MONITOR_ACCENT_ATTRIBUTE);
    swprintf_s(line,ARRAYSIZE(line),L"  RELATION SOURCE  KIND    PID     TASK   IMAGE");
    framed_text(frame,L'\x2502',line,L'\x2502');
    render_framed_line(output,(SHORT)row++,frame,MONITOR_ACCENT_ATTRIBUTE);
    framed_rule(frame,L'\x251C',L'\x2500',L'\x2524');
    render_framed_line(output,(SHORT)row++,frame,MONITOR_ACCENT_ATTRIBUTE);
    for(index=state->trace_first;index<count && row<MONITOR_SCROLL_ROW;++index) {
        const WORKER_TRACE_NODE *node=&nodes[index];
        swprintf_s(line,ARRAYSIZE(line),L"  %-8s %-7s %-7s %-7lu %-6lu %s%s",
            node->relation==TASK_TRACE_DIRECT ? L"DIRECT" : L"OBSERVED",
            node->source==TASK_TRACE_SOURCE_RECORD ? L"RECORD" : node->source==TASK_TRACE_SOURCE_HOOK ? L"HOOK" : L"DOS",
            node->kind==UINT32_MAX ? L"UNKNOWN" : kind_name(node->kind),
            node->process_id,node->task,node->parent ? L"  " : L"",node->image);
        framed_text(frame,L'\x2502',line,L'\x2502');
        render_framed_line(output,(SHORT)row++,frame,MONITOR_NORMAL_ATTRIBUTE);
    }
    if(!count) {
        swprintf_s(line,ARRAYSIZE(line),error ? L"  Trace unavailable (error %lu)." : L"  No active Direct tasks.",error);
        framed_text(frame,L'\x2502',line,L'\x2502');
        render_framed_line(output,(SHORT)row++,frame,MONITOR_NORMAL_ATTRIBUTE);
    }
    while(row<MONITOR_SCROLL_ROW) {
        framed_text(frame,L'\x2502',L"",L'\x2502');
        render_framed_line(output,(SHORT)row++,frame,MONITOR_NORMAL_ATTRIBUTE);
    }
    framed_text(frame,L'\x2502',coverage&TASK_TRACE_GAP_OBSERVATION ?
        L" Observed coverage incomplete; not proof of no descendants." : L"",L'\x2502');
    render_framed_line(output,(SHORT)row++,frame,MONITOR_ACCENT_ATTRIBUTE);
    footer_text(frame,error,0,coverage&TASK_TRACE_TRUNCATED ?
        L"ESC=Back UP/DOWN=Scroll - trace incomplete" : L"ESC=Back UP/DOWN=Scroll - read only");
    render_line(output,(SHORT)row,frame,MONITOR_STATUS_ATTRIBUTE);
    if(nodes)MIDL_user_free(nodes);
}
static void accept_snapshot(MONITOR_STATE *state,const DTASKMGR_WORKER *items,ULONG count)
{
    ULONG index;
    for(index=0;index<count;++index)
        if(same_key(&items[index].key,&state->selected_key))break;
    if(index==count) {
        /* A removed row selects its next neighbour, or the preceding last
         * row. Reordering a live node never changes selection by identity. */
        index=count ? (state->selected_row<count ? state->selected_row : count-1) : 0;
        if(count)state->selected_key=items[index].key;
        else ZeroMemory(&state->selected_key,sizeof(state->selected_key));
    }
    state->selected_row=index;
    if(state->confirm_key.category) {
        for(index=0;index<count;++index)
            if(same_key(&items[index].key,&state->confirm_key) &&
                (items[index].actions&MANAGEMENT_CAN_CLOSE))break;
        if(index==count)clear_confirmation(state);
    }
}
static DWORD refresh(MONITOR_STATE *state,DTASKMGR_WORKER **items,ULONG *count)
{
    ULONG result_count=0;
    DTASKMGR_WORKER *result=NULL;
    DWORD error=ERROR_SUCCESS;
    common_rpc_management management;
    *items=NULL;*count=0;
    if (!state->binding && !bind_basesrv(state)) {
        error=GetLastError();clear_confirmation(state);return error;
    }
    management.binding=state->binding;management.process=state->process;
    error=common_rpc_task_snapshot(&management,&result_count,&result);
    if (error) {
        /* With no authoritative snapshot, a queued kill cannot remain valid. */
        clear_confirmation(state);
        return error;
    }
    *items=result; *count=result_count;
    accept_snapshot(state,result,result_count);
    return ERROR_SUCCESS;
}
static DWORD close_selected_node(MONITOR_STATE *state)
{
    common_rpc_management management;
    if (!state->confirm_key.category) return ERROR_NOT_FOUND;
    management.binding=state->binding;management.process=state->process;
    return common_rpc_close_management_node(&management,&state->confirm_key);
}
/* One production dispatch path, also exercised by the Console fixture. TRUE
 * means exit; no-op/readonly rows cannot fall through to a worker close. */
static BOOL handle_key(MONITOR_STATE *state,const DTASKMGR_WORKER *items,
    ULONG count,WORD key,WCHAR character)
{
    if(state->trace_key.category) {
        if(key==VK_ESCAPE)ZeroMemory(&state->trace_key,sizeof(state->trace_key));
        else if(key==VK_UP && state->trace_first)--state->trace_first;
        else if(key==VK_DOWN && state->trace_first+MONITOR_BODY_ROWS<state->trace_count)++state->trace_first;
        return FALSE; /* No modal key can dispatch management close. */
    }
    if(state->confirm_key.category) {
        if(character==L'n' || character==L'N' || key==VK_ESCAPE)clear_confirmation(state);
        else if(character==L'y' || character==L'Y') {
            state->action_error=close_selected_node(state);
            clear_confirmation(state);
        }
        return FALSE;
    }
    if(key==VK_ESCAPE)return TRUE;
    accept_snapshot(state,items,count);
    if(key==VK_UP && count && state->selected_row)--state->selected_row;
    if(key==VK_DOWN && count && state->selected_row+1<count)++state->selected_row;
    if(count)state->selected_key=items[state->selected_row].key;
    if(key==VK_LEFT && state->horizontal_offset)--state->horizontal_offset;
    if(key==VK_RIGHT && state->horizontal_offset<state->horizontal_limit)++state->horizontal_offset;
    if(key==VK_DELETE) {
        state->action_error=ERROR_NOT_SUPPORTED;
        if(count && (items[state->selected_row].actions&MANAGEMENT_CAN_CLOSE)) {
            state->confirm_key=items[state->selected_row].key;
            state->confirm_pid=items[state->selected_row].process_id;
            state->action_error=ERROR_SUCCESS;
        }
    }
    if(key==VK_RETURN && count && state->selected_key.category==MANAGEMENT_WORKER) {
        state->trace_key=state->selected_key;state->trace_first=0;
    }
    return FALSE;
}
int wmain(void)
{
    MONITOR_STATE state;
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
        if(state.trace_key.category)render_trace(output,&state);
        else render(output,&state,items,count);
        wait=WaitForSingleObject(input,750);
        if (wait==WAIT_OBJECT_0) {
            INPUT_RECORD record; DWORD read=0;
            if (ReadConsoleInputW(input,&record,1,&read) && record.EventType==KEY_EVENT && record.Event.KeyEvent.bKeyDown) {
                WORD key=record.Event.KeyEvent.wVirtualKeyCode;
                if(handle_key(&state,items,count,key,record.Event.KeyEvent.uChar.UnicodeChar)) {
                    if(items)MIDL_user_free(items);
                    break;
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
