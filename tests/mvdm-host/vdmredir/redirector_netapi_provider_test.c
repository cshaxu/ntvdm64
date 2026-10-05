#include <windows.h>
#include <lm.h>
#include "mvdm_redirector_guest_copy.h"
#include "ntvdm-exe/session/session.h"
#include "ntvdm-exe/softpc/include/mvdm_guest_location.h"
#include <stdio.h>
#include <string.h>
#undef WideCharToMultiByte

void VrGetUserName(void);
void VrGetCDNames(void);
static uint8_t memory[2048];
static USHORT ax, bx, cx, di;
static ULONG carry;
static unsigned freed, assertions, failures;
static NET_API_STATUS api_error;
static WKSTA_USER_INFO_0 user0;
static WKSTA_USER_INFO_1 user1;
static WKSTA_INFO_100 wksta;
USHORT __cdecl getAX(void) { return ax; }
USHORT __cdecl getBX(void) { return bx; }
USHORT __cdecl getCX(void) { return cx; }
USHORT __cdecl getES(void) { return 0; }
USHORT __cdecl getDI(void) { return di; }
void __cdecl setAX(USHORT value) { ax = value; }
void __cdecl setCF(ULONG value) { carry = value; }
NET_API_STATUS WINAPI fixture_user_info(LPCWSTR server, DWORD level, LPBYTE *p)
{
    (void)server;
    if (api_error) return api_error;
    *p = level == 0 ? (LPBYTE)&user0 : (LPBYTE)&user1;
    return 0;
}
NET_API_STATUS WINAPI fixture_wksta_info(LPCWSTR server, DWORD level, LPBYTE *p)
{
    (void)server; (void)level;
    if (api_error) return api_error;
    *p = (LPBYTE)&wksta; return 0;
}
NET_API_STATUS WINAPI fixture_net_free(LPVOID p)
{
    if (p == &user0 || p == &user1 || p == &wksta) ++freed;
    else ++failures;
    return 0;
}
int WINAPI fixture_oem_conversion(UINT cp, DWORD flags, LPCWCH text, int chars,
    LPSTR bytes, int capacity, LPCCH replacement, LPBOOL replaced)
{
    return WideCharToMultiByte(cp == CP_OEMCP ? 932 : cp, flags, text, chars,
        bytes, capacity, replacement, replaced);
}
static int read_memory(void *context, uint32_t address, uint8_t *bytes, uint32_t n)
{
    (void)context;
    if (address > sizeof(memory) || n > sizeof(memory) - address) return 0;
    memcpy(bytes, memory + address, n); return 1;
}
static int write_memory(void *context, uint32_t address, uint8_t const *bytes,
    uint32_t n)
{
    (void)context;
    if (address > sizeof(memory) || n > sizeof(memory) - address) return 0;
    memcpy(memory + address, bytes, n); return 1;
}
int mvdm_redirector_worker_copy_to(uint16_t segment, uint16_t offset,
    uint8_t const *bytes, uint32_t n)
{
    mvdm_guest_location location;
    return mvdm_guest_location_set_real_mode(&location, segment, offset) &&
        mvdm_guest_location_copy_to_guest(&location, bytes, n);
}
int mvdm_redirector_worker_copy_from(uint16_t segment, uint16_t offset,
    uint8_t *bytes, uint32_t n)
{
    mvdm_guest_location location;
    mvdm_guest_location_lease lease;
    if (!mvdm_guest_location_set_real_mode(&location, segment, offset) ||
        !mvdm_guest_location_acquire(&location, n, GUEST_MEMORY_ACCESS_READ,
            &lease)) return 0;
    memcpy(bytes, lease.bytes, n);
    return mvdm_guest_location_release(&lease, 0);
}
static void check(int ok, char const *name)
{
    ++assertions; if (!ok) ++failures;
    printf("%s %s\n", ok ? "PASS" : "FAIL", name);
}
static void init(unsigned capacity)
{
    memset(memory, 0xa5, sizeof(memory));
    bx = 1; cx = (USHORT)capacity; di = 0x40;
    ax = 0x1234; carry = 1; freed = 0; api_error = 0;
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
    session_initialize(&s, 1);
    if (!session_activate(&s) || !session_guest_memory_begin(&s, NULL,
        read_memory, write_memory) || !session_thread_bind(&s)) return 2;
    user0.wkui0_username = L"\x65e5";
    init(2); VrGetUserName();
    check(ax == NERR_BufTooSmall && carry == 1 && memory[0x40] == 0xa5 &&
        freed == 1, "actual VrGetUserName DBCS bound and free-once");
    init(3); VrGetUserName();
    check(ax == 0 && carry == 0 && memory[0x40] == 0x93 &&
        memory[0x41] == 0xfa && memory[0x42] == 0 && memory[0x43] == 0xa5 &&
        freed == 1, "actual VrGetUserName DBCS exact capacity");
    user0.wkui0_username = L"user";
    init(1); VrGetUserName();
    check(ax == NERR_BufTooSmall && memory[0x40] == 0xa5 && freed == 1,
        "actual original character gate retained");
    init(0); VrGetUserName();
    check(ax == 0 && carry == 0 && !memcmp(memory + 0x40, "user", 5) &&
        freed == 1, "RETAINED_LIMIT original CX0 underflow is not repaired");
    init(1); bx = 0; VrGetUserName();
    check(ax == 0x1234 && carry == 1 && !memcmp(memory + 0x40, "user", 5) &&
        freed == 1, "actual BX0 unchecked copy and registers retained");
    init(5); di = 2047; VrGetUserName();
    check(ax == ERROR_INVALID_ADDRESS && carry == 1 && freed == 1,
        "actual invalid destination reports failure and frees");
    init(5); api_error = ERROR_ACCESS_DENIED; VrGetUserName();
    check(ax == ERROR_ACCESS_DENIED && carry == 1 && freed == 0,
        "actual NetAPI failure does not free absent allocation");
    init(5); di = 0x100;
    wksta.wki100_computername = L"HOST"; wksta.wki100_langroup = L"DOMAIN";
    user1.wkui1_logon_domain = L"LOGON";
    put32(0x100, 0x200); put32(0x104, 0x220); put32(0x108, 0x240);
    VrGetCDNames();
    check(!strcmp((char *)memory + 0x200, "HOST") &&
        !strcmp((char *)memory + 0x220, "DOMAIN") &&
        !strcmp((char *)memory + 0x240, "LOGON") && ax == 0x1234 &&
        carry == 1 && freed == 2, "actual CDNames copies and unchanged status ABI");
    init(5); di = 0x100;
    put32(0x100, 2047); put32(0x104, 0); put32(0x108, 0);
    VrGetCDNames();
    check(ax == 0x1234 && carry == 1 && freed == 2,
        "actual CDNames lease failure still frees, no invented error ABI");
    session_guest_memory_end(&s);
    check(session_thread_unbind(&s) && session_dispose(&s), "session cleanup");
    printf("assertions=%u failures=%u\n", assertions, failures);
    return failures ? 1 : 0;
}
