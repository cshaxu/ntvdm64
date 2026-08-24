/*
 * Source-shaped fragment from:
 *   base/mvdm/softpc.new/host/src/nt_bop.c:MS_bop_F
 *   base/mvdm/softpc.new/base/keymouse/keybd_io.c:kb_setup_vectors
 *
 * DIVERGENCE (T243 S2): the complete keyboard translation unit owns CCPU/SAS
 * pointer state, host keyboard arbitration, timer/event threads, ICA control,
 * printer and video product composition.  None is composable into the
 * headless first profile.  This retained fragment preserves the original
 * kio_table layout and the sole selected `useHostInt10` publication.  Its
 * named shim maps only checked copied RAM and typed CF output; no Bochs object
 * or pointer crosses the boundary.
 */
#include "../../adapter-softpc/spckbd_handoff_shim.h"

#include <stdint.h>

#define KIO_USE_HOST_INT10 17u
#define KIO_VERSION         32u

void kb_setup_vectors(void)
{
   word KbdSeg, useHostInt10, version;
   uint32_t address;

   KbdSeg = getDS();

   /* Original code checks the table guard after it has consumed the entire
    * table.  The first profile consumes only one field; validate the same
    * guard before its sole guest write so malformed input cannot commit a
    * partial state update. */
   if (!runtime_spckbd_table_word(KIO_VERSION, &version) ||
       version != getAX() ||
       !runtime_spckbd_table_word(KIO_USE_HOST_INT10, &useHostInt10) ||
       (uint32_t)KbdSeg > UINT32_MAX / 16u ||
       ((uint32_t)KbdSeg << 4) > UINT32_MAX - (uint32_t)useHostInt10) {
       runtime_spckbd_handoff_fail();
       return;
   }

   /* Source order retained from kb_setup_vectors:
    * useHostInt10 = *pkio_table++;
    * sas_store_no_check((sys_addr)(phy_base + useHostInt10),
    *                    getNtScreenState()); */
   address = ((uint32_t)KbdSeg << 4) + (uint32_t)useHostInt10;
   sas_store_no_check(address, getNtScreenState());
}

void MS_bop_F(void)
{
    kb_setup_vectors();
    if (runtime_spckbd_handoff_failed()) return;

    /* DIVERGENCE (T243 S2): AddrIretBopTable, ICA restart and event-thread
     * release are monitor/product-shell responsibilities. Native mantle
     * lifecycle is already active; no guest IRQ is injected here. */
    setCF(1);
}
