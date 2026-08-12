#include "bx_ntvdm_dem_path_search_service_v1.h"

#include <string.h>

static void put_word(uint8_t *p, uint16_t v)
{ p[0]=(uint8_t)v; p[1]=(uint8_t)(v>>8); }
static void event(bx_ntvdm_exception_event_v1 *e, uint64_t rip)
{ *e=(bx_ntvdm_exception_event_v1){BX_NTVDM_EXCEPTION_ABI_MAGIC,BX_NTVDM_EXCEPTION_ABI_VERSION,sizeof(*e),BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION,0u,6u,0u,0u,rip}; }
static void window(bx_ntvdm_instruction_window_v1 *w, uint8_t service)
{ bx_ntvdm_instruction_window_v1_capture(w, (const uint8_t[]){0xc4u,0xc4u,0x50u,service},4u); }
/* The path-first-entries unit never calls the ambient namespace path.  These
 * link stubs ensure this test does not import the excluded host fixture. */
int bx_ntvdm_host_namespace_v1_valid(const bx_ntvdm_host_namespace_v1 *space)
{ (void)space; return 0; }
int bx_ntvdm_host_namespace_v1_enumerate(const bx_ntvdm_host_namespace_v1 *space,
    uint8_t drive, const wchar_t *directory, bx_ntvdm_host_namespace_entry_v1 *entries,
    uint32_t capacity, uint32_t *count)
{ (void)space; (void)drive; (void)directory; (void)entries; (void)capacity; (void)count; return BX_NTVDM_HOST_NAMESPACE_V1_REJECTED; }
int main(void)
{
    bx_ntvdm_dem_dta_registration_v1 registration={0x900u,0x800u,0u,0u};
    bx_ntvdm_profile_search_snapshot_v1 snapshot={BX_NTVDM_PROFILE_SEARCH_SNAPSHOT_V1_MAGIC,BX_NTVDM_PROFILE_SEARCH_SNAPSHOT_V1_VERSION,2u,4u,{0}};
    bx_ntvdm_search_transaction_v1 search;
    bx_ntvdm_exception_event_v1 e;
    bx_ntvdm_instruction_window_v1 w;
    bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_guest_gather_read_action_v1 action;
    bx_ntvdm_multi_write_transaction_v1 transaction;
    uint8_t input[134]={0}, payload[51]; uint32_t payload_bytes=0u;
    wcscpy(snapshot.entries[0].dos_name,L"COMMAND.COM"); snapshot.entries[0].attributes=0x20u;
    wcscpy(snapshot.entries[1].dos_name,L"CONFIG.SYS");
    wcscpy(snapshot.entries[2].dos_name,L"AUTOEXEC.BAT");
    wcscpy(snapshot.entries[3].dos_name,L"TARGET.COM");
    bx_ntvdm_cpu_state_v1_initialize(&cpu,BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.ds=0x100u; cpu.edx=0x20u; cpu.ecx=0u;
    event(&e,0x1000u); window(&w,0x09u);
    if (!bx_ntvdm_dem_path_search_v1_prepare_first(&registration,&e,&cpu,&w,&action) ||
        action.range_count!=3u || action.total_bytes!=134u ||
        action.ranges[0].address!=0x1020u || action.ranges[1].address!=0x900u ||
        action.ranges[2].address!=0x800u) return 1;
    memcpy(input,"C:\\COMMAND.COM",15u); put_word(input+128u,0u); put_word(input+130u,0x90u); put_word(input+132u,0x1234u);
    bx_ntvdm_search_transaction_v1_initialize(&search);
    if (!bx_ntvdm_dem_path_search_v1_complete_first(&search,&snapshot,&e,&cpu,&action,
        input,sizeof(input),&transaction,payload,&payload_bytes) || payload_bytes!=30u ||
        memcmp(payload+17u,"COMMAND.COM",11u)!=0 ||
        transaction.result.eflags_values!=0u) return 2;
    event(&e,0x2000u); window(&w,0x0bu);
    if (!bx_ntvdm_dem_path_search_v1_prepare_next(&registration,&e,&cpu,&w,&action) ||
        action.range_count!=2u || action.total_bytes!=45u || action.ranges[0].address!=0x900u)
        return 3;
    memset(input,0,45u); put_word(input+43u,0x1234u);
    if (!bx_ntvdm_dem_path_search_v1_complete_next(&search,&e,&cpu,&action,input,45u,
        &transaction,payload,&payload_bytes) || payload_bytes!=8u ||
        transaction.result.cpu_delta.gpr16_values[0]!=0x12u) return 4;
    window(&w,0x0au);
    if (bx_ntvdm_dem_path_search_v1_prepare_first(&registration,&e,&cpu,&w,&action)) return 5;
    bx_ntvdm_search_transaction_v1_release(&search);
    return 0;
}
