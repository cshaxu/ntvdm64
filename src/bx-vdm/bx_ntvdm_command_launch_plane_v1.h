#ifndef BX_NTVDM_COMMAND_LAUNCH_PLANE_V1_H
#define BX_NTVDM_COMMAND_LAUNCH_PLANE_V1_H
#include "bx_ntvdm_cmd_set_info_service.h"
#include "bx_ntvdm_command_plane_v1.h"
typedef struct bx_ntvdm_command_launch_plane_v1 { uint32_t valid; bx_ntvdm_cmd_set_info_registration_v1 registration; } bx_ntvdm_command_launch_plane_v1;
void bx_ntvdm_command_launch_plane_v1_clear(bx_ntvdm_command_launch_plane_v1 *state);
int bx_ntvdm_command_launch_plane_v1_dispatch(bx_ntvdm_command_launch_plane_v1 *state,
 const bx_ntvdm_bop_ingress_v1 *ingress,const bx_ntvdm_bop_provider_selection_v1 *selection,
 const bx_ntvdm_exception_event_v1 *event,const bx_ntvdm_cpu_state_v1 *cpu,const bx_ntvdm_instruction_window_v1 *window,bx_ntvdm_cpu_result_v2 *result);
#endif
