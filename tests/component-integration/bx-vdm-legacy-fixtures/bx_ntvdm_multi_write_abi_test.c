#include <stdio.h>
#include "bx_ntvdm_multi_write_abi.h"
int main(void) {
 bx_ntvdm_multi_write_v1 v; bx_ntvdm_multi_write_v1_initialize(&v);
 if(!bx_ntvdm_multi_write_v1_add(&v,0x100,2,0)||!bx_ntvdm_multi_write_v1_add(&v,0x200,4,2))return 1;
 if(v.payload_bytes!=6||!bx_ntvdm_multi_write_v1_preflight(&v,0x100000,6))return 2;
 v.writes[1].guest_physical_address=0x101;if(bx_ntvdm_multi_write_v1_preflight(&v,0x100000,6))return 3;
 v.writes[1].guest_physical_address=0x200;v.writes[1].payload_offset=1;if(bx_ntvdm_multi_write_v1_preflight(&v,0x100000,6))return 4;
 puts("bx-ntvdm multi-write v1: bounds and overlap contract verified");return 0;
}
