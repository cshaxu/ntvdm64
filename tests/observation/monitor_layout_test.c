/* Native Console fixture: exercises the product renderer in a private buffer.
 * No broker, guest session or visible screen-buffer switch is needed. */
#define wmain monitor_product_main
#include "../../src/ntmon-exe/main.c"
#undef wmain
#include <assert.h>

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
        native.state=8;native.stack_depth=2;
        native.started_filetime=((uint64_t)now.dwHighDateTime<<32)|now.dwLowDateTime;
        lstrcpyW(native.image,L"ntw32.exe");
        task_line(line,ARRAYSIZE(line),&state,&native,&now);
        assert(wcsstr(line,L"WIN32") && wcsstr(line,L"1234") &&
            wcsstr(line,L"2") && !wcsstr(line,L"MEMBERS=") && wcsstr(line,L"ntw32.exe"));
        state.confirm_pid=1234;state.confirm_task_count=2;
        confirmation_text(line,ARRAYSIZE(line),&state);
        assert(wcsstr(line,L"End worker 1234 and all its 2 tasks") && !wcsstr(line,L"members"));
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
        items[i].kind=0;
        lstrcpyW(items[i].image,L"COMMAND.COM");
    }
    for (i=0;i<200;++i) items[23].image[i]=L'X';
    items[23].image[200]=0;
    state.status=ERROR_SUCCESS;
    state.selected_pid=1;
    render(output,&state,items,3);
    capture(output);
    cell(79,4,L' ',MONITOR_THUMB_ATTRIBUTE);
    state.selected_pid=2;
    render(output,&state,items,3);
    capture(output);
    assert(state.first_visible==0);
    cell(1,5,L'>',MONITOR_SELECTED_ATTRIBUTE);
    cell(79,4,L'\x2591',MONITOR_SCROLL_ATTRIBUTE);
    cell(79,5,L' ',MONITOR_THUMB_ATTRIBUTE);
    state.selected_pid=3;
    render(output,&state,items,3);
    capture(output);
    cell(79,5,L'\x2591',MONITOR_SCROLL_ATTRIBUTE);
    cell(79,6,L' ',MONITOR_THUMB_ATTRIBUTE);
    state.selected_pid=2;
    render(output,&state,items,24);
    capture(output);
    assert(state.first_visible==0);
    cell(79,5,L' ',MONITOR_THUMB_ATTRIBUTE);
    cell(79,6,L'\x2591',MONITOR_SCROLL_ATTRIBUTE);
    state.selected_pid=24;
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
    state.selected_pid=1;
    state.confirm_pid=1;
    state.confirm_task_count=2;
    render(output,&state,items,1);
    capture(output);
    assert(state.first_visible==0 && state.horizontal_offset==0);
    cell(1,4,L'>',MONITOR_SELECTED_ATTRIBUTE);
    cell(79,3,L'\x2191',MONITOR_SCROLL_ATTRIBUTE);
    cell(79,4,L' ',MONITOR_THUMB_ATTRIBUTE);
    cell(1,24,L'E',MONITOR_STATUS_ATTRIBUTE);
    state.confirm_pid=0;
    render(output,&state,NULL,0);
    capture(output);
    cell(3,4,L'N',MONITOR_NORMAL_ATTRIBUTE);
    cell(57,24,L'R',MONITOR_STATUS_ATTRIBUTE);
    CloseHandle(output);
    if (allocated) FreeConsole();
    puts("PASS: EDIT frame, four arrows, colors, 25 rows, overflow selection, horizontal extent, shrink, empty and confirmation");
    return 0;
}
