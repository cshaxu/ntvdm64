/////////////////////////////////////////////////////////////////////////
//
// Process-local, selector-blind cancellation controller for one active
// minimal-machine run.  The API transports only a fixed reason value; no
// host event, pointer, callback, guest state or Bochs object crosses it.
//
/////////////////////////////////////////////////////////////////////////

#ifndef RUNTIME_CANCELLATION_CONTROLLER_V1_H
#define RUNTIME_CANCELLATION_CONTROLLER_V1_H

#include "session/session_cancellation.h"

/* Compatibility spelling retained for existing mechanical callers.  The
 * state itself is session-owned and has no SoftPC/CCPU semantics. */
#define RUNTIME_CANCELLATION_V1_NONE APP_SESSION_CANCELLATION_V1_NONE
#define RUNTIME_CANCELLATION_V1_USER_REQUEST APP_SESSION_CANCELLATION_V1_USER_REQUEST
#define runtime_cancellation_controller_v1_activate app_session_cancellation_v1_activate
#define runtime_cancellation_controller_v1_deactivate app_session_cancellation_v1_deactivate
#define runtime_cancellation_controller_v1_request app_session_cancellation_v1_request
#define runtime_cancellation_controller_v1_requested_reason app_session_cancellation_v1_requested_reason
#if defined(RUNTIME_CANCELLATION_TESTING)
#define runtime_cancellation_controller_v1_test_poll_mark app_session_cancellation_v1_test_poll_mark
#define runtime_cancellation_controller_v1_test_poll_count app_session_cancellation_v1_test_poll_count
#endif

#endif
