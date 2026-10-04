/* Only project-owned boundaries are wrapped. No worker classification,
 * producer filtering or production source/protocol change. */
#include "worker_performance.h"
#include "ntcon-exe/frontend_session.h"
#include "ntcon-exe/console_frontend.h"
#include "ntcon-exe/window_controller.h"
#include "ntcon-exe/window_keyboard.h"
#include "ntcon-exe/window_mouse.h"
#include "ntcon-exe/window_frame.h"
#include "lib/kvm-window/render.h"
#include "opennt-abi/host-compat/include/console_grid.h"
static DWORD measured_decode(const frontend_video *,kvm_window_frame *);
static DWORD measured_present(frontend_window_controller *,const kvm_window_frame *,BOOL);
DWORD measured_actual_destroy(frontend_session *);
DWORD measured_actual_park(frontend_session *);
DWORD measured_actual_read(frontend_session *,BOOL,INPUT_RECORD *,DWORD,DWORD *);
DWORD measured_actual_video(frontend_session *,const void *,frontend_video *,BOOL);
static HANDLE measured_create_buffer(DWORD,DWORD,const SECURITY_ATTRIBUTES *,DWORD,void *);
static BOOL measured_read_cells(HANDLE,CHAR_INFO *,COORD,COORD,SMALL_RECT *);
static BOOL measured_write_cells(HANDLE,const CHAR_INFO *,COORD,COORD,SMALL_RECT *);
static BOOL measured_window(HANDLE,BOOL,const SMALL_RECT *);
static BOOL measured_buffer_size(HANDLE,COORD);
static UINT measured_output_code_page(void);
/* Test-only per-publication aggregation avoids one sample per character.
 * Independent frontend threads cannot charge another thread's publication. */
static __declspec(thread) BOOL measuring_code_page;
static __declspec(thread) LONGLONG code_page_ticks;
static __declspec(thread) DWORD code_page_calls;
#define GetConsoleOutputCP measured_output_code_page
#define CreateConsoleScreenBuffer measured_create_buffer
#define ReadConsoleOutputW measured_read_cells
#define WriteConsoleOutputW measured_write_cells
#define SetConsoleWindowInfo measured_window
#define SetConsoleScreenBufferSize measured_buffer_size
#define frontend_session_video measured_actual_video
#define frontend_window_decode_frame measured_decode
#define frontend_window_present measured_present
#define frontend_session_destroy measured_actual_destroy
#define frontend_session_park measured_actual_park
#define frontend_session_read measured_actual_read
#include "../../src/ntcon-exe/frontend_session.c"
#undef GetConsoleOutputCP
#undef frontend_session_video
#undef SetConsoleScreenBufferSize
#undef SetConsoleWindowInfo
#undef WriteConsoleOutputW
#undef ReadConsoleOutputW
#undef CreateConsoleScreenBuffer
#undef frontend_session_read
#undef frontend_session_park
#undef frontend_session_destroy
#undef frontend_window_present
#undef frontend_window_decode_frame
/* Attribute the synchronous publication's host Console work without changing
 * the production implementation, order, arguments, errors or cancellation.
 * Nested samples overlap: do not add API durations to the parent duration. */
static HANDLE measured_create_buffer(DWORD access,DWORD share,
    const SECURITY_ATTRIBUTES *security,DWORD flags,void *reserved)
{
    BOOL enabled=worker_performance_enabled();
    LONGLONG start=enabled ? worker_performance_clock() : 0;
    HANDLE result=CreateConsoleScreenBuffer(access,share,security,flags,reserved);
    DWORD error=GetLastError();
    if(enabled)worker_performance_record("frontend-buffer-create",start,0,
        result==INVALID_HANDLE_VALUE ? error : 0);
    SetLastError(error);return result;
}
static BOOL measured_read_cells(HANDLE output,CHAR_INFO *cells,COORD size,
    COORD origin,SMALL_RECT *rect)
{
    BOOL enabled=worker_performance_enabled();
    LONGLONG start=enabled ? worker_performance_clock() : 0;
    BOOL result=ReadConsoleOutputW(output,cells,size,origin,rect);
    DWORD error=GetLastError();
    if(enabled)worker_performance_record("frontend-cell-read",start,
        (DWORD)size.X*(DWORD)size.Y,result ? 0 : error);
    SetLastError(error);return result;
}
static BOOL measured_write_cells(HANDLE output,const CHAR_INFO *cells,COORD size,
    COORD origin,SMALL_RECT *rect)
{
    BOOL enabled=worker_performance_enabled();
    LONGLONG start=enabled ? worker_performance_clock() : 0;
    BOOL result=WriteConsoleOutputW(output,cells,size,origin,rect);
    DWORD error=GetLastError();
    if(enabled)worker_performance_record("frontend-cell-write",start,
        (DWORD)size.X*(DWORD)size.Y,result ? 0 : error);
    SetLastError(error);return result;
}
static BOOL measured_window(HANDLE output,BOOL absolute,const SMALL_RECT *rect)
{
    BOOL enabled=worker_performance_enabled();
    LONGLONG start=enabled ? worker_performance_clock() : 0;
    BOOL result=SetConsoleWindowInfo(output,absolute,rect);
    DWORD error=GetLastError();
    if(enabled)worker_performance_record("frontend-window-size",start,0,result ? 0 : error);
    SetLastError(error);return result;
}
static BOOL measured_buffer_size(HANDLE output,COORD size)
{
    BOOL enabled=worker_performance_enabled();
    LONGLONG start=enabled ? worker_performance_clock() : 0;
    BOOL result=SetConsoleScreenBufferSize(output,size);
    DWORD error=GetLastError();
    if(enabled)worker_performance_record("frontend-buffer-size",start,
        (DWORD)size.X*(DWORD)size.Y,result ? 0 : error);
    SetLastError(error);return result;
}
static UINT measured_output_code_page(void)
{
    LONGLONG start=measuring_code_page ? worker_performance_clock() : 0;
    UINT result=GetConsoleOutputCP();DWORD error=GetLastError();
    if(measuring_code_page){code_page_ticks+=worker_performance_clock()-start;++code_page_calls;}
    SetLastError(error);return result;
}
DWORD frontend_session_video(frontend_session *owner,const void *source,
    frontend_video *video,BOOL import_text)
{
    BOOL enabled=worker_performance_enabled();
    LONGLONG start=enabled ? worker_performance_clock() : 0;
    DWORD result,error;
    code_page_ticks=0;code_page_calls=0;measuring_code_page=enabled;
    result=measured_actual_video(owner,source,video,import_text);error=GetLastError();
    measuring_code_page=FALSE;
    if(enabled) {
        worker_performance_record("frontend-video-commit",start,video->description.bytes,result);
        worker_performance_record_total("frontend-codepage-total",code_page_ticks,code_page_calls,result);
    }
    SetLastError(error);return result;
}
static DWORD measured_decode(const frontend_video *video,kvm_window_frame *frame)
{
    BOOL enabled=worker_performance_enabled();
    LONGLONG start=enabled ? worker_performance_clock() : 0;
    DWORD result=frontend_window_decode_frame(video,frame),error=GetLastError();
    if(enabled)worker_performance_record("frontend-decode",start,video->description.bytes,result);
    SetLastError(error);return result;
}
static DWORD measured_present(frontend_window_controller *window,const kvm_window_frame *frame,BOOL graphics)
{
    BOOL enabled=worker_performance_enabled();
    LONGLONG start=enabled ? worker_performance_clock() : 0;
    DWORD result=frontend_window_present(window,frame,graphics),error=GetLastError();
    /* This is library presentation completion, not a physical display scan. */
    if(enabled)worker_performance_record("frontend-present",start,0,result);
    SetLastError(error);return result;
}
DWORD frontend_session_read(frontend_session *owner,BOOL peek,INPUT_RECORD *records,DWORD capacity,DWORD *count)
{
    BOOL enabled=worker_performance_enabled();
    LONGLONG start=enabled ? worker_performance_clock() : 0;
    DWORD result=measured_actual_read(owner,peek,records,capacity,count),error=GetLastError();
    if(enabled)worker_performance_record(peek ? "frontend-peek" : "frontend-read",start,result ? 0 : *count,result);
    SetLastError(error);return result;
}
DWORD frontend_session_park(frontend_session *owner)
{
    DWORD result=measured_actual_park(owner),error=GetLastError();
    if(!result)worker_performance_flush();
    SetLastError(error);return result;
}
DWORD frontend_session_destroy(frontend_session *owner)
{
    DWORD result=measured_actual_destroy(owner),error=GetLastError();
    if(!result)worker_performance_flush();
    SetLastError(error);return result;
}
