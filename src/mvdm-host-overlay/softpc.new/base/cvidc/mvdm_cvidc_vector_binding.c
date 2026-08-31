/*
 * DIVERGENCE(MVDM-HOST-DIV-156): the retained CCPU40 manifest contains the
 * original CCPU carrier and C-VID generated table but not their historical
 * product-generated binding unit.  Preserve both original global names and
 * tables; bind them once at C-VID's existing setup_vga_globals assembly point.
 */
#include <insignia.h>
#include <host_def.h>
#include "../../../../mvdm-host/softpc.new/base/inc/cpu4.h"
#include "../../../../mvdm-host/softpc.new/base/cvidc/evidgen.h"
#include "mvdm_cvidc_vector_binding.h"

extern struct VideoVector C_Video;

void mvdm_cvidc_bind_vectors(void)
{
    Video = C_Video;
    Cpu.Video = (IHP)&Video;
}
