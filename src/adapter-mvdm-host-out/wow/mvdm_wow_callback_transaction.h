#ifndef MVDM_WOW_CALLBACK_TRANSACTION_H
#define MVDM_WOW_CALLBACK_TRANSACTION_H

#include <stdint.h>

/* Copied callback request/result. It contains the original callback's
 * observable numeric values but no guest/native pointer or handle. */
#define MVDM_WOW_CALLBACK_PARAMETER_MAXIMUM 16u

/* This is the numeric, packed portion of the original WOW32 CBVDMFRAME in
 * mvdm-support/inc/wow.h.  It deliberately retains the original field order
 * and 16-byte PARM16 union extent, while replacing only historical typedefs
 * with fixed-width C types so its byte layout is identical on x86 and x64.
 * It is copied through a scoped guest lease; it is never a host pointer. */
#pragma pack(push, 1)
typedef struct mvdm_wow_callback_guest_frame {
    uint16_t w_tdb;
    uint16_t w_return_id;
    uint16_t w_local_bp;
    uint8_t parm16[MVDM_WOW_CALLBACK_PARAMETER_MAXIMUM];
    uint32_t vpfn_proc;
    uint32_t vp_stack;
    uint16_t w_ax;
    uint16_t w_dx;
    uint16_t w_gen_use1;
    uint16_t w_gen_use2;
} mvdm_wow_callback_guest_frame;
#pragma pack(pop)

_Static_assert(sizeof(mvdm_wow_callback_guest_frame) == 38u,
    "CBVDMFRAME numeric layout must remain source-shaped");

typedef struct mvdm_wow_callback_transaction {
    uint32_t struct_bytes;
    uint32_t return_id;
    uint32_t procedure;
    uint32_t callback_stack;
    uint32_t original_stack;
    uint16_t task16;
    uint16_t ax;
    uint16_t dx;
    uint16_t parameter_bytes;
    uint16_t reserved0;
    uint8_t parameters[MVDM_WOW_CALLBACK_PARAMETER_MAXIMUM];
    mvdm_wow_callback_guest_frame guest_frame;
} mvdm_wow_callback_transaction;

/* The input is copied before the source-shaped synchronous guest transaction.
 * No session command/control dispatcher participates in CallBack16. */
int mvdm_wow_callback_transaction_invoke(uint32_t return_id,
    uint32_t procedure, const uint8_t *parameters, uint16_t parameter_bytes,
    uint32_t *return_value_out);

#endif
