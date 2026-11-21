#ifndef MVDM_VM_INGRESS_H
#define MVDM_VM_INGRESS_H
#include <windows.h>
#include <stdint.h>

#include "mvdm_command_launch.h"
#include "mvdm_vm_text.h"

/* Called synchronously on the submitting CPU thread. Success transfers both
 * prepared objects and publishes a real executor token; failure transfers
 * neither. Register before CPU entry; unbind after CPU stops, before draining
 * the executor. The borrowed context must outlive all admitted work. */
typedef DWORD (*mvdm_vm_command_submit)(void *, uint32_t,
    ntvdm_command_launch *, ntvdm_vm_text *, uint64_t *);
void mvdm_vm_command_bind(mvdm_vm_command_submit, void *context);

/* Admission returns the executor token in the current BOP. */
DWORD mvdm_vm_command_admit(uint32_t vm, uint32_t descriptor, uint32_t bytes,
    uint64_t *token);
#endif
