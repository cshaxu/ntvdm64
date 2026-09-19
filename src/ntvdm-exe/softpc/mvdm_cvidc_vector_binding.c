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

/* The selected original sources retain the C-VID access shims but omit their
 * generated CCPU timing provider. qevnt.c owns the original restart state;
 * retaining that owner prevents the vector binder from becoming a second
 * quick-event state manager. */
extern ULONG qevJumpRestart;

IUH mvdm_cvidc_get_jump_calibration(void)
{
    /* The recovered CCPU fallback has no instruction-jump calibration
     * counter.  qevnt.c treats zero as the original no-recalibration path;
     * do not conflate that with its independently configurable restart. */
    return 0;
}

IUH mvdm_cvidc_get_jump_restart(void)
{
    return (IUH)qevJumpRestart;
}

void mvdm_cvidc_set_jump_restart(IUH value)
{
    qevJumpRestart = (ULONG)value;
}

static struct CpuPrivateVector CvidCpuPrivate;

/* Original host/src/nt_cpu.c::setSTATUS (A3CPU): restore host-supplied FLAGS
 * without guest POPF privilege filtering. That translation unit's A3CPU
 * runtime is not the selected CCPU40 backend; bind its finite flag operation
 * to the existing original CCPU setters, not a new instruction provider. */
static void mvdm_cvidc_set_status(IU16 flags)
{
    setNT((flags >> 14) & 1);
    setIOPL((flags >> 12) & 3);
    setOF((flags >> 11) & 1);
    setDF((flags >> 10) & 1);
    setIF((flags >> 9) & 1);
    setTF((flags >> 8) & 1);
    setSF((flags >> 7) & 1);
    setZF((flags >> 6) & 1);
    setAF((flags >> 4) & 1);
    setPF((flags >> 2) & 1);
    setCF(flags & 1);
}

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
