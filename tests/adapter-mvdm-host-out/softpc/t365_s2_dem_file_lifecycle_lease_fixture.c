#include <windows.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "mvdm_guest_location.h"
#include "mvdm_host_identity.h"
#include "session/session.h"

typedef struct fixture_memory {
    uint8_t bytes[0x20000];
} fixture_memory;

static int read_memory(void *context, uint32_t address, uint8_t *bytes,
    uint32_t byte_count)
{
    fixture_memory *memory = (fixture_memory *)context;

    if (memory == NULL || bytes == NULL || address > sizeof(memory->bytes) ||
        byte_count > sizeof(memory->bytes) - address) return 0;
    memcpy(bytes, memory->bytes + address, byte_count);
    return 1;
}

static int write_memory(void *context, uint32_t address,
    uint8_t const *bytes, uint32_t byte_count)
{
    fixture_memory *memory = (fixture_memory *)context;

    if (memory == NULL || bytes == NULL || address > sizeof(memory->bytes) ||
        byte_count > sizeof(memory->bytes) - address) return 0;
    memcpy(memory->bytes + address, bytes, byte_count);
    return 1;
}

int main(void)
{
    static uint8_t const payload[] = { 'C', 'O', 'M', 0x1au };
    CHAR temporaryDirectory[MAX_PATH];
    CHAR temporaryPath[MAX_PATH];
    CHAR pathCopy[MAX_PATH];
    HANDLE file = INVALID_HANDLE_VALUE;
    fixture_memory memory;
    session instance;
    mvdm_guest_location pathLocation;
    mvdm_guest_location bufferLocation;
    mvdm_guest_location invalidLocation;
    mvdm_guest_location_lease lease;
    uint16_t handleHigh;
    uint16_t handleLow;
    uintptr_t nativeFile;
    uint32_t copiedBytes;
    DWORD bytesWritten;
    DWORD bytesRead;
    uint32_t identity;
    int result = 1;

    memset(&memory, 0, sizeof(memory));
    memset(temporaryPath, 0, sizeof(temporaryPath));
    if (GetTempPathA((DWORD)sizeof(temporaryDirectory), temporaryDirectory) ==
        0u || GetTempFileNameA(temporaryDirectory, "t65", 0u,
            temporaryPath) == 0u) goto done;
    file = CreateFileA(temporaryPath, GENERIC_READ | GENERIC_WRITE, 0, NULL,
        CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, NULL);
    if (file == INVALID_HANDLE_VALUE || !WriteFile(file, payload,
        (DWORD)sizeof(payload), &bytesWritten, NULL) || bytesWritten !=
        sizeof(payload) || SetFilePointer(file, 0, NULL, FILE_BEGIN) ==
        INVALID_SET_FILE_POINTER) goto done;

    session_initialize(&instance, 3652u);
    if (!session_activate(&instance) || !session_guest_memory_begin(&instance,
        &memory, read_memory, write_memory) || !session_thread_bind(&instance))
        goto done;

    memcpy(memory.bytes + 0x1200u, temporaryPath, strlen(temporaryPath) + 1u);
    if (!mvdm_guest_location_set_real_mode(&pathLocation, 0x0120u, 0u) ||
        !mvdm_guest_location_copy_c_string(&pathLocation,
            (uint8_t *)pathCopy, MAX_PATH, &copiedBytes) ||
        copiedBytes != strlen(temporaryPath) + 1u ||
        strcmp(pathCopy, temporaryPath) != 0) {
        result = 2;
        goto dispose_session;
    }

    if (!mvdm_host_identity_publish_words((uintptr_t)file, &handleHigh,
        &handleLow) || (identity = ((uint32_t)handleHigh << 16u) |
        handleLow) == 0u ||
        (nativeFile = mvdm_host_identity_resolve_words(handleHigh, handleLow))
        != (uintptr_t)file) {
        result = 3;
        goto dispose_session;
    }

    if (!mvdm_guest_location_set_real_mode(&bufferLocation, 0x0100u, 0u) ||
        !mvdm_guest_location_acquire(&bufferLocation, (uint32_t)sizeof(payload),
            GUEST_MEMORY_ACCESS_WRITE, &lease) || !ReadFile((HANDLE)nativeFile,
            lease.bytes, (DWORD)sizeof(payload), &bytesRead, NULL) ||
        bytesRead != sizeof(payload) || !mvdm_guest_location_release(&lease, 1) ||
        memcmp(memory.bytes + 0x1000u, payload, sizeof(payload)) != 0) {
        result = 4;
        goto release_identity;
    }

    if (!mvdm_guest_location_set_real_mode(&invalidLocation, 0xffffu, 0xffffu)
        || mvdm_guest_location_acquire(&invalidLocation, 2u,
            GUEST_MEMORY_ACCESS_WRITE, &lease) ||
        memcmp(memory.bytes + 0x1000u, payload, sizeof(payload)) != 0) {
        result = 5;
        goto release_identity;
    }
    result = 0;

release_identity:
    if (!CloseHandle(file) && result == 0) result = 6;
    file = INVALID_HANDLE_VALUE;
    if (!mvdm_host_identity_release(identity) && result == 0) result = 7;
dispose_session:
    if (!session_thread_unbind(&instance) && result == 0) result = 8;
    session_guest_memory_end(&instance);
    if (!session_dispose(&instance) && result == 0) result = 9;
done:
    if (file != INVALID_HANDLE_VALUE) CloseHandle(file);
    if (temporaryPath[0] != 0) DeleteFileA(temporaryPath);
    return result;
}
