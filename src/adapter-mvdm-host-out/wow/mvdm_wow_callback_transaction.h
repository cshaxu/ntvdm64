#ifndef MVDM_WOW_CALLBACK_TRANSACTION_H
#define MVDM_WOW_CALLBACK_TRANSACTION_H

#include <stdint.h>

/* Copied callback request/result. It contains the original callback's
 * observable numeric values but no guest/native pointer or handle. */
#define MVDM_WOW_CALLBACK_CONTROL_OPERATION 0x574f5701u
#define MVDM_WOW_CALLBACK_PARAMETER_MAXIMUM 64u

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
} mvdm_wow_callback_transaction;

/* The input is copied before dispatch. The app/session dispatch operation is
 * synchronous; it may only update the numeric AX/DX result in this record. */
int mvdm_wow_callback_transaction_invoke(uint32_t return_id,
    uint32_t procedure, const uint8_t *parameters, uint16_t parameter_bytes,
    uint32_t *return_value_out);

#endif
