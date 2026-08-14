#ifndef BX_NTVDM_SPCKBD_INIT_SERVICE_H
#define BX_NTVDM_SPCKBD_INIT_SERVICE_H

#include "bx_ntvdm_cpu_result_v2.h"
#include "bx_ntvdm_exception_abi.h"
#include "bx_ntvdm_guest_gather_read_action_v1.h"
#include "bx_ntvdm_multi_write_transaction.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Guest-visible completion of NTIO's BOP 5F SoftPC C-BIOS handoff. Native
 * Bochs owns PIC/PIT/8042 mechanics; this service owns only the source-defined
 * x86 continuation for the verified BEEF request. */
int bx_ntvdm_spckbd_init_service_v1_dispatch(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result);

/* This is a closed profile value, not a host display setting.  S87 accepts
 * only the source-defined stream branch; the caller must supply it explicitly
 * and no zero/default profile is treated as stream I/O. */
#define BX_NTVDM_SPCKBD_DISPLAY_STREAM_IO 2u

/* First half of the source-derived BOP 5F state publication.  DS:SI names
 * the K.SYS kio table and index 17 is the word offset of use_host_int10. */
int bx_ntvdm_spckbd_stream_state_v1_prepare(
    uint32_t display_mode,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_guest_gather_read_action_v1 *action);

/* Completes exactly the matching two-byte kio-table read.  On success it
 * produces one checked ordinary-RAM byte write of STREAM_IO and the original
 * BOP 5F CF/RIP continuation. */
int bx_ntvdm_spckbd_stream_state_v1_complete(
    uint32_t display_mode,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_guest_gather_read_action_v1 *action,
    const uint8_t table_bytes[2], uint64_t byte_count,
    bx_ntvdm_multi_write_transaction_v1 *transaction,
    uint8_t payload[1]);

#ifdef __cplusplus
}
#endif

#endif
