#include "worker_performance.h"
#include "common/console/client.h"
#include "worker-base/connection.h"
static DWORD measured_exchange(ntcon_worker_client *,console_io_request *,console_io_reply *);
static DWORD measured_video(ntcon_worker_client *,const console_video_description *,const void *);
static DWORD measured_call(ntcon_worker_client *,console_io_request *,console_io_reply *);
static DWORD measured_close(HANDLE *,HANDLE *,HANDLE *);
#define ntcon_worker_exchange measured_exchange
#define ntcon_worker_video measured_video
#define ntcon_worker_call measured_call
#define worker_base_io_close measured_close
#include "../../src/ntvdm-exe/win32/console_client.c"
#undef ntcon_worker_exchange
#undef ntcon_worker_video
#undef ntcon_worker_call
#undef worker_base_io_close
static DWORD measured_exchange(ntcon_worker_client *client,console_io_request *request,console_io_reply *reply)
{
    DWORD result;LONGLONG start;
    if(!worker_performance_enabled())return ntcon_worker_exchange(client,request,reply);
    start=worker_performance_clock();result=ntcon_worker_exchange(client,request,reply);
    if(request->operation==CONSOLE_IO_READ_INPUT)
        worker_performance_record("worker-input-read",start,result ? 0 : reply->state.count,result);
    return result;
}
static DWORD measured_call(ntcon_worker_client *client,console_io_request *request,console_io_reply *reply)
{
    DWORD result;LONGLONG start;
    if(!worker_performance_enabled())return ntcon_worker_call(client,request,reply);
    start=worker_performance_clock();result=ntcon_worker_call(client,request,reply);
    if(request->operation==CONSOLE_IO_BARRIER)
        worker_performance_record("handoff-barrier",start,0,result);
    return result;
}
static DWORD measured_video(ntcon_worker_client *client,const console_video_description *description,const void *payload)
{
    DWORD result;LONGLONG start;
    if(!worker_performance_enabled())return ntcon_worker_video(client,description,payload);
    start=worker_performance_clock();result=ntcon_worker_video(client,description,payload);
    worker_performance_record("video-transfer",start,description ? description->bytes : 0,result);return result;
}
static DWORD measured_close(HANDLE *pipe,HANDLE *peer,HANDLE *ready)
{
    DWORD result;LONGLONG start=0;BOOL enabled=worker_performance_enabled();
    if(enabled)start=worker_performance_clock();
    result=worker_base_io_close(pipe,peer,ready);
    if(enabled)worker_performance_record("io-close-ack",start,0,result);
    if(!result)worker_performance_flush();return result;
}
