/*
 * C-VID is intentionally compiled without CCPU so its generated access
 * wrappers use the public Cpu vector.  The selected CCPU translation units
 * use their direct c_cpu_* spellings instead.  This fixture is the narrow
 * ABI boundary between those two original profiles; it must not initialize
 * a second CPU, SAS, or video state.
 */
#include <stdio.h>

#include "insignia.h"
#include "host_def.h"
#include "evidgen.h"
#define CPU_PRIVATE
#include "cpu4.h"
#include "sas.h"
#include "mvdm_cvidc_vector_binding.h"

extern struct VideoVector C_Video;
extern struct VideoVector Video;
extern struct SasVector Sas;

/* This focused link intentionally provides only storage and inert providers.
 * It is not a second SoftPC implementation: the tested object is the actual
 * binder, while all selected provider semantics remain covered by the formal
 * CCPU40 composition. */
struct CpuVector Cpu;
struct SasVector Sas;
struct VideoVector Video;
struct VideoVector C_Video;
IU32 c_cpu_calc_q_ev_inst_for_time(IU32 value) { return value; }
void c_sas_overwrite_memory(PHY_ADDR address, PHY_ADDR length)
{
    (void)address;
    (void)length;
}

int main(void)
{
    /* The pre-init negative case is meaningful only for an explicit
     * unsupported C-VID operation.  It must remain null rather than being
     * rebound to a CCPU pointer with a different ABI. */
    if (Cpu.ClearHwInt != NULL) {
        fputs("unexpected pre-init C-VID ClearHwInt provider\n", stderr);
        return 1;
    }

    mvdm_cvidc_bind_vectors();
    if (Cpu.CalcQuickEventInstTime != c_cpu_calc_q_ev_inst_for_time ||
        Cpu.CalcQuickEventInstTime(37UL) != 37UL ||
        Cpu.Sas != &Sas || Cpu.Video != (IHP)&Video ||
        Video.GetVideolatches != C_Video.GetVideolatches ||
        Sas.Sas_overwrite_memory != c_sas_overwrite_memory ||
        Cpu.Private == NULL) {
        fputs("C-VID/CCPU vector publication did not select its providers\n",
            stderr);
        return 2;
    }
    if (Cpu.GetJumpCalibrateVal() != 0 || Cpu.GetJumpInitialVal() != 100) {
        fputs("C-VID quick-event calibration/restart providers conflated\n", stderr);
        return 4;
    }
    Cpu.SetJumpInitialVal(73);
    if (Cpu.GetJumpCalibrateVal() != 0 || Cpu.GetJumpInitialVal() != 73) {
        fputs("C-VID quick-event restart state did not remain independent\n", stderr);
        return 5;
    }

    /* A second publication is the normal setup_vga_globals path after the
     * early video-only bind in main.c.  It must be idempotent. */
    mvdm_cvidc_bind_vectors();
    if (Cpu.CalcQuickEventInstTime != c_cpu_calc_q_ev_inst_for_time ||
        Cpu.ClearHwInt != NULL || Cpu.Sas != &Sas ||
        Sas.Sas_overwrite_memory != c_sas_overwrite_memory) {
        fputs("C-VID/CCPU repeated publication changed the contract\n", stderr);
        return 3;
    }
    return 0;
}
