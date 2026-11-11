#include "ntmon-exe/trace_view.h"
#include <assert.h>
#include <stdio.h>
#include <wchar.h>
int main(void)
{
    common_task_trace_node nodes[5]={0};monitor_trace_row rows[5];
    WCHAR text[600];uint32_t visible=99;
    nodes[0].entered_order=3;nodes[0].parent=1;
    nodes[1].entered_order=1;nodes[1].state=TASK_TRACE_UNCERTAIN;
    nodes[2].entered_order=2;nodes[2].state=TASK_TRACE_EXITED;
    nodes[3].entered_order=4;
    /* Unknown terminal state stays visible. An exited ancestor cannot
     * reorder its live child. There is no historical-record display. */
    assert(monitor_trace_order(nodes,5,rows,&visible) && visible==4);
    assert(rows[0].index==1 && rows[1].index==0 && rows[2].index==3 && rows[3].index==4);
    assert(!monitor_trace_order(nodes,TASK_TRACE_MAX_NODES+1,rows,&visible));
    assert(!visible && monitor_trace_order(NULL,0,NULL,&visible) && !visible);
    nodes[0].kind=3;nodes[0].relation=TASK_TRACE_OBSERVED;
    nodes[0].created_filetime=10000000;lstrcpyW(nodes[0].image,L"C:\\TEST\\APP.EXE");
    monitor_trace_primary(&nodes[0],2,660000000,text,ARRAYSIZE(text));
    assert(wcsstr(text,L"2 OBSERVED") && wcsstr(text,L"WIN64") &&
        wcsstr(text,L"00:01:05") && wcsstr(text,L"C:\\TEST\\APP.EXE"));
    assert(!wcsstr(text,L"PID=") && !wcsstr(text,L"HISTORY"));
    nodes[0].created_filetime=0;
    monitor_trace_primary(&nodes[0],2,660000000,text,ARRAYSIZE(text));
    assert(wcsstr(text,L"--:--:--"));
    puts("PASS live/unconfirmed tasks, broker entry order, elapsed/path and no diagnostic/history UI");
    return 0;
}
