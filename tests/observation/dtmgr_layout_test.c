/* Native Console fixture: exercises the product renderer in a private buffer.
 * No broker, guest session or visible screen-buffer switch is needed. */
#define wmain dtmgr_product_main
#include "../../src/dtmgr-exe/main.c"
#undef wmain
#include <assert.h>

static CHAR_INFO screen[DTMGR_COLUMNS*DTMGR_ROWS];
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
    DTASKMGR_STATE state={0};
    DTASKMGR_WORKER items[24]={0};
    ULONG i;
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
    cell(0,0,L' ',DTMGR_TAB_ATTRIBUTE);
    cell(31,0,L'N',DTMGR_TAB_ATTRIBUTE);
    cell(79,0,L' ',DTMGR_TAB_ATTRIBUTE);
    cell(0,1,L'\x250c',DTMGR_NORMAL_ATTRIBUTE);
    cell(79,1,L'\x2510',DTMGR_NORMAL_ATTRIBUTE);
    cell(0,3,L'\x251c',DTMGR_NORMAL_ATTRIBUTE);
    cell(79,3,L'\x2191',DTMGR_SCROLL_ATTRIBUTE);
    cell(79,4,L' ',DTMGR_THUMB_ATTRIBUTE);
    cell(79,5,L'\x2591',DTMGR_SCROLL_ATTRIBUTE);
    cell(79,6,L'\x2591',DTMGR_SCROLL_ATTRIBUTE);
    cell(79,22,L'\x2193',DTMGR_SCROLL_ATTRIBUTE);
    cell(0,23,L'\x2502',DTMGR_NORMAL_ATTRIBUTE);
    cell(79,23,L'\x2502',DTMGR_NORMAL_ATTRIBUTE);
    cell(1,23,L'\x2190',DTMGR_SCROLL_ATTRIBUTE);
    cell(2,23,L' ',DTMGR_THUMB_ATTRIBUTE);
    cell(3,23,L'\x2591',DTMGR_SCROLL_ATTRIBUTE);
    cell(78,23,L'\x2192',DTMGR_SCROLL_ATTRIBUTE);
    cell(57,24,L'B',DTMGR_STATUS_ATTRIBUTE);
    for (i=0;i<24;++i) {
        items[i].sequence=i+1;
        items[i].kind=1;
        lstrcpyW(items[i].image,L"COMMAND.COM");
    }
    for (i=0;i<200;++i) items[23].image[i]=L'X';
    items[23].image[200]=0;
    state.status=ERROR_SUCCESS;
    state.selected_sequence=1;
    render(output,&state,items,3);
    capture(output);
    cell(79,4,L' ',DTMGR_THUMB_ATTRIBUTE);
    state.selected_sequence=2;
    render(output,&state,items,3);
    capture(output);
    assert(state.first_visible==0);
    cell(1,5,L'>',DTMGR_SELECTED_ATTRIBUTE);
    cell(79,4,L'\x2591',DTMGR_SCROLL_ATTRIBUTE);
    cell(79,5,L' ',DTMGR_THUMB_ATTRIBUTE);
    state.selected_sequence=3;
    render(output,&state,items,3);
    capture(output);
    cell(79,5,L'\x2591',DTMGR_SCROLL_ATTRIBUTE);
    cell(79,6,L' ',DTMGR_THUMB_ATTRIBUTE);
    state.selected_sequence=2;
    render(output,&state,items,24);
    capture(output);
    assert(state.first_visible==0);
    cell(79,5,L' ',DTMGR_THUMB_ATTRIBUTE);
    cell(79,6,L'\x2591',DTMGR_SCROLL_ATTRIBUTE);
    state.selected_sequence=24;
    render(output,&state,items,24);
    capture(output);
    assert(state.first_visible==5);
    assert(state.horizontal_limit>0);
    cell(1,22,L'>',DTMGR_SELECTED_ATTRIBUTE);
    cell(79,21,L' ',DTMGR_THUMB_ATTRIBUTE);
    state.horizontal_offset=state.horizontal_limit;
    render(output,&state,items,24);
    capture(output);
    cell(77,23,L' ',DTMGR_THUMB_ATTRIBUTE);
    cell(78,22,L'X',DTMGR_SELECTED_ATTRIBUTE);
    state.selected_sequence=1;
    state.confirm_sequence=1;
    state.confirm_task_count=2;
    render(output,&state,items,1);
    capture(output);
    assert(state.first_visible==0 && state.horizontal_offset==0);
    cell(1,4,L'>',DTMGR_SELECTED_ATTRIBUTE);
    cell(79,3,L'\x2191',DTMGR_SCROLL_ATTRIBUTE);
    cell(79,4,L' ',DTMGR_THUMB_ATTRIBUTE);
    cell(1,24,L'E',DTMGR_STATUS_ATTRIBUTE);
    state.confirm_sequence=0;
    render(output,&state,NULL,0);
    capture(output);
    cell(3,4,L'N',DTMGR_NORMAL_ATTRIBUTE);
    cell(57,24,L'R',DTMGR_STATUS_ATTRIBUTE);
    CloseHandle(output);
    if (allocated) FreeConsole();
    puts("PASS: EDIT frame, four arrows, colors, 25 rows, overflow selection, horizontal extent, shrink, empty and confirmation");
    return 0;
}
