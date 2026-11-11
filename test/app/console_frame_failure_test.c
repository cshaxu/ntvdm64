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
static __declspec(thread) DWORD restore_fault,restore_seen;
static BOOL restore_test_failure(DWORD bit)
{
    if(!restore_fault)return FALSE;
    restore_seen|=bit;
    if(!(restore_fault&bit))return FALSE;
    SetLastError(ERROR_WRITE_FAULT+bit);
    return TRUE;
}
static BOOL restore_test_mode(HANDLE input,DWORD mode)
{ return restore_test_failure(1) ? FALSE : SetConsoleMode(input,mode); }
static BOOL restore_test_palette(HANDLE output,PCONSOLE_SCREEN_BUFFER_INFOEX info)
{ return restore_test_failure(2) ? FALSE : SetConsoleScreenBufferInfoEx(output,info); }
static BOOL restore_test_cursor(HANDLE output,const CONSOLE_CURSOR_INFO *cursor)
{ return restore_test_failure(4) ? FALSE : SetConsoleCursorInfo(output,cursor); }
static BOOL restore_test_buffer(HANDLE output)
{ return restore_test_failure(8) ? FALSE : SetConsoleActiveScreenBuffer(output); }
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
#define SetConsoleMode restore_test_mode
#define SetConsoleScreenBufferInfoEx restore_test_palette
#define SetConsoleCursorInfo restore_test_cursor
#define SetConsoleActiveScreenBuffer restore_test_buffer
#include "../../src/ntcon-exe/frontend_session.c"
#undef HeapAlloc
#undef WriteConsoleOutputW
#undef SetConsoleMode
#undef SetConsoleScreenBufferInfoEx
#undef SetConsoleCursorInfo
#undef SetConsoleActiveScreenBuffer
static int test_restore_failures(void)
{
    DWORD bits[]={1,2,4,8,15},i,mode,error;
    HANDLE input=NULL,output=NULL;
    frontend_session *frontend=NULL;
#define RESTORE_CHECK(x) do {if(!(x)){fprintf(stderr,"FAIL restore line=%u error=%lu\n",__LINE__,GetLastError());return 1;}}while(0)
    for(i=0;i<ARRAYSIZE(bits);++i) {
        RESTORE_CHECK(!frontend_session_create(&frontend));
        RESTORE_CHECK(!frontend_session_console(frontend,&input,&output));
        RESTORE_CHECK(GetConsoleMode(input,&mode));
        RESTORE_CHECK(SetConsoleMode(input,mode^ENABLE_ECHO_INPUT));
        /* Force each actual restoration API to run, including palette setter. */
        frontend->original_palette[0]^=0xffffff;
        frontend->console_surface=CreateConsoleScreenBuffer(GENERIC_READ|GENERIC_WRITE,
            FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,CONSOLE_TEXTMODE_BUFFER,NULL);
        RESTORE_CHECK(frontend->console_surface!=INVALID_HANDLE_VALUE);
        restore_seen=0;restore_fault=bits[i];
        error=frontend_session_destroy(frontend);
        restore_fault=0;
        RESTORE_CHECK(restore_seen==15);
        RESTORE_CHECK(error==ERROR_WRITE_FAULT+(bits[i]==15 ? 8 : bits[i]));
        RESTORE_CHECK(GetConsoleMode(input,&mode));
        RESTORE_CHECK(mode==(frontend->original_input_mode^((bits[i]&1) ? ENABLE_ECHO_INPUT : 0)));
        RESTORE_CHECK(!frontend->thread); /* Joined before restoring/releasing. */
        RESTORE_CHECK(!frontend_session_destroy(frontend));frontend=NULL;
        CloseHandle(input);CloseHandle(output);input=output=NULL;
    }
    puts("PASS restoration API faults: all independent steps attempted, first error retained, input mode and retained cleanup verified");
    return 0;
#undef RESTORE_CHECK
}
#define NTCON_FRAME_FAILURE_TEST
#include "console_channel_lifetime_test.c"
