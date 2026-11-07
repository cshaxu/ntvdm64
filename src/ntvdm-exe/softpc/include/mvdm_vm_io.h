#ifndef MVDM_VM_IO_H
#define MVDM_VM_IO_H
#include "mvdm_vm_continuation.h"
#include "ntvdm-exe/frontend/vm_frontend.h"
#include "ntvdm-exe/frontend/vm_text.h"

typedef struct mvdm_vm_io mvdm_vm_io;
/* Borrow model/continuation until registry retirement joins all callbacks.
 * Title is copied ANSI handoff metadata, not a guest address. The caller owns
 * VM identity and registration; this provider performs no lifecycle RPC. */
DWORD mvdm_vm_io_create(ntvdm_vm_text *, mvdm_vm_continuation *, const char *, mvdm_vm_io **);
HANDLE mvdm_vm_io_ready(mvdm_vm_io *);
extern const ntvdm_vm_io mvdm_vm_io_operations;
/* Producer supplies a translated guest key, never host layout guesses. Failure
 * means not accepted; the producer must not consume that guest event. */
DWORD mvdm_vm_io_key(mvdm_vm_io *, const KEY_EVENT_RECORD *);
/* Entire translated-character sequence is accepted or left with its producer. */
DWORD mvdm_vm_io_keys(mvdm_vm_io *, const KEY_EVENT_RECORD *, uint32_t count);
/* After producers/callbacks join, transfer unread records to the VM owner. */
DWORD mvdm_vm_io_return_input(mvdm_vm_io *, uint32_t vm, uint64_t generation);
void mvdm_vm_io_destroy(mvdm_vm_io *); /* Only after callbacks/producers join. */
#endif
