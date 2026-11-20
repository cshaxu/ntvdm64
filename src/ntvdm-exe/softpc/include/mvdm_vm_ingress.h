#ifndef MVDM_VM_INGRESS_H
#define MVDM_VM_INGRESS_H
#include <windows.h>
#include <stdint.h>
#include "compat/include/mvdm_vm_command_entry.h"

/* Registration is reachable only through the signed ring-zero bridge BOP. */
DWORD mvdm_vm_ingress_register(uint32_t callback);
DWORD mvdm_vm_command_admit(uint32_t vm, uint32_t descriptor, uint32_t bytes,
    uint64_t *token);
#endif
