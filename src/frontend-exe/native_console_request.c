#include "native_console_request.h"
#include "native_request_protocol.h"
struct run16_native_request {
    run16_native_frontend *frontend;
    HANDLE root_capability,stop,pipe,sender,execution,event,thread;
};
static DWORD launch_request(run16_native_request *request,BYTE *payload,DWORD bytes,HANDLE *target)
{
    run16_native_launch_packet header;
    run16_native_start start={0};
    WCHAR *strings[4];HANDLE local[3]={0};DWORD i,j,error;
    error=run16_native_launch_unpack(payload,bytes,&header,strings);
    if(error)return error;
    /* These authorities come only from broker attachments, not wire numbers. */
    if(header.capabilities[0] || header.capabilities[1])return ERROR_INVALID_DATA;
    start.application=*strings[0] ? strings[0] : NULL;start.command=strings[1];
    start.directory=strings[2];start.environment=strings[3];start.console_mask=header.console_mask;
    start.capabilities[0]=request->root_capability;start.capabilities[1]=request->execution;
    for(i=0;i<3;++i) {
        HANDLE source;
        if(header.standard[i]>(uint64_t)(ULONG_PTR)-1) { error=ERROR_INVALID_HANDLE;goto done; }
        source=(HANDLE)(ULONG_PTR)header.standard[i];
        if(header.console_mask&(1u<<i))continue;
        if(!source || source==INVALID_HANDLE_VALUE) { start.standard[i]=source;continue; }
        for(j=0;j<i;++j)if(header.standard[j]==header.standard[i] && local[j])break;
        if(j<i)local[i]=local[j];
        else if(!DuplicateHandle(request->sender,source,GetCurrentProcess(),&local[i],
            0,FALSE,DUPLICATE_SAME_ACCESS)) { error=GetLastError();goto done; }
        if(GetFileType(local[i])!=FILE_TYPE_DISK && GetFileType(local[i])!=FILE_TYPE_PIPE &&
            GetFileType(local[i])!=FILE_TYPE_CHAR) {
            error=ERROR_INVALID_HANDLE;goto done;
        }
        start.standard[i]=local[i];
    }
    error=run16_native_frontend_launch(request->frontend,&start,target);
done:
    for(i=0;i<3;++i)if(local[i]) {
        for(j=0;j<i;++j)if(local[j]==local[i])break;
        if(j==i)CloseHandle(local[i]);
    }
    return error;
}
static DWORD WINAPI serve(void *context)
{
    run16_native_request *request=context;
    native_request_header header;
    native_request_reply reply={NATIVE_REQUEST_VERSION,0,0};
    BYTE *payload=NULL;HANDLE target=NULL,remote=NULL,receipt=NULL,remote_receipt=NULL;DWORD error;
    error=frontend_request_transfer(request->pipe,request->sender,request->stop,request->event,FALSE,&header,sizeof(header));
    if(error)goto done;
    if(header.version!=NATIVE_REQUEST_VERSION || header.bytes<sizeof(run16_native_launch_packet)) {
        reply.error=ERROR_INVALID_DATA;
    } else {
        payload=HeapAlloc(GetProcessHeap(),0,header.bytes);
        if(!payload)reply.error=ERROR_NOT_ENOUGH_MEMORY;
        else {
            error=frontend_request_transfer(request->pipe,request->sender,request->stop,request->event,FALSE,payload,header.bytes);
            if(error)goto done;
            reply.error=launch_request(request,payload,header.bytes,&target);
            if(!reply.error){receipt=CreateEventW(NULL,TRUE,FALSE,NULL);if(!receipt)reply.error=GetLastError();}
            if(!reply.error && !DuplicateHandle(GetCurrentProcess(),receipt,request->sender,&remote_receipt,
                SYNCHRONIZE,FALSE,0))reply.error=GetLastError();
            if(!reply.error && !DuplicateHandle(GetCurrentProcess(),target,request->sender,&remote,
                SYNCHRONIZE|PROCESS_QUERY_LIMITED_INFORMATION,FALSE,0))reply.error=GetLastError();
            reply.target=(uint64_t)(ULONG_PTR)remote;
            reply.receipt=reply.error ? 0 : (uint64_t)(ULONG_PTR)remote_receipt;
        }
    }
    error=frontend_request_transfer(request->pipe,request->sender,request->stop,request->event,TRUE,&reply,sizeof(reply));
    if(error && remote) {
        HANDLE copy=NULL;
        /* An incomplete response cannot be consumed. Reclaim only our export
         * copy, never the handed-off target process or any descendant. */
        if(DuplicateHandle(request->sender,remote,GetCurrentProcess(),&copy,
            0,FALSE,DUPLICATE_SAME_ACCESS|DUPLICATE_CLOSE_SOURCE))CloseHandle(copy);
    }
    if((error || reply.error) && remote_receipt){
        HANDLE copy=NULL;
        if(DuplicateHandle(request->sender,remote_receipt,GetCurrentProcess(),&copy,
            0,FALSE,DUPLICATE_SAME_ACCESS|DUPLICATE_CLOSE_SOURCE))CloseHandle(copy);
    }
    if(!error && !reply.error){
        DWORD result;
        /* Includes the existing bounded final refresh. Neither failure here
         * nor frontend loss replaces the launcher's actual process result. */
        run16_native_frontend_wait(request->frontend,target,&result);
        SetEvent(receipt);
    }
done:
    if(receipt)CloseHandle(receipt);
    if(target)CloseHandle(target);
    if(payload)HeapFree(GetProcessHeap(),0,payload);
    CloseHandle(request->pipe);request->pipe=NULL;
    return error;
}
DWORD run16_native_request_start(run16_native_frontend *frontend,HANDLE root_capability,HANDLE stop,
    HANDLE channel,HANDLE sender,HANDLE execution,run16_native_request **output)
{
    run16_native_request *request=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*request));
    DWORD error;
    *output=NULL;
    if(!request) { CloseHandle(channel);CloseHandle(sender);CloseHandle(execution);return ERROR_NOT_ENOUGH_MEMORY; }
    request->frontend=frontend;request->root_capability=root_capability;request->stop=stop;
    request->pipe=channel;request->sender=sender;request->execution=execution;
    request->event=CreateEventW(NULL,TRUE,FALSE,NULL);
    if(request->event)request->thread=CreateThread(NULL,0,serve,request,0,NULL);
    if(!request->event || !request->thread) { error=GetLastError();run16_native_request_close(request);return error; }
    *output=request;return ERROR_SUCCESS;
}
HANDLE run16_native_request_thread(run16_native_request *request) { return request->thread; }
void run16_native_request_close(run16_native_request *request)
{
    if(!request)return;
    if(request->thread) {
        do { CancelSynchronousIo(request->thread); }
        while(WaitForSingleObject(request->thread,50)==WAIT_TIMEOUT);
        CloseHandle(request->thread);
    }
    if(request->pipe)CloseHandle(request->pipe);
    if(request->sender)CloseHandle(request->sender);
    if(request->execution)CloseHandle(request->execution);
    if(request->event)CloseHandle(request->event);
    HeapFree(GetProcessHeap(),0,request);
}
