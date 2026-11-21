#include "mvdm_vm_command.h"
#include "compat/include/guest_location.h"
#include "ntvdm-exe/session/session.h"
#include "compat/include/mvdm_vm_text.h"
#include <string.h>

DWORD mvdm_vm_command_snapshot(uint32_t service, uint32_t address,
    uint32_t extent, mvdm_command_capture **capture, ntvdm_vm_text **text)
{
    uint32_t descriptor[5];
    session *owner = session_thread_current();
    guest_memory_lease *lease;
    uint8_t *bytes;
    DWORD error;
    if (capture) *capture = NULL;
    if (text) *text = NULL;
    if (!capture || !text) return ERROR_INVALID_PARAMETER;
    if (service != 8 && service != 10) return ERROR_INVALID_FUNCTION;
    if (extent != sizeof(descriptor)) return ERROR_INVALID_DATA;
    if (!owner || !session_guest_memory_acquire(owner, address, extent,
            GUEST_MEMORY_ACCESS_READ, &lease, &bytes)) return ERROR_INVALID_ADDRESS;
    memcpy(descriptor, bytes, sizeof(descriptor));
    if (!session_guest_memory_release(owner, lease, 0)) return ERROR_INVALID_ADDRESS;
    /* Validate the extent before leasing or allocating the guest-owned frame. */
    if (descriptor[4] < sizeof(ntvdm_vm_text_header) ||
        descriptor[4] > sizeof(ntvdm_vm_text_header) + 255u * 256u * 2u)
        return ERROR_INVALID_DATA;
    if (!session_guest_memory_acquire(owner, descriptor[3], descriptor[4],
            GUEST_MEMORY_ACCESS_READ, &lease, &bytes)) return ERROR_INVALID_ADDRESS;
    error = ntvdm_vm_text_create(bytes, descriptor[4], text);
    if (!session_guest_memory_release(owner, lease, 0)) error = ERROR_INVALID_ADDRESS;
    if (!error)
        error = mvdm_vm_command_capture(service, descriptor[0], descriptor[1],
            descriptor[2], capture);
    if (error) {
        ntvdm_vm_text_destroy(*text);
        *text = NULL;
    }
    return error;
}
