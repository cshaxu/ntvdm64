/* DIVERGENCE(BOP-DIV-080): this same-signature mirror boundary retains only
 * the original cassette_io entry.  The admitted AH=88h source subset is
 * private overlay code because it exceeds the mirror threshold. */
#define cassette_io opennt_bop_overlay_cassette_io
#include "opennt-bop-overlay/softpc/tape_io.c"
#undef cassette_io

void cassette_io(void)
{
    opennt_bop_overlay_cassette_io();
}
