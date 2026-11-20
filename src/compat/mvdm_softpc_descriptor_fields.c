/*
 * Source-shaped CPU40 descriptor-field bridge.
 *
 * `tape_io.c` retains the historical `retrieve_descr_fields` call form for
 * INT 15h move-block.  The selected CPU40 package already owns descriptor
 * decoding in `read_descriptor_linear`; this bridge only presents its result
 * in the exact older SoftPC output layout.
 */

#include <insignia.h>
#include <host_def.h>
#include <xt.h>
#include <c_main.h>
#include <c_bsic.h>

/* DIVERGENCE(ADAPTER-SOFTPC-032): no selected OpenNT C body defines the
 * historical four-output helper.  CPU40's original read_descriptor_linear
 * performs the required guest-linear read, descriptor decode and fault
 * behavior.  Preserve the original helper name, argument order and narrow
 * output stores; no host pointer, monitor descriptor table or reconstructed
 * CPU30 state is introduced. */
VOID retrieve_descr_fields(half_word *AR, sys_addr *base, word *limit,
    sys_addr descr_addr)
{
    CPU_DESCR descriptor;

    read_descriptor_linear((IU32)descr_addr, &descriptor);
    *AR = (half_word)descriptor.AR;
    *base = (sys_addr)descriptor.base;
    *limit = (word)descriptor.limit;
}
