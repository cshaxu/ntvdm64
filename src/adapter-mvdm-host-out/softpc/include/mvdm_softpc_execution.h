#ifndef MVDM_SOFTPC_EXECUTION_H
#define MVDM_SOFTPC_EXECUTION_H

#include <stdint.h>

#include "session/session.h"

#define MVDM_SOFTPC_EXECUTION_VERSION UINT32_C(1)
#define SESSION_MECHANICAL_STATUS_SOFTPC_RETURNED UINT32_C(1)

/* A composition-owned outer invocation.  It preserves the original
 * host_start_cpu() execution body and observes only its eventual return; it
 * does not reinterpret nested c_cpu_unsimulate() returns as session stops. */
int mvdm_softpc_execution_run_until_return(session *owner);

/* Top-level composition entry.  The original `obj.vdm/ntvdm.c` body is
 * compiled with only its C entry symbol renamed, then retains its own
 * TimerInit -> CpuEnvInit -> nls_init -> host_main order. */
int mvdm_softpc_execution_run_original_entry(session *owner, int argc,
    char **argv, int *result_out);

#endif
