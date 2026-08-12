#include "bx_ntvdm_dem_ioctl_changeable_service.h"
#include <stdio.h>
int main(void) {
    uint8_t types[26] = {0}; bx_ntvdm_exception_event_v1 e = {BX_NTVDM_EXCEPTION_ABI_MAGIC,BX_NTVDM_EXCEPTION_ABI_VERSION,sizeof(e),BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION,0,6,0,0,0x443cu}; bx_ntvdm_cpu_state_v1 c; bx_ntvdm_instruction_window_v1 w; bx_ntvdm_cpu_result_v2 r;
    bx_ntvdm_cpu_state_v1_initialize(&c, BX_NTVDM_CPU_EXECUTION_REAL); c.eax=0x4408u; c.ebx=2u; types[2]=3u; bx_ntvdm_instruction_window_v1_capture(&w,(const uint8_t[]){0xc4,0xc4,0x50,0x21},4u);
    if (!bx_ntvdm_dem_ioctl_changeable_service_v1_dispatch(types,&e,&c,&w,&r) || r.resume_rip != 0x4440u || r.cpu_delta.gpr16_values[0] != 1u || r.eflags_values != 0u) return 1;
    types[2]=2u; if (!bx_ntvdm_dem_ioctl_changeable_service_v1_dispatch(types,&e,&c,&w,&r) || r.cpu_delta.gpr16_values[0] != 0u) return 2;
    c.eax=0x4409u; if (bx_ntvdm_dem_ioctl_changeable_service_v1_dispatch(types,&e,&c,&w,&r)) return 3;
    puts("bx-ntvdm DEM IOCTL changeable service: fixed/removable/pass-through contracts verified"); return 0;
}
