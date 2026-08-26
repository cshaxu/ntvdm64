#ifndef RUNTIME_CCPU_FRAME_CONTEXT_H
#define RUNTIME_CCPU_FRAME_CONTEXT_H

/* Same-shaped CCPU/SAS compatibility for one borrowed copied CPU frame.
 * This mechanical adapter has no selector, provider, session or guest-owner
 * meaning. Its caller supplies only bounded callbacks and the fixed CPU/result
 * records that are already accepted by the machine boundary. */

#include <stdint.h>

#include "cpu_result.h"
#include "cpu_state_abi.h"

#define RUNTIME_CCPU_FRAME_CONTEXT_MAGIC 0x42584346u
#define RUNTIME_CCPU_FRAME_CONTEXT_VERSION 1u

typedef int (*runtime_ccpu_guest_read_fn)(void *state, uint32_t address,
    uint8_t *bytes, uint32_t byte_count);
typedef int (*runtime_ccpu_guest_write_fn)(void *state, uint32_t address,
    const uint8_t *bytes, uint32_t byte_count);

typedef struct runtime_ccpu_frame_context {
    uint32_t magic, abi_version, struct_bytes, reserved0;
    runtime_cpu_state *cpu;
    runtime_cpu_result *result;
    void *guest_state;
    runtime_ccpu_guest_read_fn guest_read;
    runtime_ccpu_guest_write_fn guest_write;
} runtime_ccpu_frame_context;

#ifdef __cplusplus
extern "C" {
#endif

int runtime_ccpu_frame_context_valid(const runtime_ccpu_frame_context *context);
int runtime_ccpu_frame_context_begin(const runtime_ccpu_frame_context *context);
void runtime_ccpu_frame_context_end(void);

uint16_t runtime_ccpu_get_ax(void); uint16_t runtime_ccpu_get_bx(void);
uint16_t runtime_ccpu_get_cx(void); uint16_t runtime_ccpu_get_dx(void);
uint16_t runtime_ccpu_get_si(void); uint16_t runtime_ccpu_get_bp(void);
uint16_t runtime_ccpu_get_cs(void); uint16_t runtime_ccpu_get_ds(void);
uint16_t runtime_ccpu_get_es(void); uint16_t runtime_ccpu_get_ss(void);
uint16_t runtime_ccpu_get_ip(void); uint8_t runtime_ccpu_get_al(void);
uint8_t runtime_ccpu_get_ah(void);
void runtime_ccpu_set_ax(uint16_t value); void runtime_ccpu_set_al(uint16_t value);
void runtime_ccpu_set_bx(uint16_t value); void runtime_ccpu_set_cx(uint16_t value);
void runtime_ccpu_set_dx(uint16_t value); void runtime_ccpu_set_ds(uint16_t value);
void runtime_ccpu_set_es(uint16_t value); void runtime_ccpu_set_ip(uint16_t value);
void runtime_ccpu_set_cf(int value);
void runtime_ccpu_sas_load(uint32_t address, uint8_t *target);
int runtime_ccpu_set_pending(void);
int runtime_ccpu_set_controlled_stop(void);

#ifdef __cplusplus
}
#endif

#endif
