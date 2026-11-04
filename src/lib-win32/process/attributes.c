#include "attributes.h"
DWORD win32_process_attributes_init(win32_process_attributes *state, DWORD_PTR attribute,
    void *value, SIZE_T size)
{
    SIZE_T bytes=0;
    if(!state)return ERROR_INVALID_PARAMETER;
    if(state->list || state->initialized)return ERROR_INVALID_STATE;
    InitializeProcThreadAttributeList(NULL,1,0,&bytes);
    state->list=HeapAlloc(GetProcessHeap(),0,bytes);
    if(!state->list)return ERROR_NOT_ENOUGH_MEMORY;
    if(!InitializeProcThreadAttributeList(state->list,1,0,&bytes))return GetLastError();
    state->initialized=TRUE;
    if(!UpdateProcThreadAttribute(state->list,0,attribute,value,size,NULL,NULL))return GetLastError();
    return ERROR_SUCCESS;
}
void win32_process_attributes_destroy(win32_process_attributes *state)
{
    if(!state)return;
    if(state->initialized)DeleteProcThreadAttributeList(state->list);
    if(state->list)HeapFree(GetProcessHeap(),0,state->list);
    state->list=NULL;state->initialized=FALSE;
}
