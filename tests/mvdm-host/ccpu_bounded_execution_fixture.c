#include <stdint.h>
#include <stdio.h>
#include <setjmp.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>

#include "ntvdm-exe/softpc/include/mvdm_softpc_execution.h"
#include "ntvdm-exe/softpc/include/mvdm_a20.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_effective_address.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_physical_mapping.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_guest_memory.h"
#include "insignia.h"
#include "host_def.h"
#include "xt.h"
#include "ios.h"
#include "dma.h"
#include "ica.h"
#include "sas.h"
#include "c_page.h"
#include "c_tlb.h"

static LONG WINAPI fixture_unhandled_exception(EXCEPTION_POINTERS *exception)
{
    uintptr_t address = (uintptr_t)exception->ExceptionRecord->ExceptionAddress;
    uintptr_t image = (uintptr_t)GetModuleHandleA(NULL);

    fprintf(stderr, "exception=%08lx address=%p rva=%#llx\n",
        (unsigned long)exception->ExceptionRecord->ExceptionCode,
        exception->ExceptionRecord->ExceptionAddress,
        (unsigned long long)(address - image));
    return EXCEPTION_EXECUTE_HANDLER;
}

static int fixture_breakpoint_filter(DWORD exception_code)
{
    return exception_code == EXCEPTION_BREAKPOINT ? EXCEPTION_EXECUTE_HANDLER :
        EXCEPTION_CONTINUE_SEARCH;
}

/* These are the original CCPU/SoftPC public spellings.  The fixture does not
 * provide an executor, memory implementation, BOP handler, or stop hook. */
extern void sas_init(PHY_ADDR size);
extern void sas_term(void);
extern void c_sas_store(IU32 address, IU8 value);
extern IU8 c_sas_hw_at(IU32 address);
extern IU16 c_sas_w_at(IU32 address);
extern IU32 c_sas_dw_at(IU32 address);
extern void c_sas_storedw(IU32 address, IU32 value);
extern void c_sas_loads(IU32 source, IU8 *destination, IU32 length);
extern void c_sas_stores(IU32 destination, IU8 *source, IU32 length);
extern void c_sas_move_bytes_forward(IU32 source, IU32 destination,
    IU32 length);
extern void c_sas_fills(IU32 destination, IU8 value, IU32 length);
extern void c_sas_fillsw(IU32 destination, IU16 value, IU32 length);
extern IU32 c_sas_memory_size(void);
extern void c_sas_connect_memory(IU32 low, IU32 high, SAS_MEM_TYPE type);
extern SAS_MEM_TYPE c_sas_memory_type(IU32 address);
extern IU8 *c_GetPhyAdd(IU32 address);
extern void c_cpu_init(void);
extern uint16_t c_getIP(void);
extern void c_setIP(uint16_t value);
extern uintptr_t c_setDS(uint16_t value);
extern uint16_t c_getAX(void);
extern void c_setAX(uint16_t value);
extern int32_t c_getCF(void);
extern void c_setCF(int32_t value);
extern void load_sw_cpu_access_functions(void);
extern void (*host_simulate_func)(void);
extern uint16_t (*getAX_func)(void);
extern void (*setAX_func)(uint16_t value);
extern int32_t (*getCF_func)(void);
extern void (*setCF_func)(int32_t value);
extern void c_cpu_simulate(void);
extern void EDL_fast_bop(ULONG immed);
extern struct SasVector cSasPtrs;
extern void host_start_cpu(void);
extern void host_simulate(void);
extern jmp_buf *ccpu386ThrdExptnPtr(void);
extern NTSTATUS VdmAddVirtualMemory(ULONG host_address, ULONG size,
    PULONG intel_address);
extern NTSTATUS VdmRemoveVirtualMemory(ULONG intel_address);
extern unsigned fixture_eoi_hook_calls;
extern unsigned fixture_wow_idle_calls;
extern unsigned fixture_host_close_calls;

extern IU32 c_getCR0(void);
extern IU32 c_getCR3(void);
extern void c_setCR0(IU32 value);
extern void c_setCR3(IU32 value);
extern ISM32 c_getCPL(void);
extern void c_setCPL(ISM32 value);
extern IU32 c_getEAX(void);
extern IU32 c_getGDT_BASE(void);
extern IU16 c_getGDT_LIMIT(void);
extern void c_setGDT_BASE_LIMIT(IU32 base, IU16 limit);
extern IU16 c_getCS(void);
extern IU16 c_getDS(void);
extern ISM32 c_setCS(IU16 value);
extern IU16 c_getMSW(void);
extern void c_setMSW(IU16 value);
extern void phy_w32(IU32 address, IU32 value);
extern NTSTATUS VdmAllocateVirtualMemory(PULONG address, ULONG size, BOOL commit);
extern NTSTATUS VdmFreeVirtualMemory(ULONG address);
extern IU16 c_getSS(void), c_getFS(void), c_getSP(void);
extern ISM32 c_setSS(IU16 value);
extern IUH c_setFS(IU16 value);
extern void c_setSP(IU16 value), c_setECX(IU32 value), c_setEDX(IU32 value);

/* Execute the unchanged 127-byte HMValidateHandle body from hash-verified
 * USER.EXE. Only its documented runtime shared-data input is initialized.
 * No product/guest image is patched. The runner must pin media provenance. */
static int fixture_original_user_handle(IU32 gdt)
{
    const char *path = getenv("NTVDM_TEST_USER_IMAGE");
    FILE *input;
    IU8 header[64], ne[64], segment[8], code[127];
    IU32 ne_offset, code_offset, i;
    const struct { IU32 handle, type, object, expected; } cases[] = {
        {0x12340001u, 1, 0x40000, 0x3f000},
        {0x56780001u, 1, 0x40000, 0},
        {0x00000001u, 1, 0x40000, 0x3f000},
        {0xffff0001u, 1, 0x40000, 0x3f000},
        {0x12340004u, 1, 0x40000, 0},
        {0x12340001u, 2, 0x40000, 0},
        {0x12340001u, 255, 0x40000, 0x3f000},
        {0x12340001u, 1, 0x3ffff, 0x3ffff},
        {0x12340001u, 1, 0x4ffff, 0x4efff},
        {0x12340001u, 1, 0x50000, 0x50000}
    };
    if (!path) return 1; /* Other packages' fixture runs do not load WOW. */
    input = fopen(path, "rb");
    if (!input) return 0;
    if (fread(header, 1, sizeof(header), input) != sizeof(header)) goto bad;
    memcpy(&ne_offset, header + 0x3c, 4);
    if (fseek(input, ne_offset, SEEK_SET) || fread(ne, 1, sizeof(ne), input) != sizeof(ne) ||
        ne[0] != 'N' || ne[1] != 'E' || ne[0x33] || ne[0x32] > 16) goto bad;
    if (fseek(input, ne_offset + ne[0x22] + ((IU32)ne[0x23] << 8), SEEK_SET) ||
        fread(segment, 1, sizeof(segment), input) != sizeof(segment)) goto bad;
    code_offset = ((IU32)segment[0] | ((IU32)segment[1] << 8)) << ne[0x32];
    if (fseek(input, code_offset + 0x8f8, SEEK_SET) ||
        fread(code, 1, sizeof(code), input) != sizeof(code) ||
        code[0] != 0x66 || code[126] != 0xcb) goto bad;
    fclose(input);
    for (i = 0; i < sizeof(code); ++i) c_sas_store(0x108f8 + i, code[i]);
    c_sas_storedw(0x15da2, 0x31000); /* original wow16gpsi runtime field */
    c_sas_storedw(0x31000, 0x31100);
    c_sas_storedw(0x31004, 0x32000);
    c_sas_storedw(0x31104, 4);
    c_sas_storedw(0x32018, 0x12340001); /* type=1, flags=0, uniqueness=1234 */
    c_sas_storedw(0x30018, 0x30000);
    c_sas_storedw(0x3005c, 0x33000);
    c_sas_storedw(0x30060, 0x1000);
    c_sas_storedw(0x33000, 0x40000);
    c_sas_storedw(0x33004, 0x50000);
    phy_w32(gdt + 0x28, 0x0000ffff); phy_w32(gdt + 0x2c, 0x0000f200);
    phy_w32(gdt + 0x30, 0x0000ffff); phy_w32(gdt + 0x34, 0x0000fa01);
    phy_w32(gdt + 0x38, 0x00000fff); phy_w32(gdt + 0x3c, 0x0000f203);
    c_setGDT_BASE_LIMIT(gdt, 0x3f);
    (void)c_setSS(0x2b); (void)c_setFS(0x3b);
    c_sas_store(0x8800, 0xd6); c_sas_store(0x8801, 0xfe);
    for (i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        c_sas_storedw(0x32010, cases[i].object);
        c_sas_storedw(0x7000, 0x001b8800);
        c_setSP(0x7000); c_setECX(cases[i].handle); c_setEDX(cases[i].type);
        (void)c_setCS(0x33); c_setIP(0x8f8);
        host_simulate();
        if (c_getEAX() != cases[i].expected || c_getIP() != 0x8802 || c_getSP() != 0x7004) {
            fprintf(stderr, "original USER validator case %lu failed eax=%lx expected=%lx\n",
                (unsigned long)i, (unsigned long)c_getEAX(), (unsigned long)cases[i].expected);
            return 0;
        }
    }
    puts("CCPU_ORIGINAL_USER_HANDLE_VALIDATOR_OK cases=10");
    return 1;
bad:
    fclose(input);
    return 0;
}

/* S40 feasibility proof only: original CCPU page walking can expose a high
 * guest-linear page backed by ordinary low physical RAM. This does not install
 * a product mapping or supply USER/TEB data. All page tables/payloads are local
 * fixture data; immutable guest media are neither loaded nor changed. */
static DWORD WINAPI fixture_native_clock_writer(void *context)
{
    LONG *page = (LONG *)context;
    DWORD attempt;
    /* Only access the thread's native-owned allocation. No CPU TLS, SAS,
     * guest lease or callback is used by this asynchronous publisher. */
    for (attempt = 0; attempt < 2000u; ++attempt) {
        if (InterlockedCompareExchange(&page[1024], 0, 0) == 1) {
            InterlockedExchange(&page[0], 0x056789ab);
            return 0;
        }
        Sleep(1);
    }
    return 1;
}

static int fixture_high_linear_page(void)
{
    ULONG allocation = 0;
    IU32 directory, table, first, second, pde, pte;
    const IU32 linear = 0x7ffe0000u;
    const IU32 old_cr0 = c_getCR0();
    const IU32 old_cr3 = c_getCR3();
    const ISM32 old_cpl = c_getCPL();
    const IU32 old_gdt = c_getGDT_BASE();
    const IU16 old_gdt_limit = c_getGDT_LIMIT();
    const IU16 old_cs = c_getCS(), old_ds = c_getDS(), old_ip = c_getIP();
    const IU16 old_ss = c_getSS(), old_fs = c_getFS(), old_sp = c_getSP();
    IU32 index;
    /* Self-authored bounded program using USER16's observed clock-reading
     * instruction forms, ending at CCPU's existing direct unsimulate opcode.
     * It is not a modified USER.EXE or a substitute Win16 acceptance app. */
    const IU8 clock_program[] = {
        0x66,0xba,0x00,0x00,0xfe,0x7f, /* mov edx,7ffe0000h */
        0x67,0x66,0x8b,0x02,           /* mov eax,[edx] */
        0x67,0x66,0xf7,0x62,0x04,      /* mul dword [edx+4] */
        0x66,0x0f,0xac,0xd0,0x18,      /* shrd eax,edx,24 */
        0xd6,0xfe
    };
    IU32 translated = 0;
    IU8 bytes[4] = {0};
    session memory_owner;
    guest_memory_lease *lease = NULL;
    uint8_t *lease_bytes = NULL;
    int memory_bound = 0;
    ULONG external_address = 0;
    LONG *external_page = NULL;
    int external_registered = 0;
    HANDLE publisher = NULL;
    DWORD publisher_result = 1;
    const IU8 updated_tick[4] = {0x78, 0x56, 0x34, 0x02};
    int passed = 0;

    /* SAS reserves extended RAM but does not initially commit it. Use the
     * original allocator/commit contract rather than writing an arbitrary
     * address merely because it is below c_sas_memory_size(). */
    if (VdmAllocateVirtualMemory(&allocation, 6u * 4096u, TRUE) != STATUS_SUCCESS)
        return 0;
    directory = allocation;
    table = allocation + 4096u;
    first = allocation + 8192u;
    second = allocation + 5u * 4096u;
    pde = directory + ((linear >> 22) * 4u);
    pte = table + (((linear >> 12) & 1023u) * 4u);
    session_initialize(&memory_owner, 40u);
    if (!session_activate(&memory_owner) ||
        !session_thread_bind(&memory_owner)) goto done;
    memory_bound = 1;
    if (!mvdm_softpc_guest_memory_begin(&memory_owner)) goto done;
    /* Start with empty tables, then one read-only user page and an adjacent
     * writable user page with deliberately noncontiguous physical backing. */
    c_sas_fills(directory, 0, 6u * 4096u);
    phy_w32(pde, table | 7u);
    phy_w32(pte, first | 5u);
    phy_w32(pte + 4u, second | 7u);
    phy_w32(first, 0x01234567u);
    phy_w32(first + 4u, 0x01000000u);
    phy_w32(first + 4092u, 0x44332211u);
    phy_w32(second, 0x88776655u);
    /* Identity-map the fixture's existing 2 MiB RAM for instruction fetch
     * and descriptor reads; the high page remains a distinct read-only alias. */
    phy_w32(directory, (allocation + 3u * 4096u) | 7u);
    for (index = 0; index < 512u; ++index)
        phy_w32(allocation + 3u * 4096u + index * 4u, (index * 4096u) | 7u);
    phy_w32(allocation + 4u * 4096u + 0x18u, 0x0000ffffu);
    phy_w32(allocation + 4u * 4096u + 0x1cu, 0x0000fa00u); /* 16-bit code */
    phy_w32(allocation + 4u * 4096u + 0x20u, 0x0000ffffu);
    phy_w32(allocation + 4u * 4096u + 0x24u, 0x00cff200u); /* flat data */
    for (index = 0; index < sizeof(clock_program); ++index)
        c_sas_store(0x8000u + index, clock_program[index]);
    c_setCR3(directory);
    c_setCR0(old_cr0 | 0x80000001u);
    c_setCPL(3);

    if (!xtrn2phy(linear, PG_U | PG_R, &translated) || translated != first ||
        xtrn2phy(linear, PG_U | PG_W, &translated) ||
        xtrn2phy(linear + 8192u, PG_U | PG_R, &translated)) {
        fputs("high-page mapping/protection/absence failed\n", stderr);
        goto done;
    }
    /* Exercise both CPU-owned access checking/translation and SAS external
     * scalar/bulk consumers, not a fixture implementation of page walking. */
    if (usr_chk_dword(linear, PG_R) != first ||
        spr_read_dword(linear) != 0x01234567u ||
        c_sas_dw_at(linear + 4u) != 0x01000000u ||
        c_sas_dw_at(linear + 4094u) != 0x66554433u) {
        fputs("high-page scalar/cross-page read failed\n", stderr);
        goto done;
    }
    c_sas_loads(linear + 4094u, bytes, sizeof(bytes));
    if (memcmp(bytes, "\x33\x44\x55\x66", 4u) != 0) {
        fputs("high-page SAS bulk read failed\n", stderr);
        goto done;
    }
    c_setGDT_BASE_LIMIT(allocation + 4u * 4096u, 0x27u);
    (void)c_setCS(0x1bu);
    (void)c_setDS(0x23u);
    c_setIP(0x8000u);
    host_simulate();
    if (c_getIP() != 0x8000u + sizeof(clock_program) ||
        c_getEAX() != 0x01234567u) {
        fputs("high-page original instruction execution failed\n", stderr);
        goto done;
    }
    puts("CCPU_HIGH_LINEAR_CLOCK_INSTRUCTIONS_OK");
    /* A host publisher can use the allocated low canonical address with the
     * existing bounded lease. There is no reason to broaden that API to
     * arbitrary high guest addresses just to maintain this shared clock. */
    if (mvdm_softpc_guest_memory_acquire(linear, 4u,
            GUEST_MEMORY_ACCESS_READ, &lease, &lease_bytes) ||
        lease != NULL || lease_bytes != NULL ||
        !mvdm_softpc_guest_memory_copy_to(first, updated_tick, 4u) ||
        !mvdm_softpc_guest_memory_copy_from(first, bytes, 4u) ||
        memcmp(bytes, updated_tick, 4u) != 0 ||
        c_sas_dw_at(linear) != 0x02345678u) {
        fputs("shared clock low-address lease publication failed\n", stderr);
        goto done;
    }
    c_setIP(0x8000u);
    host_simulate();
    if (c_getEAX() != 0x02345678u ||
        c_getIP() != 0x8000u + sizeof(clock_program)) {
        fputs("guest did not observe the published clock update\n", stderr);
        goto done;
    }
    /* Original MOV_CR rejects PG=1/PE=0. A standalone mode boundary must
     * suspend PG before DOSX clears PE, then restore it after PE is set.
     * This proves only that order, not the product's DOSX frame handling. */
    c_setCR0(c_getCR0() & ~0x80000000u);
    c_setMSW((IU16)(c_getMSW() & ~1u));
    if ((c_getCR0() & 0x80000001u) != 0u ||
        !mvdm_softpc_guest_memory_copy_from(first, bytes, 4u) ||
        memcmp(bytes, updated_tick, 4u) != 0) {
        fputs("low-address lease changed when paging was suspended\n", stderr);
        goto done;
    }
    c_setMSW((IU16)(c_getMSW() | 1u));
    c_setCR3(directory);
    c_setCR0(c_getCR0() | 0x80000000u);
    c_setCPL(3);
    (void)c_setCS(0x1bu);
    (void)c_setDS(0x23u);
    c_setIP(0x8000u);
    host_simulate();
    if (c_getEAX() != 0x02345678u ||
        c_getIP() != 0x8000u + sizeof(clock_program)) {
        fputs("guest clock failed after protected-mode reentry\n", stderr);
        goto done;
    }
    puts("CCPU_SHARED_PAGE_EXISTING_LEASE_AND_MODE_ORDER_OK");
    /* Original nt_mem can register native-owned backing; the publisher need
     * not retain a guest lease or call the CPU from its own thread. This
     * checks the mapping, not asynchronous timer or worker teardown policy. */
    if (!mvdm_softpc_physical_mapping_initialize(c_GetPhyAdd(0),
            c_sas_memory_size())) goto done;
    external_page = (LONG *)VirtualAlloc(NULL, 8192u,
        MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (external_page == NULL) goto done;
    external_page[0] = 0x03456789;
    external_page[1] = 0x01000000;
    if (VdmAddVirtualMemory((ULONG)(uintptr_t)external_page, 8192u,
            &external_address) != STATUS_SUCCESS) goto done;
    external_registered = 1;
    phy_w32(pte, external_address | 5u);
    c_setCR3(directory);
    c_setIP(0x8000u);
    host_simulate();
    if (c_getEAX() != 0x03456789u) goto done;
    InterlockedExchange(&external_page[0], 0x0456789a);
    c_setIP(0x8000u);
    host_simulate();
    if (c_getEAX() != 0x0456789au ||
        xtrn2phy(linear, PG_U | PG_W, &translated)) goto done;
    puts("CCPU_SHARED_PAGE_NATIVE_BACKING_OK");
    {
        /* Announce entry through a separate writable page, then poll the
         * read-only clock without a BOP. The writer cannot publish early. */
        const IU8 poll_program[] = {
            0x66,0xba,0x00,0x00,0xfe,0x7f,
            0x67,0x66,0xc7,0x82,0x00,0x10,0x00,0x00,0x01,0x00,0x00,0x00,
            0x67,0x66,0x8b,0x02,
            0x66,0x3d,0xab,0x89,0x67,0x05,
            0x75,0xf4,
            0xd6,0xfe
        };
        phy_w32(pte + 4u, (external_address + 4096u) | 7u);
        c_setCR3(directory);
        for (index = 0; index < sizeof(poll_program); ++index)
            c_sas_store(0x8100u + index, poll_program[index]);
        publisher = CreateThread(NULL, 0, fixture_native_clock_writer,
            external_page, 0, NULL);
        if (publisher == NULL) goto done;
        c_setIP(0x8100u);
        host_simulate();
        if (WaitForSingleObject(publisher, 5000u) != WAIT_OBJECT_0 ||
            !GetExitCodeThread(publisher, &publisher_result) ||
            publisher_result != 0 || c_getEAX() != 0x056789abu ||
            c_getIP() != 0x8100u + sizeof(poll_program)) goto done;
        puts("CCPU_SHARED_CLOCK_WITHOUT_BOP_PROGRESS_OK");
    }
    /* Changing CR3 is the original TLB invalidation contract. Stale entries
     * must not retain the old object after a remap or unmap. */
    phy_w32(pte, second | 5u);
    c_setCR3(directory);
    if (usr_chk_dword(linear, PG_R) != second ||
        c_sas_dw_at(linear) != 0x88776655u) {
        fputs("high-page remap retained stale backing\n", stderr);
        goto done;
    }
    phy_w32(pte, 0u);
    c_setCR3(directory);
    if (xtrn2phy(linear, PG_U | PG_R, &translated)) {
        fputs("high-page unmap retained stale translation\n", stderr);
        goto done;
    }
    if (!fixture_original_user_handle(allocation + 4u * 4096u)) goto done;
    passed = 1;
done:
    if (publisher != NULL) {
        /* Its bounded wait touches backing until it returns. Join before
         * removing the mapping or freeing that backing, also on failure. */
        WaitForSingleObject(publisher, INFINITE);
        CloseHandle(publisher);
    }
    c_setCR0(old_cr0);
    c_setCR3(old_cr3);
    c_setCPL(old_cpl);
    c_setGDT_BASE_LIMIT(old_gdt, old_gdt_limit);
    (void)c_setCS(old_cs);
    (void)c_setDS(old_ds);
    (void)c_setSS(old_ss);
    (void)c_setFS(old_fs);
    c_setSP(old_sp);
    c_setIP(old_ip);
    if (external_registered &&
        VdmRemoveVirtualMemory(external_address) != STATUS_SUCCESS) passed = 0;
    if (external_page != NULL) VirtualFree(external_page, 0, MEM_RELEASE);
    if (memory_bound) {
        mvdm_softpc_guest_memory_end(&memory_owner);
        if (!session_thread_unbind(&memory_owner) ||
            !session_dispose(&memory_owner)) passed = 0;
    }
    if (VdmFreeVirtualMemory(allocation) != STATUS_SUCCESS) passed = 0;
    if (passed) puts("CCPU_HIGH_LINEAR_PAGE_BOUNDARY_OK");
    return passed;
}

int main(void)
{
    session physical_owner;
    /* The program uses ordinary original CCPU decode and SAS RAM access
     * before its `D6 FE` exit.  `c_main.c` advances IP before calling
     * c_cpu_unsimulate(), which returns through the original CCPU TLS
     * simulation frame. */
    SetUnhandledExceptionFilter(fixture_unhandled_exception);
    /* Before SAS/CCPU initialization there is no TLS simulation context.
     * The original pointer-returning exception-frame API must decline this
     * state with a typed null result, never an undefined native pointer. */
    if (ccpu386ThrdExptnPtr() != NULL) {
        fputs("exception-frame accessor accepted an uninitialized TLS context\n", stderr);
        return 1;
    }
    fputs("sas-init\n", stderr);
    session_initialize(&physical_owner, 2u);
    if (!session_activate(&physical_owner) ||
        !session_thread_bind(&physical_owner)) return 1;
    sas_init(UINT32_C(0x00200000));
    if (Sas.Sas_hw_at != cSasPtrs.Sas_hw_at ||
        Sas.Sas_store != cSasPtrs.Sas_store ||
        Sas.SasPtrToPhysAddrByte != cSasPtrs.SasPtrToPhysAddrByte ||
        Sas.Sas_overwrite_memory != NULL) {
        fputs("selected original SAS vector was not installed as expected\n", stderr);
        sas_term();
        return 1;
    }
    Sas.Sas_store(UINT32_C(0x00000123), UINT8_C(0xa6));
    if (Sas.Sas_hw_at(UINT32_C(0x00000123)) != UINT8_C(0xa6)) {
        fputs("selected original SAS vector did not dispatch RAM access\n", stderr);
        sas_term();
        return 1;
    }
    /* XMS retains the historical SAS A20 surface.  Its adapter only reaches
     * the selected original CCPU/SAS owner; it does not create a second RAM
     * implementation or a private mapping namespace. */
    sas_enable_20_bit_wrapping();
    if (!sas_twenty_bit_wrapping_enabled()) {
        fputs("XMS A20 enable did not reach original SAS state\n", stderr);
        sas_term();
        return 1;
    }
    sas_disable_20_bit_wrapping();
    if (sas_twenty_bit_wrapping_enabled()) {
        fputs("XMS A20 disable did not reach original SAS state\n", stderr);
        sas_term();
        return 1;
    }
    {
        IU8 source_bytes[4] = { UINT8_C(0x19), UINT8_C(0x27),
            UINT8_C(0x35), UINT8_C(0x43) };
        IU8 loaded_bytes[4] = { 0u, 0u, 0u, 0u };

        /* This is the original CCPU SAS RAM algorithm, not a controller,
         * A20, EMS or external physical-page test. */
        if (c_sas_memory_size() != UINT32_C(0x00200000) ||
            c_sas_memory_type(UINT32_C(0x00000123)) != SAS_RAM) {
            fputs("original SAS RAM size/type contract was not installed\n", stderr);
            sas_term();
            return 1;
        }
        c_sas_stores(UINT32_C(0x00000200), source_bytes,
            (IU32)sizeof(source_bytes));
        c_sas_loads(UINT32_C(0x00000200), loaded_bytes,
            (IU32)sizeof(loaded_bytes));
        if (memcmp(source_bytes, loaded_bytes, sizeof(source_bytes)) != 0) {
            fputs("original SAS string load/store contract failed\n", stderr);
            sas_term();
            return 1;
        }
        c_sas_move_bytes_forward(UINT32_C(0x00000200), UINT32_C(0x00000210),
            (IU32)sizeof(source_bytes));
        c_sas_fills(UINT32_C(0x00000220), UINT8_C(0x5c), UINT32_C(3));
        c_sas_fillsw(UINT32_C(0x00000224), UINT16_C(0x9a7e), UINT32_C(2));
        c_sas_storedw(UINT32_C(0x00000230), UINT32_C(0xd4c3b2a1));
        if (c_sas_hw_at(UINT32_C(0x00000210)) != UINT8_C(0x19) ||
            c_sas_hw_at(UINT32_C(0x00000213)) != UINT8_C(0x43) ||
            c_sas_hw_at(UINT32_C(0x00000220)) != UINT8_C(0x5c) ||
            c_sas_hw_at(UINT32_C(0x00000222)) != UINT8_C(0x5c) ||
            c_sas_w_at(UINT32_C(0x00000224)) != UINT16_C(0x9a7e) ||
            c_sas_w_at(UINT32_C(0x00000226)) != UINT16_C(0x9a7e) ||
            c_sas_dw_at(UINT32_C(0x00000230)) != UINT32_C(0xd4c3b2a1)) {
            fputs("original SAS scalar/move/fill RAM contract failed\n", stderr);
            sas_term();
            return 1;
        }
        c_sas_connect_memory(UINT32_C(0x00000300), UINT32_C(0x000003ff),
            SAS_ROM);
        if (c_sas_memory_type(UINT32_C(0x00000300)) != SAS_ROM) {
            fputs("original SAS memory-type transition failed\n", stderr);
            sas_term();
            return 1;
        }
        c_sas_connect_memory(UINT32_C(0x00000300), UINT32_C(0x000003ff),
            SAS_RAM);
    }
    {
        uint8_t *external_page = (uint8_t *)VirtualAlloc(NULL,
            UINT32_C(4096), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
        uint8_t *resolved_page = NULL;
        ULONG intel_address = 0u;

        if (external_page == NULL ||
            VdmAddVirtualMemory((ULONG)(uintptr_t)external_page, UINT32_C(4096),
                &intel_address) != STATUS_SUCCESS) {
            fputs("external physical-page binding setup failed\n", stderr);
            sas_term();
            if (external_page != NULL) (void)VirtualFree(external_page, 0u, MEM_RELEASE);
            return 1;
        }
        if (mvdm_softpc_physical_mapping_resolve(
                UINT32_C(0xffffffff), &resolved_page)) {
            fputs("external physical-page binding accepted invalid span\n", stderr);
            (void)session_thread_unbind(&physical_owner);
            (void)session_dispose(&physical_owner);
            sas_term();
            (void)VirtualFree(external_page, 0u, MEM_RELEASE);
            return 1;
        }
        /* `c_GetPhyAdd` is the selected original CCPU physical-access
         * operation.  `c_sas_store` is a linear BIOS/SAS entry and is not a
         * proof that the external physical page was selected. */
        *c_GetPhyAdd(intel_address) = UINT8_C(0x6d);
        if (VdmMapDosMemory(UINT32_C(0x100), intel_address >> 12,
                UINT32_C(1)) != STATUS_SUCCESS ||
            *c_GetPhyAdd(UINT32_C(0x00100000)) != UINT8_C(0x6d)) {
            fputs("source-shaped DOS physical-page alias was not installed\n",
                stderr);
            (void)session_thread_unbind(&physical_owner);
            (void)session_dispose(&physical_owner);
            sas_term();
            (void)VirtualFree(external_page, 0u, MEM_RELEASE);
            return 1;
        }
        *c_GetPhyAdd(UINT32_C(0x00100000)) = UINT8_C(0x7a);
        if (external_page[0] != UINT8_C(0x7a) ||
            *c_GetPhyAdd(intel_address) != UINT8_C(0x7a) ||
            VdmUnmapDosMemory(UINT32_C(0x100), UINT32_C(1)) !=
                STATUS_SUCCESS ||
            VdmUnmapDosMemory(UINT32_C(0x100), UINT32_C(1)) !=
                (NTSTATUS)UINT32_C(0xc0000225) ||
            VdmRemoveVirtualMemory(intel_address) != STATUS_SUCCESS ||
            mvdm_softpc_physical_mapping_resolve(intel_address,
                &resolved_page)) {
            fputs("external physical-page binding did not remain live and release\n",
                stderr);
            (void)session_thread_unbind(&physical_owner);
            (void)session_dispose(&physical_owner);
            sas_term();
            (void)VirtualFree(external_page, 0u, MEM_RELEASE);
            return 1;
        }
        if (!session_thread_unbind(&physical_owner) ||
            !session_dispose(&physical_owner)) {
            fputs("external physical-page binding teardown failed\n", stderr);
            sas_term();
            (void)VirtualFree(external_page, 0u, MEM_RELEASE);
            return 1;
        }
        (void)VirtualFree(external_page, 0u, MEM_RELEASE);
    }
    fputs("cpu-init\n", stderr);
    c_cpu_init();
    {
        int observed_default_breakpoint = 0;

        __try {
            EDL_fast_bop((ULONG)0x000003fe);
        }
        __except (fixture_breakpoint_filter(GetExceptionCode())) {
            observed_default_breakpoint = 1;
        }
        if (!observed_default_breakpoint) {
            fputs("EDL fast-BOP default did not preserve debug-break failure\n",
                stderr);
            sas_term();
            return 1;
        }
    }
    /* `c_effective_addr` is the selected CCPU external contract: its result
     * remains a fixed-width guest-linear number, never a host pointer. */
    if (c_effective_addr(UINT16_C(0x1234), UINT32_C(0x5678)) !=
        UINT32_C(0x000179b8) ||
        c_effective_addr(UINT16_C(0), UINT32_C(0x00008000)) !=
        UINT32_C(0x00008000)) {
        fputs("CCPU effective-address contract did not preserve guest numerics\n",
            stderr);
        sas_term();
        return 1;
    }
    fputs("access-init\n", stderr);
    load_sw_cpu_access_functions();
    if (getAX_func != c_getAX || setAX_func != c_setAX ||
        getCF_func != c_getCF || setCF_func != c_setCF ||
        host_simulate_func != c_cpu_simulate) {
        fputs("selected CCPU access table has an unexpected provider\n", stderr);
        sas_term();
        return 1;
    }
    setAX_func(UINT16_C(0x4a21));
    setCF_func(1);
    if (getAX_func() != UINT16_C(0x4a21) || getCF_func() == 0) {
        fputs("selected CCPU access table did not preserve register/flag ABI\n", stderr);
        sas_term();
        return 1;
    }
    /* The selected machine owns the I/O fabric and DMA controller in their
     * original source files.  This is only an initialization and port-dispatch
     * proof: it does not manufacture a device DMA request or memory backend. */
    io_init();
    dma_init();
    dma_post();
    if (Ios_in_adapter_table[DMA_CH1_ADDRESS] != DMA_ADAPTOR ||
        Ios_out_adapter_table[DMA_CH1_ADDRESS] != DMA_ADAPTOR ||
        Ios_in_adapter_table[DMA_CH5_ADDRESS] != DMA_ADAPTOR ||
        Ios_out_adapter_table[DMA_CH5_ADDRESS] != DMA_ADAPTOR ||
        Ios_in_adapter_table[DMA_FLA_PAGE_REG] != DMA_PAGE_ADAPTOR ||
        Ios_out_adapter_table[DMA_FLA_PAGE_REG] != DMA_PAGE_ADAPTOR) {
        fputs("original DMA ports were not installed in original I/O tables\n", stderr);
        sas_term();
        return 1;
    }
    {
        half_word dma_value = 0u;

        outb(DMA_CH1_ADDRESS, UINT8_C(0x34));
        outb(DMA_CH1_ADDRESS, UINT8_C(0x12));
        outb(DMA_CLEAR_FLIP_FLOP, 0u);
        inb(DMA_CH1_ADDRESS, &dma_value);
        if (dma_value != UINT8_C(0x34)) {
            fputs("original DMA low address register did not round-trip\n", stderr);
            sas_term();
            return 1;
        }
        inb(DMA_CH1_ADDRESS, &dma_value);
        if (dma_value != UINT8_C(0x12)) {
            fputs("original DMA high address register did not round-trip\n", stderr);
            sas_term();
            return 1;
        }
        outb(DMA_FLA_PAGE_REG, UINT8_C(0x56));
        inb(DMA_FLA_PAGE_REG, &dma_value);
        if (dma_value != UINT8_C(0x56)) {
            fputs("original DMA page register did not round-trip\n", stderr);
            sas_term();
            return 1;
        }
    }
    {
        IU32 hook_address = 0u;
        IS32 interrupt_vector;

        /* Keep the original BIOS ICA initialization order, then make the
         * original timer IRQ request visible through original PIC INTACK. */
        ica0_init();
        ica0_post();
        ica1_init();
        ica1_post();
        ica_hw_interrupt(ICA_MASTER, CPU_TIMER_INT, 1);
        interrupt_vector = ica_intack(&hook_address);
        if (interrupt_vector != 0x08 || fixture_eoi_hook_calls != 0u ||
            fixture_wow_idle_calls != 1u) {
            fprintf(stderr, "original timer IRQ did not traverse original PIC INTACK: vector=%ld eoi=%u wow=%u\n",
                (long)interrupt_vector, fixture_eoi_hook_calls, fixture_wow_idle_calls);
            sas_term();
            return 1;
        }
    }
    fputs("seed\n", stderr);
    (void)c_setDS(0u);
    /* mov al,5ah; mov [8000h],al; fld1; fstp dword [8004h]; d6 fe */
    c_sas_store(UINT32_C(0x000ffff0), UINT8_C(0xb0));
    c_sas_store(UINT32_C(0x000ffff1), UINT8_C(0x5a));
    c_sas_store(UINT32_C(0x000ffff2), UINT8_C(0xa2));
    c_sas_store(UINT32_C(0x000ffff3), UINT8_C(0x00));
    c_sas_store(UINT32_C(0x000ffff4), UINT8_C(0x80));
    c_sas_store(UINT32_C(0x000ffff5), UINT8_C(0xd9));
    c_sas_store(UINT32_C(0x000ffff6), UINT8_C(0xe8));
    c_sas_store(UINT32_C(0x000ffff7), UINT8_C(0xd9));
    c_sas_store(UINT32_C(0x000ffff8), UINT8_C(0x1e));
    c_sas_store(UINT32_C(0x000ffff9), UINT8_C(0x04));
    c_sas_store(UINT32_C(0x000ffffa), UINT8_C(0x80));
    c_sas_store(UINT32_C(0x000ffffb), UINT8_C(0xd6));
    c_sas_store(UINT32_C(0x000ffffc), UINT8_C(0xfe));
    fputs("start\n", stderr);
    /* `nt_cprgs.c` selects the original CCPU executor through this historical
     * SoftPC CPU-access dispatch slot.  The fixture must not bypass that
     * original selection by calling c_cpu_simulate directly. */
    if (host_simulate_func == NULL) {
        fputs("original host_simulate selection was not installed\n", stderr);
        sas_term();
        return 1;
    }
    /* Enter through the original host wrapper rather than directly calling
     * the CCPU executor.  Under CPU_40_STYLE+CCPU its untouched source body
     * resolves cpu_simulate to c_cpu_simulate. */
    {
        session owner;
        session_initialize(&owner, 1u);
        if (!session_select_machine_backend(&owner, SESSION_MACHINE_BACKEND_SOFTPC) ||
            !session_activate(&owner) ||
            !mvdm_softpc_execution_run_until_return(&owner)) {
            fputs("SoftPC outer return failed\n", stderr);
            sas_term();
            return 1;
        }
        (void)session_dispose(&owner);
    }
    fputs("returned-start\n", stderr);
    if (c_getIP() != UINT16_C(0xfffd) ||
        c_sas_hw_at(UINT32_C(0x00008000)) != UINT8_C(0x5a) ||
        c_sas_hw_at(UINT32_C(0x00008004)) != UINT8_C(0x00) ||
        c_sas_hw_at(UINT32_C(0x00008005)) != UINT8_C(0x00) ||
        c_sas_hw_at(UINT32_C(0x00008006)) != UINT8_C(0x80) ||
        c_sas_hw_at(UINT32_C(0x00008007)) != UINT8_C(0x3f)) {
        fprintf(stderr, "CCPU host_start_cpu did not execute original RAM/FPU writes: IP=%04x\n",
            (unsigned)c_getIP());
        sas_term();
        return 1;
    }

    /* Re-enter through the original recursive SoftPC spelling.  The source
     * body preserves its historical lock/TEB guard, then invokes the same
     * CPU macro.  Reusing the direct exit keeps this a bounded mechanical
     * proof, not a BIOS callback claim. */
    c_setIP(UINT16_C(0xfff0));
    fputs("reenter\n", stderr);
    host_simulate();
    fputs("returned-recursive\n", stderr);
    if (c_getIP() != UINT16_C(0xfffd) ||
        c_sas_hw_at(UINT32_C(0x00008000)) != UINT8_C(0x5a) ||
        c_sas_hw_at(UINT32_C(0x00008004)) != UINT8_C(0x00) ||
        c_sas_hw_at(UINT32_C(0x00008005)) != UINT8_C(0x00) ||
        c_sas_hw_at(UINT32_C(0x00008006)) != UINT8_C(0x80) ||
        c_sas_hw_at(UINT32_C(0x00008007)) != UINT8_C(0x3f)) {
        fprintf(stderr, "CCPU host_simulate did not execute original RAM/FPU writes: IP=%04x\n",
            (unsigned)c_getIP());
        sas_term();
        return 1;
    }

    if (fixture_host_close_calls != 1u || !fixture_high_linear_page()) {
        sas_term();
        return 1;
    }
    sas_term();
    return 0;
}
