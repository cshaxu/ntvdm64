#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <windows.h>
#include "session/session.h"
#include "mvdm_softpc_physical_mapping.h"

/* Adapter-only contract test, not a guest/CCPU or WOW acceptance workload. */
#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "failed line %d: %s\n", __LINE__, #condition); return 1; \
} } while (0)

int main(int argc, char **argv)
{
    session owner;
    uint8_t bytes[32];
    uint8_t *resolved;
    uint32_t identifier, alignment, translated;
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
    CHECK(!mvdm_softpc_physical_mapping_prepare(0x1234, 16, &alignment));
    CHECK(VdmMapDosMemory(0x100, 0x200, 1) == 0);
    CHECK(mvdm_softpc_physical_mapping_translate(0x100005, &translated));
    CHECK(translated == 0x200005);
    CHECK(VdmMapDosMemory(0x100, 0x300, 1) == 0);
    CHECK(mvdm_softpc_physical_mapping_translate(0x100005, &translated));
    CHECK(translated == 0x300005);
    CHECK(VdmUnmapDosMemory(0x100, 1) == 0);
    CHECK(VdmUnmapDosMemory(0x100, 1) == (int32_t)0xc0000225);
    CHECK(!mvdm_softpc_physical_mapping_translate(0x100005, &translated));
    CHECK(translated == 0x100005);
    CHECK(mvdm_softpc_physical_mapping_publish(bytes + 1, 16, &identifier));
    CHECK(mvdm_softpc_physical_mapping_prepare(identifier, 16, &alignment));
    SetLastError(12345);
    errno = EDOM;
    VdmSetPhysRecStructs(identifier, 0x400000, 4096);
    CHECK(GetLastError() == 12345 && errno == EDOM);
    CHECK(mvdm_softpc_physical_mapping_resolve(0x400000 + alignment, &resolved));
    CHECK(resolved == bytes + 1);
    *resolved = 0x5a;
    CHECK(bytes[1] == 0x5a);
    for (i = 0; i < 10000; ++i)
        CHECK(mvdm_softpc_physical_mapping_resolve(0x400000 + alignment, &resolved));
    VdmSetPhysRecStructs(0, 0x400000, 4096);
    CHECK(!mvdm_softpc_physical_mapping_resolve(0x400000 + alignment, &resolved));
    VdmSetPhysRecStructs(0, 0x400000, 4096);
    CHECK(session_thread_unbind(&owner));
    CHECK(session_dispose(&owner));
    if (locked_log != INVALID_HANDLE_VALUE) {
        CHECK(GetFileSize(locked_log, NULL) == 0);
        CloseHandle(locked_log);
    }
    puts("physical mapping unchanged-contract observation test passed");
    return 0;
}
