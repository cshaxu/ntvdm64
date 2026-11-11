/* Diagnostic-only TU: real modern provider, real original WSHELL callback,
 * original RET_WINEXEC, and actual adapter unwind. Never a release input. */
#define wow_shell_execute diagnostic_shell_execute
#include "../../src/wow32-dll/source/wow_shell_callback.c"
#undef wow_shell_execute
#include <stdio.h>

static __declspec(thread) LPFNWOWSHELLEXECCB guest_callback;
static __declspec(thread) BOOL exception_injected;

static void record_stack(const char *stage)
{
    char path[MAX_PATH], line[192]; DWORD size, written, error=GetLastError();
    PTD task=CURRENTPTD();
    DWORD logical=task->vpStack, cpu=VDMSTACK();
    HANDLE file;
    size=GetEnvironmentVariableA("MVDM_TEST_SHELL_EXCEPTION",path,sizeof(path));
    if(size && size+7<sizeof(path)) {
        strcat_s(path,sizeof(path),".stack");
        file=CreateFileA(path,FILE_APPEND_DATA,FILE_SHARE_READ|FILE_SHARE_WRITE,
            NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
        if(file!=INVALID_HANDLE_VALUE) {
            size=(DWORD)sprintf_s(line,sizeof(line),"%lu %lu %u %s logical=%08lX cpu=%08lX\r\n",
                GetCurrentProcessId(),GetCurrentThreadId(),task->htask16,stage,logical,cpu);
            WriteFile(file,line,size,&written,NULL);CloseHandle(file);
        }
    }
    SetLastError(error);
}

static void record_boundary(const char *stage, DWORD value)
{
    char path[MAX_PATH], line[160]; DWORD size, written, error=GetLastError();
    HANDLE file;
    size=GetEnvironmentVariableA("MVDM_TEST_SHELL_EXCEPTION",path,sizeof(path));
    if(size && size<sizeof(path)) {
        file=CreateFileA(path,FILE_APPEND_DATA,FILE_SHARE_READ|FILE_SHARE_WRITE,
            NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
        if(file!=INVALID_HANDLE_VALUE) {
            size=(DWORD)sprintf_s(line,sizeof(line),"%lu %lu %u %s %lu\r\n",
                GetCurrentProcessId(),GetCurrentThreadId(),CURRENTPTD()->htask16,stage,value);
            WriteFile(file,line,size,&written,NULL);CloseHandle(file);
        }
    }
    SetLastError(error);
}

static DWORD WINAPI fault_after_original(LPSTR command, WORD show)
{
    DWORD result;
    record_stack("before-original");
    result=guest_callback(command,show);
    record_stack("after-original");
    if (!exception_injected) {
        exception_injected=TRUE;
        record_boundary("original-return",result);
        RaiseException(0xe4470004,0,0,NULL);
    }
    return result;
}

HINSTANCE WINAPI wow_shell_execute(HWND window,LPCSTR operation,LPCSTR file,
    LPSTR parameters,LPCSTR directory,INT show,LPFNWOWSHELLEXECCB callback)
{
    LPFNWOWSHELLEXECCB saved=guest_callback;
    HINSTANCE result;
    guest_callback=callback;
    __try {
        result=diagnostic_shell_execute(window,operation,file,parameters,directory,
            show,callback?fault_after_original:NULL);
    } __finally {
        /* The included production adapter must already have unwound its TLS. */
        record_boundary("tls-clear",current_callback==NULL);
        record_stack("unwind");
        guest_callback=saved;
    }
    return result;
}
