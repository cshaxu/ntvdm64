#ifndef NTMON_TRACE_VIEW_H
#define NTMON_TRACE_VIEW_H
#include <windows.h>
#include "common/protocol/task_trace.h"
typedef struct monitor_trace_row {uint32_t index;} monitor_trace_row;
/* Presentation only: filter confirmed exits, then order by broker entry. */
BOOL monitor_trace_order(const common_task_trace_node *,uint32_t,monitor_trace_row *,uint32_t *);
void monitor_trace_primary(const common_task_trace_node *,uint32_t,uint64_t,WCHAR *,size_t);
#endif
