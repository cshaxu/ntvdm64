#include "bx_ntvdm_xms_dpmi_plane_v1.h"

int main(void) {
    bx_ntvdm_instruction_window_v1 w; bx_ntvdm_bop_ingress_v1 i;
    bx_ntvdm_bop_provider_selection_v1 s; bx_ntvdm_xms_dpmi_plane_record_v1 r;
    uint32_t n; int bad=0;
    for(n=0;n<12;n++) { uint8_t b[4]={0xc4,0xc4,0x52,(uint8_t)n};
        bx_ntvdm_instruction_window_v1_capture(&w,b,4); bad|=!bx_ntvdm_bop_ingress_v1_classify(&w,&i);
        bad|=!bx_ntvdm_bop_provider_registry_v1_select(&i,&s); bad|=!bx_ntvdm_xms_dpmi_plane_v1_classify(&i,&s,&r);
        bad|=r.service!=n || r.provider_family!=BX_NTVDM_BOP_PROVIDER_XMS || r.disposition!=BX_NTVDM_XMS_DPMI_DEFERRED; }
    for(n=0;n<25;n++) { uint8_t b[4]={0xc4,0xc4,0x53,(uint8_t)n};
        bx_ntvdm_instruction_window_v1_capture(&w,b,4); bad|=!bx_ntvdm_bop_ingress_v1_classify(&w,&i);
        bad|=!bx_ntvdm_bop_provider_registry_v1_select(&i,&s); bad|=!bx_ntvdm_xms_dpmi_plane_v1_classify(&i,&s,&r);
        bad|=r.service!=n || r.provider_family!=BX_NTVDM_BOP_PROVIDER_DPMI ||
            r.disposition!=((n==18)?BX_NTVDM_XMS_DPMI_EXPLICIT_UNAVAILABLE:BX_NTVDM_XMS_DPMI_DEFERRED); }
    { uint8_t b[4]={0xc4,0xc4,0x53,25}; bx_ntvdm_instruction_window_v1_capture(&w,b,4);
      bad|=!bx_ntvdm_bop_ingress_v1_classify(&w,&i); bad|=!bx_ntvdm_bop_provider_registry_v1_select(&i,&s);
      bad|=bx_ntvdm_xms_dpmi_plane_v1_classify(&i,&s,&r); }
    return bad?1:0;
}
