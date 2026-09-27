/* Test-only byte-pipe observation. No protocol or completion substitution.
 * Include before the selected transport translation unit, never in product. */
#include <windows.h>
#include <stdio.h>
#include <string.h>
typedef struct observed_console_pending {
    HANDLE pipe;
    OVERLAPPED *io;
    void *buffer;
    DWORD bytes;
    BOOL write;
} observed_console_pending;
static __declspec(thread) observed_console_pending observed_console_io;
static void observed_console_log(const char *phase,HANDLE pipe,BOOL write,
    const void *buffer,DWORD requested,DWORD done,DWORD error)
{
    DWORD saved=GetLastError(),length,written,words[6]={0};
    char prefix[MAX_PATH],path[MAX_PATH],line[256];HANDLE file;
    length=GetEnvironmentVariableA("MVDM_TEST_CONSOLE_WIRE_PATH",prefix,sizeof(prefix));
    if(!length || length>=sizeof(prefix))goto end;
    if(sprintf_s(path,sizeof(path),"%s-%lu.log",prefix,GetCurrentProcessId())<0)goto end;
    if(buffer && (write ? requested : done)>=sizeof(words))memcpy(words,buffer,sizeof(words));
    length=(DWORD)sprintf_s(line,sizeof(line),
        "%llu tid=%lu %s pipe=%p %s requested=%lu done=%lu error=%lu words=%08lx,%08lx,%08lx,%08lx,%08lx,%08lx\r\n",
        GetTickCount64(),GetCurrentThreadId(),phase,pipe,write ? "write":"read",
        requested,done,error,words[0],words[1],words[2],words[3],words[4],words[5]);
    if(!length || length>=sizeof(line))goto end;
    file=CreateFileA(path,FILE_APPEND_DATA,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_ALWAYS,0,NULL);
    if(file!=INVALID_HANDLE_VALUE){WriteFile(file,line,length,&written,NULL);CloseHandle(file);}
end:
    SetLastError(saved);
}
static BOOL observed_console_transfer(BOOL write,HANDLE pipe,void *buffer,DWORD bytes,
    LPDWORD transferred,LPOVERLAPPED io)
{
    BOOL ok;DWORD error,last;
    observed_console_log("begin",pipe,write,buffer,bytes,0,0);
    ok=write ? WriteFile(pipe,buffer,bytes,transferred,io) : ReadFile(pipe,buffer,bytes,transferred,io);
    last=GetLastError();error=ok ? ERROR_SUCCESS:last;
    if(!ok && error==ERROR_IO_PENDING && io) {
        observed_console_io.pipe=pipe;observed_console_io.io=io;
        observed_console_io.buffer=buffer;observed_console_io.bytes=bytes;
        observed_console_io.write=write;
    }
    observed_console_log("submit",pipe,write,buffer,bytes,ok && transferred ? *transferred:0,error);
    SetLastError(last);return ok;
}
static BOOL WINAPI observed_console_read(HANDLE pipe,LPVOID buffer,DWORD bytes,LPDWORD done,LPOVERLAPPED io)
{ return observed_console_transfer(FALSE,pipe,buffer,bytes,done,io); }
static BOOL WINAPI observed_console_write(HANDLE pipe,LPCVOID buffer,DWORD bytes,LPDWORD done,LPOVERLAPPED io)
{ return observed_console_transfer(TRUE,pipe,(void *)buffer,bytes,done,io); }
static BOOL WINAPI observed_console_complete(HANDLE pipe,LPOVERLAPPED io,LPDWORD done,BOOL wait)
{
    BOOL ok=GetOverlappedResult(pipe,io,done,wait);
    DWORD last=GetLastError(),error=ok ? ERROR_SUCCESS:last;
    if(observed_console_io.pipe==pipe && observed_console_io.io==io) {
        observed_console_log("complete",pipe,observed_console_io.write,
            observed_console_io.buffer,observed_console_io.bytes,ok ? *done:0,error);
        if(ok || error!=ERROR_IO_INCOMPLETE)ZeroMemory(&observed_console_io,sizeof(observed_console_io));
    }
    SetLastError(last);return ok;
}
#define ReadFile observed_console_read
#define WriteFile observed_console_write
#define GetOverlappedResult observed_console_complete
