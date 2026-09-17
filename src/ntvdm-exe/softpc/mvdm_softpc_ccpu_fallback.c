/*
 * DIVERGENCE(MVDM-HOST-DIV-127): the selected x86 CCPU manifest retains
 * declarations for these historical fallback hooks but has no provider unit.
 * This file carries only their exact NTVDMx64 source-proven fallback forms;
 * it deliberately does not import the patch's unrelated empty placeholders.
 */
#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>

#include "insignia.h"
#include "host_def.h"
#include <intrin.h>

#if defined(MVDM_CCPU_ACTIVITY_CHECK_ONLY)

/* The selected original callers own idling; this fallback neither schedules
 * nor terminates a worker. */
VOID ActivityCheckAfterTimeSlice(VOID)
{
}

#elif defined(MVDM_CCPU_VECTOR_DEFAULTS_ONLY)

/* These source-unselected vector tails must fail immediately if reached. */
VOID EDL_fast_bop(ULONG immed)
{
    __debugbreak();
}

IU32 c_VirtualiseInstruction(IU32 eipInRom, IUH size,
                             IU32 linearAddrOrPort, IU32 dataIn)
{
    __debugbreak();
    return 0;
}

IU8 *c_sas_touch(IU32 addr, IU32 length)
{
    __debugbreak();
    return NULL;
}

#else
#error This source must be compiled only as a selected CCPU fallback provider.
#endif
