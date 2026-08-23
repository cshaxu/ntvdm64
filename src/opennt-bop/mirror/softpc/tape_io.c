/*
 * Direct source mirror fragment:
 *   src/opennt/base/mvdm/softpc.new/base/bios/tape_io.c
 *
 * This active first profile has neither the historical PM/CMOS path nor an
 * adopted timer, keyboard, DMA or interrupt-controller product shell.  The
 * reached real-mode AH=88h path compiles the original source's non-PM branch,
 * whose complete observable body is `setAX(0)`.  Other cassette_io switch
 * cases remain outside this fragment and must be admitted by their owning
 * machine package rather than fabricated here.
 */

#include "../../../adapter-softpc/softpc_tape_io_shim.h"

void cassette_io(void)
{
    switch (getAH())
    {
    case INT15_EMS_DETERMINE:
        /* Original tape_io.c, non-PM branch. */
        setAX(0);
        break;
    }
}
