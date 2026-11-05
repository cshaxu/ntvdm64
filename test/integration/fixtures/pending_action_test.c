#include "pending_action.h"
int main(void) {
  runtime_pending_action a;
  runtime_pending_action_clear(&a);
  a.action_id=1; a.kind=RUNTIME_PENDING_ACTION_READ; a.range_count=1;
  a.ranges[0].address=0x100; a.ranges[0].length=4; a.total_bytes=4;
  if(!runtime_pending_action_valid(&a,0x100000)) return 1;
  a.total_bytes=3;
  if(runtime_pending_action_valid(&a,0x100000)) return 2;
  a.total_bytes=4; a.action_id=0;
  return runtime_pending_action_valid(&a,0x100000) ? 3 : 0;
}
