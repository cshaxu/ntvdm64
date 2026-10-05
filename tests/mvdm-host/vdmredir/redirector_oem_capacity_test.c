#include "mvdm_redirector_guest_copy.h"
#include "ntvdm-exe/session/session.h"
#include "ntvdm-exe/softpc/include/mvdm_guest_location.h"
#include <lmerr.h>
#include <stdio.h>
#include <string.h>
#undef WideCharToMultiByte

static uint8_t memory[2048];
static unsigned assertions, failures, writes;
static void check(int ok, char const *name)
{
    ++assertions; if (!ok) ++failures;
    printf("%s %s\n", ok ? "PASS" : "FAIL", name);
}
int WINAPI fixture_oem_conversion(UINT cp, DWORD flags, LPCWCH text, int chars,
    LPSTR bytes, int capacity, LPCCH replacement, LPBOOL replaced)
{
    return WideCharToMultiByte(cp == CP_OEMCP ? 932 : cp, flags, text, chars,
        bytes, capacity, replacement, replaced);
}
static int read_memory(void *context, uint32_t address, uint8_t *bytes,
    uint32_t count)
{
    (void)context;
    if (address > sizeof(memory) || count > sizeof(memory) - address) return 0;
    memcpy(bytes, memory + address, count); return 1;
}
static int write_memory(void *context, uint32_t address, uint8_t const *bytes,
    uint32_t count)
{
    (void)context;
    if (address > sizeof(memory) || count > sizeof(memory) - address) return 0;
    ++writes; memcpy(memory + address, bytes, count); return 1;
}
/* Controlled real-mode addressing boundary, not a test of the production
 * CCPU protected-mode selector resolver. All copy/lease logic remains real. */
int mvdm_redirector_worker_copy_to(uint16_t segment, uint16_t offset,
    uint8_t const *bytes, uint32_t count)
{
    mvdm_guest_location location;
    return mvdm_guest_location_set_real_mode(&location, segment, offset) &&
        mvdm_guest_location_copy_to_guest(&location, bytes, count);
}
int mvdm_redirector_worker_copy_from(uint16_t segment, uint16_t offset,
    uint8_t *bytes, uint32_t count)
{
    mvdm_guest_location location;
    mvdm_guest_location_lease lease;
    if (!mvdm_guest_location_set_real_mode(&location, segment, offset) ||
        !mvdm_guest_location_acquire(&location, count, GUEST_MEMORY_ACCESS_READ,
            &lease)) return 0;
    memcpy(bytes, lease.bytes, count);
    return mvdm_guest_location_release(&lease, 0);
}
static void put32(unsigned address, uint32_t value)
{
    memory[address] = (uint8_t)value;
    memory[address+1] = (uint8_t)(value >> 8);
    memory[address+2] = (uint8_t)(value >> 16);
    memory[address+3] = (uint8_t)(value >> 24);
}
int main(void)
{
    session s;
    char encoded[8];
    int count;
    session_initialize(&s, 1);
    if (!session_activate(&s) || !session_guest_memory_begin(&s, NULL,
        read_memory, write_memory) || !session_thread_bind(&s)) return 2;
    memset(memory, 0xa5, sizeof(memory));
    count = WideCharToMultiByte(932, 0, L"\x65e5", -1, encoded, sizeof(encoded),
        NULL, NULL);
    check(count == 3, "real DBCS character needs two bytes plus NUL");
    check(mvdm_redirector_copy_wide_to_guest(0, 0x40, L"\x65e5", 2) ==
        NERR_BufTooSmall && writes == 0 && memory[0x40] == 0xa5,
        "DBCS byte capacity rejects without writing");
    check(mvdm_redirector_copy_wide_to_guest(0, 0x40, L"\x65e5", 3) == 0 &&
        !memcmp(memory + 0x40, encoded, 3) && memory[0x43] == 0xa5,
        "exact DBCS capacity includes NUL and keeps sentinel");
    check(mvdm_redirector_copy_wide_to_guest(0, 0x50, L"user", 4) ==
        NERR_BufTooSmall && memory[0x50] == 0xa5, "ASCII capacity includes NUL");
    check(mvdm_redirector_copy_wide_to_guest(0, 0x50, L"user", 5) == 0 &&
        !memcmp(memory + 0x50, "user", 5), "ASCII exact capacity");
    check(mvdm_redirector_copy_wide_to_guest(0, 0x60, L"", 0) ==
        NERR_BufTooSmall && memory[0x60] == 0xa5, "zero capacity rejects even NUL");
    check(mvdm_redirector_copy_wide_to_guest(0, 0x60, L"", 1) == 0 &&
        memory[0x60] == 0, "empty string fits one byte");
    check(mvdm_redirector_copy_wide_to_guest(0, 0x70, L"user", UINT32_MAX) == 0,
        "caller-selected original unchecked capacity retained");
    check(mvdm_redirector_copy_wide_to_guest(0, 2047, L"user", 5) ==
        ERROR_INVALID_ADDRESS, "out-of-memory-span copy fails");
    check(mvdm_redirector_copy_wide_to_guest(0, 0x80, NULL, 5) ==
        ERROR_INVALID_ADDRESS, "null source fails");
    put32(0x100, 0x200); put32(0x104, 0); put32(0x108, 0x220);
    check(mvdm_redirector_write_cd_names(0, 0x100, "HOST", "DOMAIN", NULL) &&
        !strcmp((char *)memory + 0x200, "HOST") && memory[0x220] == 0,
        "CD null far target skipped and null name clears only");
    put32(0x100, 0x300); put32(0x104, 0x301); put32(0x108, 0);
    check(mvdm_redirector_write_cd_names(0, 0x100, "ABC", "XY", NULL) &&
        !memcmp(memory + 0x300, "AXY", 4), "CD overlapping strings use original order");
    put32(0x100, 0x104); put32(0x104, 0x420); put32(0x108, 0);
    check(mvdm_redirector_write_cd_names(0, 0x100, "", "DOM", NULL) &&
        !strcmp((char *)memory + 0x400, "DOM"),
        "CD destination alias preserves sequential far-field read");
    session_guest_memory_end(&s);
    check(mvdm_redirector_copy_wide_to_guest(0, 0x70, L"user", 5) ==
        ERROR_INVALID_ADDRESS, "dead guest memory fails");
    check(session_thread_unbind(&s) && session_dispose(&s), "session cleanup");
    printf("assertions=%u failures=%u\n", assertions, failures);
    return failures ? 1 : 0;
}
