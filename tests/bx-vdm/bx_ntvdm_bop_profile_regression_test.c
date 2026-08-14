#include "bx_ntvdm_bop_ingress_v1.h"
#include "bx_ntvdm_bop_provider_registry_v1.h"
#include "bx_ntvdm_dem_plane_v1.h"
#include "bx_ntvdm_command_plane_v1.h"
#include "bx_ntvdm_xms_dpmi_plane_v1.h"
#include "bx_ntvdm_system_plane_v1.h"

static int classify(uint8_t selector, uint32_t count) {
    bx_ntvdm_instruction_window_v1 w; bx_ntvdm_bop_ingress_v1 i;
    bx_ntvdm_bop_provider_selection_v1 s; uint32_t n; int bad=0;
    for(n=0;n<count;n++) { uint8_t b[4]={0xc4,0xc4,selector,(uint8_t)n};
        bx_ntvdm_instruction_window_v1_capture(&w,b,4); bad|=!bx_ntvdm_bop_ingress_v1_classify(&w,&i);
        bad|=!bx_ntvdm_bop_provider_registry_v1_select(&i,&s); bad|=i.service!=n;
        if(selector==0x50u){bx_ntvdm_dem_plane_record_v1 p;bad|=!bx_ntvdm_dem_plane_v1_classify(&i,&s,&p);bad|=p.service!=n;}
        if(selector==0x54u){bx_ntvdm_command_plane_record_v1 p;bad|=!bx_ntvdm_command_plane_v1_classify(&i,&s,&p);bad|=p.service!=n;}
        if(selector==0x52u||selector==0x53u){bx_ntvdm_xms_dpmi_plane_record_v1 p;bad|=!bx_ntvdm_xms_dpmi_plane_v1_classify(&i,&s,&p);bad|=p.service!=n;}
        if(selector==0x57u){bx_ntvdm_system_plane_record_v1 p;bad|=!bx_ntvdm_system_plane_v1_classify(&i,&s,&p);bad|=p.service!=n;}
    } return bad;
}
int main(void) { bx_ntvdm_instruction_window_v1 w; bx_ntvdm_bop_ingress_v1 i; int bad=0; uint32_t n;
    bad|=classify(0x50u,73); bad|=classify(0x54u,17); bad|=classify(0x52u,12); bad|=classify(0x53u,25);bad|=classify(0x57u,50);
    {uint8_t b[4]={0xc4,0xc4,0x56,0x90};bx_ntvdm_bop_provider_selection_v1 s;bx_ntvdm_system_plane_record_v1 p;bx_ntvdm_instruction_window_v1_capture(&w,b,4);bad|=!bx_ntvdm_bop_ingress_v1_classify(&w,&i);bad|=i.has_service!=0u;bad|=!bx_ntvdm_bop_provider_registry_v1_select(&i,&s);bad|=!bx_ntvdm_system_plane_v1_classify(&i,&s,&p);bad|=p.component!=BX_NTVDM_SYSTEM_COMPONENT_DEBUG_DISPATCH;}
    for(n=0;n<16;n++){uint8_t selector[]={0x50,0x51,0x52,0x53,0x54,0x56,0x57,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,0xfd,0xfe};uint8_t b[4]={0xc4,0xc4,selector[n],0};bx_ntvdm_instruction_window_v1_capture(&w,b,4);bad|=!bx_ntvdm_bop_ingress_v1_classify(&w,&i);}
    return bad?1:0;
}
