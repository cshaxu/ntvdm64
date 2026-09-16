#include "insignia.h"
#include "host_def.h"
#include "cpu4.h"
#include "sas.h"

/* DIVERGENCE(MVDM-HOST-DIV-138): this is the exact direct SAS invalidation
 * facade from the selected original ntstubs.c.  That source also defines
 * alternate Cpu/Gdp/Video and FPU-facing state which conflicts with the
 * selected CCPU40 owners, so only this required source subset is composed. */
IMPORT void c_sas_overwrite_memory IPT2(PHY_ADDR, addr, PHY_ADDR, length);

#undef sas_overwrite_memory
void sas_overwrite_memory IFN2(PHY_ADDR, addr, PHY_ADDR, len)
{
    c_sas_overwrite_memory(addr, len);
}
