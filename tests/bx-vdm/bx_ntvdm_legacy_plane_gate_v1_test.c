#include "bop-v1/bx_ntvdm_legacy_plane_gate_v1.h"
int main(void) { bx_ntvdm_instruction_window_v1 w; int bad=0; uint8_t d[4]={0xc4,0xc4,0x50,0x1b}, c[4]={0xc4,0xc4,0x54,0x05};
 bx_ntvdm_instruction_window_v1_capture(&w,d,4);bad|=!bx_ntvdm_legacy_plane_gate_v1_dem(&w,0x1b);bad|=bx_ntvdm_legacy_plane_gate_v1_dem(&w,0x1c);
 bx_ntvdm_instruction_window_v1_capture(&w,c,4);bad|=!bx_ntvdm_legacy_plane_gate_v1_command(&w,0x05);bad|=bx_ntvdm_legacy_plane_gate_v1_command(&w,0x04);return bad?1:0;}
