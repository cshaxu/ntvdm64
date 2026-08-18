#include "bx_ntvdm_command_plane_v1.h"
int main(void) {
    bx_ntvdm_instruction_window_v1 w; bx_ntvdm_bop_ingress_v1 i;
    bx_ntvdm_bop_provider_selection_v1 s; bx_ntvdm_command_plane_record_v1 r; uint32_t n; int bad=0;
    for(n=0;n<17;n++) { uint8_t b[4]={0xc4,0xc4,0x54,(uint8_t)n};
        bx_ntvdm_instruction_window_v1_capture(&w,b,4); bad|=!bx_ntvdm_bop_ingress_v1_classify(&w,&i);
        bad|=!bx_ntvdm_bop_provider_registry_v1_select(&i,&s); bad|=!bx_ntvdm_command_plane_v1_classify(&i,&s,&r);
        bad|=r.service!=(uint8_t)n || r.component==BX_NTVDM_COMMAND_COMPONENT_NONE;
        bad|=r.disposition!=(uint32_t)((n==3)?BX_NTVDM_COMMAND_PLANE_ORIGINAL_NOOP:((n==6||n==8||n==10)?BX_NTVDM_COMMAND_PLANE_EXPLICIT_UNAVAILABLE:BX_NTVDM_COMMAND_PLANE_DEFERRED)); }
    { uint8_t b[4]={0xc4,0xc4,0x54,17}; bx_ntvdm_instruction_window_v1_capture(&w,b,4);
      bad|=!bx_ntvdm_bop_ingress_v1_classify(&w,&i); bad|=!bx_ntvdm_bop_provider_registry_v1_select(&i,&s);
      bad|=bx_ntvdm_command_plane_v1_classify(&i,&s,&r); } return bad?1:0;
}
