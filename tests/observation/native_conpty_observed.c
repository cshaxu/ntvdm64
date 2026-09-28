/* Test-only observation of the unchanged production resource implementation.
 * It neither retries nor changes transport, completion or error semantics. */
#define _WIN32_WINNT 0x0A00
#include <windows.h>
static BOOL WINAPI observed_read(HANDLE,LPVOID,DWORD,LPDWORD,LPOVERLAPPED);
#define ReadFile observed_read
#define ntkvm_conpty_launch observed_conpty_launch
#define ntkvm_conpty_write observed_conpty_write
#define ntkvm_conpty_resize observed_conpty_resize
#include "../../src/ntkvm-exe/native_conpty.c"
#undef ntkvm_conpty_write
#undef ntkvm_conpty_resize
#undef ntkvm_conpty_launch
#undef ReadFile

static void record_result(const char *operation,DWORD error,const void *bytes,
    DWORD count,DWORD delivered)
{
    WCHAR path[MAX_PATH];char line[2048];DWORD length,written,i;HANDLE file;
    const BYTE *data=bytes;int used;
    length=GetEnvironmentVariableW(L"NTKVM_TEST_CONPTY_TRACE",path,MAX_PATH);
    if(!length || length>=MAX_PATH)return;
    file=CreateFileW(path,FILE_APPEND_DATA,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,
        OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
    if(file==INVALID_HANDLE_VALUE)return;
    used=sprintf_s(line,sizeof(line),"%llu %s error=%lu bytes=%lu delivered=%lu hex=",
        GetTickCount64(),operation,error,count,delivered);
    for(i=0;data && i<count && i<800;++i)
        used+=sprintf_s(line+used,sizeof(line)-(size_t)used,"%02x",data[i]);
    used+=sprintf_s(line+used,sizeof(line)-(size_t)used,"\r\n");
    WriteFile(file,line,(DWORD)used,&written,NULL);CloseHandle(file);
}

static BOOL WINAPI observed_read(HANDLE pipe,LPVOID data,DWORD bytes,LPDWORD count,LPOVERLAPPED overlapped)
{
    BOOL ok=ReadFile(pipe,data,bytes,count,overlapped);DWORD error=ok ? 0 : GetLastError();
    record_result("read",error,ok ? data : NULL,ok && count ? *count : 0,0);
    if(!ok)SetLastError(error);return ok;
}
DWORD ntkvm_conpty_launch(ntkvm_conpty *pty,const run16_native_start *start,PROCESS_INFORMATION *process)
{
    DWORD error=observed_conpty_launch(pty,start,process);
    record_result("launch",error,start->command,(DWORD)(wcslen(start->command)*sizeof(WCHAR)),start->console_mask);
    return error;
}

DWORD ntkvm_conpty_write(ntkvm_conpty *pty,const void *data,DWORD bytes,DWORD *delivered)
{
    DWORD error=observed_conpty_write(pty,data,bytes,delivered);
    record_result("write",error,data,bytes,delivered ? *delivered : 0);
    return error;
}
DWORD ntkvm_conpty_resize(ntkvm_conpty *pty,COORD size)
{
    DWORD error=observed_conpty_resize(pty,size);
    record_result("resize",error,NULL,(DWORD)size.X,(DWORD)size.Y);
    return error;
}
