#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "console_snapshot.h"
#include "input_milestone.h"

int main(void)
{
    observer_text_view view = {0};
    unsigned failed = 0;
#define CHECK(value) do { if (!(value)) { ++failed; printf("FAIL line=%d\n",__LINE__); } } while (0)
    view.columns=80; view.rows=25; view.cursor_row=24; view.cursor_column=9;
    memset(view.cells,' ',sizeof(view.cells));
    memcpy(view.cells+24*80,"Z:\\WINNT>",9);
    CHECK(observer_prompt_echo(&view,""));
    CHECK(!observer_prompt_echo(&view,"mem")); /* queue delivery is not echo */
    memcpy(view.cells+24*80+9,"mem",3); view.cursor_column=12;
    CHECK(observer_prompt_echo(&view,"mem"));
    memcpy(view.cells+24*80+9,"MEM",3);
    CHECK(!observer_prompt_echo(&view,"mem")); /* do not forgive wrong key state */
    memcpy(view.cells+24*80+9,"mem",3);
    CHECK(!observer_prompt_echo(&view,"")); /* armed input is not completion */
    CHECK(!observer_prompt_echo(&view,"me")); /* partial consumption */
    view.cursor_row=23;
    CHECK(!observer_prompt_echo(&view,"mem")); /* old row must not acknowledge */
    view.cursor_row=24; view.cursor_column=11;
    CHECK(!observer_prompt_echo(&view,"mem")); /* cursor not committed */
    view.cursor_column=12; view.cells[24*80+40]='x';
    CHECK(!observer_prompt_echo(&view,"mem")); /* dirty input tail */
    view.cursor_row=25;
    CHECK(!observer_prompt_echo(&view,"mem")); /* invalid cursor */
    view.cursor_row=24;
    memcpy(view.cells+10*80,"Welcome to the MS-DOS Editor",28);
    CHECK(observer_view_contains(&view,"Welcome to"));
    memset(view.cells+10*80,' ',80);
    CHECK(!observer_view_contains(&view,"Welcome to"));
    /* Reject an HWND from a different PID before any remote-memory copy. */
    CHECK(!observer_text_read(INVALID_HANDLE_VALUE,GetDesktopWindow(),GetCurrentProcessId(),&view));
    if(failed)return 1;
    puts("PASS fresh echo, partial input, stale prompt, cursor/tail and Window PID negatives");
    return 0;
}
