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

static int alias_contract(void);

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
    return alias_contract();
}

static int unchanged(uint32_t address)
{
    uint32_t output = 0xdeadbeefu;
    SetLastError(0x74478301u);errno = EDOM;
    CHECK(!mvdm_softpc_physical_mapping_translate(address, &output));
    CHECK(output == address && GetLastError() == 0x74478301u && errno == EDOM);
    CHECK(!mvdm_softpc_physical_mapping_translate(address, NULL));
    CHECK(GetLastError() == 0x74478301u && errno == EDOM);
    return 0;
}

static int alias_contract(void)
{
    const uint32_t probes[] = {0u, 0xfffffu, 0x100000u, 0x101fffu,
        0x102000u, 0x7fffffffu, UINT32_MAX};
    session first, second;
    uint32_t output, cleanup_count, i;
    for (i = 0; i < sizeof(probes) / sizeof(probes[0]); ++i)
        CHECK(unchanged(probes[i]) == 0); /* Empty and unbound. */
    session_initialize(&first, 44783u);session_initialize(&second, 44784u);
    CHECK(session_activate(&first) && session_activate(&second));
    CHECK(session_thread_bind(&first));
    for (i = 0; i < sizeof(probes) / sizeof(probes[0]); ++i)
        CHECK(unchanged(probes[i]) == 0);
    cleanup_count = first.host_context.cleanup_count;
    first.host_context.cleanup_count = COMPAT_HOST_CLEANUP_LIMIT + 1u;
    CHECK(unchanged(0x100000u) == 0);
    first.host_context.cleanup_count = cleanup_count;
    CHECK(VdmMapDosMemory(0x100u, 0x200u, 2u) == 0);
    CHECK(unchanged(0xfffffu) == 0 && unchanged(0x102000u) == 0);
    CHECK(mvdm_softpc_physical_mapping_translate(0x100000u, &output) && output == 0x200000u);
    CHECK(mvdm_softpc_physical_mapping_translate(0x101fffu, &output) && output == 0x201fffu);
    cleanup_count = first.host_context.cleanup_count;
    first.host_context.cleanup_count = COMPAT_HOST_CLEANUP_LIMIT + 1u;
    CHECK(unchanged(0x100000u) == 0);
    first.host_context.cleanup_count = cleanup_count;
    CHECK(session_thread_unbind(&first));
    CHECK(unchanged(0x100000u) == 0); /* Nonempty, no bound owner. */
    CHECK(session_thread_bind(&second));
    CHECK(unchanged(0x100000u) == 0); /* Nonempty, different owner. */
    CHECK(VdmMapDosMemory(0x500u, 0x600u, 1u) == 0);
    CHECK(mvdm_softpc_physical_mapping_translate(0x50000fu, &output) && output == 0x60000fu);
    CHECK(session_thread_unbind(&second) && session_thread_bind(&first));
    CHECK(unchanged(0x50000fu) == 0);
    CHECK(VdmMapDosMemory(0x100u, 0x300u, 2u) == 0);
    CHECK(mvdm_softpc_physical_mapping_translate(0x101fffu, &output) && output == 0x301fffu);
    CHECK(session_thread_unbind(&first) && session_thread_bind(&second));
    CHECK(session_dispose(&first)); /* Drops only first's pending alias. */
    CHECK(unchanged(0x100000u) == 0);
    CHECK(mvdm_softpc_physical_mapping_translate(0x50000fu, &output) && output == 0x60000fu);
    CHECK(VdmUnmapDosMemory(0x500u, 1u) == 0);
    CHECK(VdmUnmapDosMemory(0x500u, 1u) == (int32_t)0xc0000225u);
    CHECK(unchanged(0x50000fu) == 0);
    CHECK(session_thread_unbind(&second) && session_dispose(&second));
    CHECK(unchanged(UINT32_MAX) == 0);
    puts("PHYSICAL_ALIAS_EMPTY_CONTRACT errors=0 empty=1 null=1 invalid_owner=1 bounds=1 multi_owner=1 remap=1 teardown=1 physical_retained=1");
    return 0;
}
