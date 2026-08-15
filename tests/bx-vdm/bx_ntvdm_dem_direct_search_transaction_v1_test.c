#include "bx_ntvdm_search_transaction_v1.h"

#include <string.h>
#include <wctype.h>
#include <stdio.h>

static void put_u32(uint8_t *out, uint32_t value)
{ out[0]=(uint8_t)value; out[1]=(uint8_t)(value>>8); out[2]=(uint8_t)(value>>16); out[3]=(uint8_t)(value>>24); }

int main(void)
{
    wchar_t temporary[MAX_PATH], path[MAX_PATH], short_path[MAX_PATH], alpha[MAX_PATH], zeta[MAX_PATH];
    char oem[128] = {0}; uint8_t drive, request[128] = {0}, dta[43] = {0}, fcb_path[128] = {0}, payload[51];
    bx_ntvdm_host_drive_snapshot_v1 snapshot = {0}; bx_ntvdm_host_namespace_v1 space;
    bx_ntvdm_search_transaction_v1 search; bx_ntvdm_profile_search_snapshot_v1 declared = {0}; bx_ntvdm_search_query_v1 query; static bx_ntvdm_host_namespace_entry_v1 entries[BX_NTVDM_HOST_NAMESPACE_V1_MAX_ENTRIES]; bx_ntvdm_exception_event_v1 event = {
        BX_NTVDM_EXCEPTION_ABI_MAGIC, BX_NTVDM_EXCEPTION_ABI_VERSION, sizeof(event),
        BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION, 0u, 6u, 0u, 0u, 0x200u };
    bx_ntvdm_cpu_state_v1 cpu; bx_ntvdm_multi_write_transaction_v1 tx; uint32_t bytes = 0u;
    HANDLE file; DWORD written; int failure = 0;
    if (!GetTempPathW(MAX_PATH, temporary) || !GetTempFileNameW(temporary, L"n64", 0u, path) || !DeleteFileW(path) ||
        !CreateDirectoryW(path, 0) || !GetShortPathNameW(path, short_path, MAX_PATH) ||
        short_path[1] != L':') return 1;
    swprintf_s(alpha, MAX_PATH, L"%s\\ALPHA.TXT", short_path);
    swprintf_s(zeta, MAX_PATH, L"%s\\ZETA.TXT", short_path);
    file=CreateFileW(alpha,GENERIC_WRITE,0,0,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,0);
    if(file==INVALID_HANDLE_VALUE || !WriteFile(file,"a",1u,&written,0) || written!=1u) return 2;
    CloseHandle(file); file=CreateFileW(zeta,GENERIC_WRITE,0,0,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,0);
    if(file==INVALID_HANDLE_VALUE || !WriteFile(file,"z",1u,&written,0) || written!=1u) return 3;
    CloseHandle(file); drive=(uint8_t)(towupper((wint_t)short_path[0])-L'A');
    snapshot.magic=BX_NTVDM_HOST_DRIVE_SNAPSHOT_V1_MAGIC; snapshot.version=BX_NTVDM_HOST_DRIVE_SNAPSHOT_V1_VERSION;
    snapshot.present_mask=UINT32_C(1)<<drive; snapshot.admitted_mask=snapshot.present_mask; snapshot.types[drive]=DRIVE_FIXED;
    if(!bx_ntvdm_host_namespace_v1_initialize(&space,&snapshot) || !WideCharToMultiByte(CP_OEMCP,0,short_path,-1,oem,(int)sizeof(oem)-8,0,0)) failure=4;
    strcat_s(oem,sizeof(oem),"\\*.TXT"); memcpy(request,oem,strlen(oem)+1u); memcpy(fcb_path,oem,strlen(oem)+1u);
    bx_ntvdm_cpu_state_v1_initialize(&cpu,BX_NTVDM_CPU_EXECUTION_REAL); bx_ntvdm_search_transaction_v1_initialize(&search);
    declared.magic=BX_NTVDM_PROFILE_SEARCH_SNAPSHOT_V1_MAGIC; declared.version=BX_NTVDM_PROFILE_SEARCH_SNAPSHOT_V1_VERSION; declared.drive_index=drive; declared.entry_count=4u;
    wcscpy_s(declared.entries[0].dos_name,13u,L"COMMAND.COM"); declared.entries[0].attributes=0x20u; declared.entries[0].byte_count=7u; declared.entries[0].dos_time=7u; declared.entries[0].dos_date=8u;
    wcscpy_s(declared.entries[1].dos_name,13u,L"CONFIG.SYS"); wcscpy_s(declared.entries[2].dos_name,13u,L"AUTOEXEC.BAT"); wcscpy_s(declared.entries[3].dos_name,13u,L"TARGET.COM");
    if(!failure&&!bx_ntvdm_search_request_v1_decode_first_path(request,0u,&query))failure=41;
    if(!failure){int listed=bx_ntvdm_host_namespace_v1_enumerate(&space,query.drive_index,query.relative_directory,entries,BX_NTVDM_HOST_NAMESPACE_V1_MAX_ENTRIES,&bytes);if(listed!=BX_NTVDM_HOST_NAMESPACE_V1_OK){wprintf(L"direct-search enum=%d error=%lu drive=%u dir=%ls\n",listed,GetLastError(),query.drive_index,query.relative_directory);failure=42-listed;}}
    if(!failure){int got=bx_ntvdm_search_transaction_v1_path_first(&search,&space,&event,&cpu,0x1234u,0x900u,request,0u,&tx,payload,&bytes);if(got!=1)failure=50+(got+1);else if(bytes!=30u||memcmp(payload+17u,"ALPHA.TXT",9u)!=0)failure=5;}
    if(!failure){ put_u32(dta,(uint32_t)payload[0]|((uint32_t)payload[1]<<8)|((uint32_t)payload[2]<<16)|((uint32_t)payload[3]<<24)); put_u32(dta+4u,(uint32_t)payload[4]|((uint32_t)payload[5]<<8)|((uint32_t)payload[6]<<16)|((uint32_t)payload[7]<<24)); if(bx_ntvdm_search_transaction_v1_path_next(&search,&event,&cpu,0x1234u,0x900u,dta,&tx,payload,&bytes)!=1 || bytes!=30u || memcmp(payload+17u,"ZETA.TXT",8u)!=0) failure=6; }
    bx_ntvdm_search_transaction_v1_release(&search); bx_ntvdm_search_transaction_v1_initialize(&search);
    memset(request,0,sizeof(request)); sprintf_s((char *)request,sizeof(request),"%c:\\COMMAND.COM",(char)('A'+drive));
    if(!failure){int got=bx_ntvdm_search_transaction_v1_path_first_merged(&search,&space,&declared,&event,&cpu,0x1234u,0x900u,request,0u,&tx,payload,&bytes);if(got!=1||bytes!=30u||memcmp(payload+17u,"COMMAND.COM",11u)!=0||payload[13u]!=7u)failure=8;}
    bx_ntvdm_search_transaction_v1_release(&search); bx_ntvdm_search_transaction_v1_initialize(&search);
    if(!failure){int got=bx_ntvdm_search_transaction_v1_fcb_first_merged(&search,&space,&declared,&event,&cpu,0x1234u,0xa00u,request,0u,0u,&tx,payload,&bytes);if(got!=1||bytes!=51u||memcmp(payload,"COMMAND COM",11u)!=0||payload[15u]!=7u)failure=9;}
    if(!failure && (bx_ntvdm_search_transaction_v1_fcb_first(&search,&space,&event,&cpu,0x1234u,0xa00u,fcb_path,0u,0u,&tx,payload,&bytes)!=1 || bytes!=51u || memcmp(payload,"ALPHA   TXT",11u)!=0)) failure=7;
    bx_ntvdm_search_transaction_v1_release(&search); bx_ntvdm_host_namespace_v1_release(&space);
    DeleteFileW(alpha); DeleteFileW(zeta); RemoveDirectoryW(path); return failure;
}
