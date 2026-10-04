#include "guest_environment.h"
#include <string.h>

DWORD run16_guest_environment_root(PCSTR input,DWORD bytes,PCSTR root,
    PSTR *output,DWORD *output_bytes)
{
    static const char prefix[]="SYSTEMROOT=";
    DWORD offset=0;
    size_t kept=0,entry,root_bytes,total;
    PSTR copy,destination;
    PCSTR end;
    if(!output || !output_bytes)return ERROR_INVALID_PARAMETER;
    *output=NULL;*output_bytes=0;
    if(!input || bytes<2 || input[bytes-1] || input[bytes-2] ||
        !root || !*root)return ERROR_INVALID_PARAMETER;
    while(offset<bytes && input[offset]) {
        end=memchr(input+offset,0,bytes-offset);
        if(!end)return ERROR_INVALID_PARAMETER;
        entry=(size_t)(end-(input+offset))+1u;
        if(_strnicmp(input+offset,prefix,sizeof(prefix)-1u))kept+=entry;
        offset+=(DWORD)entry;
    }
    /* Only terminators may remain, not hidden trailing entries. */
    while(offset<bytes)if(input[offset++])return ERROR_INVALID_PARAMETER;
    root_bytes=strlen(root);
    total=kept+sizeof(prefix)-1u+root_bytes+2u;
    if(total>=65535u)return ERROR_BUFFER_OVERFLOW;
    copy=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,total+1u);
    if(!copy)return ERROR_NOT_ENOUGH_MEMORY;
    destination=copy;
    for(offset=0;input[offset];offset+=(DWORD)entry) {
        entry=strlen(input+offset)+1u;
        if(_strnicmp(input+offset,prefix,sizeof(prefix)-1u)) {
            memcpy(destination,input+offset,entry);destination+=entry;
        }
    }
    memcpy(destination,prefix,sizeof(prefix)-1u);destination+=sizeof(prefix)-1u;
    memcpy(destination,root,root_bytes);
    *output=copy;*output_bytes=(DWORD)total;
    return ERROR_SUCCESS;
}
