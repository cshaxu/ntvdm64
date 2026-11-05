#include <stdio.h>
#include "multi_write_abi.h"
int main(void) {
 runtime_multi_write v; runtime_multi_write_initialize(&v);
 if(!runtime_multi_write_add(&v,0x100,2,0)||!runtime_multi_write_add(&v,0x200,4,2))return 1;
 if(v.payload_bytes!=6||!runtime_multi_write_preflight(&v,0x100000,6))return 2;
 v.writes[1].guest_physical_address=0x101;if(runtime_multi_write_preflight(&v,0x100000,6))return 3;
 v.writes[1].guest_physical_address=0x200;v.writes[1].payload_offset=1;if(runtime_multi_write_preflight(&v,0x100000,6))return 4;
 puts("runtime multi-write v1: bounds and overlap contract verified");return 0;
}
