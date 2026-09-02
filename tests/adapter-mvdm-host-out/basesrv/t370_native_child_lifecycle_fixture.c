#include <windows.h>

#include <string.h>

#include "base_vdm_local.h"
#include "mvdm_command_native_child.h"
#include "session/session.h"

typedef struct fixture_memory {
    unsigned char bytes[0x10000];
} fixture_memory;

typedef struct worker_context {
    session *owner;
} worker_context;

static int fixture_read(void *context, uint32_t address, uint8_t *buffer,
    uint32_t count)
{
    fixture_memory *memory = (fixture_memory *)context;
    if (memory == NULL || buffer == NULL || address > sizeof(memory->bytes) ||
        count > sizeof(memory->bytes) - address) return 0;
    memcpy(buffer, memory->bytes + address, count);
    return 1;
}

static int fixture_write(void *context, uint32_t address, const uint8_t *buffer,
    uint32_t count)
{
    fixture_memory *memory = (fixture_memory *)context;
    if (memory == NULL || buffer == NULL || address > sizeof(memory->bytes) ||
        count > sizeof(memory->bytes) - address) return 0;
    memcpy(memory->bytes + address, buffer, count);
    return 1;
}

static DWORD WINAPI reentry_worker(LPVOID parameter)
{
    worker_context *context = (worker_context *)parameter;
    VDMINFO request;
    if (context == NULL || !session_thread_bind_owned_source(context->owner,
            SESSION_THREAD_BINDING_ORIGINAL_WORKER, "cmdCreateProcess")) return 1u;
    ZeroMemory(&request, sizeof(request));
    request.VDMState = INCREMENT_REENTER_COUNT;
    if (!base_vdm_local_dispatch(&request)) return 2u;
    Sleep(40u);
    mvdm_command_native_child_finish();
    request.VDMState = DECREMENT_REENTER_COUNT;
    if (!base_vdm_local_dispatch(&request)) return 3u;
    return session_thread_unbind(context->owner) ? 0u : 4u;
}

int main(void)
{
    fixture_memory memory;
    session owner;
    base_vdm_local local;
    base_vdm_command command;
    worker_context worker;
    VDMINFO delivery;
    VDMINFO wait_request;
    ULONG handles[3];
    const char *snapshot_command;
    const char *snapshot_environment;
    HANDLE thread;
    DWORD exit_code;

    ZeroMemory(&memory, sizeof(memory));
    session_initialize(&owner, 0x370u);
    if (!session_activate(&owner) || !session_thread_bind(&owner) ||
        !session_guest_memory_begin(&owner, &memory, fixture_read, fixture_write)) return 1;
    base_vdm_local_initialize(&local);
    if (!base_vdm_local_bind(&local, &owner)) return 2;

    ZeroMemory(&command, sizeof(command));
    command.struct_bytes = sizeof(command);
    command.command_owner = BASE_VDM_COMMAND_DOS;
    command.command = (const uint8_t *)"child\0";
    command.command_bytes = 6u;
    if (!base_vdm_local_publish(&local, &command)) return 3;
    ZeroMemory(&delivery, sizeof(delivery));
    delivery.VDMState = ASKING_FOR_DOS_BINARY;
    delivery.CmdLine = (PCHAR)memory.bytes + 0x7000u;
    delivery.CmdSize = 128u;
    if (!GetNextVDMCommand(&delivery) || local.dos_record_state !=
            BASE_VDM_DOS_RECORD_BUSY) return 4;

    memcpy(memory.bytes + 0x1000u, "cmd /c exit 37\r", 15u);
    memcpy(memory.bytes + 0x2000u, "T370=1\0COMSPEC=C:\\CMD.EXE\0\0", 25u);
    memset(memory.bytes + 0x3000u, 0xff, 12u);
    if (!mvdm_command_native_child_capture_guest(0x100u, 0u, 0x200u, 0u,
            0x300u, 0u)) return 5;
    snapshot_command = mvdm_command_native_child_command();
    snapshot_environment = mvdm_command_native_child_environment();
    if (snapshot_command == NULL || snapshot_environment == NULL ||
        strcmp(snapshot_command, "cmd /c exit 37") != 0 ||
        strcmp(snapshot_environment, "T370=1") != 0 ||
        memory.bytes[0x100eu] != 0u ||
        !mvdm_command_native_child_std_handles(handles) ||
        handles[0] != UINT32_MAX || handles[1] != UINT32_MAX ||
        handles[2] != UINT32_MAX) return 6;
    if (!base_vdm_local_native_child_begin()) return 7;

    worker.owner = &owner;
    thread = CreateThread(NULL, 0u, reentry_worker, &worker, 0u, NULL);
    if (thread == NULL) return 8;
    ZeroMemory(&wait_request, sizeof(wait_request));
    wait_request.VDMState = NO_PARENT_TO_WAKE | RETURN_ON_NO_COMMAND;
    if (GetNextVDMCommand(&wait_request) || wait_request.CmdSize != 0u ||
        local.reentry_count != 0u || local.native_child_launch_pending != 0u)
        return 9;
    if (WaitForSingleObject(thread, 1000u) != WAIT_OBJECT_0 ||
        !GetExitCodeThread(thread, &exit_code) || exit_code != 0u) return 10;
    CloseHandle(thread);
    if (mvdm_command_native_child_command() != NULL ||
        mvdm_command_native_child_environment() != NULL) return 11;
    if (!base_vdm_local_unbind(&local) || !session_thread_unbind(&owner) ||
        !session_dispose(&owner)) return 12;
    return 0;
}
