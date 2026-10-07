/* Test-only wrapper of the actual client. No replaced result or payload. */
#include <windows.h>
#include <rpc.h>
#include <stdio.h>
#include "service.h"
#include <base_client.h>
#include <base_command.h>
#include <base_rpc_client.h>
static void note(const char *stage,DWORD result,ULONG a,ULONG b,ULONG c)
{
    DWORD saved=GetLastError(),written;HANDLE file;char path[MAX_PATH],line[256];
    DWORD n=GetEnvironmentVariableA("NTVDM_COMMAND_DIAGNOSTIC",path,sizeof(path));
    if(!n || n>=sizeof(path) || !strstr(path,"\\build\\M0-T436\\S2\\")){SetLastError(saved);return;}
    file=CreateFileA(path,FILE_APPEND_DATA,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,
        OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
    if(file!=INVALID_HANDLE_VALUE) {
        /* Literal stages plus seven bounded32-bit numbers fit256 bytes. */
        int count=wsprintfA(line,
            "pid=%lu tid=%lu stage=%s result=%lu last=%lu a=%lu b=%lu c=%lu\r\n",
            GetCurrentProcessId(),GetCurrentThreadId(),stage,result,saved,a,b,c);
        if(count>0)WriteFile(file,line,(DWORD)count,&written,NULL);
        CloseHandle(file);
    }
    SetLastError(saved);
}
static error_status_t traced_get(handle_t binding,VDM_CONNECTION connection,HANDLE process,
    unsigned long generation,unsigned long bytes,byte *request,
    unsigned long *wait_count,HANDLE **waits,unsigned long *pipe_mask,
    unsigned long *pipe_count,HANDLE **pipes,unsigned long *file_mask,
    unsigned long *file_count,HANDLE **files,unsigned long *reply_bytes,byte **reply)
{
    error_status_t result=Client_Get(binding,connection,process,generation,bytes,request,
        wait_count,waits,pipe_mask,pipe_count,pipes,file_mask,file_count,files,reply_bytes,reply);
    note("rpc-get",result,*wait_count,*pipe_count,*file_count);
    note("rpc-masks",result,*pipe_mask,*file_mask,*reply_bytes);
    return result;
}
static BOOL traced_apply(const void *data,uint32_t bytes,uint32_t generation,
    uint32_t request,PBASE_API_MSG message)
{
    BOOL result=OpenNtBaseApplyGetCommand(data,bytes,generation,request,message);
    note("decode",result,result ? message->ReturnValue : 0,bytes,
        result ? message->u.GetNextVDMCommand.VDMState : 0);
    return result;
}
#define Client_Get traced_get
#define OpenNtBaseApplyGetCommand traced_apply
#define OpenNtBaseClientSetCommandBinding diagnostic_actual_binding
#include "../../src/ntsrv-exe/opennt/source/base_rpc_client.c"
#undef Client_Get
#undef OpenNtBaseApplyGetCommand
#undef OpenNtBaseClientSetCommandBinding
static DWORD (*saved_ready)(void *);
static DWORD traced_ready(void *context)
{
    DWORD result=saved_ready(context);note("command-ready",result,0,0,0);return result;
}
void OpenNtBaseClientSetCommandBinding(DWORD (*ready)(void *),void *context)
{
    saved_ready=ready;diagnostic_actual_binding(ready ? traced_ready : NULL,context);
}
