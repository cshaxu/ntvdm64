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
#endif
