#ifndef MVDM_VM_INGRESS_H
#define MVDM_VM_INGRESS_H
#include <windows.h>
#include <stdint.h>

/* Admission returns the executor token in the current BOP. */
DWORD mvdm_vm_command_admit(uint32_t vm, uint32_t descriptor, uint32_t bytes,
    uint64_t *token);
#endif
