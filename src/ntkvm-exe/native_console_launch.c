#include "native_launch.h"
#include "product-abi/console_io.h"
#include <stdio.h>
#include <string.h>
#include <wchar.h>

DWORD run16_native_launch_pack(const run16_native_start *start,BYTE **output,DWORD *bytes)
{
    PCWSTR strings[4],entry;
    run16_native_launch_packet header={0};
    uint64_t total=sizeof(header);
    DWORD i;
    BYTE *cursor;
    if(!output || !bytes) return ERROR_INVALID_PARAMETER;
    *output=NULL;*bytes=0;
    if(!start || !start->command || !*start->command || !start->directory ||
        !*start->directory || !start->environment || start->console_mask>7) return ERROR_INVALID_PARAMETER;
    strings[0]=start->application ? start->application : L"";
    strings[1]=start->command;strings[2]=start->directory;strings[3]=start->environment;
    for(i=0;i<3;++i) {
        size_t count=wcslen(strings[i])+1;
        if(count>32767) return ERROR_FILENAME_EXCED_RANGE;
        header.characters[i]=(uint32_t)count;
    }
    entry=strings[3];
    if(!*entry) header.characters[3]=2;
    else {
        while(*entry) entry+=wcslen(entry)+1;
        if((uint64_t)(entry-strings[3])+1>MAXDWORD) return ERROR_ARITHMETIC_OVERFLOW;
        header.characters[3]=(uint32_t)(entry-strings[3])+1;
    }
    for(i=0;i<4;++i) total+=(uint64_t)header.characters[i]*sizeof(WCHAR);
    if(total>MAXDWORD) return ERROR_ARITHMETIC_OVERFLOW;
    header.console_mask=start->console_mask;
    for(i=0;i<3;++i) header.standard[i]=(uint64_t)(ULONG_PTR)start->standard[i];
    for(i=0;i<2;++i) header.capabilities[i]=(uint64_t)(ULONG_PTR)start->capabilities[i];
    *output=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,(SIZE_T)total);
    if(!*output) return ERROR_NOT_ENOUGH_MEMORY;
    memcpy(*output,&header,sizeof(header));cursor=*output+sizeof(header);
    for(i=0;i<4;++i) {
        SIZE_T length=(SIZE_T)header.characters[i]*sizeof(WCHAR);
        /* An empty environment is explicitly a double NUL, even if the input
         * pointer designates only one readable terminating character. */
        if(i!=3 || *strings[3]) memcpy(cursor,strings[i],length);
        cursor+=length;
    }
    *bytes=(DWORD)total;
    return ERROR_SUCCESS;
}

DWORD run16_native_launch_unpack(BYTE *payload,DWORD bytes,run16_native_launch_packet *header,WCHAR **strings)
{
    uint64_t total=sizeof(*header);
    DWORD i,offset;
    BYTE *cursor;
    if(!payload || bytes<sizeof(*header)) return ERROR_INVALID_DATA;
    memcpy(header,payload,sizeof(*header));
    if(header->console_mask>7) return ERROR_INVALID_DATA;
    for(i=0;i<4;++i) {
        if(header->characters[i]<(i==3 ? 2u : 1u) || (i<3 && header->characters[i]>32767))
            return ERROR_INVALID_DATA;
        total+=(uint64_t)header->characters[i]*sizeof(WCHAR);
    }
    if(total!=bytes) return ERROR_INVALID_DATA;
    cursor=payload+sizeof(*header);
    for(i=0;i<4;++i) {
        strings[i]=(WCHAR *)cursor;
        if(strings[i][header->characters[i]-1]) return ERROR_INVALID_DATA;
        if(i<3 && wcslen(strings[i])+1!=header->characters[i]) return ERROR_INVALID_DATA;
        cursor+=(SIZE_T)header->characters[i]*sizeof(WCHAR);
    }
    if(!*strings[1] || !*strings[2] || strings[3][header->characters[3]-2]) return ERROR_INVALID_DATA;
    if(!*strings[3]) return header->characters[3]==2 ? ERROR_SUCCESS : ERROR_INVALID_DATA;
    offset=0;
    while(offset<header->characters[3]-1 && strings[3][offset])
        offset+=(DWORD)wcslen(strings[3]+offset)+1;
    return offset==header->characters[3]-1 ? ERROR_SUCCESS : ERROR_INVALID_DATA;
}

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
            swprintf_s(values[i],80,L"%ls%lx",names[i],(unsigned long)(ULONG_PTR)capabilities[i]);
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

DWORD run16_native_launch_start(BYTE *payload,DWORD bytes,PROCESS_INFORMATION *process)
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
        if(i<3 && (header.console_mask&(1u<<i))) source[i]=GetStdHandle(i==0 ? STD_INPUT_HANDLE : i==1 ? STD_OUTPUT_HANDLE : STD_ERROR_HANDLE);
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
        CREATE_UNICODE_ENVIRONMENT|EXTENDED_STARTUPINFO_PRESENT,
        environment,strings[2],&startup.StartupInfo,process)) error=GetLastError();
done:
    if(initialized) DeleteProcThreadAttributeList(startup.lpAttributeList);
    if(startup.lpAttributeList) HeapFree(GetProcessHeap(),0,startup.lpAttributeList);
    if(environment) HeapFree(GetProcessHeap(),0,environment);
    for(i=0;i<used;++i) CloseHandle(unique[i]);
    return error;
}
