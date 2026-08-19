#ifndef BX_NTVDM_BOP_DEMFILE_H
#define BX_NTVDM_BOP_DEMFILE_H
#include "../bx_ntvdm_dem_whole_provider_v1.h"
int bx_ntvdm_bop_dem_ch_mod_v2(bx_ntvdm_dem_whole_provider_v1 *provider,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu, uint8_t drive,
    const wchar_t *relative_path, int startup_path,
    bx_ntvdm_cpu_result_v2 *result);
#endif