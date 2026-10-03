#include "common/console/members.h"

DWORD common_console_members_read(DWORD initial,DWORD limit,DWORD reads,
    DWORD **members,DWORD *count)
{
    DWORD capacity=initial,attempt=0,found,error;
    DWORD *buffer=NULL;
    if(!members || !count)return ERROR_INVALID_PARAMETER;
    *members=NULL;*count=0;
    if(!initial || initial>limit || limit>MAXDWORD/sizeof(*buffer))
        return ERROR_INVALID_PARAMETER;
    for(;;) {
        buffer=HeapAlloc(GetProcessHeap(),0,(SIZE_T)capacity*sizeof(*buffer));
        if(!buffer)return ERROR_NOT_ENOUGH_MEMORY;
        SetLastError(ERROR_SUCCESS);
        found=GetConsoleProcessList(buffer,capacity);
        ++attempt;
        if(!found) {
            error=GetLastError();
            if(!error)error=ERROR_GEN_FAILURE;
            break;
        }
        if(found<=capacity) {
            *members=buffer;*count=found;
            return ERROR_SUCCESS;
        }
        if(reads && attempt>=reads){error=ERROR_RETRY;break;}
        if(found>limit){error=ERROR_BUFFER_OVERFLOW;break;}
        HeapFree(GetProcessHeap(),0,buffer);buffer=NULL;
        capacity=found;
    }
    HeapFree(GetProcessHeap(),0,buffer);
    return error;
}

void common_console_members_release(DWORD *members)
{
    if(members)HeapFree(GetProcessHeap(),0,members);
}
