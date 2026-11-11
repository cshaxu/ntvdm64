/* Test-link only: unchanged project-owned producer, not a guest substitute. */
#include "worker_performance.h"
#include "ntvdm-exe/win32/console_text.h"
#include "ntvdm-exe/win32/console_client.h"
#include "common/protocol/console_io.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_text_video.h"
static __declspec(thread) LONGLONG assembly_started;
static BOOL measured_publish(const console_video_description *,const void *,size_t);
#define NtvdmConsoleUpdateText measured_actual_update_text
#define ntvdm_console_publish_video measured_publish
#include "../../src/ntvdm-exe/win32/console_text.c"
#undef ntvdm_console_publish_video
#undef NtvdmConsoleUpdateText
static BOOL measured_publish(const console_video_description *description,const void *data,size_t bytes)
{
    if(assembly_started)worker_performance_record("text-assembly",assembly_started,bytes,0);
    return ntvdm_console_publish_video(description,data,bytes);
}
BOOL NtvdmConsoleUpdateText(HPALETTE palette)
{
    BOOL enabled=worker_performance_enabled(),result;
    LONGLONG previous=assembly_started;
    DWORD error;
    assembly_started=enabled ? worker_performance_clock() : 0;
    result=measured_actual_update_text(palette);error=GetLastError();
    if(enabled)worker_performance_record("text-producer-total",assembly_started,0,result ? 0 : error);
    assembly_started=previous;SetLastError(error);return result;
}
