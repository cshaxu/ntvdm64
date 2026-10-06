/* Read-only facts. None of these values grants execution or close authority. */
#ifndef COMMON_PROTOCOL_TASK_TRACE_H
#define COMMON_PROTOCOL_TASK_TRACE_H
#include <stdint.h>
#include <wchar.h>
#define TASK_TRACE_MAX_NODES 256u
#define TASK_TRACE_DIRECT 1u
#define TASK_TRACE_OBSERVED 2u
#define TASK_TRACE_SOURCE_RECORD 1u
#define TASK_TRACE_SOURCE_HOOK 2u
#define TASK_TRACE_SOURCE_DOS 3u
#define TASK_TRACE_LIVE 1u
#define TASK_TRACE_EXITED 2u
#define TASK_TRACE_UNCERTAIN 3u
#define TASK_TRACE_GAP_OBSERVATION 1u
#define TASK_TRACE_TRUNCATED 2u
typedef struct common_task_trace_node {
    uint64_t node,parent;
    uint32_t relation,source,kind,state;
    uint32_t process_id,task,flags,reserved;
    wchar_t image[260];
} common_task_trace_node;
#endif
