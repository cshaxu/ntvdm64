/*
 * Source-shaped ordinary-profile recovery for mvdm-host/dbg/dbg.c::DBGDispatch.
 *
 * The original dispatcher decodes a WORD mode on the VDM stack.  In the
 * ordinary non-debug profile, its event callbacks either have no effect or
 * return FALSE; `DBG_TOOLHELP` is the one startup mode that unconditionally
 * records guest hook metadata.  The historic Sim32 pointer was a durable
 * process alias.  DIVERGENCE(ADAPTER-MVDM-DEBUGGER-003): obtain only the
 * finite words required by this call through the existing session mapping
 * manager, then release them before returning.  This preserves original
 * guest values, ordering and no-event results without exposing host memory.
 */

#include <stdint.h>
#include <string.h>

#include "adapter-mvdm-host-out/debugger/include/dbg_state.h"
#include "adapter-mvdm-host-out/softpc/include/mvdm_softpc_effective_address.h"
#include "adapter-mvdm-host-out/softpc/include/mvdm_softpc_guest_memory.h"

/* Original CPU40 C entry forms used by cpu4gen.h's getSS/getSP/setAX macros. */
extern uint16_t c_getSS(void);
extern uint16_t c_getSP(void);
extern void c_setAX(uint16_t value);

/* The original dbgsvc.h value, retained here so this source remains narrowly
 * coupled to the actual dispatch contract rather than a broader debugger
 * product header. */
#define MVDM_DBG_TOOLHELP 15u
#define MVDM_DBG_SINGLESTEP 5u
#define MVDM_DBG_BREAK 6u
#define MVDM_DBG_GPFAULT 7u
#define MVDM_DBG_DIVOVERFLOW 8u
#define MVDM_DBG_DLLSTART 12u

void mvdm_debugger_private_transport_unavailable(const char *origin);

static int mvdm_debugger_stack_words(IU16 *words, IU32 word_count)
{
    guest_memory_lease *lease;
    uint8_t *bytes;
    IU32 linear;
    IU32 byte_count;

    if (words == NULL || word_count == 0u ||
        word_count > UINT32_MAX / (IU32)sizeof(*words)) return 0;
    byte_count = word_count * (IU32)sizeof(*words);
    if (!mvdm_softpc_effective_address(c_getSS(), c_getSP(), &linear) ||
        !mvdm_softpc_guest_memory_acquire(linear, byte_count,
            GUEST_MEMORY_ACCESS_READ, &lease, &bytes)) return 0;
    memcpy(words, bytes, byte_count);
    return mvdm_softpc_guest_memory_release(lease, 0);
}

void DBGDispatch(void)
{
    IU16 frame[4];
    IU16 mode;

    /* `dbg.c` reads mode before its switch.  A failed modern checked read is
     * made visible as the original unknown-event direction, not as a fake
     * debugger event or a direct host pointer fault. */
    if (!mvdm_debugger_stack_words(frame, 1u)) {
        c_setAX(0u);
        return;
    }
    mode = frame[0];

    if (mode == MVDM_DBG_TOOLHELP) {
        if (!mvdm_debugger_stack_words(frame, 4u)) {
            c_setAX(0u);
            return;
        }
        /* Original order: mode, offset, selector, f386. */
        mvdm_debugger_set_toolhelp((DWORD)frame[1] | ((DWORD)frame[2] << 16),
            (BOOL)frame[3]);
        return;
    }

    /* The original private event delivery is only reached when a debugger is
     * attached.  Preserve its explicit unavailable direction rather than
     * fabricating a debugger reply. */
    if (mvdm_debugger_is_debugged()) {
        mvdm_debugger_private_transport_unavailable("debugger:DBGDispatch");
        return;
    }

    /* These source modes call functions whose initial non-debug result is
     * FALSE and then store it in AX. */
    switch (mode) {
    case MVDM_DBG_SINGLESTEP:
    case MVDM_DBG_BREAK:
    case MVDM_DBG_GPFAULT:
    case MVDM_DBG_DIVOVERFLOW:
    case MVDM_DBG_DLLSTART:
        c_setAX(0u);
        return;
    default:
        break;
    }

    /* SEGLOAD/SEGMOVE/SEGFREE/MODFREE and TASKSTOP have void no-event paths;
     * MODLOAD and ATTACH are explicit original no-ops.  Every other unlisted
     * mode takes dbg.c's `default: setAX(0)` branch. */
    switch (mode) {
    case 0u: /* DBG_SEGLOAD */
    case 1u: /* DBG_SEGMOVE */
    case 2u: /* DBG_SEGFREE */
    case 3u: /* DBG_MODLOAD */
    case 4u: /* DBG_MODFREE */
    case 11u: /* DBG_TASKSTOP */
    case 14u: /* DBG_ATTACH */
        return;
    default:
        c_setAX(0u);
        return;
    }
}
