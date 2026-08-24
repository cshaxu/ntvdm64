/////////////////////////////////////////////////////////////////////////
//
// Process-local, selector-blind cancellation controller for one active
// minimal-machine run.  The API transports only a fixed reason value; no
// host event, pointer, callback, guest state or Bochs object crosses it.
//
/////////////////////////////////////////////////////////////////////////

#ifndef BX_NTVDM_CANCELLATION_CONTROLLER_V1_H
#define BX_NTVDM_CANCELLATION_CONTROLLER_V1_H

#include "session/session_cancellation_v1.h"

/* Compatibility spelling retained for existing mechanical callers.  The
 * state itself is session-owned and has no SoftPC/CCPU semantics. */
#define BX_NTVDM_CANCELLATION_V1_NONE NTDOS64_SESSION_CANCELLATION_V1_NONE
#define BX_NTVDM_CANCELLATION_V1_USER_REQUEST NTDOS64_SESSION_CANCELLATION_V1_USER_REQUEST
#define bx_ntvdm_cancellation_controller_v1_activate ntdos64_session_cancellation_v1_activate
#define bx_ntvdm_cancellation_controller_v1_deactivate ntdos64_session_cancellation_v1_deactivate
#define bx_ntvdm_cancellation_controller_v1_request ntdos64_session_cancellation_v1_request
#define bx_ntvdm_cancellation_controller_v1_requested_reason ntdos64_session_cancellation_v1_requested_reason
#if defined(BX_NTVDM_CANCELLATION_TESTING)
#define bx_ntvdm_cancellation_controller_v1_test_poll_mark ntdos64_session_cancellation_v1_test_poll_mark
#define bx_ntvdm_cancellation_controller_v1_test_poll_count ntdos64_session_cancellation_v1_test_poll_count
#endif

#endif
