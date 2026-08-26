/* DIVERGENCE(BOP-DIV-081): `nt_bop.c` is the original-name linkage boundary
 * for the reached MS_bop_E/UMBNotify fragment.
 * DIVERGENCE(BOP-DIV-110): it also retains the separately reached MS_bop_F
 * keyboard-vector fragment.  Both bodies are private because the full NT4
 * product dispatcher exceeds the mirror threshold. */
#include "opennt-bop-overlay/softpc/opennt_config_complete_composition.c"
#include "opennt-bop-overlay/softpc/nt_bop_config_done.c"
/* DIVERGENCE(BOP-DIV-110): `MS_bop_E` uses the DEM source facade's macro spellings.  MS_bop_F is a
 * separate original fragment with its own same-named SoftPC facade, so end
 * the first private include's macro scope at this mirror boundary. */
#undef getAX
#undef getSI
#undef getDS
#undef getCS
#undef setCF
#include "opennt-bop-overlay/softpc/nt_bop_keyboard.c"
