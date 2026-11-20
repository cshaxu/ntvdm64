#ifndef MVDM_VM_NOTIFICATION_H
#define MVDM_VM_NOTIFICATION_H
#include <windows.h>
#include <stdint.h>

/* One physical emulated PIC belongs to one CCPU worker. This doorbell owns
 * no work items, VM identities or lifecycle policy. Its caller owns the work
 * queue and acknowledges only after draining the corresponding publication.
 * All calls require initialized original ICA services. The worker owner must
 * stop/join producers before close/reopen; publication serials never rewind. */
DWORD mvdm_vm_notification_open(WORD *irq);
DWORD mvdm_vm_notification_ready(void);
DWORD mvdm_vm_notification_post(uint64_t *serial);
DWORD mvdm_vm_notification_snapshot(uint64_t *serial);
DWORD mvdm_vm_notification_acknowledge(uint64_t serial);
DWORD mvdm_vm_notification_close(void);
#endif
