/* DIVERGENCE(BOP-DIV-023): this same-signature mirror boundary retains only
 * the original memory_size entry.  The source-derived one-function subset is
 * private overlay code because it exceeds the mirror threshold. */
#define memory_size opennt_bop_overlay_memory_size
#include "opennt-bop-overlay/softpc/mem_size.c"
#undef memory_size

void memory_size(void)
{
    opennt_bop_overlay_memory_size();
}
