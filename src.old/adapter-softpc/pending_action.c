#include "pending_action.h"
#include <string.h>

void runtime_pending_action_clear(runtime_pending_action *a)
{
    if (!a) return;
    memset(a, 0, sizeof(*a)); a->magic=RUNTIME_PENDING_ACTION_MAGIC;
    a->abi_version=RUNTIME_PENDING_ACTION_VERSION; a->struct_bytes=sizeof(*a);
    runtime_cpu_result_pass_through(&a->result); runtime_multi_write_initialize(&a->writes);
}
int runtime_pending_action_valid(const runtime_pending_action *a, uint64_t aperture)
{
    uint32_t i; uint64_t total=0;
    if(!a || a->magic!=RUNTIME_PENDING_ACTION_MAGIC || a->abi_version!=1u ||
       a->struct_bytes!=sizeof(*a) || a->action_id==0u || !runtime_cpu_result_valid(&a->result)) return 0;
    if(a->kind==RUNTIME_PENDING_ACTION_READ || a->kind==RUNTIME_PENDING_ACTION_GATHER_READ) {
      if(a->range_count==0u || a->range_count>RUNTIME_GUEST_GATHER_READ_ACTION_MAX_RANGES) return 0;
      for(i=0;i<a->range_count;i++){if(!runtime_guest_range_within(aperture,&a->ranges[i]) || total>UINT64_MAX-a->ranges[i].length)return 0; total+=a->ranges[i].length;}
      return total==a->total_bytes;
    }
    if(a->kind==RUNTIME_PENDING_ACTION_MULTI_WRITE) return a->range_count==0u && a->total_bytes==a->writes.payload_bytes && runtime_multi_write_preflight(&a->writes,aperture,a->total_bytes);
    if(a->kind==RUNTIME_PENDING_ACTION_BULK_WRITE) return a->range_count==1u && a->total_bytes==a->ranges[0].length && runtime_guest_range_within(aperture,&a->ranges[0]);
    return 0;
}
