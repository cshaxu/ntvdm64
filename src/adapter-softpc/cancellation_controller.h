/////////////////////////////////////////////////////////////////////////
//
// Process-local, selector-blind cancellation controller for one active
// minimal-machine run.  The API transports only a fixed reason value; no
// host event, pointer, callback, guest state or Bochs object crosses it.
//
/////////////////////////////////////////////////////////////////////////

#ifndef RUNTIME_CANCELLATION_CONTROLLER_H
#define RUNTIME_CANCELLATION_CONTROLLER_H

#include "session/session_cancellation.h"

/* Compatibility spelling retained for existing mechanical callers.  The
 * state itself is session-owned and has no SoftPC/CCPU semantics. */
#define RUNTIME_CANCELLATION_NONE APP_SESSION_CANCELLATION_NONE
#define RUNTIME_CANCELLATION_USER_REQUEST APP_SESSION_CANCELLATION_USER_REQUEST
#define runtime_cancellation_controller_activate app_session_cancellation_activate
#define runtime_cancellation_controller_deactivate app_session_cancellation_deactivate
#define runtime_cancellation_controller_request app_session_cancellation_request
#define runtime_cancellation_controller_requested_reason app_session_cancellation_requested_reason
#if defined(RUNTIME_CANCELLATION_TESTING)
#define runtime_cancellation_controller_test_poll_mark app_session_cancellation_test_poll_mark
#define runtime_cancellation_controller_test_poll_count app_session_cancellation_test_poll_count
#endif

#endif
