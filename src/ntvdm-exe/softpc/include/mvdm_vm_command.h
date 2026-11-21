#ifndef MVDM_VM_COMMAND_H
#define MVDM_VM_COMMAND_H
#include "compat/include/mvdm_vm_capture.h"

/* Copy the addon callback's five-word descriptor, validate/copy its screen,
 * then capture COMMAND arguments. Both results are independently owned and
 * contain no retained guest memory. On failure both outputs are NULL. */
struct ntvdm_vm_text;
DWORD mvdm_vm_command_snapshot(uint32_t service, uint32_t descriptor_address,
    uint32_t descriptor_bytes, mvdm_command_capture **, struct ntvdm_vm_text **);
#endif
