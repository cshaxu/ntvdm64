#ifndef WOW_CALLBACK_FRAME_LEASE_H
#define WOW_CALLBACK_FRAME_LEASE_H

#include <stdint.h>

#include "ntvdm-exe/session/guest_memory_lease.h"

/*
 * Bounded copied memory for the remaining resource and task-parent callers.
 * CallBack16 uses its original protected-mode resolver, not this SAS path.
 */
typedef struct wow_callback_frame_lease {
    guest_memory_lease *lease;
    uint8_t *bytes;
    uint32_t guest_address;
    uint32_t byte_count;
    uint32_t access;
} wow_callback_frame_lease;

/* ADAPTER-WOW-099: only the abandoned call's control continuation, never a
 * complete CPU/device snapshot. Storage belongs to the native call frame. */
typedef struct wow_callback_control {
    uint16_t cs, ip;
    int32_t pe, vm, cpl;
    int captured;
} wow_callback_control;
void __cdecl wow_callback_control_capture(wow_callback_control *control);
/* Zero means terminal worker abandonment: do not touch even the old TD.
 * An active but impossible restoration fails through the existing boundary. */
int __cdecl wow_callback_control_restore(const wow_callback_control *control,
    uint32_t caller_stack);

/* Resource pointers use explicit protected addressing even during a real-mode
 * WOW entry. The original caller must first resolve/lock the selector and
 * supply its full resource size; this binding does not fault in segments. */
int wow_callback_frame_acquire_protected_vp(uint32_t vp, uint32_t byte_count,
    uint32_t access, wow_callback_frame_lease *view_out);

/* The linear form is reserved for callers that already obtained a checked
 * CCPU/SIM32 linear guest address. */
int wow_callback_frame_acquire_linear(uint32_t guest_address, uint32_t byte_count,
    uint32_t access, wow_callback_frame_lease *view_out);
int wow_callback_frame_release(wow_callback_frame_lease *view, int commit);

/* Default-off observation only.  The original CallBack16 owner supplies its
 * already-computed frame values immediately before/after the recursive CPU
 * transition; this binding neither maps memory nor changes callback state. */
void wow_callback_frame_trace(const char *phase, uint32_t ret_id,
    uint32_t vp_stack, uint32_t vp_callback_stack,
    uint32_t callback_linear, uint16_t task, uint32_t procedure,
    uint16_t message, uint32_t parameter);

#endif
