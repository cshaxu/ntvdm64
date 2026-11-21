#ifndef MVDM_VM_CAPTURE_H
#define MVDM_VM_CAPTURE_H
#include "mvdm_command_native_child.h"

/* CPU callback only. Copy the VMM frame and DOS directory before returning.
 * Returned captures own their bytes and retain no guest pointers. */
DWORD mvdm_vm_command_capture(uint32_t service, uint32_t frame_address,
    uint32_t linear_base, uint32_t directory_address, mvdm_command_capture **output);
/* Read the original service preceding the restored IP in this VM's mapping. */
DWORD mvdm_vm_command_service(uint32_t descriptor_address, uint32_t descriptor_bytes,
    uint32_t *service);
/* Copy the addon callback's five-word descriptor, validate/copy its screen,
 * then capture COMMAND arguments. Both results are independently owned and
 * contain no retained guest memory. On failure both outputs are NULL. */
struct ntvdm_vm_text;
DWORD mvdm_vm_command_snapshot(uint32_t service, uint32_t descriptor_address,
    uint32_t descriptor_bytes, mvdm_command_capture **, struct ntvdm_vm_text **);
#endif
