#include "mvdm_softpc_wow_page_domain.h"
#include "mvdm_softpc_effective_address.h"

#include <windows.h>

#include "ntvdm-exe/session/session.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_termination.h"

/* Keep this non-mirror binding on the narrow, fixed-width CCPU/nt_mem ABI it
 * consumes.  Pulling the historical macro headers into a new adapter would
 * make its build depend on source-era include ordering. */
extern uint32_t c_sas_memory_size(void);
extern uint32_t c_sas_dw_at(uint32_t address);
extern int c_sas_twenty_bit_wrapping_enabled(void);
extern void c_sas_fills(uint32_t address, uint8_t value, uint32_t byte_count);
extern void c_sas_storedw(uint32_t address, uint32_t value);
extern void c_sas_storew(uint32_t address, uint16_t value);
extern void c_sas_loads(uint32_t source, uint8_t *destination,
    uint32_t byte_count);
extern void c_sas_stores(uint32_t destination, uint8_t *source,
    uint32_t byte_count);
extern uint32_t c_getCR0(void);
extern uint32_t c_getCR3(void);
extern void c_setCR0(uint32_t value);
extern void c_setCR3(uint32_t value);
extern uint32_t c_getGDT_BASE(void);
extern uint16_t c_getGDT_LIMIT(void);
extern uint32_t c_getLDT_BASE(void);
extern uint16_t c_getLDT_LIMIT(void);
extern void c_setGDT_BASE_LIMIT(uint32_t base, uint16_t limit);
extern LONG VdmAllocateVirtualMemory(ULONG *address, ULONG size, BOOL commit);
extern LONG VdmFreeVirtualMemory(ULONG address);
extern LONG VdmAddVirtualMemory(ULONG host_address, ULONG size,
    ULONG *intel_address);
extern LONG VdmRemoveVirtualMemory(ULONG intel_address);

#define MVDM_SOFTPC_PAGE_BYTES 4096u
#define MVDM_SOFTPC_PAGE_PRESENT_USER 7u
#define MVDM_SOFTPC_PAGE_PRESENT_USER_READ 5u
#define MVDM_SOFTPC_CR0_PG 0x80000000u
#define MVDM_SOFTPC_SHARED_CLOCK_LINEAR 0x7ffe0000u
#define MVDM_SOFTPC_GDT_ENTRY_BYTES 8u
#define MVDM_SOFTPC_FLAT_ADDRESS_ENTRIES 0x1fffu
#define MVDM_SOFTPC_FLAT_ADDRESS_BYTES \
    (MVDM_SOFTPC_FLAT_ADDRESS_ENTRIES * sizeof(ULONG))
#define MVDM_SOFTPC_KGDT_R3_DATA 0x20u
#define MVDM_SOFTPC_KGDT_R3_TEB 0x38u
#define MVDM_SOFTPC_TEB_BYTES 4096u
/* These are offsets read from the pinned immutable USER.EXE profile, not
 * inferred from a modern host TEB. */
#define MVDM_SOFTPC_TEB_SELF 0x18u
#define MVDM_SOFTPC_TEB_EXPECTED_WINDOWS_VERSION 0x50u
#define MVDM_SOFTPC_TEB_DESKTOP_INFO 0x5cu
#define MVDM_SOFTPC_TEB_CLIENT_DELTA 0x60u
#define MVDM_SOFTPC_TEB_CACHED_HWND 0x6cu
#define MVDM_SOFTPC_TEB_CACHED_WND 0x70u
#define MVDM_SOFTPC_SHAREDINFO_SERVERINFO 0u
#define MVDM_SOFTPC_SHAREDINFO_HANDLE_TABLE 4u
#define MVDM_SOFTPC_SERVERINFO_HANDLE_COUNT 4u
/* These offsets and bounds are read from the pinned PMODE32 USER.EXE
 * consumer, then cross-checked against original USER's SERVERINFO.  They are
 * not modern USER structure offsets. */
#define MVDM_SOFTPC_SERVERINFO_METRICS 0x170u
#define MVDM_SOFTPC_SERVERINFO_METRIC_COUNT 76u
#define MVDM_SOFTPC_SERVERINFO_COLORS 0x2a0u
#define MVDM_SOFTPC_SERVERINFO_COLOR_COUNT 25u
#define MVDM_SOFTPC_SERVERINFO_CURSOR_X 0x304u
#define MVDM_SOFTPC_SERVERINFO_CURSOR_Y 0x308u
/* USER16's immutable PMODE32 profile consumes the checked/debug HANDLEENTRY
 * layout: phead, pOwner, bType, bFlags, wUniq, debug tail.  Do not use the
 * host compiler's HANDLEENTRY size here. */
#define MVDM_SOFTPC_HANDLE_ENTRY_BYTES 16u
#define MVDM_SOFTPC_HANDLE_ENTRY_COUNT 0x10000u
#define MVDM_SOFTPC_HANDLE_TABLE_BYTES \
    (MVDM_SOFTPC_HANDLE_ENTRY_BYTES * MVDM_SOFTPC_HANDLE_ENTRY_COUNT)
#define MVDM_SOFTPC_CLIENT_PREFIX_BYTES (2u * MVDM_SOFTPC_PAGE_BYTES)
/* Fixed x86 layouts are asserted against the selected original USER source by
 * wow_user_client_view_layout.h.  This worker carrier writes only the fields
 * immutable USER16 directly consumes before it can enter an ordinary thunk. */
#define MVDM_SOFTPC_WND_BYTES 176u
#define MVDM_SOFTPC_DESKTOP_INFO_BYTES 100u
#define MVDM_SOFTPC_CLIENT_OBJECT_BYTES MVDM_SOFTPC_PAGE_BYTES
#define MVDM_SOFTPC_CLIENT_DELTA 0x70000000u
#define MVDM_SOFTPC_DESKTOP_HANDLE 0x00010001u
#define MVDM_SOFTPC_WND_HEAD_HANDLE 0u
#define MVDM_SOFTPC_WND_HEAD_SELF 12u
#define MVDM_SOFTPC_WND_RECT 40u
#define MVDM_SOFTPC_WND_CLIENT_RECT 56u
#define MVDM_SOFTPC_WND_EXSTYLE 164u
#define MVDM_SOFTPC_WND_STYLE 168u
#define MVDM_SOFTPC_WND_CLASS 76u
#define MVDM_SOFTPC_DESKTOP_BASE 0u
#define MVDM_SOFTPC_DESKTOP_LIMIT 4u
#define MVDM_SOFTPC_DESKTOP_WINDOW 8u
#define MVDM_SOFTPC_CLS_BYTES 108u
#define MVDM_SOFTPC_CLS_ATOM 4u
#define MVDM_SOFTPC_CLS_FNID 6u
#define MVDM_SOFTPC_CLS_WINDOW_COUNT 20u
#define MVDM_SOFTPC_CLS_FLAGS 24u
#define MVDM_SOFTPC_CLS_WOW0 36u
#define MVDM_SOFTPC_CLS_WOW1 40u
#define MVDM_SOFTPC_CLS_TASK 44u
#define MVDM_SOFTPC_CLS_STYLE 64u
#define MVDM_SOFTPC_CLS_PROCEDURE 68u
#define MVDM_SOFTPC_CLS_WINDOW_EXTRA 76u
#define MVDM_SOFTPC_CLS_MODULE 80u

typedef struct mvdm_softpc_wow_page_domain {
    session *owner;
    ULONG table_allocation;
    ULONG table_bytes;
    ULONG clock_intel_address;
    volatile ULONG *clock_page;
    HANDLE clock_stop;
    HANDLE clock_thread;
    ULONG directory;
    ULONG client_allocation;
    ULONG guest_shared_info;
    ULONG guest_server_info;
    ULONG guest_handle_table;
    ULONG guest_teb;
    ULONG guest_csr_flag;
    ULONG flat_address_array;
    ULONG guest_client_objects;
    ULONG guest_client_objects_limit;
    ULONG guest_desktop_info;
    ULONG guest_desktop_window;
    ULONG client_delta;
    ULONG guest_windows[MVDM_SOFTPC_HANDLE_ENTRY_COUNT];
    ULONG wow_gdt;
    ULONG dosx_gdt;
    USHORT dosx_gdt_limit;
    int wow_context_selected;
    int active;
} mvdm_softpc_wow_page_domain;

typedef struct mvdm_softpc_wow_class_allocation {
    struct mvdm_softpc_wow_class_allocation *next;
    ULONG client;
} mvdm_softpc_wow_class_allocation;

static mvdm_softpc_wow_page_domain domain;
static mvdm_softpc_wow_class_allocation *class_allocations;

static int page_domain_client_desktop(ULONG desktop_info, ULONG delta);

static ULONG page_domain_round_page(ULONG bytes)
{
    return (bytes + MVDM_SOFTPC_PAGE_BYTES - 1u) &
        ~(MVDM_SOFTPC_PAGE_BYTES - 1u);
}

/* Default-off lifecycle evidence for the finite S41 binding.  The existing
 * scalar observer performs no I/O unless its report-path environment variable
 * is explicitly set; no guest address, descriptor, page or session state is
 * read or changed by this call. */
static void page_domain_report(char const *stage)
{
    mvdm_softpc_report_dpmi_mode_state(stage, (unsigned long)c_getCR0(),
        (unsigned long)c_getCR3(), 0u, 0u, 0u, 0u);
}

static DWORD WINAPI clock_publisher(void *context)
{
    mvdm_softpc_wow_page_domain *value = context;

    for (;;) {
        if (WaitForSingleObject(value->clock_stop, 10u) != WAIT_TIMEOUT)
            return 0;
        value->clock_page[0] = GetTickCount();
        value->clock_page[1] = 0x01000000u;
    }
}

static void page_domain_dispose(void *context)
{
    mvdm_softpc_wow_page_domain *value = context;
    mvdm_softpc_wow_class_allocation *class_allocation;
    unsigned index;

    mvdm_softpc_wow_page_domain_leave_protected();
    if (value->clock_stop != NULL) SetEvent(value->clock_stop);
    if (value->clock_thread != NULL) {
        WaitForSingleObject(value->clock_thread, INFINITE);
        CloseHandle(value->clock_thread);
    }
    /* The provider normally retires every original-layout object itself.
     * Worker teardown is the exceptional backstop: no outstanding client
     * object may survive this worker's VDM allocation domain. */
    for (index = 0u; index < MVDM_SOFTPC_HANDLE_ENTRY_COUNT; ++index) {
        if (value->guest_windows[index] != 0u) {
            (void)VdmFreeVirtualMemory(value->guest_windows[index]);
            value->guest_windows[index] = 0u;
        }
    }
    while ((class_allocation = class_allocations) != NULL) {
        class_allocations = class_allocation->next;
        (void)VdmFreeVirtualMemory(class_allocation->client);
        HeapFree(GetProcessHeap(), 0u, class_allocation);
    }
    if (value->clock_stop != NULL) CloseHandle(value->clock_stop);
    if (value->clock_intel_address != 0u)
        (void)VdmRemoveVirtualMemory(value->clock_intel_address);
    if (value->clock_page != NULL)
        (void)VirtualFree((void *)value->clock_page, 0u, MEM_RELEASE);
    if (value->table_allocation != 0u)
        (void)VdmFreeVirtualMemory(value->table_allocation);
    if (value->client_allocation != 0u)
        (void)VdmFreeVirtualMemory(value->client_allocation);
    if (value->flat_address_array != 0u)
        (void)VdmFreeVirtualMemory(value->flat_address_array);
    if (value->wow_gdt != 0u)
        (void)VdmFreeVirtualMemory(value->wow_gdt);
    ZeroMemory(value, sizeof(*value));
}

static void page_domain_store_descriptor(ULONG gdt, ULONG selector,
    ULONG base, ULONG limit)
{
    ULONG offset = selector & ~7u;
    ULONG low = (limit & 0xffffu) | ((base & 0xffffu) << 16);
    ULONG high = ((base >> 16) & 0xffu) | 0x0000f200u |
        (((limit >> 16) & 0xfu) << 16) | (base & 0xff000000u);

    if (limit > 0xffffu) high |= 0x00c00000u; /* G=1, default 32-bit */

    c_sas_storedw(gdt + offset, low);
    c_sas_storedw(gdt + offset + 4u, high);
}

static int page_domain_create_client_view(void)
{
    ULONG address = 0u;
    ULONG handle_table;
    ULONG teb;
    ULONG client_bytes;
    ULONG index;
    NTSTATUS status;

    handle_table = MVDM_SOFTPC_CLIENT_PREFIX_BYTES;
    teb = page_domain_round_page(handle_table + MVDM_SOFTPC_HANDLE_TABLE_BYTES);
    client_bytes = teb + MVDM_SOFTPC_TEB_BYTES + MVDM_SOFTPC_PAGE_BYTES +
        MVDM_SOFTPC_CLIENT_OBJECT_BYTES;
    if (client_bytes < teb) return 0;
    status = VdmAllocateVirtualMemory(&address, client_bytes, TRUE);
    if (status < 0) return 0;
    domain.client_allocation = address;
    domain.guest_shared_info = address;
    domain.guest_server_info = address + MVDM_SOFTPC_PAGE_BYTES;
    domain.guest_handle_table = address + handle_table;
    domain.guest_teb = address + teb;
    /* WU32NotifyWow's original zero-initialized CallCsrFlag is shared only
     * with USER16's TEST/CLEARCALLSERVERCONDITION macros. Its writable byte
     * needs guest backing, not the WOW32 DLL's native static address. */
    domain.guest_csr_flag = domain.guest_teb + MVDM_SOFTPC_TEB_BYTES;
    domain.guest_client_objects = domain.guest_csr_flag + MVDM_SOFTPC_PAGE_BYTES;
    domain.guest_client_objects_limit = domain.guest_client_objects +
        MVDM_SOFTPC_CLIENT_OBJECT_BYTES;
    c_sas_fills(address, 0u, client_bytes);

    /* SHAREDINFO and SERVERINFO use the source-pinned USER16 offsets.  The
     * initial free-table sentinel follows original handtabl.c; no native
     * phead/pOwner pointer is ever copied into guest memory. */
    c_sas_storedw(domain.guest_shared_info + MVDM_SOFTPC_SHAREDINFO_SERVERINFO,
        domain.guest_server_info);
    c_sas_storedw(domain.guest_shared_info + MVDM_SOFTPC_SHAREDINFO_HANDLE_TABLE,
        domain.guest_handle_table);
    c_sas_storedw(domain.guest_server_info + MVDM_SOFTPC_SERVERINFO_HANDLE_COUNT,
        MVDM_SOFTPC_HANDLE_ENTRY_COUNT);
    /* The original server owns this read-only shared snapshot.  In the
     * standalone worker the public system queries are its finite host edge:
     * publish every complete scalar before USER16 receives SHAREDINFO, not
     * lazily during an unrelated thunk. */
    for (index = 0u; index < MVDM_SOFTPC_SERVERINFO_METRIC_COUNT; ++index) {
        c_sas_storedw(domain.guest_server_info +
            MVDM_SOFTPC_SERVERINFO_METRICS + index * sizeof(ULONG),
            (ULONG)GetSystemMetrics((int)index));
    }
    for (index = 0u; index < MVDM_SOFTPC_SERVERINFO_COLOR_COUNT; ++index) {
        c_sas_storedw(domain.guest_server_info +
            MVDM_SOFTPC_SERVERINFO_COLORS + index * sizeof(ULONG),
            (ULONG)GetSysColor((int)index));
    }
    {
        POINT cursor;

        /* A noninteractive worker need not have a public cursor.  This is a
         * snapshot field, not a prerequisite for constructing the complete
         * original shared view; zero remains the initialized no-position
         * value when the public query is unavailable. */
        if (GetCursorPos(&cursor)) {
            c_sas_storedw(domain.guest_server_info +
                MVDM_SOFTPC_SERVERINFO_CURSOR_X, (ULONG)cursor.x);
            c_sas_storedw(domain.guest_server_info +
                MVDM_SOFTPC_SERVERINFO_CURSOR_Y, (ULONG)cursor.y);
        }
    }
    for (index = 0u; index < MVDM_SOFTPC_HANDLE_ENTRY_COUNT; ++index) {
        ULONG entry = domain.guest_handle_table +
            index * MVDM_SOFTPC_HANDLE_ENTRY_BYTES;

        c_sas_storedw(entry, index + 1u);
        c_sas_storedw(entry + 4u, 0u);
        c_sas_storedw(entry + 8u, 0x00010000u);
    }

    /* Native NT's context switch makes FS:18h a self pointer. This projection
     * only publishes that source-defined guest-linear value; it never aliases
     * host TLS/TEB. CLIENTINFO begins at TEB+44h and is deliberately all zero
     * until its original task/desktop owners publish it. */
    c_sas_storedw(domain.guest_teb + MVDM_SOFTPC_TEB_SELF, domain.guest_teb);
    return 1;
}

static int page_domain_create_desktop_root(void)
{
    ULONG desktop = domain.guest_client_objects;
    ULONG root = (desktop + MVDM_SOFTPC_DESKTOP_INFO_BYTES + 7u) & ~7u;
    ULONG server_base;
    ULONG server_limit;
    ULONG server_desktop;
    ULONG server_root;
    ULONG handle;
    LONG width;
    LONG height;

    if (root < desktop || root + MVDM_SOFTPC_WND_BYTES < root ||
            root + MVDM_SOFTPC_WND_BYTES > domain.guest_client_objects_limit)
        return 0;
    if (desktop > 0xffffffffu - MVDM_SOFTPC_CLIENT_DELTA ||
            domain.guest_client_objects_limit >
                0xffffffffu - MVDM_SOFTPC_CLIENT_DELTA ||
            root > 0xffffffffu - MVDM_SOFTPC_CLIENT_DELTA)
        return 0;
    server_base = domain.guest_client_objects + MVDM_SOFTPC_CLIENT_DELTA;
    server_limit = domain.guest_client_objects_limit + MVDM_SOFTPC_CLIENT_DELTA;
    server_desktop = desktop + MVDM_SOFTPC_CLIENT_DELTA;
    server_root = root + MVDM_SOFTPC_CLIENT_DELTA;
    handle = domain.guest_handle_table + MVDM_SOFTPC_HANDLE_ENTRY_BYTES;
    width = GetSystemMetrics(SM_CXSCREEN);
    height = GetSystemMetrics(SM_CYSCREEN);
    if (width <= 0 || height <= 0) return 0;

    /* This is the selected original client representation of the native
     * desktop concept, not a native USER pointer or a replacement window
     * manager.  All pointer-bearing fields use the original server form. */
    c_sas_storedw(desktop + MVDM_SOFTPC_DESKTOP_BASE, server_base);
    c_sas_storedw(desktop + MVDM_SOFTPC_DESKTOP_LIMIT, server_limit);
    c_sas_storedw(desktop + MVDM_SOFTPC_DESKTOP_WINDOW, server_root);
    c_sas_storedw(root + MVDM_SOFTPC_WND_HEAD_HANDLE,
        MVDM_SOFTPC_DESKTOP_HANDLE);
    c_sas_storedw(root + MVDM_SOFTPC_WND_HEAD_SELF, server_root);
    c_sas_storedw(root + MVDM_SOFTPC_WND_RECT + 8u, (ULONG)width);
    c_sas_storedw(root + MVDM_SOFTPC_WND_RECT + 12u, (ULONG)height);
    c_sas_storedw(root + MVDM_SOFTPC_WND_CLIENT_RECT + 8u, (ULONG)width);
    c_sas_storedw(root + MVDM_SOFTPC_WND_CLIENT_RECT + 12u, (ULONG)height);
    c_sas_storedw(root + MVDM_SOFTPC_WND_EXSTYLE, 0u);
    c_sas_storedw(root + MVDM_SOFTPC_WND_STYLE, WS_VISIBLE);
    c_sas_storedw(handle, server_root);
    c_sas_storedw(handle + 4u, 0u);
    c_sas_storedw(handle + 8u, MVDM_SOFTPC_DESKTOP_HANDLE);
    domain.guest_desktop_info = desktop;
    domain.guest_desktop_window = root;
    domain.client_delta = MVDM_SOFTPC_CLIENT_DELTA;
    return 1;
}

static int page_domain_create_wow_gdt(void)
{
    ULONG address = 0u;
    ULONG source = c_getGDT_BASE();
    USHORT limit = c_getGDT_LIMIT();
    ULONG bytes;
    ULONG allocation_bytes;
    uint8_t *image;
    NTSTATUS status;

    if (source == 0u || limit < MVDM_SOFTPC_KGDT_R3_TEB + 7u) return 0;
    bytes = (ULONG)limit + 1u;
    allocation_bytes = (bytes + MVDM_SOFTPC_PAGE_BYTES - 1u) &
        ~(MVDM_SOFTPC_PAGE_BYTES - 1u);
    if (allocation_bytes < bytes) return 0;
    image = HeapAlloc(GetProcessHeap(), 0u, bytes);
    if (image == NULL) return 0;
    status = VdmAllocateVirtualMemory(&address, allocation_bytes, TRUE);
    if (status < 0) {
        HeapFree(GetProcessHeap(), 0u, image);
        return 0;
    }
    domain.wow_gdt = address;
    domain.dosx_gdt = source;
    domain.dosx_gdt_limit = limit;
    c_sas_loads(source, image, bytes);
    c_sas_stores(address, image, bytes);
    HeapFree(GetProcessHeap(), 0u, image);
    page_domain_store_descriptor(address, MVDM_SOFTPC_KGDT_R3_DATA, 0u,
        0xffffffffu);
    page_domain_store_descriptor(address, MVDM_SOFTPC_KGDT_R3_TEB,
        domain.guest_teb, MVDM_SOFTPC_TEB_BYTES - 1u);
    return 1;
}

static int page_domain_create(session *owner)
{
    ULONG sas_bytes = c_sas_memory_size();
    ULONG identity_tables;
    ULONG table_pages;
    ULONG index;
    ULONG allocation;
    ULONG clock_table;
    NTSTATUS status;

    page_domain_report("wow-domain-create-begin");
    /* The fixed USER clock mapping is above the one-megabyte A20 boundary.
     * DOSX owns the real-mode-to-protected-mode A20 transition; accepting a
     * wrapped SAS view here would silently bind 7FFE:0000 to the wrong page. */
    if (c_sas_twenty_bit_wrapping_enabled() || sas_bytes == 0u ||
        (sas_bytes & (MVDM_SOFTPC_PAGE_BYTES - 1u)) != 0u)
        return 0;
    identity_tables = (sas_bytes + 0x3fffffu) >> 22;
    table_pages = 2u + identity_tables;
    if (table_pages > 0xffffffffu / MVDM_SOFTPC_PAGE_BYTES)
        return 0;
    domain.table_bytes = table_pages * MVDM_SOFTPC_PAGE_BYTES;
    allocation = 0u;
    status = VdmAllocateVirtualMemory(&allocation, domain.table_bytes, TRUE);
    if (status < 0) goto fail;
    domain.table_allocation = allocation;
    domain.directory = allocation;
    c_sas_fills(allocation, 0u, domain.table_bytes);
    for (index = 0u; index < identity_tables; ++index) {
        ULONG table = allocation + MVDM_SOFTPC_PAGE_BYTES * (1u + index);
        ULONG first = index << 22;
        ULONG count = sas_bytes - first;
        ULONG page;

        if (count > 0x400000u) count = 0x400000u;
        c_sas_storedw(allocation + index * 4u,
            table | MVDM_SOFTPC_PAGE_PRESENT_USER);
        for (page = 0u; page < (count >> 12); ++page)
            c_sas_storedw(table + page * 4u,
                (first + page * MVDM_SOFTPC_PAGE_BYTES) |
                MVDM_SOFTPC_PAGE_PRESENT_USER);
    }
    page_domain_report("wow-domain-identity-ready");
    page_domain_report("wow-domain-clock-allocate");
    domain.clock_page = VirtualAlloc(NULL, MVDM_SOFTPC_PAGE_BYTES,
        MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (domain.clock_page == NULL) goto fail;
    page_domain_report("wow-domain-clock-allocated");
    domain.clock_page[0] = GetTickCount();
    domain.clock_page[1] = 0x01000000u;
    page_domain_report("wow-domain-clock-register");
    status = VdmAddVirtualMemory((ULONG)(ULONG_PTR)domain.clock_page,
        MVDM_SOFTPC_PAGE_BYTES, &domain.clock_intel_address);
    if (status < 0) goto fail;
    page_domain_report("wow-domain-clock-registered");
    clock_table = allocation + MVDM_SOFTPC_PAGE_BYTES * (1u + identity_tables);
    c_sas_storedw(allocation + ((MVDM_SOFTPC_SHARED_CLOCK_LINEAR >> 22) * 4u),
        clock_table | MVDM_SOFTPC_PAGE_PRESENT_USER);
    c_sas_storedw(clock_table +
        (((MVDM_SOFTPC_SHARED_CLOCK_LINEAR >> 12) & 1023u) * 4u),
        domain.clock_intel_address | MVDM_SOFTPC_PAGE_PRESENT_USER_READ);
    page_domain_report("wow-domain-clock-ready");
    domain.clock_stop = CreateEventA(NULL, TRUE, FALSE, NULL);
    if (domain.clock_stop == NULL) goto fail;
    domain.clock_thread = CreateThread(NULL, 0u, clock_publisher, &domain, 0u,
        NULL);
    if (domain.clock_thread == NULL) goto fail;
    page_domain_report("wow-domain-thread-ready");
    if (!page_domain_create_client_view() || !page_domain_create_desktop_root() ||
            !page_domain_create_wow_gdt())
        goto fail;
    page_domain_report("wow-domain-view-ready");
    domain.owner = owner;
    if (!session_register_teardown(owner, page_domain_dispose, &domain)) goto fail;
    domain.active = 1;
    return 1;
fail:
    page_domain_report("wow-domain-create-failed");
    page_domain_dispose(&domain);
    return 0;
}

int mvdm_softpc_wow_page_domain_enter(void)
{
    session *owner = session_thread_current();

    if (owner == NULL || !session_valid(owner) || (c_getCR0() & 1u) == 0u)
        return 0;
    if (!domain.active && !page_domain_create(owner)) return 0;
    if (domain.owner != owner) return 0;
    c_setCR3(domain.directory);
    c_setCR0(c_getCR0() | MVDM_SOFTPC_CR0_PG);
    /* desktop.c's client publication is a protected client-view operation:
     * it must run only after this worker has selected its own page tables.
     * The root object is allocated at domain creation, but publishing its
     * server/client relation earlier would violate that existing contract. */
    if (!page_domain_client_desktop(domain.guest_desktop_info,
            domain.client_delta)) {
        mvdm_softpc_wow_page_domain_leave_protected();
        return 0;
    }
    if (domain.wow_context_selected)
        c_setGDT_BASE_LIMIT(domain.wow_gdt, domain.dosx_gdt_limit);
    page_domain_report("wow-domain-entered");
    return 1;
}

void mvdm_softpc_wow_page_domain_leave_protected(void)
{
    if (domain.wow_context_selected && c_getGDT_BASE() == domain.wow_gdt) {
        c_setGDT_BASE_LIMIT(domain.dosx_gdt, domain.dosx_gdt_limit);
    }
    if (domain.active && (c_getCR0() & MVDM_SOFTPC_CR0_PG) != 0u)
        c_setCR0(c_getCR0() & ~MVDM_SOFTPC_CR0_PG);
    page_domain_report("wow-domain-left");
}

int mvdm_softpc_wow_page_domain_reenter_protected(void)
{
    if (!domain.active) return 1;
    if ((c_getCR0() & 1u) == 0u) return 0;
    c_setCR3(domain.directory);
    c_setCR0(c_getCR0() | MVDM_SOFTPC_CR0_PG);
    /* The native NT user GDT survives a DOSX real-mode excursion. Our
     * temporary DOSX view must likewise not revoke the already selected
     * WOW flat/TEB selectors when the original protected caller resumes. */
    if (domain.wow_context_selected)
        c_setGDT_BASE_LIMIT(domain.wow_gdt, domain.dosx_gdt_limit);
    page_domain_report("wow-domain-reentered");
    return 1;
}

int mvdm_softpc_wow_page_domain_active(void)
{
    return domain.active;
}

int mvdm_softpc_wow_page_domain_activate_wow_context(void)
{
    if (!domain.active || domain.wow_gdt == 0u ||
        (c_getCR0() & MVDM_SOFTPC_CR0_PG) == 0u) return 0;
    c_setGDT_BASE_LIMIT(domain.wow_gdt, domain.dosx_gdt_limit);
    domain.wow_context_selected = 1;
    page_domain_report("wow-domain-wow-gdt-active");
    return 1;
}

unsigned long mvdm_softpc_wow_page_domain_flat_address_array(
    const unsigned long *entries)
{
    ULONG address;
    ULONG index;

    if (!domain.active || entries == NULL) {
        mvdm_softpc_report_wow_fast_callback_binding("flat-address-array-unavailable", 0u);
        return 0u;
    }
    if (domain.flat_address_array == 0u) {
        address = 0u;
        if (VdmAllocateVirtualMemory(&address, MVDM_SOFTPC_FLAT_ADDRESS_BYTES,
                TRUE) < 0)
            return 0u;
        domain.flat_address_array = address;
        /* Host FlatAddress contains native addresses (including original
         * DIB aliases). PMODE32's view contains guest-linear bases instead.
         * Read the original descriptor representation, never subtract an
         * assumed IntelBase from a possibly aliased host pointer. */
        for (index = 0u; index < MVDM_SOFTPC_FLAT_ADDRESS_ENTRIES; ++index) {
            ULONG base = 0u;
            if (entries[index] != 0u &&
                    index * 8u + 7u <= c_getLDT_LIMIT()) {
                ULONG low = c_sas_dw_at(c_getLDT_BASE() + index * 8u);
                ULONG high = c_sas_dw_at(c_getLDT_BASE() + index * 8u + 4u);
                base = (low >> 16) | ((high & 0xffu) << 16) |
                    (high & 0xff000000u);
            }
            c_sas_storedw(address + index * sizeof(ULONG), base);
        }
    }
    mvdm_softpc_report_wow_fast_callback_binding("flat-address-array",
        domain.flat_address_array);
    return domain.flat_address_array;
}

void mvdm_softpc_wow_page_domain_update_flat_address(unsigned short selector,
    unsigned long base)
{
    ULONG index = ((ULONG)selector & 0xfff8u) >> 3;

    if (domain.flat_address_array == 0u ||
            index >= MVDM_SOFTPC_FLAT_ADDRESS_ENTRIES)
        return;
    c_sas_storedw(domain.flat_address_array + index * sizeof(ULONG), base);
}

unsigned long mvdm_softpc_wow_page_domain_guest_shared_info(void)
{
    return domain.active ? domain.guest_shared_info : 0u;
}

unsigned long mvdm_softpc_wow_page_domain_guest_teb(void)
{
    return domain.active ? domain.guest_teb : 0u;
}

int mvdm_softpc_wow_page_domain_set_expected_windows_version(
    unsigned long version)
{
    /* queue.c::xxxInitTask owns the value. This narrow write only carries
     * that already-selected task value into USER16's original client TEB; it
     * neither selects a compatibility version nor introduces a task table. */
    if (!domain.active || domain.owner != session_thread_current() ||
        !(c_getCR0() & MVDM_SOFTPC_CR0_PG) || c_getCR3() != domain.directory)
        return 0;
    c_sas_storedw(domain.guest_teb + MVDM_SOFTPC_TEB_EXPECTED_WINDOWS_VERSION,
        (ULONG)version);
    return 1;
}

int mvdm_softpc_wow_page_domain_publish_cursor(long x, long y)
{
    /* cursor.c::InternalSetCursorPos publishes its already bounded input
     * position before it produces mouse work.  The standalone worker has no
     * NT USER input thread, but SoftPC's host_os_mouse_pointer has already
     * performed the corresponding Console-to-guest transformation.  Do not
     * sample a host pointer here: its coordinate system is not the guest
     * virtual screen contract. */
    if (!domain.active || domain.owner != session_thread_current() ||
        !(c_getCR0() & MVDM_SOFTPC_CR0_PG) || c_getCR3() != domain.directory)
        return 0;
    c_sas_storedw(domain.guest_server_info +
        MVDM_SOFTPC_SERVERINFO_CURSOR_X, (ULONG)x);
    c_sas_storedw(domain.guest_server_info +
        MVDM_SOFTPC_SERVERINFO_CURSOR_Y, (ULONG)y);
    return 1;
}

int mvdm_softpc_wow_page_domain_publish_system_color(unsigned long index,
    unsigned long rgb)
{
    /* sysmet.c::SetSysColor publishes exactly one accepted colour into
     * gpsi->argbSystem[icol]. The public call remains the original WOW32
     * authority for success/failure; this carrier only mirrors its completed
     * worker-local result into the direct USER16 view. */
    if (!domain.active || domain.owner != session_thread_current() ||
        index >= MVDM_SOFTPC_SERVERINFO_COLOR_COUNT ||
        !(c_getCR0() & MVDM_SOFTPC_CR0_PG) || c_getCR3() != domain.directory)
        return 0;
    c_sas_storedw(domain.guest_server_info +
        MVDM_SOFTPC_SERVERINFO_COLORS + index * sizeof(ULONG), (ULONG)rgb);
    return 1;
}

unsigned long mvdm_softpc_wow_page_domain_guest_csr_flag(void)
{
    return domain.active ? domain.guest_csr_flag : 0u;
}

int mvdm_softpc_wow_page_domain_client_desktop_ready(void)
{
    return domain.active && domain.guest_desktop_info != 0u &&
        domain.guest_desktop_window != 0u && domain.client_delta != 0u &&
        c_sas_dw_at(domain.guest_teb + MVDM_SOFTPC_TEB_DESKTOP_INFO) != 0u &&
        c_sas_dw_at(domain.guest_teb + MVDM_SOFTPC_TEB_CLIENT_DELTA) != 0u;
}

static int page_domain_client_desktop(ULONG desktop_info, ULONG delta)
{
    if (!domain.active || domain.owner != session_thread_current() ||
        !(c_getCR0() & MVDM_SOFTPC_CR0_PG) || c_getCR3() != domain.directory)
        return 0;
    /* Original desktop.c publishes client pDeskInfo and ulClientDelta as
     * one thread context. CCPU is suspended at this caller-owned boundary.
     * Invalidate ValidateHwnd's cached pair before changing that context. */
    c_sas_storedw(domain.guest_teb + MVDM_SOFTPC_TEB_CACHED_HWND, 0u);
    c_sas_storedw(domain.guest_teb + MVDM_SOFTPC_TEB_CACHED_WND, 0u);
    c_sas_storedw(domain.guest_teb + MVDM_SOFTPC_TEB_CLIENT_DELTA, delta);
    c_sas_storedw(domain.guest_teb + MVDM_SOFTPC_TEB_DESKTOP_INFO, desktop_info);
    return 1;
}

int mvdm_softpc_wow_page_domain_set_client_desktop(
    unsigned long desktop_info, unsigned long delta)
{
    /* Original client/desktop.c uses a separate client view and carries a
     * nonzero server-to-client delta.  A zero delta would turn server-view
     * HANDLEENTRY/DESKTOPINFO pointers into an accidental direct alias and
     * violates the original DispatchClientMessage contract. */
    if (!desktop_info || !delta) return 0;
    return page_domain_client_desktop(desktop_info, delta);
}

int mvdm_softpc_wow_page_domain_clear_client_desktop(void)
{
    return page_domain_client_desktop(0u, 0u);
}

static void page_domain_window_info(ULONG guest_window, const WINDOWINFO *info)
{
    const LONG *rect = &info->rcWindow.left;
    const LONG *client = &info->rcClient.left;
    unsigned i;

    /* Original rtl/wow.c::_ClientToScreen and _GetClientRect consume a
     * screen-relative WND.rcClient, not GetClientRect's zero-origin result.
     * GetWindowInfo supplies both rectangles in that original coordinate
     * space. No native pointer or native WND layout enters guest memory. */
    for (i = 0; i < 4u; ++i) {
        c_sas_storedw(guest_window + MVDM_SOFTPC_WND_RECT + i * 4u,
            (ULONG)rect[i]);
        c_sas_storedw(guest_window + MVDM_SOFTPC_WND_CLIENT_RECT + i * 4u,
            (ULONG)client[i]);
    }
    c_sas_storedw(guest_window + MVDM_SOFTPC_WND_STYLE, info->dwStyle);
    c_sas_storedw(guest_window + MVDM_SOFTPC_WND_EXSTYLE, info->dwExStyle);
}

int mvdm_softpc_wow_page_domain_refresh_window(unsigned long window)
{
    WORD index = LOWORD(window);
    ULONG guest_window;
    WINDOWINFO info = { sizeof(info) };

    if (!domain.active || !index || index == 0xffffu || index == 1u ||
            domain.owner != session_thread_current() ||
            !(c_getCR0() & MVDM_SOFTPC_CR0_PG) || c_getCR3() != domain.directory)
        return 0;
    guest_window = domain.guest_windows[index];
    if (!guest_window || c_sas_dw_at(guest_window + MVDM_SOFTPC_WND_HEAD_HANDLE)
            != window || !GetWindowInfo((HWND)(ULONG_PTR)window, &info))
        return 0;
    page_domain_window_info(guest_window, &info);
    return 1;
}

int mvdm_softpc_wow_page_domain_callback_window(unsigned long window,
    unsigned long saved[2])
{
    ULONG backing;
    if (!saved || !mvdm_softpc_wow_page_domain_refresh_window(window)) return 0;
    backing = domain.guest_windows[LOWORD(window)];
    /* ssend.c LOCKPWND publishes a client PWND while the caller retains the
     * existing window borrow. ValidateHwnd then uses this exact pair. */
    saved[0] = c_sas_dw_at(domain.guest_teb + MVDM_SOFTPC_TEB_CACHED_HWND);
    saved[1] = c_sas_dw_at(domain.guest_teb + MVDM_SOFTPC_TEB_CACHED_WND);
    c_sas_storedw(domain.guest_teb + MVDM_SOFTPC_TEB_CACHED_WND, backing);
    c_sas_storedw(domain.guest_teb + MVDM_SOFTPC_TEB_CACHED_HWND, window);
    return 1;
}

int mvdm_softpc_wow_page_domain_restore_callback(const unsigned long saved[2])
{
    if (!saved || !domain.active || domain.owner != session_thread_current() ||
            !(c_getCR0() & MVDM_SOFTPC_CR0_PG) || c_getCR3() != domain.directory)
        return 0;
    /* ssend.c UNLOCKPWND restores the outer borrow before its own unlock. */
    c_sas_storedw(domain.guest_teb + MVDM_SOFTPC_TEB_CACHED_WND, saved[1]);
    c_sas_storedw(domain.guest_teb + MVDM_SOFTPC_TEB_CACHED_HWND, saved[0]);
    return 1;
}

/* HMValidateHandle rebases only desktop-owned server pointers. All projected
 * WND/CLS allocations use this worker's one client delta, but nt_mem may put
 * them outside the initial desktop-root page. Publish the containing interval
 * before exposing each object. No server-form alias is mapped; this changes
 * no lease, allocator or HANDLEENTRY validation. Like a heap reservation, the
 * interval survives individual frees until worker teardown. */
static void page_domain_include_object(ULONG server, ULONG size)
{
    ULONG desktop = domain.guest_desktop_info;
    ULONG base = c_sas_dw_at(desktop + MVDM_SOFTPC_DESKTOP_BASE);
    ULONG limit = c_sas_dw_at(desktop + MVDM_SOFTPC_DESKTOP_LIMIT);
    if (server < base)
        c_sas_storedw(desktop + MVDM_SOFTPC_DESKTOP_BASE, server);
    if (server + size > limit)
        c_sas_storedw(desktop + MVDM_SOFTPC_DESKTOP_LIMIT, server + size);
}

int mvdm_softpc_wow_page_domain_publish_handle(unsigned short index,
    unsigned short uniqueness, unsigned char type, unsigned char flags,
    unsigned long window, unsigned long class_server)
{
    ULONG guest_window;
    ULONG server_window;
    ULONG entry;
    WINDOWINFO info = { sizeof(info) };
    NTSTATUS status;

    if (!domain.active || !index || index == 0xffffu || index == 1u ||
            !domain.client_delta || domain.owner != session_thread_current() ||
            !(c_getCR0() & MVDM_SOFTPC_CR0_PG) || c_getCR3() != domain.directory)
        return 0;
    if (domain.guest_windows[index] != 0u) return 0;
    /* A native lookup failure must not publish a successful empty WND.
     * Complete sampling before allocating or exposing the guest record. */
    if (window != 0u && !GetWindowInfo((HWND)(ULONG_PTR)window, &info))
        return 0;
    guest_window = 0u;
    status = VdmAllocateVirtualMemory(&guest_window, MVDM_SOFTPC_WND_BYTES,
        TRUE);
    if (status < 0) return 0;
    if (guest_window > 0xffffffffu - domain.client_delta - MVDM_SOFTPC_WND_BYTES) {
        (void)VdmFreeVirtualMemory(guest_window);
        return 0;
    }
    server_window = guest_window + domain.client_delta;
    entry = domain.guest_handle_table + (ULONG)index *
        MVDM_SOFTPC_HANDLE_ENTRY_BYTES;
    c_sas_fills(guest_window, 0u, MVDM_SOFTPC_WND_BYTES);
    /* The host HWND is sampled only while this call runs. No native pointer
     * is retained in the original-layout client view. */
    c_sas_storedw(guest_window + MVDM_SOFTPC_WND_HEAD_HANDLE,
        ((ULONG)uniqueness << 16) | index);
    c_sas_storedw(guest_window + MVDM_SOFTPC_WND_HEAD_SELF, server_window);
    c_sas_storedw(guest_window + MVDM_SOFTPC_WND_CLASS, class_server);
    page_domain_window_info(guest_window, &info);
    page_domain_include_object(server_window, MVDM_SOFTPC_WND_BYTES);
    c_sas_storedw(entry, server_window);
    c_sas_storedw(entry + 4u, 0u);
    c_sas_storedw(entry + 8u, ((ULONG)uniqueness << 16) |
        ((ULONG)flags << 8) | type);
    domain.guest_windows[index] = guest_window;
    return 1;
}

int mvdm_softpc_wow_page_domain_retire_handle(unsigned short index,
    unsigned long *retained_backing)
{
    ULONG entry;
    ULONG guest_window;

    if (!domain.active || !index || index == 0xffffu || index == 1u ||
            domain.owner != session_thread_current() ||
            !(c_getCR0() & MVDM_SOFTPC_CR0_PG) || c_getCR3() != domain.directory)
        return 0;
    guest_window = domain.guest_windows[index];
    if (!guest_window) return 0;
    /* Original HMMarkObjectDestroy defers physical destruction while the
     * caller retains a WND borrow. Transfer that backing to the existing WW
     * owner, not to a second object registry. Unborrowed callers free here. */
    if (!retained_backing && VdmFreeVirtualMemory(guest_window) < 0) return 0;
    entry = domain.guest_handle_table +
        (ULONG)index * MVDM_SOFTPC_HANDLE_ENTRY_BYTES;
    c_sas_storedw(entry, (ULONG)index + 1u);
    c_sas_storedw(entry + 4u, 0u);
    c_sas_storedw(entry + 8u, 0x00010000u);
    domain.guest_windows[index] = 0u;
    if (retained_backing) *retained_backing = guest_window;
    return 1;
}

unsigned long mvdm_softpc_wow_page_domain_publish_class(unsigned short atom,
    unsigned short fnid, unsigned long flags, unsigned long wow0, unsigned long wow1,
    unsigned long task, unsigned long style, unsigned long procedure,
    unsigned long window_extra, unsigned long module)
{
    ULONG client = 0u;
    ULONG server;
    mvdm_softpc_wow_class_allocation *allocation;

    if (!domain.active || !atom || !domain.client_delta ||
            domain.owner != session_thread_current() ||
            !(c_getCR0() & MVDM_SOFTPC_CR0_PG) || c_getCR3() != domain.directory)
        return 0u;
    allocation = HeapAlloc(GetProcessHeap(), 0u, sizeof(*allocation));
    if (allocation == NULL) return 0u;
    if (VdmAllocateVirtualMemory(&client, MVDM_SOFTPC_CLS_BYTES, TRUE) < 0)
        goto fail;
    if (client > 0xffffffffu - domain.client_delta - MVDM_SOFTPC_CLS_BYTES) {
        (void)VdmFreeVirtualMemory(client);
        goto fail;
    }
    server = client + domain.client_delta;
    c_sas_fills(client, 0u, MVDM_SOFTPC_CLS_BYTES);
    /* This selected original server CLS shape carries numeric source fields.
     * It deliberately omits native menu/cursor/object identities: none may
     * cross into the guest client view. */
    c_sas_storedw(client + MVDM_SOFTPC_CLS_ATOM, atom);
    c_sas_storew(client + MVDM_SOFTPC_CLS_FNID, fnid);
    c_sas_storedw(client + MVDM_SOFTPC_CLS_FLAGS, flags);
    c_sas_storedw(client + MVDM_SOFTPC_CLS_WOW0, wow0);
    c_sas_storedw(client + MVDM_SOFTPC_CLS_WOW1, wow1);
    c_sas_storedw(client + MVDM_SOFTPC_CLS_TASK, task);
    c_sas_storedw(client + MVDM_SOFTPC_CLS_STYLE, style);
    c_sas_storedw(client + MVDM_SOFTPC_CLS_PROCEDURE, procedure);
    c_sas_storedw(client + MVDM_SOFTPC_CLS_WINDOW_EXTRA, window_extra);
    /* hModule is a server-side native identity in the original CLS.  The
     * standalone client view cannot expose a host HMODULE as a guest value;
     * this provider has no source-proven 16-bit module conversion yet. */
    (void)module;
    c_sas_storedw(client + MVDM_SOFTPC_CLS_MODULE, 0u);
    page_domain_include_object(server, MVDM_SOFTPC_CLS_BYTES);
    allocation->client = client;
    allocation->next = class_allocations;
    class_allocations = allocation;
    return server;
fail:
    HeapFree(GetProcessHeap(), 0u, allocation);
    return 0u;
}

int mvdm_softpc_wow_page_domain_retire_class(unsigned long server)
{
    ULONG client;
    mvdm_softpc_wow_class_allocation **link;
    mvdm_softpc_wow_class_allocation *allocation;

    if (!domain.active || server <= domain.client_delta ||
            domain.owner != session_thread_current() ||
            !(c_getCR0() & MVDM_SOFTPC_CR0_PG) || c_getCR3() != domain.directory)
        return 0;
    client = server - domain.client_delta;
    for (link = &class_allocations; (allocation = *link) != NULL;
            link = &allocation->next) {
        if (allocation->client == client) {
            if (VdmFreeVirtualMemory(client) < 0) return 0;
            *link = allocation->next;
            HeapFree(GetProcessHeap(), 0u, allocation);
            return 1;
        }
    }
    return 0;
}
