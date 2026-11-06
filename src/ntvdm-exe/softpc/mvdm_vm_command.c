#include "mvdm_vm_command.h"
#include "mvdm_guest_location.h"
#include "ntvdm-exe/session/session.h"
#include "ntvdm-exe/frontend/vm_text.h"
#include <string.h>

/* Documented first 72 bytes of VMM Client_Reg_Struc (MS-DOS 6 SDVXD/VMM.INC).
 * The complete 108-byte save area additionally includes alternate registers. Segment
 * fields occupy four bytes, including their reserved high word. These are
 * guest values, never a layout cast of native CCPU or Windows objects. */
typedef struct vm_client_frame {
    uint32_t edi, esi, ebp, saved_esp, ebx, edx, ecx, eax;
    uint32_t error, eip, cs, flags, esp, ss, es, ds, fs, gs;
} vm_client_frame;
typedef char vm_client_frame_size[(sizeof(vm_client_frame) == 72) ? 1 : -1];

DWORD mvdm_vm_command_service(uint32_t address, uint32_t extent, uint32_t *service)
{
    uint32_t descriptor[5];
    vm_client_frame frame;
    session *owner = session_thread_current();
    guest_memory_lease *lease;
    uint8_t *bytes;
    mvdm_guest_location location;
    mvdm_guest_location_lease instruction;
    if (!service) return ERROR_INVALID_PARAMETER;
    *service = 0;
    if (extent != sizeof(descriptor)) return ERROR_INVALID_DATA;
    if (!owner || !session_guest_memory_acquire(owner, address, extent,
        GUEST_MEMORY_ACCESS_READ, &lease, &bytes)) return ERROR_INVALID_ADDRESS;
    memcpy(descriptor, bytes, sizeof(descriptor));
    if (!session_guest_memory_release(owner, lease, 0)) return ERROR_INVALID_ADDRESS;
    if (!session_guest_memory_acquire(owner, descriptor[0], sizeof(frame),
        GUEST_MEMORY_ACCESS_READ, &lease, &bytes)) return ERROR_INVALID_ADDRESS;
    memcpy(&frame, bytes, sizeof(frame));
    if (!session_guest_memory_release(owner, lease, 0)) return ERROR_INVALID_ADDRESS;
    mvdm_guest_location_set_real_mode(&location, (uint16_t)frame.cs,
        (uint16_t)(frame.eip - 1));
    if (!mvdm_guest_location_acquire_vm(&location, descriptor[1], 1,
        GUEST_MEMORY_ACCESS_READ, &instruction)) return ERROR_INVALID_ADDRESS;
    *service = instruction.bytes[0];
    if (!mvdm_guest_location_release(&instruction, 0)) return ERROR_INVALID_ADDRESS;
    return *service == 8 || *service == 10 ? ERROR_SUCCESS : ERROR_INVALID_FUNCTION;
}

DWORD mvdm_vm_command_capture(uint32_t service, uint32_t frame_address,
    uint32_t linear_base, uint32_t directory_address, mvdm_command_capture **output)
{
    vm_client_frame frame;
    mvdm_command_capture_input input = {0};
    mvdm_command_capture_view view;
    guest_memory_lease *lease;
    uint8_t *bytes;
    session *owner = session_thread_current();
    char comspec[MAX_PATH];
    char directory[67]; /* drive, colon, slash, DOS AH=47's 64-byte result */
    DWORD error;
    if (!output) return ERROR_INVALID_PARAMETER;
    *output = NULL;
    if (service != 8 && service != 10) return ERROR_INVALID_FUNCTION;
    if (!owner || !session_guest_memory_acquire(owner, frame_address,
        sizeof(frame), GUEST_MEMORY_ACCESS_READ, &lease, &bytes))
        return ERROR_INVALID_ADDRESS;
    memcpy(&frame, bytes, sizeof(frame));
    if (!session_guest_memory_release(owner, lease, 0))
        return ERROR_INVALID_ADDRESS;
    input.command_segment = (uint16_t)frame.ds;
    input.command_offset = (uint16_t)frame.esi;
    input.environment_segment = (uint16_t)frame.es;
    input.standard_segment = (uint16_t)frame.ss;
    input.standard_offset = (uint16_t)frame.ebp;
    input.default_drive = (uint8_t)frame.eax;
    input.vm_linear_base = linear_base;
    if (input.default_drive >= 26 || !session_guest_memory_acquire(owner,
        directory_address, 64, GUEST_MEMORY_ACCESS_READ, &lease, &bytes))
        return ERROR_INVALID_ADDRESS;
    memcpy(directory + 3, bytes, 64);
    if (!session_guest_memory_release(owner, lease, 0)) return ERROR_INVALID_ADDRESS;
    if (!memchr(directory + 3, 0, 64)) return ERROR_BAD_PATHNAME;
    directory[0] = (char)('A' + input.default_drive);
    directory[1] = ':'; directory[2] = '\\';
    input.directory = directory;
    /* Preserve cmdExec's AH branch, not a command-name/metacharacter guess.
     * The launch owner receives the original tail and explicit service. */
    if (service == 8 && (frame.eax & 0xff00u))
        input.service = MVDM_COMMAND_SERVICE_EXECUTE;
    if (service == 10) {
        error = mvdm_command_prepare_comspec(comspec, NULL);
        if (error) return error;
        input.host_command = comspec;
        input.service = MVDM_COMMAND_SERVICE_INTERACTIVE;
    }
    if (!mvdm_command_capture_create(&input, output)) return ERROR_INVALID_ADDRESS;
    if (input.service == MVDM_COMMAND_SERVICE_EXECUTE) {
        /* cmdExec validates the host COMSPEC after copying the guest tail.
         * Preserve its rejection even though the existing launcher carries
         * the explicit service/tail rather than the redundant combined text. */
        if (!mvdm_command_capture_read(*output, &view) ||
            mvdm_command_prepare_comspec(comspec, view.command)) {
            mvdm_command_capture_destroy(*output);
            *output = NULL;
            return ERROR_BAD_ENVIRONMENT;
        }
    }
    return ERROR_SUCCESS;
}

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
