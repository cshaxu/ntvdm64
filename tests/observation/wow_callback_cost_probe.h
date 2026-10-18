#include <tlhelp32.h>
#include "wow_callback_cost_data.h"

/* Read-only native diagnostic ABI, never a guest/NT4 layout decoder. */
static BOOL callback_cost_open(DWORD pid,const char *name,DWORD rva,
    HANDLE *process,ULONG_PTR *address)
{
    MODULEENTRY32 module={sizeof(module)};
    HANDLE snapshot;
    *process=NULL;*address=0;
    if(!rva)return FALSE;
    *process=OpenProcess(PROCESS_VM_READ|PROCESS_QUERY_INFORMATION,FALSE,pid);
    if(!*process)return FALSE;
    snapshot=CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,pid);
    if(snapshot!=INVALID_HANDLE_VALUE){
        if(Module32First(snapshot,&module))do{
            if(!_stricmp(module.szModule,name) &&
                    module.modBaseSize>=sizeof(callback_cost_data) &&
                    rva<=module.modBaseSize-sizeof(callback_cost_data)){
                *address=(ULONG_PTR)module.modBaseAddr+rva;break;
            }
        }while(Module32Next(snapshot,&module));
        CloseHandle(snapshot);
    }
    if(!*address){CloseHandle(*process);*process=NULL;return FALSE;}
    return TRUE;
}
/* 1=complete immutable sample;0=not yet published;-1=invalid ABI/identity. */
static int callback_cost_read(HANDLE process,ULONG_PTR address,
    callback_cost_data *data)
{
    SIZE_T bytes;
    LONG after;
    LARGE_INTEGER frequency;
    ULONG i;
    if(!ReadProcessMemory(process,(const void *)address,data,sizeof(*data),&bytes) ||
            bytes!=sizeof(*data))return -1;
    QueryPerformanceFrequency(&frequency);
    if(data->magic!=CALLBACK_COST_MAGIC || data->version!=2 ||
            data->capacity!=CALLBACK_COST_CAPACITY || data->reserved<0 ||
            data->reserved>(LONG)CALLBACK_COST_CAPACITY)return -1;
    if(!data->reserved || !data->frequency)return 0;
    if(data->frequency!=frequency.QuadPart)return -1;
    if(!ReadProcessMemory(process,(const void *)(address+
            FIELD_OFFSET(callback_cost_data,reserved)),&after,sizeof(after),&bytes) ||
            bytes!=sizeof(after))return -1;
    if(after!=data->reserved)return 0;
    for(i=0;i<(ULONG)data->reserved;++i){
        const callback_cost_row *row=&data->rows[i];
        if(!row->complete)return 0;
        if(row->complete!=1)return -1;
        if(!row->window || !row->thread || row->entered<=0 ||
                row->returned<row->entered ||
                (row->message!=WM_RBUTTONDOWN && row->message!=WM_RBUTTONUP &&
                    row->message!=WM_LBUTTONDOWN && row->message!=WM_LBUTTONUP))return -1;
        if(row->normal && row->accepted && (row->phase_count!=1 ||
                row->phase_entered<row->entered || row->phase_returned<row->phase_entered ||
                row->phase_returned>row->returned))return -1;
    }
    return 1;
}
static BOOL callback_cost_dump(DWORD pid,HWND window,DWORD rva)
{
    HANDLE process;
    ULONG_PTR address;
    callback_cost_data data;
    ULONGLONG deadline=GetTickCount64()+5000;
    int status;
    ULONG i,count;
    if(!callback_cost_open(pid,"WOW32.DLL",rva,&process,&address))return FALSE;
    do{
        status=callback_cost_read(process,address,&data);count=0;
        if(status<0)break;
        if(status==1){
            for(i=0;i<(ULONG)data.reserved;++i)
                if(data.rows[i].window==(DWORD)(ULONG_PTR)window)++count;
            if(count>=30)break;
        }
        Sleep(1); /* Readiness only, after all measured input intervals. */
    }while(GetTickCount64()<deadline);
    CloseHandle(process);
    if(status!=1 || count!=30)return FALSE;
    for(i=0;i<(ULONG)data.reserved;++i){
        const callback_cost_row *row=&data.rows[i];
        if(row->window!=(DWORD)(ULONG_PTR)window)continue;
        if(!row->normal || !row->accepted)return FALSE;
        printf("callback-cost slot=%lu window=%08lx message=%lu thread=%lu entered=%lld returned=%lld accepted=%ld normal=%ld result=%ld error=%lu frequency=%lld phase-entered=%lld phase-returned=%lld phase-count=%ld\n",
            i,row->window,row->message,row->thread,row->entered,row->returned,
            row->accepted,row->normal,row->result,row->error,data.frequency,
            row->phase_entered,row->phase_returned,row->phase_count);
    }
    return TRUE;
}
