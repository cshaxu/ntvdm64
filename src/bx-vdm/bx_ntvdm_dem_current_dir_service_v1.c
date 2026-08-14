#include "bx_ntvdm_dem_current_dir_service_v1.h"

#define APERTURE UINT64_C(0x100000)
#define CDS_BYTES 71u
#define CDS_END_OFFSET 69u
#define ROOT_BYTES 4u
#define ERROR_INVALID_FUNCTION 1u
#define ERROR_ACCESS_DENIED 5u

static int match(const bx_ntvdm_exception_event_v1 *e,
    const bx_ntvdm_cpu_state_v1 *c, const bx_ntvdm_instruction_window_v1 *w)
{ return e&&c&&w&&bx_ntvdm_exception_event_v1_valid(e)&&
  bx_ntvdm_cpu_state_v1_valid(c)&&bx_ntvdm_instruction_window_v1_valid(w)&&
  e->vector==6u&&c->execution_mode==BX_NTVDM_CPU_EXECUTION_REAL&&
  e->fault_rip<=UINT64_MAX-4u&&w->valid_bytes>=4u&&w->bytes[0]==0xc4u&&
  w->bytes[1]==0xc4u&&w->bytes[2]==0x50u&&
  (w->bytes[3]==0x13u||w->bytes[3]==0x18u||w->bytes[3]==0x1au); }
static int physical(uint16_t seg,uint16_t off,uint64_t len,uint64_t *out)
{ uint64_t v=(uint64_t)seg*16u+off;if(!out||v>APERTURE-len)return 0;*out=v;return 1; }
static int admitted(const bx_ntvdm_host_drive_snapshot_v1 *d,uint8_t drive)
{ return d&&bx_ntvdm_host_drive_snapshot_v1_valid(d)&&drive<26u&&
  (d->admitted_mask&(UINT32_C(1)<<drive))!=0u; }
static int root(const uint8_t *b,uint8_t drive)
{ return b&&b[0]==(uint8_t)('A'+drive)&&b[1]==':'&&b[2]=='\\'&&b[3]==0u; }
static int result(bx_ntvdm_multi_write_transaction_v1 *t,
 const bx_ntvdm_exception_event_v1 *e,const bx_ntvdm_cpu_state_v1 *c,
 uint16_t ax,int cf)
{ bx_ntvdm_multi_write_transaction_v1_initialize(t,e,c);return
  bx_ntvdm_cpu_result_v2_resume(&t->result,e->fault_rip+4u)&&
  bx_ntvdm_cpu_delta_v1_set_gpr16(&t->result.cpu_delta,0u,ax)&&
  bx_ntvdm_cpu_result_v2_set_cf(&t->result,cf); }

int bx_ntvdm_dem_current_dir_service_v1_prepare(const bx_ntvdm_host_drive_snapshot_v1 *d,
 const bx_ntvdm_exception_event_v1 *e,const bx_ntvdm_cpu_state_v1 *c,
 const bx_ntvdm_instruction_window_v1 *w,bx_ntvdm_guest_gather_read_action_v1 *a)
{ bx_ntvdm_guest_range r;uint8_t service,drive;uint16_t off;
 if(!a||!match(e,c,w))return 0;service=w->bytes[3];
 drive=service==0x13u?(uint8_t)c->eax:service==0x1au?(uint8_t)c->edx:0u;
 if((service==0x13u||service==0x1au)&&!admitted(d,drive))return 0;
 off=service==0x18u?(uint16_t)c->edx:(uint16_t)c->esi;
 if(!physical(c->ds,off,service==0x13u?CDS_BYTES:ROOT_BYTES,&r.address))return 0;
 r.length=service==0x13u?CDS_BYTES:ROOT_BYTES;
 return bx_ntvdm_guest_gather_read_action_v1_need_read_resume(a,&r,1u,e->fault_rip+4u); }

int bx_ntvdm_dem_current_dir_service_v1_complete(const bx_ntvdm_host_drive_snapshot_v1 *d,
 uint8_t service,const bx_ntvdm_exception_event_v1 *e,const bx_ntvdm_cpu_state_v1 *c,
 const bx_ntvdm_guest_gather_read_action_v1 *a,const uint8_t *b,uint64_t n,
 bx_ntvdm_multi_write_transaction_v1 *t,uint8_t p[BX_NTVDM_MULTI_WRITE_MAX_PAYLOAD])
{ uint8_t drive;uint16_t off;uint64_t address,len;
 if(!d||!e||!c||!a||!b||!t||!p||!bx_ntvdm_exception_event_v1_valid(e)||
  !bx_ntvdm_cpu_state_v1_valid(c)||!bx_ntvdm_guest_gather_read_action_v1_valid(a)||
  (service!=0x13u&&service!=0x18u&&service!=0x1au))return 0;
 drive=service==0x13u?(uint8_t)c->eax:service==0x1au?(uint8_t)c->edx:0u;
 off=service==0x18u?(uint16_t)c->edx:(uint16_t)c->esi;
 len=service==0x13u?CDS_BYTES:ROOT_BYTES;
 if(!physical(c->ds,off,len,&address)||a->range_count!=1u||a->total_bytes!=len||n!=len||
  a->ranges[0].address!=address||a->ranges[0].length!=len)return 0;
 if(service==0x1au&&!root(b,drive))return result(t,e,c,ERROR_INVALID_FUNCTION,1);
 if(service==0x18u){drive=(uint8_t)(b[0]-'A');if(b[0]<'A'||b[0]>'Z'||!root(b,drive))return result(t,e,c,ERROR_ACCESS_DENIED,1);}
 if(!admitted(d,drive))return result(t,e,c,ERROR_ACCESS_DENIED,1);
 if(service!=0x13u)return result(t,e,c,0u,0);
 bx_ntvdm_multi_write_transaction_v1_initialize(t,e,c);
 p[0]=(uint8_t)('A'+drive);p[1]=':';p[2]='\\';p[3]=0u;p[4]=2u;p[5]=0u;
 return bx_ntvdm_multi_write_v1_add(&t->writes,address,ROOT_BYTES,0u)&&
  bx_ntvdm_multi_write_v1_add(&t->writes,address+CDS_END_OFFSET,2u,4u)&&
  bx_ntvdm_cpu_result_v2_resume(&t->result,e->fault_rip+4u)&&
  bx_ntvdm_cpu_result_v2_set_cf(&t->result,0)&&
  bx_ntvdm_multi_write_transaction_v1_preflight(t,APERTURE,6u); }
