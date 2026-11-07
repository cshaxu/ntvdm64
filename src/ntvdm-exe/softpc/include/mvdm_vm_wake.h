#ifndef MVDM_VM_WAKE_H
#define MVDM_VM_WAKE_H
#include <stdint.h>
#include "common/protocol/console_io.h"
typedef struct {
    uint32_t vm;
    uint64_t generation;
} mvdm_vm_wake_identity;
/* CCPU publishes VMM identity/lifetime; host producers publish wake records.
 * No guest pointer, RPC, VMM call or blocking wait crosses this boundary.
 * Registration requires an open notification session. Owner stops producers
 * before reset; ICA serializes queue and doorbell. */
uint32_t mvdm_vm_wake_register(uint32_t vm, uint64_t generation);
uint32_t mvdm_vm_wake_lookup(uint32_t vm, mvdm_vm_wake_identity *identity);
uint32_t mvdm_vm_wake_retire(uint32_t vm, uint64_t generation);
uint32_t mvdm_vm_wake_publish(uint32_t vm, uint64_t generation, uint64_t *serial);
uint32_t mvdm_vm_wake_take(mvdm_vm_wake_identity *identity);
/* Unread records outlive command completion, but never their exact VM.
 * Append copies atomically. Read permits peeking; consuming is explicit. */
uint32_t mvdm_vm_wake_return_input(uint32_t vm, uint64_t generation,
    const console_io_input *keys, uint32_t count);
uint32_t mvdm_vm_wake_read_input(uint32_t vm, uint64_t generation,
    console_io_input *keys, uint32_t capacity, uint32_t *count, int consume);
void mvdm_vm_wake_reset(void);
#endif
