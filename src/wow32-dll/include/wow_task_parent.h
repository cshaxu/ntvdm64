#ifndef WOW_TASK_PARENT_H
#define WOW_TASK_PARENT_H

#include <windows.h>

/* Copy only the original TDB's parent selector; no guest pointer escapes. */
BOOL wow_task_parent(DWORD task, WORD *parent);

#endif
