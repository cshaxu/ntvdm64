#include "bx_ntvdm_dem_path_search_service_v1.h"

#define BX_NTVDM_DEM_PATH_SEARCH_APERTURE UINT64_C(0x100000)

static uint16_t word(const uint8_t *p) { return (uint16_t)p[0] | ((uint16_t)p[1] << 8); }
static int address(uint16_t segment, uint16_t offset, uint64_t size, uint64_t *out)
{
    uint64_t value = ((uint64_t)segment << 4) + offset;
    if (!out || size == 0u || value > BX_NTVDM_DEM_PATH_SEARCH_APERTURE - size) return 0;
    *out = value; return 1;
}
static int match(const bx_ntvdm_exception_event_v1 *e, const bx_ntvdm_cpu_state_v1 *c,
    const bx_ntvdm_instruction_window_v1 *w)
{ return e && c && w && bx_ntvdm_exception_event_v1_valid(e) && bx_ntvdm_cpu_state_v1_valid(c) && bx_ntvdm_instruction_window_v1_valid(w) && e->vector == 6u && c->execution_mode == BX_NTVDM_CPU_EXECUTION_REAL && w->valid_bytes >= 4u && w->bytes[0] == 0xc4u && w->bytes[1] == 0xc4u && w->bytes[2] == 0x50u && w->bytes[3] == 0x09u && e->fault_rip <= UINT64_MAX - 4u; }
static int match_next(const bx_ntvdm_exception_event_v1 *e, const bx_ntvdm_cpu_state_v1 *c,
    const bx_ntvdm_instruction_window_v1 *w)
{ return e && c && w && bx_ntvdm_exception_event_v1_valid(e) && bx_ntvdm_cpu_state_v1_valid(c) && bx_ntvdm_instruction_window_v1_valid(w) && e->vector == 6u && c->execution_mode == BX_NTVDM_CPU_EXECUTION_REAL && w->valid_bytes >= 4u && w->bytes[0] == 0xc4u && w->bytes[1] == 0xc4u && w->bytes[2] == 0x50u && w->bytes[3] == 0x0bu && e->fault_rip <= UINT64_MAX - 4u; }
int bx_ntvdm_dem_path_search_v1_prepare_first(const bx_ntvdm_dem_dta_registration_v1 *r,
    const bx_ntvdm_exception_event_v1 *e, const bx_ntvdm_cpu_state_v1 *c,
    const bx_ntvdm_instruction_window_v1 *w, bx_ntvdm_guest_gather_read_action_v1 *a)
{
    bx_ntvdm_guest_range ranges[3];
    if (!r || !a || !match(e,c,w) || !address(c->ds,(uint16_t)c->edx,128u,&ranges[0].address)) return 0;
    ranges[0].length=128u; ranges[1].address=r->dta_location; ranges[1].length=4u;
    ranges[2].address=r->current_pdb; ranges[2].length=2u;
    return bx_ntvdm_guest_gather_read_action_v1_need_read_resume(a,ranges,3u,e->fault_rip+4u);
}
int bx_ntvdm_dem_path_search_v1_prepare_next(const bx_ntvdm_dem_dta_registration_v1 *r,
    const bx_ntvdm_exception_event_v1 *e, const bx_ntvdm_cpu_state_v1 *c,
    const bx_ntvdm_instruction_window_v1 *w, bx_ntvdm_guest_gather_read_action_v1 *a)
{
    bx_ntvdm_guest_range ranges[2];
    if (!r || !a || !match_next(e,c,w)) return 0;
    ranges[0].address=r->dta_location; ranges[0].length=43u;
    ranges[1].address=r->current_pdb; ranges[1].length=2u;
    return bx_ntvdm_guest_gather_read_action_v1_need_read_resume(a,ranges,2u,e->fault_rip+4u);
}
int bx_ntvdm_dem_path_search_v1_complete_next(bx_ntvdm_search_transaction_v1 *s,
    const bx_ntvdm_exception_event_v1 *e, const bx_ntvdm_cpu_state_v1 *c,
    const bx_ntvdm_guest_gather_read_action_v1 *a, const uint8_t *b,
    uint64_t count, bx_ntvdm_multi_write_transaction_v1 *t, uint8_t p[51], uint32_t *out)
{
    if (out) *out=0u;
    if (!s || !e || !c || !a || !b || !t || !p || !out || count != 45u ||
        a->range_count != 2u || a->ranges[0].length != 43u ||
        a->ranges[1].length != 2u) return 0;
    return bx_ntvdm_search_transaction_v1_path_next(s,e,c,word(b+43u),
        a->ranges[0].address,b,t,p,out) >= 0;
}
int bx_ntvdm_dem_path_search_v1_complete_first(bx_ntvdm_search_transaction_v1 *s,
    const bx_ntvdm_profile_search_snapshot_v1 *n, const bx_ntvdm_exception_event_v1 *e,
    const bx_ntvdm_cpu_state_v1 *c, const bx_ntvdm_guest_gather_read_action_v1 *a,
    const uint8_t *b, uint64_t count, bx_ntvdm_multi_write_transaction_v1 *t, uint8_t p[51], uint32_t *out)
{
    uint64_t dta;
    if (out) *out=0u;
    if (!s || !bx_ntvdm_profile_search_snapshot_v1_valid(n) || !e || !c || !a || !b || !t || !p || !out || count != 134u || a->range_count != 3u || a->ranges[0].length != 128u || a->ranges[1].length != 4u || a->ranges[2].length != 2u || !address(word(b+130u),word(b+128u),43u,&dta)) return 0;
    return bx_ntvdm_search_transaction_v1_path_first_entries(s,n->entries,n->entry_count,(uint8_t)n->drive_index,e,c,word(b+132u),dta,b,(uint16_t)c->ecx,t,p,out) >= 0;
}
