/*
 * Direct source fragment from:
 *   base/mvdm/softpc.new/base/keymouse/keybd_io.c:UpdateKbdInt15
 *
 * DIVERGENCE (T241 S2): the full translation unit owns PPI, keyboard, timer,
 * mouse, video and host_simulate product state.  The two historical globals
 * are therefore staged by the named selector-blind IVT-watch shim around the
 * unchanged comparison/clear/update body below.
 */
#include "../../adapter-softpc/softpc_int15_watch_shim.h"

static word sp_int15_handler_off;
static word sp_int15_handler_seg;

int runtime_softpc_int15_watch_source_begin(void)
{
    return runtime_softpc_int15_watch_state_load(&sp_int15_handler_off,
        &sp_int15_handler_seg);
}

/* The source body is intentionally kept in its original order. */
void UpdateKbdInt15(word Seg,word Off)
{
    word int15Off, int15Seg;

    /* make sure nobody has hooked since the last time */
    /* we stored the i15 vector */
    sas_loadw(0x15*4 ,    &int15Off);
    sas_loadw(0x15*4 + 2, &int15Seg);
    if(int15Off != sp_int15_handler_off || int15Seg != sp_int15_handler_seg)
      {
#ifndef PROD
       printf("NTVDM: UpdateKbdInt15 Nuking I15 offsets\n");
#endif
       sp_int15_handler_off = sp_int15_handler_seg = 0;
       return;
       }

    sp_int15_handler_off = Off;
    sp_int15_handler_seg = Seg;
}

int runtime_softpc_int15_watch_source_end(void)
{
    return runtime_softpc_int15_watch_state_store(sp_int15_handler_off,
        sp_int15_handler_seg);
}

void runtime_softpc_int15_watch_source_reset(void)
{
    sp_int15_handler_off = sp_int15_handler_seg = 0;
}
