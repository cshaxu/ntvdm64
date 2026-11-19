#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <windows.h>
#include "ntvdm-exe/session/session.h"
#include "compat/include/physical_mapping_host.h"

/* Adapter-only contract test, not a guest/CCPU or WOW acceptance workload. */
#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "failed line %d: %s\n", __LINE__, #condition); return 1; \
} } while (0)

int main(int argc, char **argv)
{
    session owner;
    session other;
    uint8_t *bytes = (uint8_t *)VirtualAlloc(NULL, 8192,
        MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    uint8_t *normal = (uint8_t *)VirtualAlloc(NULL, 0x800000,
        MEM_RESERVE, PAGE_READWRITE);
    uint8_t *resolved;
    uint32_t translated;
    unsigned i;
    HANDLE locked_log = INVALID_HANDLE_VALUE;

    (void)argv;
    if (argc > 1) {
        FILETIME created, exited, kernel, user;
        char path[MAX_PATH];
        CHECK(GetProcessTimes(GetCurrentProcess(), &created, &exited, &kernel, &user));
        (void)snprintf(path, sizeof(path),
            "O:\\ntvdm64\\logs\\physical-mapping-%lu-%08lx%08lx.log",
            GetCurrentProcessId(), created.dwHighDateTime, created.dwLowDateTime);
        locked_log = CreateFileA(path, GENERIC_READ | GENERIC_WRITE, 0, NULL,
            CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
        CHECK(locked_log != INVALID_HANDLE_VALUE);
    }

    SetLastError(12345);
    errno = EDOM;
    CHECK(VdmMapDosMemory(0x100, 0x200, 1) == (int32_t)0xc000000d);
    CHECK(GetLastError() == 12345 && errno == EDOM);
    session_initialize(&owner, 406);
    CHECK(session_activate(&owner) && session_thread_bind(&owner));
    CHECK(bytes != NULL && normal != NULL);
    CHECK(!mvdm_softpc_physical_mapping_initialize(normal, 1));
    CHECK(mvdm_softpc_physical_mapping_initialize(normal, 0x800000));
    CHECK(!mvdm_softpc_physical_mapping_initialize(normal, 0x800000));
    CHECK(VdmMapDosMemory(0x100, 0x200, 1) == 0);
    /* An alias-only instance has its own cleanup even before page storage is
     * initialized. Switching back must recover the first instance unchanged. */
    session_initialize(&other, 407u);
    CHECK(session_activate(&other));
    CHECK(session_thread_unbind(&owner) && session_thread_bind(&other));
    CHECK(!mvdm_softpc_physical_mapping_translate(0x100005, &translated));
    CHECK(translated == 0x100005);
    CHECK(VdmMapDosMemory(0x100, 0x500, 1) == 0);
    CHECK(other.host_context.cleanup_count == 1u);
    CHECK(mvdm_softpc_physical_mapping_translate(0x100005, &translated));
    CHECK(translated == 0x500005);
    CHECK(session_thread_unbind(&other) && session_thread_bind(&owner));
    CHECK(session_dispose(&other));
    CHECK(owner.host_context.cleanup_count == 1u);
    CHECK(mvdm_softpc_physical_mapping_translate(0x100005, &translated));
    CHECK(translated == 0x200005);
    CHECK(VdmMapDosMemory(0x100, 0x300, 1) == 0);
    CHECK(mvdm_softpc_physical_mapping_translate(0x100005, &translated));
    CHECK(translated == 0x300005);
    CHECK(VdmUnmapDosMemory(0x100, 1) == 0);
    CHECK(VdmUnmapDosMemory(0x100, 1) == (int32_t)0xc0000225);
    CHECK(!mvdm_softpc_physical_mapping_translate(0x100005, &translated));
    CHECK(translated == 0x100005);
    SetLastError(12345);
    errno = EDOM;
    VdmSetPhysRecStructs((uint32_t)(uintptr_t)bytes, 0x400000, 8192);
    CHECK(GetLastError() == 12345 && errno == EDOM);
    CHECK(mvdm_softpc_physical_mapping_resolve(0x400001, &resolved));
    CHECK(resolved == bytes + 1);
    *resolved = 0x5a;
    CHECK(bytes[1] == 0x5a);
    CHECK(mvdm_softpc_physical_mapping_resolve(0x401fff, &resolved));
    CHECK(resolved == bytes + 8191);
    CHECK(!mvdm_softpc_physical_mapping_resolve(0x402000, &resolved));
    for (i = 0; i < 10000; ++i)
        CHECK(mvdm_softpc_physical_mapping_resolve(0x400001, &resolved));
    VdmSetPhysRecStructs((uint32_t)(uintptr_t)(normal + 0x400000), 0x400000, 8192);
    CHECK(!mvdm_softpc_physical_mapping_resolve(0x400001, &resolved));
    CHECK(!mvdm_softpc_physical_mapping_resolve(0x401fff, &resolved));
    CHECK(session_thread_unbind(&owner));
    CHECK(session_dispose(&owner));
    /* Reusing the same owner address must not recover old aliases or pages. */
    session_initialize(&owner, 408u);
    CHECK(session_activate(&owner) && session_thread_bind(&owner));
    CHECK(!mvdm_softpc_physical_mapping_translate(0x100005, &translated));
    CHECK(!mvdm_softpc_physical_mapping_resolve(0x400001, &resolved));
    session_complete(&owner, 0u);
    CHECK(VdmMapDosMemory(0x100, 0x200, 1) == (int32_t)0xc0000001u);
    CHECK(!mvdm_softpc_physical_mapping_initialize(normal, 0x800000));
    CHECK(!mvdm_softpc_physical_mapping_translate(0x100005, &translated));
    CHECK(session_thread_unbind(&owner) && session_dispose(&owner));
    CHECK(VirtualFree(bytes, 0, MEM_RELEASE));
    CHECK(VirtualFree(normal, 0, MEM_RELEASE));
    if (locked_log != INVALID_HANDLE_VALUE) {
        CHECK(GetFileSize(locked_log, NULL) == 0);
        CloseHandle(locked_log);
    }
    puts("physical page binding and observation test passed");
    return 0;
}
