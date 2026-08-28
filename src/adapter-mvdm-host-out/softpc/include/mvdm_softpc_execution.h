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

#endif
