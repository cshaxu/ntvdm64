#ifndef MVDM_VM_INGRESS_H
#define MVDM_VM_INGRESS_H
#include <windows.h>
#include <stdint.h>

/* Registration is reachable only through the signed ring-zero bridge BOP.
 * Zero clears admission during driver/worker teardown; no wait or RPC. */
DWORD mvdm_vm_ingress_register(uint32_t callback);
void mvdm_vm_ingress_reset(void);
/* Original MS_bop_4 returns immediately when this installs a real CALLF;
 * otherwise it executes its unchanged original dispatcher and IP increment. */
int mvdm_vm_command_enter(uint32_t service);
DWORD mvdm_vm_command_admit(uint32_t vm, uint32_t descriptor, uint32_t bytes,
    uint64_t *token);
#endif
