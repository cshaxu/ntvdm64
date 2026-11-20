#ifndef MVDM_VM_COMMAND_ENTRY_H
#define MVDM_VM_COMMAND_ENTRY_H

#include <windows.h>
#include <stdint.h>

/* The caller holds ICA while validating admission and installing the target. */
DWORD mvdm_vm_command_install_callback(uint32_t callback);
void mvdm_vm_ingress_reset(void);

/* A true result installs a CCPU far call in place of normal BOP dispatch. */
int mvdm_vm_command_enter(uint32_t service);

#endif
