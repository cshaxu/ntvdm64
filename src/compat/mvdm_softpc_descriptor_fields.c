/*
 * Source-shaped CPU40 descriptor-field bridge.
 *
 * `tape_io.c` retains the historical `retrieve_descr_fields` call form for
 * INT 15h move-block.  The selected CPU40 package already owns descriptor
 * decoding in `read_descriptor_linear`; this bridge only presents its result
 * with the full segment limit consumed by the XP BIOS.
 */

#include "include/mvdm_cpu_access.h"

/* DIVERGENCE(ADAPTER-SOFTPC-032): no selected OpenNT C body defines the
 * historical four-output helper.  CPU40's original read_descriptor_linear
 * performs the required guest-linear read, descriptor decode and fault
 * behavior.  Preserve the original helper name, argument order and typed
 * output stores; no host pointer, monitor descriptor table or reconstructed
 * CPU30 state is introduced. */
VOID retrieve_descr_fields(BYTE *AR, ULONG *base, ULONG *limit,
    ULONG descr_addr)
{
    ULONG decoded_base, decoded_limit;
    USHORT access;

    mvdm_cpu_access_current()->descriptor_fields(descr_addr, &decoded_base,
        &decoded_limit, &access);
    *AR = (BYTE)access;
    *base = decoded_base;
    *limit = decoded_limit;
}
