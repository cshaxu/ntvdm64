#ifndef COMPAT_MVDM_VM_INPUT_H
#define COMPAT_MVDM_VM_INPUT_H

#include "console_io.h"
#include "mvdm_vm_continuation_store.h"

typedef struct mvdm_vm_input {
    mvdm_vm_continuation *continuation;
    SRWLOCK input_lock;
    HANDLE ready;
    console_io_input keys[CONSOLE_IO_INPUT_CAPACITY];
    uint32_t head, count;
} mvdm_vm_input;

/* The executor links this node for the lifetime of an admitted command.
 * The rundown lock guards list membership, retirement and make/release pairs. */
typedef struct mvdm_vm_input_job {
    struct mvdm_vm_input_job *next;
    mvdm_vm_input *input;
    uint64_t token, generation;
    uint32_t vm;
    BOOL finishing;
    KEY_EVENT_RECORD held[256];
} mvdm_vm_input_job;

DWORD mvdm_vm_input_begin(mvdm_vm_input *, mvdm_vm_continuation *);
DWORD mvdm_vm_input_pending(mvdm_vm_input *, BOOL *);
DWORD mvdm_vm_input_read(mvdm_vm_input *, const console_io_request *, console_io_reply *);
DWORD mvdm_vm_input_prepend(mvdm_vm_input *, const console_io_request *, console_io_reply *);
DWORD mvdm_vm_input_key(mvdm_vm_input *, const KEY_EVENT_RECORD *);
DWORD mvdm_vm_input_keys(mvdm_vm_input *, const KEY_EVENT_RECORD *, uint32_t count);
DWORD mvdm_vm_input_mouse(mvdm_vm_input *, const MOUSE_EVENT_RECORD *);
/* After all producers and callbacks join, transfer unread records to the VM. */
DWORD mvdm_vm_input_return(mvdm_vm_input *, uint32_t vm, uint64_t generation);

/* A translated guest event, scoped to the submitting VM and exact live token.
 * The I/O owner also checks continuation invalidation before enqueueing. */
DWORD mvdm_vm_input_send_key(mvdm_vm_rundown *, uint32_t vm, uint64_t token,
    const KEY_EVENT_RECORD *);
/* CPU-only BOP input: eight DWORDs (token low/high, down, repeat, virtual key,
 * scan, Unicode character, control state). Copy before enqueue; never retain
 * guest memory. Failure means the producer still owns the event. */
DWORD mvdm_vm_input_key_bop(mvdm_vm_rundown *, uint32_t vm,
    uint32_t address, uint32_t extent);
/* CPU-only seven-DWORD descriptor: token low/high, physical scan (bit7=E0),
 * kind (0 release, 1 make, 2 BIOS-only character), BIOS scan/OEM word,
 * BIOS417/418, BIOS496. Kind2 has physical0 and a nonzero scanless OEM byte;
 * it reconstructs native character records, not a physical make/release pair.
 * The BIOS word is zero on release or a raw modifier make.
 * Match real releases to accepted makes in this command, never synthesize one.
 * Failure leaves the producer's event and pairing state unconsumed. */
DWORD mvdm_vm_input_bios_key_bop(mvdm_vm_rundown *, uint32_t vm,
    uint32_t address, uint32_t extent);
/* Token low/high, INT33 mask, buttons, text cells X/Y, BIOS417. */
DWORD mvdm_vm_input_mouse_bop(mvdm_vm_rundown *, uint32_t vm,
    uint32_t address, uint32_t extent);
#endif
