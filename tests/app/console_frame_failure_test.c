/* Test-only allocator fault at the actual frontend import boundary. The
 * production source is compiled here, not replaced by a fixture renderer.
 * Thread-local arming cannot fault the independent presentation thread. */
#include <windows.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
static __declspec(thread) BOOL fail_frame_allocation;
static __declspec(thread) HANDLE fail_projection_target;
static volatile LONG arm_pipe_projection;
static LPVOID frame_test_heap_alloc(HANDLE heap,DWORD flags,SIZE_T bytes)
{
    if(fail_frame_allocation) {
        fail_frame_allocation=FALSE;
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return NULL;
    }
    return HeapAlloc(heap,flags,bytes);
}
static BOOL frame_test_write_console_output(HANDLE output,const CHAR_INFO *cells,
    COORD size,COORD origin,PSMALL_RECT region)
{
    if(output==fail_projection_target) {
        fail_projection_target=NULL;
        SetLastError(ERROR_WRITE_FAULT);
        return FALSE;
    }
    return WriteConsoleOutputW(output,cells,size,origin,region);
}
#define HeapAlloc frame_test_heap_alloc
#define WriteConsoleOutputW frame_test_write_console_output
#include "../../src/ntcon-exe/frontend_session.c"
#undef HeapAlloc
#undef WriteConsoleOutputW
#define NTCON_FRAME_FAILURE_TEST
#include "console_channel_lifetime_test.c"
