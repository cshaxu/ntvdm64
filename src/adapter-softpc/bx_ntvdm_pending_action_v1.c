#include "bx_ntvdm_pending_action_v1.h"
#include <string.h>

void bx_ntvdm_pending_action_v1_clear(bx_ntvdm_pending_action_v1 *a)
{
    if (!a) return;
    memset(a, 0, sizeof(*a)); a->magic=BX_NTVDM_PENDING_ACTION_V1_MAGIC;
    a->abi_version=BX_NTVDM_PENDING_ACTION_V1_VERSION; a->struct_bytes=sizeof(*a);
    bx_ntvdm_cpu_result_v2_pass_through(&a->result); bx_ntvdm_multi_write_v1_initialize(&a->writes);
}
int bx_ntvdm_pending_action_v1_valid(const bx_ntvdm_pending_action_v1 *a, uint64_t aperture)
{
    uint32_t i; uint64_t total=0;
    if(!a || a->magic!=BX_NTVDM_PENDING_ACTION_V1_MAGIC || a->abi_version!=1u ||
       a->struct_bytes!=sizeof(*a) || a->action_id==0u || !bx_ntvdm_cpu_result_v2_valid(&a->result)) return 0;
    if(a->kind==BX_NTVDM_PENDING_ACTION_V1_READ || a->kind==BX_NTVDM_PENDING_ACTION_V1_GATHER_READ) {
      if(a->range_count==0u || a->range_count>BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_MAX_RANGES) return 0;
      for(i=0;i<a->range_count;i++){if(!bx_ntvdm_guest_range_within(aperture,&a->ranges[i]) || total>UINT64_MAX-a->ranges[i].length)return 0; total+=a->ranges[i].length;}
      return total==a->total_bytes;
    }
    if(a->kind==BX_NTVDM_PENDING_ACTION_V1_MULTI_WRITE) return a->range_count==0u && a->total_bytes==a->writes.payload_bytes && bx_ntvdm_multi_write_v1_preflight(&a->writes,aperture,a->total_bytes);
    if(a->kind==BX_NTVDM_PENDING_ACTION_V1_BULK_WRITE) return a->range_count==1u && a->total_bytes==a->ranges[0].length && bx_ntvdm_guest_range_within(aperture,&a->ranges[0]);
    return 0;
}
