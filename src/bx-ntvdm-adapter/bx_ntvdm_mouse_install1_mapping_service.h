#ifndef BX_NTVDM_MOUSE_INSTALL1_MAPPING_SERVICE_H
#define BX_NTVDM_MOUSE_INSTALL1_MAPPING_SERVICE_H

#include "bx_ntvdm_guest_gather_read_action_v1.h"
#include "bx_ntvdm_multi_write_transaction.h"

/* The headless replacement for the reached OpenNT BOP C8 first-phase mouse
 * installer.  It transports only mio_table[0..3] to IVT INT 33h; neither
 * mouse state, IRQ/EOI behavior nor host input belongs here. */
int bx_ntvdm_mouse_install1_mapping_service_v1_prepare(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_guest_gather_read_action_v1 *action);
int bx_ntvdm_mouse_install1_mapping_service_v1_complete(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_guest_gather_read_action_v1 *action,
    const uint8_t *table_bytes, uint64_t byte_count,
    bx_ntvdm_multi_write_transaction_v1 *transaction,
    uint8_t payload[4]);

#endif
