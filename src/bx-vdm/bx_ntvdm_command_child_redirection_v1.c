#include "bx_ntvdm_command_child_redirection_v1.h"

#include <windows.h>
#include <string.h>

#define BX_NTVDM_COMMAND_CHILD_REDIRECTION_COMMAND_BYTES 260u

static int selected_execution(const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window)
{
    return event != 0 && cpu != 0 && window != 0 &&
        bx_ntvdm_exception_event_v1_valid(event) &&
        bx_ntvdm_cpu_state_v1_valid(cpu) &&
        bx_ntvdm_instruction_window_v1_valid(window) && event->vector == 6u &&
        cpu->execution_mode == BX_NTVDM_CPU_EXECUTION_REAL &&
        event->fault_rip <= UINT64_MAX - 4u && window->valid_bytes >= 4u &&
        window->bytes[0] == 0xc4u && window->bytes[1] == 0xc4u &&
        window->bytes[2] == 0x54u && window->bytes[3] == 8u;
}
static int terminated(const uint8_t *value, uint32_t bytes)
{ return value != 0 && bytes >= 2u && value[bytes - 1u] == 0u; }
static int environment_terminated(const uint8_t *value, uint32_t bytes)
{ return value != 0 && bytes >= 2u && value[bytes - 1u] == 0u && value[bytes - 2u] == 0u; }
void bx_ntvdm_command_child_redirection_v1_initialize(bx_ntvdm_command_child_redirection_v1 *value)
{ if (value) { memset(value,0,sizeof(*value)); value->magic=BX_NTVDM_COMMAND_CHILD_REDIRECTION_V1_MAGIC; value->abi_version=BX_NTVDM_COMMAND_CHILD_REDIRECTION_V1_VERSION; value->struct_bytes=sizeof(*value); } }
int bx_ntvdm_command_child_redirection_v1_valid(const bx_ntvdm_command_child_redirection_v1 *value)
{ return value && value->magic==BX_NTVDM_COMMAND_CHILD_REDIRECTION_V1_MAGIC && value->abi_version==BX_NTVDM_COMMAND_CHILD_REDIRECTION_V1_VERSION && value->struct_bytes==sizeof(*value) && value->reserved0==0u; }
int bx_ntvdm_command_child_redirection_v1_launch(bx_ntvdm_command_child_redirection_v1 *value,const bx_ntvdm_command_stream_session_v1 *stream,const uint8_t *command,uint32_t command_bytes,const uint8_t *environment,uint32_t environment_bytes,const bx_ntvdm_command_host_context_v1 *host_context,const bx_ntvdm_exception_event_v1 *event,const bx_ntvdm_cpu_state_v1 *cpu,const bx_ntvdm_instruction_window_v1 *window,bx_ntvdm_cpu_result_v2 *result)
{
    wchar_t command_wide[BX_NTVDM_COMMAND_CHILD_REDIRECTION_COMMAND_BYTES], environment_wide[BX_NTVDM_COMMAND_HOST_CONTEXT_V1_ENVIRONMENT_BYTES], directory_wide[BX_NTVDM_COMMAND_HOST_CONTEXT_V1_DIRECTORY_BYTES];
    HANDLE duplicated[3]={INVALID_HANDLE_VALUE,INVALID_HANDLE_VALUE,INVALID_HANDLE_VALUE}; STARTUPINFOW startup={0}; PROCESS_INFORMATION process={0}; DWORD exit_code=0u,error=ERROR_NOT_SUPPORTED; uint32_t slot;
    if(!bx_ntvdm_command_child_redirection_v1_valid(value)||!bx_ntvdm_command_stream_session_v1_valid(stream)||!terminated(command,command_bytes)||command_bytes>BX_NTVDM_COMMAND_CHILD_REDIRECTION_COMMAND_BYTES||!environment_terminated(environment,environment_bytes)||environment_bytes>BX_NTVDM_COMMAND_HOST_CONTEXT_V1_ENVIRONMENT_BYTES||!bx_ntvdm_command_host_context_v1_valid(host_context)||!result||!selected_execution(event,cpu,window))return 0;
    if(!MultiByteToWideChar(CP_OEMCP,0,(const char*)command,(int)command_bytes,command_wide,BX_NTVDM_COMMAND_CHILD_REDIRECTION_COMMAND_BYTES)||!MultiByteToWideChar(CP_OEMCP,0,(const char*)environment,(int)environment_bytes,environment_wide,BX_NTVDM_COMMAND_HOST_CONTEXT_V1_ENVIRONMENT_BYTES)){error=GetLastError();goto complete;}
    for(slot=0u;slot<3u;++slot)if(!DuplicateHandle(GetCurrentProcess(),(HANDLE)stream->private_handle_values[slot],GetCurrentProcess(),&duplicated[slot],0u,TRUE,DUPLICATE_SAME_ACCESS)){error=GetLastError();goto complete;}
    startup.cb=sizeof(startup);startup.dwFlags=STARTF_USESTDHANDLES;startup.hStdInput=duplicated[0];startup.hStdOutput=duplicated[1];startup.hStdError=duplicated[2];
    if(!MultiByteToWideChar(CP_OEMCP,0,(const char*)host_context->selected_directory,(int)host_context->directory_bytes,directory_wide,BX_NTVDM_COMMAND_HOST_CONTEXT_V1_DIRECTORY_BYTES)){error=GetLastError();goto complete;}
    if(!CreateProcessW(0,command_wide,0,0,TRUE,CREATE_UNICODE_ENVIRONMENT,environment_wide,GetFileAttributesW(directory_wide)==INVALID_FILE_ATTRIBUTES?0:directory_wide,&startup,&process)){error=GetLastError();goto complete;}
    ++value->launch_count;if(WaitForSingleObject(process.hProcess,INFINITE)!=WAIT_OBJECT_0||!GetExitCodeProcess(process.hProcess,&exit_code))error=GetLastError();else{value->last_result=exit_code;++value->completion_count;error=ERROR_SUCCESS;}
complete:
    if(process.hThread)CloseHandle(process.hThread);if(process.hProcess)CloseHandle(process.hProcess);for(slot=0u;slot<3u;++slot)if(duplicated[slot]!=INVALID_HANDLE_VALUE)CloseHandle(duplicated[slot]);value->last_error=error;
    return bx_ntvdm_cpu_result_v2_resume(result,event->fault_rip+4u)&&bx_ntvdm_cpu_result_v2_set_cf(result,0)&&bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta,0u,(uint16_t)((cpu->eax&0xff00u)|((error==ERROR_SUCCESS?exit_code:error)&0xffu)));
}