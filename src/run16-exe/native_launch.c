/* Native process creation uses a restricted inherited-handle list, not ConPTY.
 * Keep the modern declaration requirement outside original OpenNT units. */
#if !defined(_WIN32_WINNT) || _WIN32_WINNT < 0x0A00
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif
#include "run16-exe/native_launch.h"
#include "common/protocol/console_io.h"
#include <stdio.h>
#include <string.h>
#include <wchar.h>

static DWORD environment_copy(PCWSTR source,HANDLE *capabilities,PWSTR *output)
{
    static const WCHAR *names[2]={L"NTVDM_FRONTEND_CAPABILITY=",L"NTVDM_EXECUTION_CONSOLE="};
    WCHAR values[2][80];
    PCWSTR entry;
    PWSTR destination;
    SIZE_T count=2,length;
    DWORD i;
    if((capabilities[0]!=NULL)!=(capabilities[1]!=NULL)) return ERROR_INVALID_PARAMETER;
    *output=NULL;
    for(entry=source;*entry;entry+=wcslen(entry)+1) count+=wcslen(entry)+1;
    for(i=0;i<2;++i) {
        values[i][0]=0;
        if(capabilities[i]) {
            swprintf_s(values[i],80,L"%ls%llx",names[i],(unsigned long long)(ULONG_PTR)capabilities[i]);
            count+=wcslen(values[i])+1;
        }
    }
    if(count>MAXDWORD/sizeof(WCHAR)) return ERROR_ARITHMETIC_OVERFLOW;
    *output=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,count*sizeof(WCHAR));
    if(!*output) return ERROR_NOT_ENOUGH_MEMORY;
    destination=*output;
    for(entry=source;*entry;entry+=length+1) {
        length=wcslen(entry);
        if(!_wcsnicmp(entry,CONSOLE_COMMAND_STREAMS_WENTRY,wcslen(CONSOLE_COMMAND_STREAMS_WENTRY)))continue;
        for(i=0;i<2;++i) if(!_wcsnicmp(entry,names[i],wcslen(names[i]))) break;
        if(i<2) continue;
        memcpy(destination,entry,(length+1)*sizeof(WCHAR));destination+=length+1;
    }
    for(i=0;i<2;++i) if(values[i][0]) {
        length=wcslen(values[i])+1;memcpy(destination,values[i],length*sizeof(WCHAR));destination+=length;
    }
    return ERROR_SUCCESS;
}

static DWORD native_launch_start(BYTE *payload,DWORD bytes,PROCESS_INFORMATION *process,BOOL suspended)
{
    run16_native_launch_packet header;
    WCHAR *strings[4],*environment=NULL;
    HANDLE source[5],inherited[5]={0},unique[5];
    DWORD error,i,j,used=0;
    STARTUPINFOEXW startup={0};
    SIZE_T attributes=0;
    BOOL initialized=FALSE;
    if(!process) return ERROR_INVALID_PARAMETER;
    ZeroMemory(process,sizeof(*process));
    error=run16_native_launch_unpack(payload,bytes,&header,strings);
    if(error) return error;
    for(i=0;i<5;++i) {
        uint64_t raw=i<3 ? header.standard[i] : header.capabilities[i-3];
        if(raw>(uint64_t)(ULONG_PTR)-1) return ERROR_INVALID_HANDLE;
        source[i]=(HANDLE)(ULONG_PTR)raw;
        if(i>=3 && source[i]==INVALID_HANDLE_VALUE) return ERROR_INVALID_HANDLE;
        if(i<3 && (header.console_mask&(1u<<i))) source[i]=
            GetStdHandle(i==0 ? STD_INPUT_HANDLE : i==1 ? STD_OUTPUT_HANDLE : STD_ERROR_HANDLE);
    }
    for(i=0;i<5;++i) {
        if(!source[i] || source[i]==INVALID_HANDLE_VALUE) { inherited[i]=source[i];continue; }
        for(j=0;j<i;++j) if(source[j]==source[i]) break;
        if(j<i) { inherited[i]=inherited[j];continue; }
        if(!DuplicateHandle(GetCurrentProcess(),source[i],GetCurrentProcess(),&inherited[i],
            0,TRUE,DUPLICATE_SAME_ACCESS)) { error=GetLastError();goto done; }
        unique[used++]=inherited[i];
    }
    error=environment_copy(strings[3],inherited+3,&environment);
    if(error) goto done;
    startup.StartupInfo.cb=sizeof(startup);
    /* Explicit stream selection preserves redirected files/pipes while
     * Console slots refer to the native worker's actual attached Console. */
    startup.StartupInfo.dwFlags=STARTF_USESTDHANDLES;
    startup.StartupInfo.hStdInput=inherited[0];startup.StartupInfo.hStdOutput=inherited[1];startup.StartupInfo.hStdError=inherited[2];
    /* Only requested standard streams and the two authenticated capabilities
     * cross this creation. No protocol pipe or earlier target handle escapes. */
    InitializeProcThreadAttributeList(NULL,1,0,&attributes);
    startup.lpAttributeList=HeapAlloc(GetProcessHeap(),0,attributes);
    if(!startup.lpAttributeList) { error=ERROR_NOT_ENOUGH_MEMORY;goto done; }
    if(!InitializeProcThreadAttributeList(startup.lpAttributeList,1,0,&attributes)) { error=GetLastError();goto done; }
    initialized=TRUE;
    if(used && !UpdateProcThreadAttribute(startup.lpAttributeList,0,PROC_THREAD_ATTRIBUTE_HANDLE_LIST,
        unique,used*sizeof(HANDLE),NULL,NULL)) { error=GetLastError();goto done; }
    if(!CreateProcessW(*strings[0] ? strings[0] : NULL,strings[1],NULL,NULL,used!=0,
        CREATE_UNICODE_ENVIRONMENT|EXTENDED_STARTUPINFO_PRESENT|(suspended ? CREATE_SUSPENDED : 0),
        environment,strings[2],&startup.StartupInfo,process)) error=GetLastError();
done:
    if(initialized) DeleteProcThreadAttributeList(startup.lpAttributeList);
    if(startup.lpAttributeList) HeapFree(GetProcessHeap(),0,startup.lpAttributeList);
    if(environment) HeapFree(GetProcessHeap(),0,environment);
    for(i=0;i<used;++i) CloseHandle(unique[i]);
    return error;
}
DWORD run16_native_launch_start_suspended(BYTE *payload,DWORD bytes,PROCESS_INFORMATION *process)
{ return native_launch_start(payload,bytes,process,TRUE); }
DWORD run16_native_launch_start(BYTE *payload,DWORD bytes,PROCESS_INFORMATION *process)
{ return native_launch_start(payload,bytes,process,FALSE); }
