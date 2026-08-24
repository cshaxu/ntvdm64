#include "bx_ntvdm_pending_action_v1.h"
int main(void) {
  bx_ntvdm_pending_action_v1 a;
  bx_ntvdm_pending_action_v1_clear(&a);
  a.action_id=1; a.kind=BX_NTVDM_PENDING_ACTION_V1_READ; a.range_count=1;
  a.ranges[0].address=0x100; a.ranges[0].length=4; a.total_bytes=4;
  if(!bx_ntvdm_pending_action_v1_valid(&a,0x100000)) return 1;
  a.total_bytes=3;
  if(bx_ntvdm_pending_action_v1_valid(&a,0x100000)) return 2;
  a.total_bytes=4; a.action_id=0;
  return bx_ntvdm_pending_action_v1_valid(&a,0x100000) ? 3 : 0;
}
