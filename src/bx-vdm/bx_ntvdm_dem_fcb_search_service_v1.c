#include "bx_ntvdm_dem_fcb_search_service_v1.h"

#define APERTURE UINT64_C(0x100000)
#define SRCHBUF_BYTES 53u
#define PATH_BYTES 128u

static uint16_t word(const uint8_t *p) { return (uint16_t)p[0] | ((uint16_t)p[1] << 8); }
static int address(uint16_t seg, uint16_t off, uint64_t bytes, uint64_t *out)
{ uint64_t value=(uint64_t)seg*16u+off; if(!out||!bytes||value>APERTURE-bytes)return 0;*out=value;return 1; }
static int match(uint8_t service,const bx_ntvdm_exception_event_v1 *e,const bx_ntvdm_cpu_state_v1 *c,const bx_ntvdm_instruction_window_v1 *w)
{ return e&&c&&w&&bx_ntvdm_exception_event_v1_valid(e)&&bx_ntvdm_cpu_state_v1_valid(c)&&bx_ntvdm_instruction_window_v1_valid(w)&&e->vector==6u&&c->execution_mode==BX_NTVDM_CPU_EXECUTION_REAL&&e->fault_rip<=UINT64_MAX-4u&&w->valid_bytes>=4u&&w->bytes[0]==0xc4u&&w->bytes[1]==0xc4u&&w->bytes[2]==0x50u&&w->bytes[3]==service; }

int bx_ntvdm_dem_fcb_search_service_v1_prepare_first(const bx_ntvdm_dem_dta_registration_v1 *r,const bx_ntvdm_exception_event_v1 *e,const bx_ntvdm_cpu_state_v1 *c,const bx_ntvdm_instruction_window_v1 *w,bx_ntvdm_guest_gather_read_action_v1 *a)
{ bx_ntvdm_guest_range ranges[3];if(!r||!a||!match(0x0au,e,c,w)||!address(c->ds,(uint16_t)c->esi,SRCHBUF_BYTES,&ranges[0].address)||!address(c->es,(uint16_t)c->edi,PATH_BYTES,&ranges[1].address))return 0;ranges[0].length=SRCHBUF_BYTES;ranges[1].length=PATH_BYTES;ranges[2].address=r->current_pdb;ranges[2].length=2u;return bx_ntvdm_guest_gather_read_action_v1_need_read_resume(a,ranges,3u,e->fault_rip+4u); }
int bx_ntvdm_dem_fcb_search_service_v1_prepare_next(const bx_ntvdm_dem_dta_registration_v1 *r,const bx_ntvdm_exception_event_v1 *e,const bx_ntvdm_cpu_state_v1 *c,const bx_ntvdm_instruction_window_v1 *w,bx_ntvdm_guest_gather_read_action_v1 *a)
{ bx_ntvdm_guest_range ranges[2];if(!r||!a||!match(0x0cu,e,c,w)||!address(c->ds,(uint16_t)c->esi,SRCHBUF_BYTES,&ranges[0].address))return 0;ranges[0].length=SRCHBUF_BYTES;ranges[1].address=r->current_pdb;ranges[1].length=2u;return bx_ntvdm_guest_gather_read_action_v1_need_read_resume(a,ranges,2u,e->fault_rip+4u); }
int bx_ntvdm_dem_fcb_search_service_v1_complete_first(bx_ntvdm_search_transaction_v1 *s,const bx_ntvdm_profile_search_snapshot_v1 *n,const bx_ntvdm_exception_event_v1 *e,const bx_ntvdm_cpu_state_v1 *c,const bx_ntvdm_guest_gather_read_action_v1 *a,const uint8_t *b,uint64_t count,bx_ntvdm_multi_write_transaction_v1 *t,uint8_t p[51],uint32_t *out)
{if(out)*out=0;if(!s||!bx_ntvdm_profile_search_snapshot_v1_valid(n)||!e||!c||!a||!b||!t||!p||!out||count!=183u||a->range_count!=3u||a->ranges[0].length!=SRCHBUF_BYTES||a->ranges[1].length!=PATH_BYTES||a->ranges[2].length!=2u)return 0;return bx_ntvdm_search_transaction_v1_fcb_first_entries(s,n->entries,n->entry_count,(uint8_t)n->drive_index,e,c,word(b+181u),a->ranges[0].address,b+SRCHBUF_BYTES,(uint8_t)c->eax,(uint8_t)c->edx,t,p,out)>=0;}
int bx_ntvdm_dem_fcb_search_service_v1_complete_next(bx_ntvdm_search_transaction_v1 *s,const bx_ntvdm_exception_event_v1 *e,const bx_ntvdm_cpu_state_v1 *c,const bx_ntvdm_guest_gather_read_action_v1 *a,const uint8_t *b,uint64_t count,bx_ntvdm_multi_write_transaction_v1 *t,uint8_t p[51],uint32_t *out)
{if(out)*out=0;if(!s||!e||!c||!a||!b||!t||!p||!out||count!=55u||a->range_count!=2u||a->ranges[0].length!=SRCHBUF_BYTES||a->ranges[1].length!=2u)return 0;return bx_ntvdm_search_transaction_v1_fcb_next(s,e,c,word(b+53u),a->ranges[0].address,b,t,p,out)>=0;}
