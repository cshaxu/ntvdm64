/*
 * DIVERGENCE(MVDM-HOST-DIV-156): the retained CCPU40 manifest contains the
 * original CCPU carrier and C-VID generated table but not their historical
 * product-generated binding unit.  Preserve both original global names and
 * tables; bind them once at C-VID's existing setup_vga_globals assembly point.
 */
#include <insignia.h>
#include <host_def.h>
#include "evidgen.h"
#define CCPU
#define CPU_PRIVATE
#include "cpu4.h"
#include "sas.h"
#include "mvdm_cvidc_vector_binding.h"

extern struct VideoVector C_Video;
extern struct VideoVector Video;
extern struct SasVector Sas;
IMPORT void c_sas_overwrite_memory IPT2(PHY_ADDR, addr, PHY_ADDR, length);

/* The selected original sources retain the C-VID access shims but omit this
 * generated CCPU timing provider.  Keeping the timing value beside the
 * vector binding prevents the public access shims from being rebound to
 * themselves.  qevnt.c's original initial qevJumpRestart value is 100. */
static IUH mvdm_cvidc_jump_restart = 100;

IUH mvdm_cvidc_get_jump_calibration(void)
{
    return mvdm_cvidc_jump_restart;
}

IUH mvdm_cvidc_get_jump_restart(void)
{
    return mvdm_cvidc_jump_restart;
}

void mvdm_cvidc_set_jump_restart(IUH value)
{
    mvdm_cvidc_jump_restart = value;
}

static struct CpuPrivateVector CvidCpuPrivate;

static void mvdm_cvidc_bind_cpu_private(void)
{
#include "cvidc_cpu_binding.inc"
#define MVDM_BIND_PRIVATE(field, provider) CvidCpuPrivate.field = provider;
    MVDM_CVIDC_CPU_PRIVATE_SLOTS(MVDM_BIND_PRIVATE)
#undef MVDM_BIND_PRIVATE
}

static void mvdm_cvidc_bind_cpu_public(void)
{
#include "cvidc_cpu_binding.inc"
#define MVDM_BIND_PUBLIC(field, provider) Cpu.field = provider;
    MVDM_CVIDC_CPU_PUBLIC_SLOTS(MVDM_BIND_PUBLIC)
#undef MVDM_BIND_PUBLIC
}

void mvdm_cvidc_bind_vectors(void)
{
    mvdm_cvidc_bind_cpu_private();
    Cpu.Private = &CvidCpuPrivate;
    Cpu.Sas = &Sas;
    /* DIVERGENCE(MVDM-HOST-DIV-189): the generated source carrier leaves
     * this reached slot null although the selected CPU40 source supplies the
     * exact same-shaped no-cache-invalidation provider. */
    Sas.Sas_overwrite_memory = c_sas_overwrite_memory;
    mvdm_cvidc_bind_cpu_public();
    mvdm_cvidc_bind_video_vector();
    Cpu.Video = (IHP)&Video;
}

void mvdm_cvidc_bind_video_vector(void)
{
    Video = C_Video;
}
