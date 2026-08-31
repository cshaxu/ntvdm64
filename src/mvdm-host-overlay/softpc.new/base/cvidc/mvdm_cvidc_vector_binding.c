/*
 * DIVERGENCE(MVDM-HOST-DIV-156): the retained CCPU40 manifest contains the
 * original CCPU carrier and C-VID generated table but not their historical
 * product-generated binding unit.  Preserve both original global names and
 * tables; bind them once at C-VID's existing setup_vga_globals assembly point.
 */
#include <insignia.h>
#include <host_def.h>
#define CCPU
#define CPU_PRIVATE
#include "cpu4.h"
#include "mvdm_cvidc_vector_binding.h"

extern struct VideoVector C_Video;
extern struct VideoVector Video;
extern struct SasVector Sas;

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
    mvdm_cvidc_bind_cpu_public();
    Video = C_Video;
    Cpu.Video = (IHP)&Video;
}
