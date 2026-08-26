/* DIVERGENCE(BOP-DIV-080): this same-signature mirror boundary retains only
 * the original emm_init entry.  The no-EMS source subset is private overlay
 * code because it exceeds the mirror threshold. */
#define emm_init opennt_bop_overlay_emm_init
#include "opennt-bop-overlay/softpc/emm_fncs.c"
#undef emm_init

void emm_init(void)
{
    opennt_bop_overlay_emm_init();
}
