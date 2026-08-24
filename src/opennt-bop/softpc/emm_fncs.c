/*
 * Direct source mirror: src/opennt/base/mvdm/softpc.new/base/bios/emm_fncs.c
 * (`emm_init` only).
 *
 * DIVERGENCE (T245 S3): the historical translation unit draws EMS allocation
 * and PIF/configuration state from the NT4 SoftPC product shell.  That shell
 * cannot be independently composed and no EMS hardware is admitted here.
 * The shim therefore supplies its original no-pages condition.  The retained
 * body keeps the original DS:DX publication, `get_total_pages`, `setBX`, and
 * conditional DMA ordering; it returns the source-defined `BX=0` outcome.
 */
#include "../../adapter-softpc/softpc_emm_unavailable_shim.h"

word segment68;
word offset68;

void emm_init(void)
{
	short	total_pages;

	segment68 = getDS();
	offset68 = getDX();

	total_pages = get_total_pages();
	setBX(total_pages);

	/* Let the rest of SoftPC know that Expanded Memory is present
	 * and active.
	 */
	if( total_pages )
	{
		dma_lim_setup();
	}
	return;
}
