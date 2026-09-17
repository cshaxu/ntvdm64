/*
 * Product-owned carrier for the session-bound part of the historical monitor
 * context.  The OpenNT ABI records themselves remain in <vdm.h>.
 */
#ifndef ADAPTER_MVDM_MONITOR_CONTEXT_H
#define ADAPTER_MVDM_MONITOR_CONTEXT_H

#include <nt.h>

/*
 * DIVERGENCE(ADAPTER-MONITOR-003): OpenNT's public/internal vdm.h exposes
 * its user-mode TIB records through the historical _X86_ declaration gate.
 * The selected guest ABI remains x86 on both supported host widths; this
 * adapter-local declaration gate exposes the original record on x86 and x64
 * without selecting CPU_30_STYLE, a kernel VDM, or an x86 host-code path.
 * It is removed immediately after importing the byte-for-byte mirror.
 */
#if !defined(_X86_)
#define ADAPTER_MVDM_MONITOR_UNDEF_X86_GATE
#define _X86_
#endif
#include <vdm.h>
#ifdef ADAPTER_MVDM_MONITOR_UNDEF_X86_GATE
#undef _X86_
#undef ADAPTER_MVDM_MONITOR_UNDEF_X86_GATE
#endif

/* One standalone ntvdm worker owns exactly one VDM.  Keep this as the
 * original monitor's process-global storage, rather than a per-host-thread
 * replacement: original NTIO initialization can enter from a host worker
 * before the CPU-owning thread resumes. */
extern VDM_TIB VdmTib;
extern ULONG mvdm_monitor_ntvdm_state;

/* The historical low-address state macro is unavailable outside the NT4
 * monitor.  This process-local value has the same source-facing scalar form
 * and shares the one-worker lifetime of the original VDM TIB. */
#ifdef pNtVDMState
#undef pNtVDMState
#endif
#define pNtVDMState (&mvdm_monitor_ntvdm_state)

int mvdm_monitor_bind_current_thread(void);

#endif
