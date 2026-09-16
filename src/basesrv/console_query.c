/* App composition of the S1 short-lived run16 Console observation role. */
#include "console_query.h"
#include "basesrv/transport/console_probe.h"
#include <wchar.h>
typedef struct {HANDLE pipe;BYTE *data;DWORD bytes,error;} QUERY_WRITE;
static DWORD WINAPI send_request(void *context)
{
    QUERY_WRITE *write=context;
    DWORD done,offset=0;
    while (offset<write->bytes) {
        if (!WriteFile(write->pipe,write->data+offset,write->bytes-offset,&done,NULL)) {
            write->error=GetLastError();break;
        }
        if (!done) {write->error=ERROR_BROKEN_PIPE;break;}
        offset+=done;
    }
    CloseHandle(write->pipe);write->pipe=NULL;
    return 0;
}
static DWORD live_pid(HANDLE process,DWORD *pid)
{
    DWORD wait=WaitForSingleObject(process,0);
    if (wait!=WAIT_TIMEOUT) return wait==WAIT_FAILED ? GetLastError() : ERROR_PROCESS_ABORTED;
    *pid=GetProcessId(process);
    return *pid ? 0 : GetLastError();
}
DWORD app_console_query(const WCHAR *helper,HANDLE caller,const HANDLE *candidates,
    DWORD count,HANDLE cancel,DWORD timeout,BYTE *members)
{
    SECURITY_ATTRIBUTES sa={sizeof(sa),NULL,TRUE};
    HANDLE input=NULL,reader=NULL,output=NULL,thread=NULL,inherit[2];
    PROCESS_INFORMATION child={0};
    STARTUPINFOEXW startup={0};
    QUERY_WRITE write={0};
    BROKER_CONSOLE_PROBE_REQUEST *request;
    BROKER_CONSOLE_PROBE_REPLY reply;
    DWORD *pids,error=0,i,available,received=0,done,code=0,pid;
    BYTE *response=NULL;
    WCHAR *command=NULL;
    SIZE_T attributeBytes=0,chars;
    ULONGLONG deadline;
    if (!helper || !*helper || wcschr(helper,L'"') || !timeout || timeout>60000 ||
        count>BROKER_CONSOLE_PROBE_MAX_CANDIDATES || (count && (!candidates || !members)))
        return ERROR_INVALID_PARAMETER;
    deadline=GetTickCount64()+timeout;
    write.bytes=sizeof(*request)+count*sizeof(DWORD);
    write.data=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,write.bytes);
    response=HeapAlloc(GetProcessHeap(),0,sizeof(reply)+count);
    chars=wcslen(helper)+32;
    command=HeapAlloc(GetProcessHeap(),0,chars*sizeof(WCHAR));
    if (!write.data || !response || !command) {error=ERROR_NOT_ENOUGH_MEMORY;goto cleanup;}
    request=(BROKER_CONSOLE_PROBE_REQUEST *)write.data;
    pids=(DWORD *)(request+1);
    request->version=BROKER_CONSOLE_PROBE_VERSION;request->count=count;
    error=live_pid(caller,&pid);if (error) goto cleanup;request->caller=pid;
    for (i=0;i<count;++i) {error=live_pid(candidates[i],&pids[i]);if (error) goto cleanup;}
    if (!CreatePipe(&input,&write.pipe,&sa,0) || !CreatePipe(&reader,&output,&sa,0) ||
        !SetHandleInformation(write.pipe,HANDLE_FLAG_INHERIT,0) ||
        !SetHandleInformation(reader,HANDLE_FLAG_INHERIT,0)) {error=GetLastError();goto cleanup;}
    InitializeProcThreadAttributeList(NULL,1,0,&attributeBytes);
    startup.lpAttributeList=HeapAlloc(GetProcessHeap(),0,attributeBytes);
    if (!startup.lpAttributeList) {error=ERROR_NOT_ENOUGH_MEMORY;goto cleanup;}
    if (!InitializeProcThreadAttributeList(startup.lpAttributeList,1,0,&attributeBytes)) {
        error=GetLastError();HeapFree(GetProcessHeap(),0,startup.lpAttributeList);
        startup.lpAttributeList=NULL;goto cleanup;
    }
    inherit[0]=input;inherit[1]=output;
    if (!UpdateProcThreadAttribute(startup.lpAttributeList,0,PROC_THREAD_ATTRIBUTE_HANDLE_LIST,
        inherit,sizeof(inherit),NULL,NULL)) {error=GetLastError();goto cleanup;}
    startup.StartupInfo.cb=sizeof(startup);
    startup.StartupInfo.dwFlags=STARTF_USESTDHANDLES;
    startup.StartupInfo.hStdInput=input;
    startup.StartupInfo.hStdOutput=startup.StartupInfo.hStdError=output;
    swprintf_s(command,chars,L"\"%s\" --internal-console-probe",helper);
    if (!CreateProcessW(helper,command,NULL,NULL,TRUE,DETACHED_PROCESS|EXTENDED_STARTUPINFO_PRESENT,
        NULL,NULL,&startup.StartupInfo,&child)) {error=GetLastError();goto cleanup;}
    CloseHandle(input);input=NULL;CloseHandle(output);output=NULL;
    thread=CreateThread(NULL,0,send_request,&write,0,NULL);
    if (!thread) {error=GetLastError();goto cleanup;}
    for (;;) {
        if (cancel && WaitForSingleObject(cancel,0)!=WAIT_TIMEOUT) {error=ERROR_CANCELLED;break;}
        if (GetTickCount64()>=deadline) {error=ERROR_TIMEOUT;break;}
        if (!PeekNamedPipe(reader,NULL,0,NULL,&available,NULL)) {
            if (GetLastError()!=ERROR_BROKEN_PIPE) error=GetLastError();
            break;
        }
        if (available) {
            if (available>sizeof(reply)+count-received) {error=ERROR_INVALID_DATA;break;}
            if (!ReadFile(reader,response+received,available,&done,NULL) || !done) {
                error=GetLastError();if (!error) error=ERROR_BROKEN_PIPE;break;
            }
            received+=done;
        } else {
            if (WaitForSingleObject(child.hProcess,0)==WAIT_OBJECT_0) break;
            WaitForSingleObject(child.hProcess,10);
        }
    }
    while (!error && WaitForSingleObject(child.hProcess,0)!=WAIT_OBJECT_0) {
        if (cancel && WaitForSingleObject(cancel,0)!=WAIT_TIMEOUT) error=ERROR_CANCELLED;
        else if (GetTickCount64()>=deadline) error=ERROR_TIMEOUT;
        else WaitForSingleObject(child.hProcess,10);
    }
    if (!error && (!GetExitCodeProcess(child.hProcess,&code) || received<sizeof(reply))) error=ERROR_INVALID_DATA;
    if (!error) {
        memcpy(&reply,response,sizeof(reply));
        if (reply.version!=BROKER_CONSOLE_PROBE_VERSION || reply.reserved || code!=reply.status ||
            reply.count!=(reply.status ? 0 : count) || received!=sizeof(reply)+reply.count) error=ERROR_INVALID_DATA;
        else error=reply.status;
        if (!error) for (i=0;i<count;++i)
            if (response[sizeof(reply)+i]>1) {error=ERROR_INVALID_DATA;break;}
    }
cleanup:
    if (child.hProcess) {
        if (WaitForSingleObject(child.hProcess,0)!=WAIT_OBJECT_0) {
            if (!TerminateProcess(child.hProcess,ERROR_CANCELLED) &&
                WaitForSingleObject(child.hProcess,0)!=WAIT_OBJECT_0)
                RaiseFailFastException(NULL,NULL,0); /* Cannot abandon a live owned writer/helper. */
            WaitForSingleObject(child.hProcess,INFINITE);
        }
        CloseHandle(child.hThread);CloseHandle(child.hProcess);
    }
    /* Child closure releases a blocked writer; join before freeing its state. */
    if (thread) {WaitForSingleObject(thread,INFINITE);CloseHandle(thread);if (!error) error=write.error;}
    if (!error) {
        error=live_pid(caller,&pid);
        for (i=0;!error && i<count;++i) error=live_pid(candidates[i],&pid);
        if (!error && count) memcpy(members,response+sizeof(reply),count);
    }
    if (input) CloseHandle(input);if (output) CloseHandle(output);
    if (reader) CloseHandle(reader);if (write.pipe) CloseHandle(write.pipe);
    if (startup.lpAttributeList) {DeleteProcThreadAttributeList(startup.lpAttributeList);HeapFree(GetProcessHeap(),0,startup.lpAttributeList);}
    if (command) HeapFree(GetProcessHeap(),0,command);
    if (response) HeapFree(GetProcessHeap(),0,response);
    if (write.data) HeapFree(GetProcessHeap(),0,write.data);
    return error;
}
