#ifndef MVDM_VM_EXECUTOR_H
#define MVDM_VM_EXECUTOR_H
#include "mvdm_vm_execute.h"
#include "mvdm_vm_io.h"

typedef struct mvdm_vm_executor mvdm_vm_executor;
/* Worker owns the executor and borrowed host. Create before CPU entry; close
 * off CPU/ICA before closing/destroying the host. Closing cancels continuations
 * and joins admitted jobs; NTSRV still owns all child/chain termination. */
DWORD mvdm_vm_executor_create(ntvdm_vm_host *, mvdm_vm_executor **);
void mvdm_vm_executor_close(mvdm_vm_executor *);
void mvdm_vm_executor_destroy(mvdm_vm_executor *);
/* CPU admission: independently prepared launch/text transfer only on success.
 * No RPC or native wait occurs here. Output token is published only after a
 * real executor is queued. The caller retains both objects on any failure. */
DWORD mvdm_vm_executor_submit(mvdm_vm_executor *, uint32_t vm,
    ntvdm_command_launch *, ntvdm_vm_text *, uint64_t *token);
/* A translated guest event, scoped to the submitting VM and exact live token.
 * The I/O owner also checks continuation invalidation before enqueueing. */
DWORD mvdm_vm_executor_key(mvdm_vm_executor *, uint32_t vm, uint64_t token,
    const KEY_EVENT_RECORD *);
/* CPU-only BOP input: eight DWORDs (token low/high, down, repeat, virtual key,
 * scan, Unicode character, control state). Copy before enqueue; never retain
 * guest memory. Failure means the producer still owns the event. */
DWORD mvdm_vm_executor_key_bop(mvdm_vm_executor *, uint32_t vm,
    uint32_t address, uint32_t extent);
/* CPU-only seven-DWORD descriptor: token low/high, physical scan (bit7=E0),
 * kind (0 release, 1 make, 2 BIOS-only character), BIOS scan/OEM word,
 * BIOS417/418, BIOS496. Kind2 has physical0 and a nonzero scanless OEM byte;
 * it reconstructs native character records, not a physical make/release pair.
 * The BIOS word is zero on release or a raw modifier make.
 * Match real releases to accepted makes in this command, never synthesize one.
 * Failure leaves the producer's event and pairing state unconsumed. */
DWORD mvdm_vm_executor_bios_key_bop(mvdm_vm_executor *, uint32_t vm,
    uint32_t address, uint32_t extent);
/* Token low/high, INT33 mask, buttons, text cells X/Y, BIOS417. */
DWORD mvdm_vm_executor_mouse_bop(mvdm_vm_executor *, uint32_t vm,
    uint32_t address, uint32_t extent);
/* CPU-only generation low/high and active OEM code page. Transfer one
 * converted BIOS word/repeat count; failed conversion keeps the record. */
DWORD mvdm_vm_executor_return_key_bop(uint32_t vm, uint32_t address,
    uint32_t extent, uint32_t *word, uint32_t *repeat);
#endif
