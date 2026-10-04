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
#define frontend_window_decode_frame measured_decode
#define frontend_window_present measured_present
#define frontend_session_destroy measured_actual_destroy
#define frontend_session_park measured_actual_park
#define frontend_session_read measured_actual_read
#include "../../src/ntcon-exe/frontend_session.c"
#undef frontend_session_read
#undef frontend_session_park
#undef frontend_session_destroy
#undef frontend_window_present
#undef frontend_window_decode_frame
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
