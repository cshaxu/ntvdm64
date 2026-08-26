/*
 * Source-shaped C8 recovery from:
 *   base/mvdm/softpc.new/host/src/nt_mouse.c:host_mouse_install1
 *   base/mvdm/softpc.new/base/keymouse/mouse_io.c:mouse_install1
 *
 * DIVERGENCE(BOP-DIV-108): the historical host wrapper sets mouse_state and
 * registers an IRQ9 EOI hook.  The full mouse_install1 body also owns INT71,
 * PIC masks, video hooks, a hardware reset, and host UI.  Those are a later
 * mouse-device/input package and cannot be silently enabled by a bootstrap
 * vector handoff.  The retained fragment is exactly the first table pair
 * consumed by the NTVDM MONITOR body and its INT 33h publication.
 */
#include "../../adapter-softpc/softpc_mouse_vector_shim.h"

#define int_addr(int_no) ((int_no) * 4)

static void mouse_install1(void)
{
    runtime_softpc_mouse_word MOUSE_IO_INTERRUPT_OFFSET;
    runtime_softpc_mouse_word MOUSE_IO_INTERRUPT_SEGMENT;

    /* Original MONITOR ordering starts by decoding the resident driver's
     * CS:BX mio_table.  This headless profile retains only its first entry:
     * offset/segment for the INT 33h service entry. */
    if (!runtime_softpc_mouse_vector_load_table_word(0u,
            &MOUSE_IO_INTERRUPT_OFFSET) ||
        !runtime_softpc_mouse_vector_load_table_word(2u,
            &MOUSE_IO_INTERRUPT_SEGMENT)) return;

    /* Original NTVDM mouse_install1 eventually publishes these two words at
     * int_addr(0x33).  The shim emits one checked four-byte IVT update. */
    (void)int_addr(0x33);
    (void)runtime_softpc_mouse_vector_store_int33(
        MOUSE_IO_INTERRUPT_OFFSET, MOUSE_IO_INTERRUPT_SEGMENT);
}

void host_mouse_install1(void)
{
    /* Original: mouse_state = INSTALLED; RegisterEOIHook(9, MouseEoiHook);
     * The device-owned statements are intentionally omitted; then original
     * control flow calls mouse_install1(). */
    mouse_install1();
}
