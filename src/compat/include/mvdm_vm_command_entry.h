#ifndef MVDM_VM_COMMAND_ENTRY_H
#define MVDM_VM_COMMAND_ENTRY_H

#include <windows.h>
#include <stdint.h>

/* CPU-thread registration requires the active VM execution resource and IRQ. */
DWORD mvdm_vm_ingress_register(uint32_t callback);
void mvdm_vm_ingress_reset(void);

/* The BOP has consumed its service byte. A true result installs a CCPU far call. */
int mvdm_vm_command_enter(uint32_t service);

#endif
