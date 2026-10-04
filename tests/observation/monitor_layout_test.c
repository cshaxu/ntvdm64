/* Native Console fixture: exercises the product renderer in a private buffer.
 * No broker, guest session or visible screen-buffer switch is needed. */
#define wmain monitor_product_main
#define common_rpc_task_snapshot fixture_task_snapshot
#define common_rpc_close_management_node fixture_close_node
#include "../../src/ntmon-exe/main.c"
#undef wmain
#include <assert.h>

static const DTASKMGR_WORKER *reply_items;
static ULONG reply_count,close_calls;
static DWORD reply_error,close_error;
static DTASKMGR_KEY closed_key;
DWORD fixture_task_snapshot(const common_rpc_management *client,ULONG *count,DTASKMGR_WORKER **items)
{
    assert(client->binding && client->process);
    *count=0;*items=NULL;
    if(reply_error)return reply_error;
    if(reply_count) {
        *items=MIDL_user_allocate(reply_count*sizeof(**items));assert(*items);
        CopyMemory(*items,reply_items,reply_count*sizeof(**items));
    }
    *count=reply_count;return ERROR_SUCCESS;
}
DWORD fixture_close_node(const common_rpc_management *client,const DTASKMGR_KEY *key)
{
    assert(client->binding && client->process);
    ++close_calls;closed_key=*key;return close_error;
}
static void selection_and_input(void)
{
    MONITOR_STATE state={0};
    DTASKMGR_WORKER rows[4]={0},reordered[4],*copy=NULL;
    ULONG count=0,index;
    state.binding=(RPC_BINDING_HANDLE)(ULONG_PTR)1;state.process=(HANDLE)(ULONG_PTR)1;
    for(index=0;index<4;++index) {
        rows[index].key=(DTASKMGR_KEY){17,MANAGEMENT_WORKER,index+1,0};
        rows[index].process_id=100+index;rows[index].actions=MANAGEMENT_CAN_CLOSE;
    }
    rows[0].key.category=MANAGEMENT_FRONTEND;
    rows[1].depth=1;rows[1].parent=rows[0].key;
    rows[2].key.category=MANAGEMENT_WOW_TASK;rows[2].actions=0;
    rows[3].key.category=MANAGEMENT_GUI_TARGET;rows[3].key.object=19;
    reply_items=rows;reply_count=4;
    assert(!refresh(&state,&copy,&count) && count==4 && same_key(&state.selected_key,&rows[0].key));
    MIDL_user_free(copy);copy=NULL;
    assert(!handle_key(&state,rows,4,VK_UP,0) && state.selected_row==0);
    assert(!handle_key(&state,rows,4,VK_DOWN,0) && same_key(&state.selected_key,&rows[1].key));
    assert(!handle_key(&state,rows,4,VK_DOWN,0) && state.selected_row==2);
    assert(!handle_key(&state,rows,4,VK_DELETE,0) && state.action_error==ERROR_NOT_SUPPORTED);
    assert(!handle_key(&state,rows,4,'Y',L'y') && !close_calls && !state.confirm_key.category);
    assert(!handle_key(&state,rows,4,VK_DOWN,0) && state.selected_row==3);
    assert(!handle_key(&state,rows,4,VK_DOWN,0) && state.selected_row==3);
    assert(!handle_key(&state,rows,4,VK_DELETE,0) && same_key(&state.confirm_key,&rows[3].key));
    assert(!handle_key(&state,rows,4,VK_ESCAPE,0) && !state.confirm_key.category && !close_calls);
    assert(!handle_key(&state,rows,4,VK_DELETE,0));
    assert(!handle_key(&state,rows,4,'N',L'n') && !state.confirm_key.category && !close_calls);
    assert(!handle_key(&state,rows,4,VK_DELETE,0));
    close_error=ERROR_TIMEOUT;
    assert(!handle_key(&state,rows,4,'Y',L'Y') && close_calls==1 &&
        same_key(&closed_key,&rows[3].key) && state.action_error==ERROR_TIMEOUT && !state.confirm_key.category);
    close_error=0;
    assert(!handle_key(&state,rows,4,VK_DELETE,0));
    assert(!handle_key(&state,rows,4,'Y',L'y') && close_calls==2 && !state.action_error);
    reordered[0]=rows[3];reordered[1]=rows[0];reordered[2]=rows[1];reordered[3]=rows[2];
    accept_snapshot(&state,reordered,4);
    assert(state.selected_row==0 && same_key(&state.selected_key,&rows[3].key));
    assert(!handle_key(&state,reordered,4,VK_DOWN,0));
    assert(!handle_key(&state,reordered,4,VK_DELETE,0));
    assert(same_key(&state.confirm_key,&rows[0].key));
    reordered[1].actions=0;accept_snapshot(&state,reordered,4);
    assert(!state.confirm_key.category && close_calls==2);
    reordered[1].actions=MANAGEMENT_CAN_CLOSE;
    assert(!handle_key(&state,reordered,4,VK_DELETE,0));
    ++reordered[1].key.generation; /* Same PID is a replacement, not a target. */
    accept_snapshot(&state,reordered,4);
    assert(!state.confirm_key.category && state.selected_row==1 &&
        same_key(&state.selected_key,&reordered[1].key));
    state.selected_key=rows[1].key;accept_snapshot(&state,rows,4);
    assert(state.selected_row==1);
    accept_snapshot(&state,rows+2,2); /* Removed row selects the nearest next row. */
    assert(state.selected_row==1 && same_key(&state.selected_key,&rows[3].key));
    accept_snapshot(&state,rows,1); /* Removed tail clamps to surviving last row. */
    assert(state.selected_row==0 && same_key(&state.selected_key,&rows[0].key));
    assert(!handle_key(&state,rows,1,VK_DELETE,0));
    reply_error=RPC_S_SERVER_UNAVAILABLE;reply_items=NULL;reply_count=0;
    copy=(DTASKMGR_WORKER *)(ULONG_PTR)1;count=1;
    assert(refresh(&state,&copy,&count)==reply_error && !copy && !count && !state.confirm_key.category);
    reply_error=0;
    assert(!refresh(&state,&copy,&count) && !count && !copy && !state.selected_key.category);
    assert(!handle_key(&state,NULL,0,VK_DELETE,0) && state.action_error==ERROR_NOT_SUPPORTED);
    assert(!handle_key(&state,NULL,0,VK_DOWN,0) && !state.selected_key.category);
    assert(handle_key(&state,NULL,0,VK_ESCAPE,0));
    state.horizontal_limit=2;
    assert(!handle_key(&state,NULL,0,VK_RIGHT,0) && state.horizontal_offset==1);
    assert(!handle_key(&state,NULL,0,VK_RIGHT,0) && state.horizontal_offset==2);
    assert(!handle_key(&state,NULL,0,VK_RIGHT,0) && state.horizontal_offset==2);
    assert(!handle_key(&state,NULL,0,VK_LEFT,0) && state.horizontal_offset==1);
}

static CHAR_INFO screen[MONITOR_COLUMNS*MONITOR_ROWS];
static void capture(HANDLE output)
{
    SMALL_RECT area={0,0,79,24};
    assert(ReadConsoleOutputW(output,screen,(COORD){80,25},(COORD){0,0},&area));
}
static void cell(int x,int y,WCHAR glyph,WORD attribute)
{
    assert(screen[y*80+x].Char.UnicodeChar==glyph);
    assert(screen[y*80+x].Attributes==attribute);
}
int wmain(void)
{
    HANDLE output;
    MONITOR_STATE state={0};
    DTASKMGR_WORKER items[24]={0};
    ULONG i;
    {
        DTASKMGR_WORKER native={0};FILETIME now;WCHAR line[512];
        GetSystemTimeAsFileTime(&now);
        native.kind=2;native.process_id=1234;
        native.key=(DTASKMGR_KEY){17,MANAGEMENT_WORKER,1234,0};
        native.state=8;native.stack_depth=2;
        native.started_filetime=((uint64_t)now.dwHighDateTime<<32)|now.dwLowDateTime;
        lstrcpyW(native.image,L"ntvwm.exe");
        task_line(line,ARRAYSIZE(line),&state,&native,&now);
        assert(wcsstr(line,L"WIN32") && wcsstr(line,L"1234") &&
            wcsstr(line,L"2") && !wcsstr(line,L"MEMBERS=") && wcsstr(line,L"ntvwm.exe"));
        state.confirm_pid=1234;state.confirm_key=native.key;
        confirmation_text(line,ARRAYSIZE(line),&state);
        assert(wcsstr(line,L"End worker 1234") && !wcsstr(line,L"members"));
        ZeroMemory(&state,sizeof(state));
    }
    BOOL allocated=AllocConsole();
    output=CreateConsoleScreenBuffer(GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,CONSOLE_TEXTMODE_BUFFER,NULL);
    assert(output!=INVALID_HANDLE_VALUE);
    assert(SetConsoleWindowInfo(output,TRUE,&(SMALL_RECT){0,0,0,0}));
    assert(SetConsoleScreenBufferSize(output,(COORD){80,25}));
    state.status=RPC_S_SERVER_UNAVAILABLE;
    render(output,&state,NULL,0);
    capture(output);
    assert(state.rendered_rows==25);
    cell(0,0,L' ',MONITOR_TAB_ATTRIBUTE);
    cell(31,0,L'N',MONITOR_TAB_ATTRIBUTE);
    for (i=0;i<18;++i)
        cell(31+i,0,L"NTVDM Task Monitor"[i],MONITOR_TAB_ATTRIBUTE);
    cell(79,0,L' ',MONITOR_TAB_ATTRIBUTE);
    cell(0,1,L'\x250c',MONITOR_NORMAL_ATTRIBUTE);
    cell(79,1,L'\x2510',MONITOR_NORMAL_ATTRIBUTE);
    cell(0,3,L'\x251c',MONITOR_NORMAL_ATTRIBUTE);
    cell(79,3,L'\x2191',MONITOR_SCROLL_ATTRIBUTE);
    cell(79,4,L' ',MONITOR_THUMB_ATTRIBUTE);
    cell(79,5,L'\x2591',MONITOR_SCROLL_ATTRIBUTE);
    cell(79,6,L'\x2591',MONITOR_SCROLL_ATTRIBUTE);
    cell(79,22,L'\x2193',MONITOR_SCROLL_ATTRIBUTE);
    cell(0,23,L'\x2502',MONITOR_NORMAL_ATTRIBUTE);
    cell(79,23,L'\x2502',MONITOR_NORMAL_ATTRIBUTE);
    cell(1,23,L'\x2190',MONITOR_SCROLL_ATTRIBUTE);
    cell(2,23,L' ',MONITOR_THUMB_ATTRIBUTE);
    cell(3,23,L'\x2591',MONITOR_SCROLL_ATTRIBUTE);
    cell(78,23,L'\x2192',MONITOR_SCROLL_ATTRIBUTE);
    cell(57,24,L'B',MONITOR_STATUS_ATTRIBUTE);
    for (i=0;i<24;++i) {
        items[i].process_id=i+1;
        items[i].key=(DTASKMGR_KEY){17,MANAGEMENT_WORKER,i+1,0};
        items[i].kind=0;
        lstrcpyW(items[i].image,L"COMMAND.COM");
    }
    for (i=0;i<200;++i) items[23].image[i]=L'X';
    items[23].image[200]=0;
    state.status=ERROR_SUCCESS;
    state.selected_key=items[0].key;
    render(output,&state,items,3);
    capture(output);
    cell(79,4,L' ',MONITOR_THUMB_ATTRIBUTE);
    state.selected_key=items[1].key;
    render(output,&state,items,3);
    capture(output);
    assert(state.first_visible==0);
    cell(1,5,L'>',MONITOR_SELECTED_ATTRIBUTE);
    cell(79,4,L'\x2591',MONITOR_SCROLL_ATTRIBUTE);
    cell(79,5,L' ',MONITOR_THUMB_ATTRIBUTE);
    state.selected_key=items[2].key;
    render(output,&state,items,3);
    capture(output);
    cell(79,5,L'\x2591',MONITOR_SCROLL_ATTRIBUTE);
    cell(79,6,L' ',MONITOR_THUMB_ATTRIBUTE);
    state.selected_key=items[1].key;
    render(output,&state,items,24);
    capture(output);
    assert(state.first_visible==0);
    cell(79,5,L' ',MONITOR_THUMB_ATTRIBUTE);
    cell(79,6,L'\x2591',MONITOR_SCROLL_ATTRIBUTE);
    state.selected_key=items[23].key;
    render(output,&state,items,24);
    capture(output);
    assert(state.first_visible==5);
    assert(state.horizontal_limit>0);
    cell(1,22,L'>',MONITOR_SELECTED_ATTRIBUTE);
    cell(79,21,L' ',MONITOR_THUMB_ATTRIBUTE);
    state.horizontal_offset=state.horizontal_limit;
    render(output,&state,items,24);
    capture(output);
    cell(77,23,L' ',MONITOR_THUMB_ATTRIBUTE);
    cell(78,22,L'X',MONITOR_SELECTED_ATTRIBUTE);
    state.selected_key=items[0].key;
    state.confirm_pid=1;
    state.confirm_key=items[0].key;
    render(output,&state,items,1);
    capture(output);
    assert(state.first_visible==0 && state.horizontal_offset==0);
    cell(1,4,L'>',MONITOR_SELECTED_ATTRIBUTE);
    cell(79,3,L'\x2191',MONITOR_SCROLL_ATTRIBUTE);
    cell(79,4,L' ',MONITOR_THUMB_ATTRIBUTE);
    cell(1,24,L'E',MONITOR_STATUS_ATTRIBUTE);
    state.confirm_pid=0;
    ZeroMemory(&state.confirm_key,sizeof(state.confirm_key));
    render(output,&state,NULL,0);
    capture(output);
    cell(3,4,L'N',MONITOR_NORMAL_ATTRIBUTE);
    cell(57,24,L'R',MONITOR_STATUS_ATTRIBUTE);
    {
        const WCHAR footer[]=L"UP/DOWN=Select Task DEL=End Task ESC=EXIT";
        DTASKMGR_WORKER mixed[6]={0};
        for(i=0;i<ARRAYSIZE(footer)-1;++i)cell(1+i,24,footer[i],MONITOR_STATUS_ATTRIBUTE);
        for(i=0;i<6;++i) {
            mixed[i].key=(DTASKMGR_KEY){17,MANAGEMENT_WORKER,i+1,0};
            mixed[i].process_id=i+100;mixed[i].display_state=MANAGEMENT_BUSY;
        }
        mixed[0].key.category=MANAGEMENT_FRONTEND;
        mixed[0].display_state=MANAGEMENT_MISSING;
        mixed[1].depth=1;mixed[1].process_id=MAXDWORD;
        mixed[2].kind=1;
        mixed[3].depth=1;mixed[3].kind=1;mixed[3].process_id=0;
        mixed[3].key.category=MANAGEMENT_WOW_TASK;
        mixed[4].kind=2;mixed[4].key.category=MANAGEMENT_GUI_TARGET;
        mixed[5].depth=1;mixed[5].kind=2;
        lstrcpyW(mixed[3].image,L"WINMINE.EXE");
        ZeroMemory(&state,sizeof(state));state.selected_key=mixed[3].key;
        render(output,&state,mixed,6);capture(output);
        cell(16,2,L'K',MONITOR_NORMAL_ATTRIBUTE); /* Fixed KIND column. */
        cell(16,4,L'C',MONITOR_NORMAL_ATTRIBUTE);
        cell(16,5,L'D',MONITOR_NORMAL_ATTRIBUTE);
        cell(16,6,L'W',MONITOR_NORMAL_ATTRIBUTE);
        cell(16,7,L'W',MONITOR_SELECTED_ATTRIBUTE);
        cell(16,8,L'W',MONITOR_NORMAL_ATTRIBUTE);
        cell(16,9,L'W',MONITOR_NORMAL_ATTRIBUTE);
        cell(24,4,L'M',MONITOR_NORMAL_ATTRIBUTE);
        cell(3,5,L' ',MONITOR_NORMAL_ATTRIBUTE);
        cell(5,5,L'4',MONITOR_NORMAL_ATTRIBUTE); /* Full 10-digit PID fits. */
        cell(5,7,L'-',MONITOR_SELECTED_ATTRIBUTE);
        cell(32,7,L'-',MONITOR_SELECTED_ATTRIBUTE); /* Unknown WOW elapsed. */
        cell(43,7,L'-',MONITOR_SELECTED_ATTRIBUTE); /* No invented stack. */
        cell(49,7,L'W',MONITOR_SELECTED_ATTRIBUTE);
    }
    {
        WCHAR line[512];FILETIME now;
        DTASKMGR_WORKER child={0};
        GetSystemTimeAsFileTime(&now);
        child.key=(DTASKMGR_KEY){17,MANAGEMENT_WOW_TASK,9,12};
        child.parent=(DTASKMGR_KEY){17,MANAGEMENT_WORKER,9,0};
        child.depth=1;child.kind=1;child.display_state=MANAGEMENT_BUSY;
        lstrcpyW(child.image,L"WINMINE.EXE");
        state.selected_key=child.key;
        task_line(line,ARRAYSIZE(line),&state,&child,&now);
        assert(line[0]==L'>' && wcsstr(line,L"WIN16") && wcsstr(line,L"BUSY") &&
            wcsstr(line,L"--:--:--") && !wcsstr(line,L"UNBOUND"));
        ++state.selected_key.object;
        task_line(line,ARRAYSIZE(line),&state,&child,&now);
        assert(line[0]==L' '); /* Same PID/task owner is not same node identity. */
    }
    CloseHandle(output);
    if (allocated) FreeConsole();
    selection_and_input();
    puts("PASS: aligned mixed tree, exact title/footer, EDIT frame/colors/25 rows, scrolling, stable refresh/selection and production UP/DOWN/DEL/ESC dispatch");
    return 0;
}
