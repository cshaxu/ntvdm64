#if !defined(_WIN32_WINNT) || _WIN32_WINNT < 0x0A00
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif
#include "native_conpty.h"
#include <stdio.h>

struct ntkvm_conpty {
    HPCON console;
    HANDLE input,output,reader,stop,ended,written,cancel;
    CRITICAL_SECTION write_lock,lifecycle_lock;
    ntkvm_conpty_output consume;
    void *context;
#ifdef NTKVM_CONPTY_TEST_RELEASE
    BOOL released;
#endif
    volatile LONG error;
};

static DWORD hresult_error(HRESULT status)
{
    return HRESULT_FACILITY(status)==FACILITY_WIN32 ? HRESULT_CODE(status) : ERROR_GEN_FAILURE;
}

static void failed(ntkvm_conpty *pty,DWORD error)
{
    InterlockedCompareExchange(&pty->error,(LONG)error,0);
    SetEvent(pty->stop);SetEvent(pty->ended);
}

static DWORD WINAPI read_output(void *context)
{
    ntkvm_conpty *pty=context;
    BYTE bytes[8192];DWORD count,error=0;
    for(;;) {
        if(!ReadFile(pty->output,bytes,sizeof(bytes),&count,NULL)) {
            error=GetLastError();
            if(error==ERROR_BROKEN_PIPE)error=0;
            break;
        }
        if(!count)break;
        error=pty->consume(pty->context,bytes,count);
        if(error) {
            /* Keep draining after parser failure, so ClosePseudoConsole cannot
             * deadlock on a full output pipe. Stop publishing failed output. */
            failed(pty,error);
            while(ReadFile(pty->output,bytes,sizeof(bytes),&count,NULL) && count) {}
            break;
        }
    }
    if(error)failed(pty,error);
    SetEvent(pty->ended);
    return 0;
}

static DWORD input_pipe(HANDLE *writer,HANDLE *reader)
{
    static LONG serial;
    WCHAR name[128];DWORD error;
    swprintf_s(name,128,L"\\\\.\\pipe\\ntkvm-conpty-%lu-%lu",GetCurrentProcessId(),
        (DWORD)InterlockedIncrement(&serial));
    *writer=CreateNamedPipeW(name,PIPE_ACCESS_OUTBOUND|FILE_FLAG_OVERLAPPED|FILE_FLAG_FIRST_PIPE_INSTANCE,
        PIPE_TYPE_BYTE|PIPE_WAIT|PIPE_REJECT_REMOTE_CLIENTS,1,65536,65536,0,NULL);
    if(*writer==INVALID_HANDLE_VALUE) { *writer=NULL;return GetLastError(); }
    *reader=CreateFileW(name,GENERIC_READ,0,NULL,OPEN_EXISTING,
        SECURITY_SQOS_PRESENT|SECURITY_IDENTIFICATION,NULL);
    if(*reader==INVALID_HANDLE_VALUE) {error=GetLastError();*reader=NULL;return error;}
    return ERROR_SUCCESS;
}

DWORD ntkvm_conpty_open_events(COORD size,ntkvm_conpty_output consume,void *context,HANDLE stop,HANDLE ended,BOOL inherit_cursor,ntkvm_conpty **result)
{
    ntkvm_conpty *pty;HANDLE input=NULL,output=NULL;
    DWORD error=0;HRESULT status;
    if(!result)return ERROR_INVALID_PARAMETER;
    *result=NULL;
    if(!consume || size.X<=0 || size.Y<=0)return ERROR_INVALID_PARAMETER;
    pty=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*pty));
    if(!pty)return ERROR_NOT_ENOUGH_MEMORY;
    InitializeCriticalSection(&pty->write_lock);InitializeCriticalSection(&pty->lifecycle_lock);
    pty->consume=consume;pty->context=context;
    pty->stop=CreateEventW(NULL,TRUE,FALSE,NULL);
    if(stop && !DuplicateHandle(GetCurrentProcess(),stop,GetCurrentProcess(),&pty->cancel,
        SYNCHRONIZE,FALSE,0)) {error=GetLastError();goto done;}
    if(ended)DuplicateHandle(GetCurrentProcess(),ended,GetCurrentProcess(),&pty->ended,
        SYNCHRONIZE|EVENT_MODIFY_STATE,FALSE,0);
    else pty->ended=CreateEventW(NULL,TRUE,FALSE,NULL);
    pty->written=CreateEventW(NULL,TRUE,FALSE,NULL);
    if(!pty->stop || !pty->ended || !pty->written) {error=GetLastError();goto done;}
    error=input_pipe(&pty->input,&input);
    if(error)goto done;
    if(!CreatePipe(&pty->output,&output,NULL,65536)) {error=GetLastError();goto done;}
    /* Reader precedes ConPTY creation: even startup output cannot fill its
     * pipe while no one is available to consume it. */
    pty->reader=CreateThread(NULL,0,read_output,pty,0,NULL);
    if(!pty->reader) {error=GetLastError();goto done;}
    status=CreatePseudoConsole(size,input,output,inherit_cursor ? PSEUDOCONSOLE_INHERIT_CURSOR : 0,&pty->console);
    if(FAILED(status))error=hresult_error(status);
done:
    if(input)CloseHandle(input);
    if(output)CloseHandle(output);
    if(error)ntkvm_conpty_close(pty);else *result=pty;
    return error;
}

DWORD ntkvm_conpty_open(COORD size,ntkvm_conpty_output consume,void *context,ntkvm_conpty **result)
{
    return ntkvm_conpty_open_events(size,consume,context,NULL,NULL,FALSE,result);
}

DWORD ntkvm_conpty_launch(ntkvm_conpty *pty,const run16_native_start *start,PROCESS_INFORMATION *process)
{
    DWORD error;
    if(!pty || !process)return ERROR_INVALID_PARAMETER;
    ZeroMemory(process,sizeof(*process));
    EnterCriticalSection(&pty->lifecycle_lock);
    /* Release relinquishes the Console reference used for process attachment.
     * CreateProcess may still succeed afterward, but in the wrong Console.
     * Retirement is one-way; never report that as a successful ConPTY launch. */
    if(WaitForSingleObject(pty->ended,0)==WAIT_OBJECT_0)error=ERROR_BROKEN_PIPE;
#ifdef NTKVM_CONPTY_TEST_RELEASE
    else if(pty->released)error=ERROR_SHUTDOWN_IN_PROGRESS;
#endif
    else error=run16_native_launch_conpty(start,pty->console,process);
    LeaveCriticalSection(&pty->lifecycle_lock);
    return error;
}

DWORD ntkvm_conpty_write(ntkvm_conpty *pty,const void *data,DWORD bytes,DWORD *delivered)
{
    const BYTE *cursor=data;DWORD error=0;
    if(!delivered)return ERROR_INVALID_PARAMETER;
    *delivered=0;
    if(!pty || (!data && bytes))return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&pty->write_lock);
    while(*delivered<bytes) {
        OVERLAPPED io={0};DWORD count=0,wait;
        HANDLE events[4]={pty->written,pty->stop,pty->ended,pty->cancel};
        if(pty->cancel && WaitForSingleObject(pty->cancel,0)==WAIT_OBJECT_0) {error=ERROR_OPERATION_ABORTED;break;}
        if(WaitForSingleObject(pty->ended,0)==WAIT_OBJECT_0 && !pty->error) {
            /* Clean output EOF is already observed. No current recipient:
             * leave this unsent input with the frontend, not a sticky fault. */
            error=ERROR_NO_MORE_ITEMS;break;
        }
        if(WaitForSingleObject(pty->stop,0)==WAIT_OBJECT_0 ||
            WaitForSingleObject(pty->ended,0)==WAIT_OBJECT_0) {error=ERROR_BROKEN_PIPE;break;}
        ResetEvent(pty->written);io.hEvent=pty->written;
        if(!WriteFile(pty->input,cursor+*delivered,bytes-*delivered,&count,&io)) {
            error=GetLastError();
            if(error!=ERROR_IO_PENDING)break;
            wait=WaitForMultipleObjects(pty->cancel ? 4 : 3,events,FALSE,INFINITE);
            if(wait!=WAIT_OBJECT_0)CancelIoEx(pty->input,&io);
            if(!GetOverlappedResult(pty->input,&io,&count,TRUE)) {error=GetLastError();break;}
            error=wait==WAIT_OBJECT_0 ? 0 : ERROR_OPERATION_ABORTED;
        }
        if(count>bytes-*delivered) {error=ERROR_INVALID_DATA;break;}
        *delivered+=count;
        if(error)break;
        if(!count) {error=ERROR_BROKEN_PIPE;break;}
    }
    if(error==ERROR_NO_MORE_ITEMS && *delivered)error=ERROR_WRITE_FAULT;
    if(error && error!=ERROR_NO_MORE_ITEMS)failed(pty,error);
    LeaveCriticalSection(&pty->write_lock);
    return error;
}

DWORD ntkvm_conpty_resize(ntkvm_conpty *pty,COORD size)
{
    HRESULT status;
    if(!pty || size.X<=0 || size.Y<=0)return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&pty->lifecycle_lock);
    status=ResizePseudoConsole(pty->console,size);
    LeaveCriticalSection(&pty->lifecycle_lock);
    return FAILED(status) ? hresult_error(status) : 0;
}

#ifdef NTKVM_CONPTY_TEST_RELEASE
/* Resource experiments only; production retains HPCON until explicit close. */
DWORD ntkvm_conpty_release(ntkvm_conpty *pty)
{
    typedef HRESULT (WINAPI *release_console)(HPCON);
    release_console release;
    HRESULT status=S_OK;
    if(!pty)return ERROR_INVALID_PARAMETER;
    release=(release_console)GetProcAddress(GetModuleHandleW(L"kernel32.dll"),"ReleasePseudoConsole");
    if(!release)return ERROR_NOT_SUPPORTED;
    EnterCriticalSection(&pty->lifecycle_lock);
    if(!pty->released) {
        status=release(pty->console);
        if(SUCCEEDED(status))pty->released=TRUE;
    }
    LeaveCriticalSection(&pty->lifecycle_lock);
    return FAILED(status) ? hresult_error(status) : 0;
}
#endif

HANDLE ntkvm_conpty_ended(ntkvm_conpty *pty) { return pty ? pty->ended : NULL; }
DWORD ntkvm_conpty_error(ntkvm_conpty *pty) { return pty ? (DWORD)InterlockedCompareExchange(&pty->error,0,0) : ERROR_INVALID_PARAMETER; }
void ntkvm_conpty_cancel(ntkvm_conpty *pty) { if(pty)SetEvent(pty->stop); }

void ntkvm_conpty_close(ntkvm_conpty *pty)
{
    if(!pty)return;
    if(pty->stop)SetEvent(pty->stop);
    if(pty->console)ClosePseudoConsole(pty->console);
    if(pty->reader) {
        if(WaitForSingleObject(pty->reader,5000)!=WAIT_OBJECT_0)CancelSynchronousIo(pty->reader);
        WaitForSingleObject(pty->reader,INFINITE);CloseHandle(pty->reader);
    }
    if(pty->input)CloseHandle(pty->input);
    if(pty->output)CloseHandle(pty->output);
    if(pty->stop)CloseHandle(pty->stop);
    if(pty->cancel)CloseHandle(pty->cancel);
    if(pty->ended)CloseHandle(pty->ended);
    if(pty->written)CloseHandle(pty->written);
    DeleteCriticalSection(&pty->write_lock);DeleteCriticalSection(&pty->lifecycle_lock);
    HeapFree(GetProcessHeap(),0,pty);
}
