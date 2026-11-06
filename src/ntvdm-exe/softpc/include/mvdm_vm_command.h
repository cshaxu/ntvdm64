#ifndef MVDM_VM_COMMAND_H
#define MVDM_VM_COMMAND_H
#include "ntvdm-exe/command/include/mvdm_command_native_child.h"

/* Only on the CPU thread, in the actual VMM callback, before it waits.
 * Frame and base are VMM Client_Reg_Struc and CB_High_Linear addresses.
 * Directory is a callback-owned copy captured in the nested DOS context;
 * it is not a retained alias of the VMM's instanced V86 data buffer.
 * The admitted original BOP supplies service 8 or 10. No guest address or
 * frame pointer is retained in the returned independent capture. */
DWORD mvdm_vm_command_capture(uint32_t service, uint32_t frame_address,
    uint32_t linear_base, uint32_t directory_address, mvdm_command_capture **output);
/* Copy the addon callback's five-word descriptor, validate/copy its screen,
 * then capture COMMAND arguments. Both results are independently owned and
 * contain no retained guest memory. On failure both outputs are NULL. */
struct ntvdm_vm_text;
DWORD mvdm_vm_command_snapshot(uint32_t service, uint32_t descriptor_address,
    uint32_t descriptor_bytes, mvdm_command_capture **, struct ntvdm_vm_text **);
/* Recover the original service byte immediately before the restored return
 * IP, using the callback's own VM mapping. No ambient V86 low-memory alias. */
DWORD mvdm_vm_command_service(uint32_t descriptor_address, uint32_t descriptor_bytes,
    uint32_t *service);
#endif
