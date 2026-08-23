/////////////////////////////////////////////////////////////////////////
//
// Process-local, selector-blind cancellation controller for one active
// minimal-machine run.  The API transports only a fixed reason value; no
// host event, pointer, callback, guest state or Bochs object crosses it.
//
/////////////////////////////////////////////////////////////////////////

#ifndef BX_NTVDM_CANCELLATION_CONTROLLER_V1_H
#define BX_NTVDM_CANCELLATION_CONTROLLER_V1_H

#include <stdint.h>

enum bx_ntvdm_cancellation_reason_v1 {
  BX_NTVDM_CANCELLATION_V1_NONE = 0u,
  BX_NTVDM_CANCELLATION_V1_USER_REQUEST = 1u
};

#ifdef __cplusplus
extern "C" {
#endif

int bx_ntvdm_cancellation_controller_v1_activate(void);
void bx_ntvdm_cancellation_controller_v1_deactivate(void);
int bx_ntvdm_cancellation_controller_v1_request(uint32_t reason);
uint32_t bx_ntvdm_cancellation_controller_v1_requested_reason(void);

#if defined(BX_NTVDM_CANCELLATION_TESTING)
/* Fixture-only copied poll witness. It is omitted from every production
 * compile and carries no guest or host state. */
void bx_ntvdm_cancellation_controller_v1_test_poll_mark(void);
uint32_t bx_ntvdm_cancellation_controller_v1_test_poll_count(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
