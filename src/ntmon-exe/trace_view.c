#include "trace_view.h"
#include "common/protocol/management.h"
#include <stdio.h>
BOOL monitor_trace_order(const common_task_trace_node *nodes,uint32_t count,
    monitor_trace_row *rows,uint32_t *visible)
{
    uint32_t index,insert;
    if(!visible)return FALSE;
    *visible=0;
    if(count>TASK_TRACE_MAX_NODES || (count && (!nodes || !rows)))return FALSE;
    for(index=0;index<count;++index) {
        if(nodes[index].state==TASK_TRACE_EXITED)continue;
        insert=(*visible)++;
        /* Unknown order stays stable after proved entries. Do not infer
         * chronology from PID/PSP, ancestry or process creation time. */
        while(insert && nodes[index].entered_order &&
            (!nodes[rows[insert-1].index].entered_order ||
             nodes[rows[insert-1].index].entered_order>nodes[index].entered_order)) {
            rows[insert]=rows[insert-1];--insert;
        }
        rows[insert].index=index;
    }
    return TRUE;
}
void monitor_trace_primary(const common_task_trace_node *node,uint32_t ordinal,
    uint64_t now,WCHAR *text,size_t capacity)
{
    WCHAR elapsed[32];
    const WCHAR *kind=node->kind==MANAGEMENT_KIND_DOS ? L"DOS" :
        node->kind==MANAGEMENT_KIND_WIN16 ? L"WIN16" :
        node->kind==MANAGEMENT_KIND_WIN32 ? L"WIN32" :
        node->kind==MANAGEMENT_KIND_WIN64 ? L"WIN64" : L"UNKNOWN";
    if(node->created_filetime) {
        uint64_t seconds=now>node->created_filetime ? (now-node->created_filetime)/10000000ULL : 0;
        swprintf_s(elapsed,ARRAYSIZE(elapsed),L"%02llu:%02llu:%02llu",seconds/3600,(seconds/60)%60,seconds%60);
    } else lstrcpyW(elapsed,L"--:--:--");
    swprintf_s(text,capacity,L"%3u %-8s %-7s %-10s %s",ordinal,
        node->relation==TASK_TRACE_DIRECT ? L"DIRECT" :
        node->relation==TASK_TRACE_OBSERVED ? L"OBSERVED" : L"UNKNOWN",
        kind,elapsed,
        node->image[0] ? node->image : L"<UNKNOWN>");
}
