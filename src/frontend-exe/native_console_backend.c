#include "native_console_backend.h"
#include <stdio.h>

struct run16_native_backend {
    HANDLE pipe,helper,stop,event;
    ULONGLONG close_deadline;
    CRITICAL_SECTION lock;
};
static DWORD transfer(run16_native_backend *backend,void *data,DWORD bytes,BOOL write)
{
    BYTE *cursor=data;
    while(bytes) {
        OVERLAPPED io={0};
        HANDLE waits[3]={backend->stop,backend->event,backend->helper};
        DWORD count=0,error,wait,timeout=INFINITE;
        BOOL ok;
        if(WaitForSingleObject(backend->stop,0)==WAIT_OBJECT_0) return ERROR_OPERATION_ABORTED;
        if(backend->close_deadline) {
            ULONGLONG now=GetTickCount64();
            if(now>=backend->close_deadline) return ERROR_TIMEOUT;
            timeout=(DWORD)(backend->close_deadline-now);
        }
        ResetEvent(backend->event);io.hEvent=backend->event;
        ok=write ? WriteFile(backend->pipe,cursor,bytes,&count,&io) : ReadFile(backend->pipe,cursor,bytes,&count,&io);
        if(!ok) {
            error=GetLastError();
            if(error!=ERROR_IO_PENDING) return error;
            /* A completed final reply takes precedence over helper exit. */
            wait=WaitForMultipleObjects(3,waits,FALSE,timeout);
            if(wait!=WAIT_OBJECT_0+1) {
                error=wait==WAIT_OBJECT_0 ? ERROR_OPERATION_ABORTED : wait==WAIT_OBJECT_0+2 ? ERROR_BROKEN_PIPE :
                    wait==WAIT_TIMEOUT ? ERROR_TIMEOUT : GetLastError();
                CancelIoEx(backend->pipe,&io);GetOverlappedResult(backend->pipe,&io,&count,TRUE);
                return error;
            }
            if(!GetOverlappedResult(backend->pipe,&io,&count,FALSE)) return GetLastError();
        }
        if(!count || count>bytes) return ERROR_BROKEN_PIPE;
        cursor+=count;bytes-=count;
    }
    return ERROR_SUCCESS;
}
static DWORD call(run16_native_backend *backend,const run16_native_host_request *request,
    const void *payload,run16_native_host_reply *reply,void *data,DWORD capacity)
{
    DWORD error;
    ZeroMemory(reply,sizeof(*reply));
    error=transfer(backend,(void *)request,sizeof(*request),TRUE);
    if(!error && request->bytes) error=transfer(backend,(void *)payload,request->bytes,TRUE);
    if(!error) error=transfer(backend,reply,sizeof(*reply),FALSE);
    if(!error && (reply->version!=RUN16_NATIVE_HOST_VERSION || reply->bytes>capacity)) error=ERROR_INVALID_DATA;
    if(!error && reply->bytes) error=transfer(backend,data,reply->bytes,FALSE);
    if(error) SetEvent(backend->stop); /* A partial packet cannot be replayed. */
    return error;
}
DWORD run16_native_backend_call(run16_native_backend *backend,const run16_native_host_request *request,
    const void *payload,run16_native_host_reply *reply,void *data,DWORD capacity)
{
    DWORD error;
    if(!backend || !request || !reply || (request->bytes && !payload) || (capacity && !data)) return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&backend->lock);
    error=call(backend,request,payload,reply,data,capacity);
    LeaveCriticalSection(&backend->lock);
    return error;
}
void run16_native_backend_cancel(run16_native_backend *backend)
{
    if(backend) SetEvent(backend->stop);
}
HANDLE run16_native_backend_process(run16_native_backend *backend)
{
    return backend ? backend->helper : NULL;
}
DWORD run16_native_backend_close(run16_native_backend *backend)
{
    DWORD error=ERROR_SUCCESS;
    if(!backend) return error;
    EnterCriticalSection(&backend->lock);
    if(backend->helper && WaitForSingleObject(backend->stop,0)!=WAIT_OBJECT_0) {
        run16_native_host_request request={RUN16_NATIVE_HOST_VERSION,RUN16_NATIVE_STOP,0,0,0};
        run16_native_host_reply reply;
        /* Bound the whole STOP exchange, including partial replies, before
         * the separate helper-exit wait. Normal target I/O has no time limit. */
        backend->close_deadline=GetTickCount64()+5000;
        error=call(backend,&request,NULL,&reply,NULL,0);
        if(!error)error=reply.status;
    }
    if(backend->pipe && backend->pipe!=INVALID_HANDLE_VALUE) CloseHandle(backend->pipe);
    if(backend->helper) {
        if(WaitForSingleObject(backend->helper,5000)!=WAIT_OBJECT_0) {
            /* Only the exact root-owned I/O helper, never its native targets. */
            TerminateProcess(backend->helper,ERROR_OPERATION_ABORTED);
            WaitForSingleObject(backend->helper,5000);
            if(!error)error=ERROR_TIMEOUT;
        }
        CloseHandle(backend->helper);
    }
    if(backend->stop) CloseHandle(backend->stop);
    if(backend->event) CloseHandle(backend->event);
    LeaveCriticalSection(&backend->lock);DeleteCriticalSection(&backend->lock);
    HeapFree(GetProcessHeap(),0,backend);
    return error;
}
DWORD run16_native_backend_open(PCWSTR image,run16_native_backend **output)
{
    return run16_native_backend_open_cancel(image,NULL,output);
}
DWORD run16_native_backend_open_cancel(PCWSTR image,HANDLE stop,run16_native_backend **output)
{
    run16_native_backend *backend;
    HANDLE client=INVALID_HANDLE_VALUE;
    STARTUPINFOEXW startup={0};
    SECURITY_ATTRIBUTES security={sizeof(security),NULL,TRUE};
    PROCESS_INFORMATION process={0};
    WCHAR name[96],command[32768];
    SIZE_T attributes=0;
    BOOL initialized=FALSE;
    DWORD error=ERROR_SUCCESS,pid;
    static LONG serial;
    if(!output || !image || !*image) return ERROR_INVALID_PARAMETER;
    *output=NULL;
    backend=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*backend));
    if(!backend)return ERROR_NOT_ENOUGH_MEMORY;
    InitializeCriticalSection(&backend->lock);
    if(stop)DuplicateHandle(GetCurrentProcess(),stop,GetCurrentProcess(),&backend->stop,
        SYNCHRONIZE|EVENT_MODIFY_STATE,FALSE,0);
    else backend->stop=CreateEventW(NULL,TRUE,FALSE,NULL);
    backend->event=CreateEventW(NULL,TRUE,FALSE,NULL);
    if(!backend->stop || !backend->event) { error=GetLastError();goto done; }
    swprintf_s(name,96,L"\\\\.\\pipe\\run16-native-%lu-%lu",GetCurrentProcessId(),(DWORD)InterlockedIncrement(&serial));
    backend->pipe=CreateNamedPipeW(name,PIPE_ACCESS_DUPLEX|FILE_FLAG_OVERLAPPED|FILE_FLAG_FIRST_PIPE_INSTANCE,
        PIPE_TYPE_BYTE|PIPE_WAIT|PIPE_REJECT_REMOTE_CLIENTS,1,65536,65536,0,NULL);
    if(backend->pipe==INVALID_HANDLE_VALUE) { error=GetLastError();goto done; }
    client=CreateFileW(name,GENERIC_READ|GENERIC_WRITE,0,&security,OPEN_EXISTING,
        SECURITY_SQOS_PRESENT|SECURITY_IDENTIFICATION,NULL);
    if(client==INVALID_HANDLE_VALUE) { error=GetLastError();goto done; }
    {
        OVERLAPPED io={0};io.hEvent=backend->event;
        if(!ConnectNamedPipe(backend->pipe,&io) && GetLastError()!=ERROR_PIPE_CONNECTED) {
            DWORD ignored;error=GetLastError();CancelIoEx(backend->pipe,&io);
            GetOverlappedResult(backend->pipe,&io,&ignored,TRUE);goto done;
        }
    }
    if(!GetNamedPipeClientProcessId(backend->pipe,&pid) || pid!=GetCurrentProcessId()) { error=ERROR_ACCESS_DENIED;goto done; }
    startup.StartupInfo.cb=sizeof(startup);
    startup.StartupInfo.dwFlags=STARTF_USESTDHANDLES|STARTF_USESHOWWINDOW;startup.StartupInfo.wShowWindow=SW_HIDE;
    startup.StartupInfo.hStdInput=startup.StartupInfo.hStdOutput=startup.StartupInfo.hStdError=client;
    InitializeProcThreadAttributeList(NULL,1,0,&attributes);
    startup.lpAttributeList=HeapAlloc(GetProcessHeap(),0,attributes);
    if(!startup.lpAttributeList) { error=ERROR_NOT_ENOUGH_MEMORY;goto done; }
    if(!InitializeProcThreadAttributeList(startup.lpAttributeList,1,0,&attributes)) { error=GetLastError();goto done; }
    initialized=TRUE;
    if(!UpdateProcThreadAttribute(startup.lpAttributeList,0,PROC_THREAD_ATTRIBUTE_HANDLE_LIST,&client,sizeof(client),NULL,NULL)) { error=GetLastError();goto done; }
    if(swprintf_s(command,32768,L"\"%ls\" --internal-native-console",image)<0) { error=ERROR_FILENAME_EXCED_RANGE;goto done; }
    if(!CreateProcessW(image,command,NULL,NULL,TRUE,EXTENDED_STARTUPINFO_PRESENT,NULL,NULL,&startup.StartupInfo,&process)) { error=GetLastError();goto done; }
    backend->helper=process.hProcess;CloseHandle(process.hThread);
done:
    if(initialized) DeleteProcThreadAttributeList(startup.lpAttributeList);
    if(startup.lpAttributeList) HeapFree(GetProcessHeap(),0,startup.lpAttributeList);
    if(client!=INVALID_HANDLE_VALUE) CloseHandle(client);
    if(error)run16_native_backend_close(backend);else *output=backend;
    return error;
}

DWORD run16_native_backend_launch(run16_native_backend *backend,const run16_native_start *start,HANDLE *target)
{
    run16_native_start local;
    HANDLE originals[5],remote[5]={0};
    DWORD i,j,error,bytes=0;
    BYTE *payload=NULL;
    run16_native_host_request request={RUN16_NATIVE_HOST_VERSION,RUN16_NATIVE_LAUNCH,0,0,0};
    run16_native_host_reply reply;
    if(!backend || !start || !target) return ERROR_INVALID_PARAMETER;
    *target=NULL;local=*start;
    for(i=0;i<5;++i) originals[i]=i<3 ? start->standard[i] : start->capabilities[i-3];
    EnterCriticalSection(&backend->lock);
    for(i=0;i<5;++i) {
        if(i<3 && (start->console_mask&(1u<<i))) { local.standard[i]=NULL;continue; }
        if(!originals[i] || originals[i]==INVALID_HANDLE_VALUE) remote[i]=originals[i];
        else {
            for(j=0;j<i;++j) if((j<3)==(i<3) && originals[j]==originals[i] && remote[j]) break;
            if(j<i) remote[i]=remote[j];
            else if(!DuplicateHandle(GetCurrentProcess(),originals[i],backend->helper,&remote[i],
                i<3 ? 0 : SYNCHRONIZE,FALSE,i<3 ? DUPLICATE_SAME_ACCESS : 0)) { error=GetLastError();goto done; }
        }
        if(i<3)local.standard[i]=remote[i];else local.capabilities[i-3]=remote[i];
    }
    error=run16_native_launch_pack(&local,&payload,&bytes);
    if(error)goto done;
    request.bytes=bytes;
    error=call(backend,&request,payload,&reply,NULL,0);
    if(!error)error=reply.status;
    if(!error) {
        DWORD release_error;
        if(reply.process>(uint64_t)(ULONG_PTR)-1 || !reply.process) error=ERROR_INVALID_DATA;
        else if(!DuplicateHandle(backend->helper,(HANDLE)(ULONG_PTR)reply.process,GetCurrentProcess(),
            target,SYNCHRONIZE|PROCESS_QUERY_LIMITED_INFORMATION,FALSE,0)) error=GetLastError();
        request.operation=RUN16_NATIVE_RELEASE;request.bytes=0;
        /* Releasing an export copy is not target completion. If this reply is
         * lost, keep the actual target capability already obtained above.
         * A failed local duplication still releases the helper's export. */
        release_error=call(backend,&request,NULL,&reply,NULL,0);
        if(release_error || reply.status) SetEvent(backend->stop);
    }
done:
    if(payload)HeapFree(GetProcessHeap(),0,payload);
    for(i=0;i<5;++i) if(remote[i] && remote[i]!=INVALID_HANDLE_VALUE) {
        HANDLE copy=NULL;
        for(j=0;j<i;++j)if(remote[j]==remote[i])break;
        if(j==i && DuplicateHandle(backend->helper,remote[i],GetCurrentProcess(),&copy,
            0,FALSE,DUPLICATE_SAME_ACCESS|DUPLICATE_CLOSE_SOURCE)) CloseHandle(copy);
    }
    LeaveCriticalSection(&backend->lock);
    return error;
}

DWORD run16_native_backend_members(run16_native_backend *backend,DWORD *members)
{
    run16_native_host_request request={RUN16_NATIVE_HOST_VERSION,RUN16_NATIVE_MEMBERS,0,0,0};
    run16_native_host_reply reply;
    DWORD error;
    if(!backend || !members)return ERROR_INVALID_PARAMETER;
    error=run16_native_backend_call(backend,&request,NULL,&reply,NULL,0);
    if(!error)error=reply.status;
    if(!error)*members=reply.count;
    return error;
}

DWORD run16_native_backend_input(run16_native_backend *backend,const INPUT_RECORD *records,DWORD count)
{
    DWORD offset=0;
    if(!backend || (!records && count))return ERROR_INVALID_PARAMETER;
    while(offset<count) {
        DWORD batch=min(count-offset,RUN16_NATIVE_HOST_INPUTS),error;
        run16_native_host_request request={RUN16_NATIVE_HOST_VERSION,RUN16_NATIVE_INPUT,0,0,0};
        run16_native_host_reply reply;
        request.bytes=batch*sizeof(*records);
        error=run16_native_backend_call(backend,&request,records+offset,&reply,NULL,0);
        if(error || reply.status)return error ? error : reply.status;
        if(!reply.count || reply.count>batch)return ERROR_INVALID_DATA;
        offset+=reply.count;
    }
    return ERROR_SUCCESS;
}
