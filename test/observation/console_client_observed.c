#include "console_wire_observed.h"
#include "common/console/client.h"
/* The transport moved into common. Observe the real worker call boundary as
 * well; never substitute the shared client or alter its reply/failure. */
static DWORD observed_worker_exchange(ntcon_worker_client *,console_io_request *,console_io_reply *);
static DWORD observed_worker_video(ntcon_worker_client *,const console_video_description *,const void *);
#define ntcon_worker_exchange observed_worker_exchange
#define ntcon_worker_video observed_worker_video
#include "../../src/ntvdm-exe/win32/console_client.c"
#undef ntcon_worker_exchange
#undef ntcon_worker_video
#undef WriteFile
static DWORD observed_worker_exchange(ntcon_worker_client *client,
    console_io_request *request,console_io_reply *reply)
{
    static LONG cursor_calls;
    DWORD error=ntcon_worker_exchange(client,request,reply);
    if(request->operation==CONSOLE_IO_CURSOR_POSITION &&
        InterlockedIncrement(&cursor_calls)<=1000) {
        char prefix[MAX_PATH],path[MAX_PATH];FILE *file=NULL;void *frames[20];
        DWORD length=GetEnvironmentVariableA("MVDM_TEST_CONSOLE_WIRE_PATH",prefix,MAX_PATH);
        USHORT count=CaptureStackBackTrace(0,20,frames,NULL),index;
        if(length && length<MAX_PATH &&
            snprintf(path,MAX_PATH,"%s-%lu.cursor-stack",prefix,GetCurrentProcessId())>0 &&
            !fopen_s(&file,path,"a") && file) {
            fprintf(file,"time=%llu base=%p x=%ld y=%ld",GetTickCount64(),
                GetModuleHandleW(NULL),request->state.x,request->state.y);
            for(index=0;index<count;++index)fprintf(file," %p",frames[index]);
            fputc('\n',file);fclose(file);
        }
    }
    observed_console_log("begin",client->pipe,TRUE,request,
        offsetof(console_io_request,data),0,error);
    return error;
}
static DWORD observed_worker_video(ntcon_worker_client *client,
    const console_video_description *description,const void *payload)
{
    console_io_request record={0};
    DWORD error=ntcon_worker_video(client,description,payload);
    record.version=CONSOLE_IO_VERSION;record.generation=client->generation;
    record.sequence=client->sequence;record.operation=CONSOLE_IO_VIDEO_BEGIN;
    record.state.x=description ? (int32_t)description->kind : -1;
    record.state.y=description ? (int32_t)description->bytes : 0;
    if(!error && description && description->kind==CONSOLE_VIDEO_TEXT_FRAME) {
        char prefix[MAX_PATH],path[MAX_PATH];HANDLE file;DWORD written;
        DWORD length=GetEnvironmentVariableA("MVDM_TEST_CONSOLE_WIRE_PATH",prefix,MAX_PATH);
        if(length && length<MAX_PATH &&
            snprintf(path,MAX_PATH,"%s-%lu-%lu-%lu.frame",prefix,GetCurrentProcessId(),client->generation,client->sequence)>0) {
            file=CreateFileA(path,GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_NEW,0,NULL);
            if(file!=INVALID_HANDLE_VALUE) {
                WriteFile(file,description,sizeof(*description),&written,NULL);
                WriteFile(file,payload,description->bytes,&written,NULL);CloseHandle(file);
            }
        }
    }
    observed_console_log("begin",client->pipe,TRUE,&record,
        offsetof(console_io_request,data),0,error);
    return error;
}
