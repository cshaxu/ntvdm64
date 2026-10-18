#include "mvdm_softpc_wow_page_domain.h"
#include "mvdm_softpc_effective_address.h"

#include <windows.h>
#include <stdlib.h>

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
extern uint8_t *c_GetLinAdd(uint32_t address);
extern uint8_t *c_GetPhyAdd(uint32_t address);
extern int xtrn2phy(uint32_t address, uint8_t access, uint32_t *physical);
extern uint32_t c_getCPL(void);
extern int c_getVM(void);
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
#define MVDM_SOFTPC_MENU_BYTES 64u
#define MVDM_SOFTPC_MENU_ITEM_BYTES 64u
#define MVDM_SOFTPC_MENU_TASK 20u
#define MVDM_SOFTPC_MENU_CAPACITY 36u
#define MVDM_SOFTPC_MENU_COUNT 40u
#define MVDM_SOFTPC_MENU_ITEMS 56u
#define MVDM_SOFTPC_DESKTOP_INFO_BYTES 100u
#define MVDM_SOFTPC_CLIENT_OBJECT_BYTES MVDM_SOFTPC_PAGE_BYTES
/* Original USER16 REBASEPTR tests against 0x7ffeffff before subtracting
 * head.pSelf. A server-form value below that boundary is instead treated as
 * an already-client pointer (GetMenu then dereferences it directly). Keep
 * every projected server object in the original high address domain. */
#define MVDM_SOFTPC_CLIENT_DELTA 0x80000000u
#define MVDM_SOFTPC_WND_HEAD_HANDLE 0u
#define MVDM_SOFTPC_WND_HEAD_SELF 12u
#define MVDM_SOFTPC_WND_NEXT 20u
#define MVDM_SOFTPC_WND_PARENT 24u
#define MVDM_SOFTPC_WND_CHILD 28u
#define MVDM_SOFTPC_WND_OWNER 32u
#define MVDM_SOFTPC_WND_RECT 40u
#define MVDM_SOFTPC_WND_CLIENT_RECT 56u
#define MVDM_SOFTPC_WND_EXSTYLE 164u
#define MVDM_SOFTPC_WND_STYLE 168u
#define MVDM_SOFTPC_WND_CLASS 76u
#define MVDM_SOFTPC_WND_MENU_OR_ID 104u
#define MVDM_SOFTPC_WND_FNID 122u
#define MVDM_SOFTPC_WND_WOW_WORDS 144u
/* Original user.h FNID_DESKTOP; _IsWindowVisible terminates at this root. */
#define MVDM_SOFTPC_FNID_DESKTOP 0x029du
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
#define MVDM_SOFTPC_CLS_ANSI_NAME 100u

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
    ULONG desktop_handle;
    ULONG client_delta;
    ULONG guest_windows[MVDM_SOFTPC_HANDLE_ENTRY_COUNT];
    ULONG guest_menus[MVDM_SOFTPC_HANDLE_ENTRY_COUNT];
    ULONG guest_menu_items[MVDM_SOFTPC_HANDLE_ENTRY_COUNT];
    BYTE local_menu_state[MVDM_SOFTPC_HANDLE_ENTRY_COUNT];
    ULONG wow_gdt;
    ULONG dosx_gdt;
    USHORT dosx_gdt_limit;
    int wow_context_selected;
    int active;
} mvdm_softpc_wow_page_domain;

typedef struct mvdm_softpc_wow_class_allocation {
    struct mvdm_softpc_wow_class_allocation *next;
    ULONG client;
    int observed;
    int released;
} mvdm_softpc_wow_class_allocation;

static mvdm_softpc_wow_page_domain domain;
static mvdm_softpc_wow_class_allocation *class_allocations;

/* Failed physical frees after an atomic menu commit are lifetime obligations,
 * not published menu identities or an alternate observation cache. */
typedef struct page_domain_retired_storage {
    struct page_domain_retired_storage *next;
    ULONG address;
} page_domain_retired_storage;
static page_domain_retired_storage *retired_menu_storage;

static void page_domain_collect_menu_storage(void)
{
    page_domain_retired_storage **link = &retired_menu_storage, *record;
    while ((record = *link) != NULL) {
        if (record->address && VdmFreeVirtualMemory(record->address) < 0) {
            link = &record->next;
            continue;
        }
        *link = record->next;
        HeapFree(GetProcessHeap(), 0, record);
    }
}

static void page_domain_defer_menu_storage(page_domain_retired_storage *record)
{
    if (!record) return;
    record->next = retired_menu_storage;
    retired_menu_storage = record;
}

/* Live observation metadata, synchronous borrows and deferred retired
 * allocations share these backing-lifetime records. Publication remains
 * indexed by guest_windows; these records own no native HWND/task. A live
 * zero-reader record retains PID/TID, not an invented caller reference. */
typedef struct mvdm_softpc_wow_window_borrow {
    struct mvdm_softpc_wow_window_borrow *next;
    ULONG backing, references;
    DWORD process_id, thread_id, module, context_help_id;
    int retired, retained_owner;
    /* Publication provenance belongs to this WND generation, not its CLS.
     * A native class can have both observed and WOW-subclassed windows.
     * Written only by the managed/observation publication boundary. */
    int managed;
    void *host_storage;
    mvdm_softpc_wow_host_window_dispose dispose_host_storage;
} mvdm_softpc_wow_window_borrow;
static void dispose_window_borrow(mvdm_softpc_wow_window_borrow *borrow)
{
    DWORD error = GetLastError();
    void *storage = borrow->host_storage;
    mvdm_softpc_wow_host_window_dispose dispose = borrow->dispose_host_storage;
    borrow->host_storage = NULL;
    borrow->dispose_host_storage = NULL;
    /* Pure host deallocation only; the containing generation is unlinked.
     * Guest backing may already have been decommitted. */
    if (dispose) dispose(storage);
    HeapFree(GetProcessHeap(), 0, borrow);
    SetLastError(error);
}
static mvdm_softpc_wow_window_borrow *window_borrows;
static mvdm_softpc_wow_window_borrow **window_borrow_link(ULONG backing);
static int page_domain_window_observed(ULONG backing)
{
    mvdm_softpc_wow_window_borrow *borrow = *window_borrow_link(backing);
    return borrow && !borrow->retired && !borrow->managed;
}
static int page_domain_window_referenced(ULONG server);
static void page_domain_withdraw_window(WORD index, ULONG guest_window,
    mvdm_softpc_wow_window_borrow *borrow, int retained_owner);

static int page_domain_client_desktop(ULONG desktop_info, ULONG delta);
static int page_domain_menu_context(void);

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
    mvdm_softpc_wow_window_borrow *borrow;
    page_domain_retired_storage *storage;
    unsigned index;

    mvdm_softpc_wow_page_domain_leave_protected();
    page_domain_collect_menu_storage();
    while ((storage = retired_menu_storage) != NULL) {
        retired_menu_storage = storage->next;
        HeapFree(GetProcessHeap(), 0, storage); /* VDM allocation domain tears down next. */
    }
    if (value->clock_stop != NULL) SetEvent(value->clock_stop);
    if (value->clock_thread != NULL) {
        WaitForSingleObject(value->clock_thread, INFINITE);
        CloseHandle(value->clock_thread);
    }
    /* The provider normally retires every original-layout object itself.
     * Worker teardown is the exceptional backstop: no outstanding client
     * object may survive this worker's VDM allocation domain. */
    for (index = 0u; index < MVDM_SOFTPC_HANDLE_ENTRY_COUNT; ++index) {
        if (value->guest_menus[index] != 0u) {
            ULONG items = value->guest_menu_items[index];
            if (items) (void)VdmFreeVirtualMemory(items);
            (void)VdmFreeVirtualMemory(value->guest_menus[index]);
            value->guest_menus[index] = 0u;
            value->guest_menu_items[index] = 0u;
        }
        if (value->guest_windows[index] != 0u) {
            (void)VdmFreeVirtualMemory(value->guest_windows[index]);
            value->guest_windows[index] = 0u;
        }
    }
    while ((borrow = window_borrows) != NULL) {
        window_borrows = borrow->next;
        if (borrow->retired) (void)VdmFreeVirtualMemory(borrow->backing);
        dispose_window_borrow(borrow);
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
    /* USER16 GetDesktopWindow returns this WND's head.h. Use the native
     * identity in the existing shared namespace, not a synthetic index that
     * could collide with a real window. The backing is still guest-owned. */
    domain.desktop_handle = (ULONG)(ULONG_PTR)GetDesktopWindow();
    if (!LOWORD(domain.desktop_handle) || LOWORD(domain.desktop_handle) == 0xffffu)
        return 0;
    handle = domain.guest_handle_table +
        LOWORD(domain.desktop_handle) * MVDM_SOFTPC_HANDLE_ENTRY_BYTES;
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
        domain.desktop_handle);
    c_sas_storedw(root + MVDM_SOFTPC_WND_HEAD_SELF, server_root);
    c_sas_storew(root + MVDM_SOFTPC_WND_FNID, MVDM_SOFTPC_FNID_DESKTOP);
    c_sas_storedw(root + MVDM_SOFTPC_WND_RECT + 8u, (ULONG)width);
    c_sas_storedw(root + MVDM_SOFTPC_WND_RECT + 12u, (ULONG)height);
    c_sas_storedw(root + MVDM_SOFTPC_WND_CLIENT_RECT + 8u, (ULONG)width);
    c_sas_storedw(root + MVDM_SOFTPC_WND_CLIENT_RECT + 12u, (ULONG)height);
    c_sas_storedw(root + MVDM_SOFTPC_WND_EXSTYLE, 0u);
    c_sas_storedw(root + MVDM_SOFTPC_WND_STYLE, WS_VISIBLE);
    c_sas_storedw(handle, server_root);
    c_sas_storedw(handle + 4u, 0u);
    c_sas_storedw(handle + 8u, (domain.desktop_handle & 0xffff0000u) | 1u);
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

/* Original user.h TestwndChild compares the complete WFTYPEMASK, not just
 * WS_CHILD. The WS_POPUP|WS_CHILD form has a menu role, not a numeric ID.
 * This carrier has copied style values rather than a native/original PWND. */
static int page_domain_child_style(DWORD style)
{
    return (style & (WS_CHILD | WS_POPUP)) == WS_CHILD;
}

static void page_domain_copy_owned_ram(ULONG address, void *output, ULONG bytes);

/* ADAPTER-WOW-081: only the serialized page-domain's allocated WND/CLS
 * fields, never arbitrary guest/device RAM. Match bios_write_double's
 * original access request, including its CPL and VM rules. Failed mapping
 * and descriptor diagnostics retain its scalar virtualisation/trace path. */
static void page_domain_store_owned_words(ULONG address, const ULONG *words,
    ULONG count)
{
    uint32_t physical;
    uint8_t access = 1u; /* PG_W */
    ULONG i, bytes = count * sizeof(*words);
    if (!mvdm_softpc_wow_source_descriptor_trace_active &&
            bytes <= MVDM_SOFTPC_PAGE_BYTES &&
            (address & (MVDM_SOFTPC_PAGE_BYTES - 1u)) <=
                MVDM_SOFTPC_PAGE_BYTES - bytes) {
        if (c_getCPL() != 3u) access |= 2u; /* original PG_U */
        if (c_getVM()) access |= 4u; /* original accessed/dirty handling */
        if (xtrn2phy(address, access, &physical)) {
            CopyMemory(c_GetPhyAdd(physical), words, bytes);
            return;
        }
    }
    for (i = 0; i < count; ++i)
        c_sas_storedw(address + i * sizeof(*words), words[i]);
}

static void page_domain_window_fields(ULONG guest_window, const WINDOWINFO *info,
    LONG control_id)
{
    ULONG rectangles[8], styles[2] = {info->dwExStyle, info->dwStyle};

    /* Original rtl/wow.c::_ClientToScreen and _GetClientRect consume a
     * screen-relative WND.rcClient, not GetClientRect's zero-origin result.
     * GetWindowInfo supplies both rectangles in that original coordinate
     * space. No native pointer or native WND layout enters guest memory. */
    CopyMemory(rectangles, &info->rcWindow, sizeof(RECT));
    CopyMemory(rectangles + 4u, &info->rcClient, sizeof(RECT));
    page_domain_store_owned_words(guest_window + MVDM_SOFTPC_WND_RECT,
        rectangles, 8u);
    page_domain_store_owned_words(guest_window + MVDM_SOFTPC_WND_EXSTYLE,
        styles, 2u);
    if (page_domain_child_style(info->dwStyle))
        c_sas_storedw(guest_window + MVDM_SOFTPC_WND_MENU_OR_ID,
            (ULONG)control_id);
}

static int page_domain_window_info(ULONG guest_window, const WINDOWINFO *info,
    HWND window)
{
    LONG control_id = 0;
    LONG module = 0;
    DWORD context_help_id = 0;
    mvdm_softpc_wow_window_borrow *borrow = *window_borrow_link(guest_window);
    /* Sample native state before the non-failing copied-field commit. */
    if (page_domain_child_style(info->dwStyle)) {
        SetLastError(ERROR_SUCCESS);
        control_id = GetWindowLongA(window, GWL_ID);
        if (!control_id && GetLastError() != ERROR_SUCCESS) return 0;
    }
    if (window && borrow) {
        SetLastError(ERROR_SUCCESS);
        module = GetWindowLongA(window, GWL_HINSTANCE);
        if (!module && GetLastError() != ERROR_SUCCESS) return 0;
        context_help_id = GetWindowContextHelpId(window);
    }
    page_domain_window_fields(guest_window, info, control_id);
    if (borrow) {
        borrow->module = (DWORD)module;
        borrow->context_help_id = context_help_id;
    }
    return 1;
}

int mvdm_softpc_wow_page_domain_refresh_window(unsigned long window)
{
    WORD index = LOWORD(window);
    ULONG guest_window;
    WINDOWINFO info = { sizeof(info) };

    if (!domain.active || !index || index == 0xffffu ||
            domain.owner != session_thread_current() ||
            !(c_getCR0() & MVDM_SOFTPC_CR0_PG) || c_getCR3() != domain.directory)
        return 0;
    guest_window = window == domain.desktop_handle ?
        domain.guest_desktop_window : domain.guest_windows[index];
    if (!guest_window || c_sas_dw_at(guest_window + MVDM_SOFTPC_WND_HEAD_HANDLE)
            != window || !GetWindowInfo((HWND)(ULONG_PTR)window, &info))
        return 0;
    return page_domain_window_info(guest_window, &info,
        (HWND)(ULONG_PTR)window);
}

int mvdm_softpc_wow_page_domain_callback_window(unsigned long window,
    unsigned long saved[2])
{
    ULONG backing;
    if (!saved || !mvdm_softpc_wow_page_domain_refresh_window(window)) return 0;
    backing = window == domain.desktop_handle ?
        domain.guest_desktop_window : domain.guest_windows[LOWORD(window)];
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

/* ADAPTER-WOW-087: original USER has one CLIENTINFO per native thread. The
 * serialized CCPU uses one guest TEB address, so move only its thread-private
 * projection at the original execution handoff. Do not infer a new HWND,
 * refresh a graph, acquire ownership or consult native USER here. */
static int page_domain_callback_context_valid(const ULONG pair[2])
{
    mvdm_softpc_wow_window_borrow *borrow;
    if (!pair[0] && !pair[1]) return 1;
    if (!pair[0] || !pair[1] ||
            !(borrow = *window_borrow_link(pair[1])) ||
            (!borrow->references && !borrow->retained_owner) ||
            c_sas_dw_at(pair[1] + MVDM_SOFTPC_WND_HEAD_HANDLE) != pair[0])
        return 0;
    return 1; /* Retired but pinned callbacks remain original locked readers. */
}

int mvdm_softpc_wow_page_domain_exchange_client_context(
    unsigned long *previous_callback, unsigned long version,
    const unsigned long next_callback[2])
{
    ULONG previous[2], next[2] = {0, 0};
    DWORD saved = GetLastError();
    if (!page_domain_menu_context()) {
        SetLastError(ERROR_INVALID_STATE); return 0;
    }
    if (next_callback) CopyMemory(next, next_callback, sizeof(next));
    previous[0] = c_sas_dw_at(domain.guest_teb + MVDM_SOFTPC_TEB_CACHED_HWND);
    previous[1] = c_sas_dw_at(domain.guest_teb + MVDM_SOFTPC_TEB_CACHED_WND);
    if (!page_domain_callback_context_valid(next) ||
            (previous_callback && !page_domain_callback_context_valid(previous))) {
        SetLastError(ERROR_INVALID_STATE); return 0;
    }
    /* Preflight above is read-only. No callout/allocation can intervene in
     * this small commit; even aliased caller arrays were snapshotted first. */
    c_sas_storedw(domain.guest_teb + MVDM_SOFTPC_TEB_EXPECTED_WINDOWS_VERSION, version);
    c_sas_storedw(domain.guest_teb + MVDM_SOFTPC_TEB_CACHED_HWND, next[0]);
    c_sas_storedw(domain.guest_teb + MVDM_SOFTPC_TEB_CACHED_WND, next[1]);
    if (previous_callback) CopyMemory(previous_callback, previous, sizeof(previous));
    SetLastError(saved);
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

static mvdm_softpc_wow_class_allocation *page_domain_class_allocation(ULONG server)
{
    mvdm_softpc_wow_class_allocation *allocation;
    for (allocation = class_allocations; allocation; allocation = allocation->next)
        if (allocation->client + domain.client_delta == server) return allocation;
    return NULL;
}

/* Prepare a WND without publishing its handle slot. The ordinary borrow list
 * owns one temporary reference, keeping allocation/CLS alive until commit or
 * abort. This permits a replacement generation without overwriting readers
 * of the previous generation, and uses no second storage registry. */
static ULONG page_domain_prepare_window(ULONG full, HWND window,
    ULONG class_server, const ULONG words[3], const WINDOWINFO *info,
    const LONG *captured_id, mvdm_softpc_wow_window_borrow **record)
{
    mvdm_softpc_wow_window_borrow *pending;
    ULONG backing = 0, server;
    unsigned i;
    mvdm_softpc_wow_class_allocation *cls = page_domain_class_allocation(class_server);
    if (record) *record = NULL;
    if (cls && cls->released) return 0;
    pending = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(*pending));
    if (!pending) return 0;
    if (VdmAllocateVirtualMemory(&backing, MVDM_SOFTPC_WND_BYTES, TRUE) < 0) {
        HeapFree(GetProcessHeap(), 0, pending);
        return 0;
    }
    c_sas_fills(backing, 0u, MVDM_SOFTPC_WND_BYTES);
    pending->backing = backing;
    pending->references = 1;
    if (!captured_id)
        pending->thread_id = GetWindowThreadProcessId(window, &pending->process_id);
    pending->next = window_borrows;
    window_borrows = pending;
    if (backing > 0xffffffffu - domain.client_delta - MVDM_SOFTPC_WND_BYTES)
        goto fail;
    server = backing + domain.client_delta;
    c_sas_storedw(backing + MVDM_SOFTPC_WND_HEAD_HANDLE, full);
    c_sas_storedw(backing + MVDM_SOFTPC_WND_HEAD_SELF, server);
    c_sas_storedw(backing + MVDM_SOFTPC_WND_CLASS, class_server);
    if (captured_id) page_domain_window_fields(backing, info, *captured_id);
    else if (!page_domain_window_info(backing, info, window)) goto fail;
    if (words)
        for (i = 0; i < 3u; ++i)
            c_sas_storedw(backing + MVDM_SOFTPC_WND_WOW_WORDS + i * 4u, words[i]);
    if (record) *record = pending;
    return backing;
fail:
    pending->retired = 1;
    (void)mvdm_softpc_wow_page_domain_release_window(backing);
    return 0;
}

/* Publishing consumes no allocation and leaves temporary-reference release
 * to the caller. It may follow withdrawal of an older generation's slot. */
static void page_domain_commit_window(WORD index, ULONG backing, BYTE type,
    BYTE flags)
{
    ULONG entry = domain.guest_handle_table + (ULONG)index * MVDM_SOFTPC_HANDLE_ENTRY_BYTES;
    ULONG full = c_sas_dw_at(backing + MVDM_SOFTPC_WND_HEAD_HANDLE);
    page_domain_include_object(backing + domain.client_delta, MVDM_SOFTPC_WND_BYTES);
    c_sas_storedw(entry, backing + domain.client_delta);
    c_sas_storedw(entry + 4u, 0u);
    c_sas_storedw(entry + 8u, (full & 0xffff0000u) | ((ULONG)flags << 8) | type);
    domain.guest_windows[index] = backing;
}

static int page_domain_publish_handle(unsigned short index,
    unsigned short uniqueness, unsigned char type, unsigned char flags,
    unsigned long window, unsigned long class_server, const unsigned long words[3],
    int managed)
{
    ULONG guest_window;
    ULONG entry;
    WINDOWINFO info = { sizeof(info) };
    mvdm_softpc_wow_class_allocation *next_class;
    unsigned i;

    if (!domain.active || !index || index == 0xffffu ||
            index == LOWORD(domain.desktop_handle) ||
            !domain.client_delta || domain.owner != session_thread_current() ||
            !(c_getCR0() & MVDM_SOFTPC_CR0_PG) || c_getCR3() != domain.directory)
        return 0;
    if (domain.guest_menus[index] != 0u) return 0;
    next_class = page_domain_class_allocation(class_server);
    if (next_class && next_class->released) return 0;
    /* A native lookup failure must not publish a successful empty WND.
     * Complete sampling before allocating or exposing the guest record. */
    if (window != 0u && !GetWindowInfo((HWND)(ULONG_PTR)window, &info))
        return 0;
    guest_window = domain.guest_windows[index];
    if (guest_window && window && c_sas_dw_at(guest_window) != window) {
        ULONG replacement, retained = 0u;
        /* Native USER may reuse a dead observation during creation, before
         * the outer native return refreshes copied links. Never overwrite a
         * live HWND or managed owner. Prepare first, then use the existing
         * retained retirement so old readers/links still own old storage. */
        if (type != 1u || window != (((ULONG)uniqueness << 16) | index) ||
                !page_domain_window_observed(guest_window) ||
                IsWindow((HWND)(ULONG_PTR)c_sas_dw_at(guest_window))) return 0;
        replacement = page_domain_prepare_window(window, (HWND)(ULONG_PTR)window,
            class_server, words, &info, NULL, NULL);
        if (!replacement) return 0;
        if (!mvdm_softpc_wow_page_domain_retire_handle(index, &retained)) {
            (*window_borrow_link(replacement))->retired = 1;
            (void)mvdm_softpc_wow_page_domain_release_window(replacement);
            return 0;
        }
        (*window_borrow_link(replacement))->managed = managed;
        page_domain_commit_window(index, replacement, type, flags);
        (void)mvdm_softpc_wow_page_domain_release_window(replacement);
        (void)mvdm_softpc_wow_page_domain_release_retained_window(retained);
        return 1;
    }
    if (guest_window) {
        /* Native discovery can precede the existing managed creation owner.
         * Adopt only that identical observed WND, never a reused slot or an
         * already-managed publication. Preserve backing and links, including
         * outstanding synchronous readers; explicit creator words win.
         * Cleanup/task ownership is still installed by the managed caller. */
        if (!managed || !window || type != 1u || window != (((ULONG)uniqueness << 16) | index) ||
                c_sas_dw_at(guest_window) != window) return 0;
        if (!page_domain_window_observed(guest_window)) return 0;
        /* Subclassing an existing native window does not replace its class.
         * Creation may instead supply its recovered registration's CLS. */
        if (!class_server)
            class_server = c_sas_dw_at(guest_window + MVDM_SOFTPC_WND_CLASS);
        next_class = page_domain_class_allocation(class_server);
        if (!next_class || next_class->released) return 0;
        if (!page_domain_window_info(guest_window, &info,
                (HWND)(ULONG_PTR)window)) return 0;
        c_sas_storedw(guest_window + MVDM_SOFTPC_WND_CLASS, class_server);
        if (words)
            for (i = 0; i < 3u; ++i)
                c_sas_storedw(guest_window + MVDM_SOFTPC_WND_WOW_WORDS + i * 4u, words[i]);
        entry = domain.guest_handle_table + (ULONG)index * MVDM_SOFTPC_HANDLE_ENTRY_BYTES;
        c_sas_storedw(entry + 8u, ((ULONG)uniqueness << 16) |
            ((ULONG)flags << 8) | type);
        (*window_borrow_link(guest_window))->managed = 1;
        /* The existing observed-class sweep reclaims the old query class.
         * Do not discard its allocation record on a failed release. */
        return 1;
    }
    guest_window = page_domain_prepare_window(((ULONG)uniqueness << 16) | index,
        (HWND)(ULONG_PTR)window, class_server, words, &info, NULL, NULL);
    if (!guest_window) return 0;
    (*window_borrow_link(guest_window))->managed = managed;
    page_domain_commit_window(index, guest_window, type, flags);
    (void)mvdm_softpc_wow_page_domain_release_window(guest_window);
    return 1;
}

int mvdm_softpc_wow_page_domain_publish_handle(unsigned short index,
    unsigned short uniqueness, unsigned char type, unsigned char flags,
    unsigned long window, unsigned long class_server, const unsigned long words[3])
{
    return page_domain_publish_handle(index, uniqueness, type, flags,
        window, class_server, words, 1);
}

int mvdm_softpc_wow_page_domain_cancel_native_adoption(ULONG window, ULONG backing)
{
    mvdm_softpc_wow_window_borrow *borrow;
    mvdm_softpc_wow_class_allocation *cls;
    if (!domain.active || !domain.client_delta || domain.owner != session_thread_current() ||
            !(c_getCR0() & MVDM_SOFTPC_CR0_PG) || c_getCR3() != domain.directory ||
            !backing || domain.guest_windows[LOWORD(window)] != backing ||
            c_sas_dw_at(backing + MVDM_SOFTPC_WND_HEAD_HANDLE) != window)
        return 0;
    borrow = *window_borrow_link(backing);
    cls = page_domain_class_allocation(c_sas_dw_at(backing + MVDM_SOFTPC_WND_CLASS));
    /* Only adoption retaining its observed native CLS is reversible here.
     * No destruction, handle withdrawal, class replacement or word rewrite. */
    if (!borrow || borrow->retired || !borrow->managed || !cls || !cls->observed)
        return 0;
    borrow->managed = 0;
    return 1;
}

/* The public USER owner supplies a complete copied node. No native API or
 * guest callback runs between validation and publication. Original MENU and
 * ITEM offsets are asserted by wow_user_client_view_layout.h; unused server
 * drawing/locking fields are not synthesized. The unchanged USER16 query
 * routines consume head.h/pSelf, cItems/rgItems and the first four ITEM words. */
static int page_domain_menu_context(void)
{
    return domain.active && domain.client_delta &&
        domain.owner == session_thread_current() &&
        (c_getCR0() & MVDM_SOFTPC_CR0_PG) && c_getCR3() == domain.directory;
}

static ULONG page_domain_menu_lookup(ULONG menu)
{
    WORD index = LOWORD(menu);
    ULONG client;
    if (!index || index == LOWORD(domain.desktop_handle) || index == 0xffffu) return 0u;
    client = domain.guest_menus[index];
    return client && c_sas_dw_at(client) == menu ? client : 0u;
}

typedef struct page_domain_menu_prepared {
    ULONG client, storage;
    page_domain_retired_storage *fresh_client, *fresh_items, *old_items;
    int changed;
} page_domain_menu_prepared;

static BOOL page_domain_prepare_menu_storage(ULONG bytes,
    page_domain_retired_storage **output)
{
    page_domain_retired_storage *record = HeapAlloc(GetProcessHeap(),
        HEAP_ZERO_MEMORY, sizeof(*record));
    if (!record) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return FALSE; }
    *output = record;
    if (VdmAllocateVirtualMemory(&record->address, bytes, TRUE) < 0) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY); return FALSE;
    }
    if (bytes > 0xffffffffu - domain.client_delta ||
            record->address > 0xffffffffu - domain.client_delta - bytes) {
        SetLastError(ERROR_ARITHMETIC_OVERFLOW); return FALSE;
    }
    c_sas_fills(record->address, 0u, bytes);
    return TRUE;
}

int mvdm_softpc_wow_page_domain_publish_menus(ULONG count,
    const mvdm_softpc_wow_menu_observation *menus, ULONG item_count,
    const mvdm_softpc_wow_menu_query_item *items)
{
    page_domain_menu_prepared *prepared = NULL;
    ULONG *index = NULL, i, j;
    DWORD error, saved = GetLastError();
    int result = 0;
    if (!page_domain_menu_context()) { SetLastError(ERROR_INVALID_STATE); return 0; }
    if (!count || count >= 0x10000u || !menus || (item_count && !items) ||
            item_count > 0xffffffffu / MVDM_SOFTPC_MENU_ITEM_BYTES) {
        SetLastError(ERROR_INVALID_PARAMETER); return 0;
    }
    prepared = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, count * sizeof(*prepared));
    index = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 0x10000u * sizeof(*index));
    if (!prepared || !index) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); goto done; }
    for (i = 0; i < count; ++i) {
        const mvdm_softpc_wow_menu_observation *row = &menus[i];
        page_domain_menu_prepared *value = &prepared[i];
        WORD slot = LOWORD(row->menu);
        if (!slot || slot == LOWORD(domain.desktop_handle) || slot == 0xffffu ||
                index[slot] || domain.guest_windows[slot] ||
                row->local_state > 1u ||
                row->first_item > item_count || row->item_count > item_count - row->first_item) {
            SetLastError(ERROR_INVALID_PARAMETER); goto done;
        }
        index[slot] = i + 1u;
        value->client = domain.guest_menus[slot];
        value->storage = domain.guest_menu_items[slot];
        if (value->client && (c_sas_dw_at(value->client) != row->menu ||
                c_sas_dw_at(value->client + MVDM_SOFTPC_MENU_TASK) != row->task)) {
            SetLastError(ERROR_INVALID_MENU_HANDLE); goto done;
        }
        if (!value->client) {
            if (!page_domain_prepare_menu_storage(MVDM_SOFTPC_MENU_BYTES,
                    &value->fresh_client)) goto done;
            value->client = value->fresh_client->address;
        }
    }
    for (i = 0; i < count; ++i) {
        const mvdm_softpc_wow_menu_observation *row = &menus[i];
        page_domain_menu_prepared *value = &prepared[i];
        value->changed = value->fresh_client ||
            c_sas_dw_at(value->client + MVDM_SOFTPC_MENU_COUNT) != row->item_count;
        /* First resolve the entire row, even if its contents are unchanged. */
        for (j = 0; j < row->item_count; ++j) {
            const mvdm_softpc_wow_menu_query_item *item = &items[row->first_item + j];
            ULONG submenu = 0, child = index[LOWORD(item->submenu)];
            if (item->submenu) {
                if (child) {
                    if (child > i || menus[child - 1u].menu != item->submenu) {
                        SetLastError(ERROR_INVALID_DATA); goto done;
                    }
                    submenu = prepared[child - 1u].client;
                } else submenu = page_domain_menu_lookup(item->submenu);
                if (!submenu) { SetLastError(ERROR_INVALID_MENU_HANDLE); goto done; }
                submenu += domain.client_delta;
            }
            if (!value->changed) {
                ULONG old = value->storage + j * MVDM_SOFTPC_MENU_ITEM_BYTES;
                if (!value->storage || c_sas_dw_at(old) != item->type ||
                        c_sas_dw_at(old + 4u) != item->state ||
                        c_sas_dw_at(old + 8u) != item->id ||
                        c_sas_dw_at(old + 12u) != submenu) value->changed = 1;
            }
        }
        if (!value->changed) continue;
        if (value->storage) {
            value->old_items = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                sizeof(*value->old_items));
            if (!value->old_items) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); goto done; }
            value->old_items->address = value->storage;
        }
        value->storage = 0;
        if (row->item_count) {
            if (!page_domain_prepare_menu_storage(row->item_count * MVDM_SOFTPC_MENU_ITEM_BYTES,
                    &value->fresh_items)) goto done;
            value->storage = value->fresh_items->address;
        }
        for (j = 0; j < row->item_count; ++j) {
            const mvdm_softpc_wow_menu_query_item *item = &items[row->first_item + j];
            ULONG at = value->storage + j * MVDM_SOFTPC_MENU_ITEM_BYTES;
            ULONG child = index[LOWORD(item->submenu)];
            ULONG submenu = item->submenu ? (child ? prepared[child - 1u].client :
                page_domain_menu_lookup(item->submenu)) + domain.client_delta : 0;
            c_sas_storedw(at, item->type); c_sas_storedw(at + 4u, item->state);
            c_sas_storedw(at + 8u, item->id); c_sas_storedw(at + 12u, submenu);
        }
    }
    /* No allocation, USER call, guest entry or fallible release below here. */
    for (i = 0; i < count; ++i) {
        const mvdm_softpc_wow_menu_observation *row = &menus[i];
        page_domain_menu_prepared *value = &prepared[i];
        ULONG entry, slot = LOWORD(row->menu);
        if (row->local_state) domain.local_menu_state[slot] = 1;
        if (!value->changed) continue;
        if (value->fresh_client) {
            c_sas_storedw(value->client, row->menu);
            c_sas_storedw(value->client + 12u, value->client + domain.client_delta);
            c_sas_storedw(value->client + MVDM_SOFTPC_MENU_TASK, row->task);
        }
        if (value->storage) page_domain_include_object(value->storage + domain.client_delta,
            row->item_count * MVDM_SOFTPC_MENU_ITEM_BYTES);
        page_domain_include_object(value->client + domain.client_delta, MVDM_SOFTPC_MENU_BYTES);
        c_sas_storedw(value->client + MVDM_SOFTPC_MENU_ITEMS,
            value->storage ? value->storage + domain.client_delta : 0u);
        c_sas_storedw(value->client + MVDM_SOFTPC_MENU_CAPACITY, row->item_count);
        c_sas_storedw(value->client + MVDM_SOFTPC_MENU_COUNT, row->item_count);
        entry = domain.guest_handle_table + slot * MVDM_SOFTPC_HANDLE_ENTRY_BYTES;
        c_sas_storedw(entry, value->client + domain.client_delta);
        c_sas_storedw(entry + 4u, 0u);
        c_sas_storedw(entry + 8u, (row->menu & 0xffff0000u) | 2u);
        domain.guest_menus[slot] = value->client;
        domain.guest_menu_items[slot] = value->storage;
    }
    result = 1;
done:
    error = result ? saved : GetLastError();
    if (prepared) for (i = 0; i < count; ++i) {
        page_domain_menu_prepared *value = &prepared[i];
        if (result) {
            page_domain_defer_menu_storage(value->old_items);
            if (value->fresh_client) HeapFree(GetProcessHeap(), 0, value->fresh_client);
            if (value->fresh_items) HeapFree(GetProcessHeap(), 0, value->fresh_items);
        } else {
            page_domain_defer_menu_storage(value->fresh_client);
            page_domain_defer_menu_storage(value->fresh_items);
            if (value->old_items) HeapFree(GetProcessHeap(), 0, value->old_items);
        }
    }
    page_domain_collect_menu_storage();
    if (index) HeapFree(GetProcessHeap(), 0, index);
    if (prepared) HeapFree(GetProcessHeap(), 0, prepared);
    SetLastError(error);
    return result;
}

unsigned long mvdm_softpc_wow_page_domain_publish_menu(ULONG menu, ULONG task,
    ULONG count, const mvdm_softpc_wow_menu_query_item *items)
{
    mvdm_softpc_wow_menu_observation row = {menu, task, 0, count, 1};
    return mvdm_softpc_wow_page_domain_publish_menus(1, &row, count, items) ?
        page_domain_menu_lookup(menu) + domain.client_delta : 0;
}

static int page_domain_menu_referenced(ULONG server, int clear_external)
{
    ULONG i, j;
    /* Inspect the existing copied graph. Required local unlinking must have
     * committed first. External observations may lag: withdraw their copied
     * references in the retirement commit, never change native ownership. */
    for (i = 1u; i < MVDM_SOFTPC_HANDLE_ENTRY_COUNT; ++i) {
        ULONG window = domain.guest_windows[i];
        ULONG menu = domain.guest_menus[i];
        ULONG items = domain.guest_menu_items[i];
        if (window && !page_domain_child_style(c_sas_dw_at(window + MVDM_SOFTPC_WND_STYLE)) &&
                c_sas_dw_at(window + MVDM_SOFTPC_WND_MENU_OR_ID) == server) {
            mvdm_softpc_wow_window_borrow *record = *window_borrow_link(window);
            if (!record || !record->process_id || record->process_id == GetCurrentProcessId())
                return 1;
            if (clear_external) c_sas_storedw(window + MVDM_SOFTPC_WND_MENU_OR_ID, 0);
        }
        if (!menu || !items) continue;
        for (j = 0u; j < c_sas_dw_at(menu + MVDM_SOFTPC_MENU_COUNT); ++j)
            if (c_sas_dw_at(items + j * MVDM_SOFTPC_MENU_ITEM_BYTES + 12u) == server) {
                if (domain.local_menu_state[i]) return 1;
                if (clear_external) c_sas_storedw(items + j * MVDM_SOFTPC_MENU_ITEM_BYTES + 12u, 0);
            }
    }
    return 0;
}

int mvdm_softpc_wow_page_domain_retire_menu(unsigned long menu)
{
    ULONG client, storage, entry;
    page_domain_retired_storage *old_client, *old_items = NULL;
    DWORD saved = GetLastError();
    if (!page_domain_menu_context() || !(client = page_domain_menu_lookup(menu)))
        return 0;
    if (page_domain_menu_referenced(client + domain.client_delta, 0)) {
        SetLastError(ERROR_BUSY);
        return 0;
    }
    storage = domain.guest_menu_items[LOWORD(menu)];
    old_client = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(*old_client));
    if (storage) old_items = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(*old_items));
    if (!old_client || (storage && !old_items)) {
        if (old_client) HeapFree(GetProcessHeap(), 0, old_client);
        if (old_items) HeapFree(GetProcessHeap(), 0, old_items);
        SetLastError(ERROR_NOT_ENOUGH_MEMORY); return 0;
    }
    old_client->address = client;
    if (storage) {
        old_items->address = storage;
    }
    /* Withdraw identity atomically; failed physical release is retained,
     * never a partially emptied published MENU or a false native result. */
    (void)page_domain_menu_referenced(client + domain.client_delta, 1);
    entry = domain.guest_handle_table + (ULONG)LOWORD(menu) * MVDM_SOFTPC_HANDLE_ENTRY_BYTES;
    c_sas_storedw(entry, (ULONG)LOWORD(menu) + 1u);
    c_sas_storedw(entry + 4u, 0u);
    c_sas_storedw(entry + 8u, 0x00010000u);
    domain.guest_menus[LOWORD(menu)] = 0u;
    domain.guest_menu_items[LOWORD(menu)] = 0u;
    domain.local_menu_state[LOWORD(menu)] = 0;
    page_domain_defer_menu_storage(old_items);
    page_domain_defer_menu_storage(old_client);
    page_domain_collect_menu_storage();
    SetLastError(saved);
    return 1;
}

int mvdm_softpc_wow_page_domain_window_menu(unsigned long window,
    unsigned long menu)
{
    ULONG client, menu_client = 0u;
    if (!page_domain_menu_context() || !LOWORD(window) ||
            !(client = domain.guest_windows[LOWORD(window)]) ||
            c_sas_dw_at(client) != window ||
            page_domain_child_style(c_sas_dw_at(client + MVDM_SOFTPC_WND_STYLE))) return 0;
    if (menu && !(menu_client = page_domain_menu_lookup(menu))) return 0;
    c_sas_storedw(client + MVDM_SOFTPC_WND_MENU_OR_ID,
        menu_client ? menu_client + domain.client_delta : 0u);
    return 1;
}

static ULONG page_domain_window_lookup(ULONG window)
{
    ULONG client;
    WORD index = LOWORD(window);
    if (window == domain.desktop_handle)
        return domain.guest_desktop_window;
    if (!index || index == LOWORD(domain.desktop_handle) || index == 0xffffu) return 0u;
    client = domain.guest_windows[index];
    return client && c_sas_dw_at(client) == window ? client : 0u;
}

static mvdm_softpc_wow_window_borrow **window_borrow_link(ULONG backing)
{
    mvdm_softpc_wow_window_borrow **link = &window_borrows;
    while (*link && (*link)->backing != backing) link = &(*link)->next;
    return link;
}

static int window_borrow_is_current(const mvdm_softpc_wow_window_borrow *borrow)
{
    WORD index;
    if (borrow->retired) return 0;
    index = LOWORD(c_sas_dw_at(borrow->backing + MVDM_SOFTPC_WND_HEAD_HANDLE));
    return index && index != 0xffffu && index != LOWORD(domain.desktop_handle) &&
        domain.guest_windows[index] == borrow->backing;
}

unsigned long mvdm_softpc_wow_page_domain_window_handle(unsigned short index)
{
    ULONG backing;
    if (!page_domain_menu_context() || !index || index == 0xffffu) return 0u;
    if (index == LOWORD(domain.desktop_handle)) return domain.desktop_handle;
    if (!(backing = domain.guest_windows[index])) return 0u;
    return c_sas_dw_at(backing + MVDM_SOFTPC_WND_HEAD_HANDLE);
}

int mvdm_softpc_wow_page_domain_handle_metadata(unsigned short index,
    unsigned long *metadata)
{
    if (!page_domain_menu_context() || !metadata) return 0;
    *metadata = c_sas_dw_at(domain.guest_handle_table +
        (ULONG)index * MVDM_SOFTPC_HANDLE_ENTRY_BYTES + 8u);
    return 1;
}

/* ADAPTER-WOW-088: known is call-local metadata under excluded guest execution,
 * not yet an owning reference. Unknown/cold storage keeps the safe list lookup
 * and original allocation path; only a successful increment grants a pin. */
static mvdm_softpc_wow_window_borrow *borrow_window_backing(ULONG backing,
    mvdm_softpc_wow_window_borrow *known)
{
    if (!known) {
        mvdm_softpc_wow_window_borrow **link = window_borrow_link(backing);
        known = *link;
        if (!known) {
            known = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(*known));
            if (!known) return NULL;
            known->backing = backing;
            *link = known;
        }
    }
    if (known->backing != backing || known->retired ||
            known->references == 0xffffffffu) return NULL;
    ++known->references;
    return known;
}

unsigned long mvdm_softpc_wow_page_domain_borrow_window(unsigned long window)
{
    ULONG backing;
    if (!page_domain_menu_context() ||
            !(backing = page_domain_window_lookup(window))) return 0u;
    return borrow_window_backing(backing, NULL) ? backing : 0u;
}

/* The private transaction holds this exact record's pin. Public arbitrary
 * backing inputs still resolve below before reaching this helper. */
static int release_window_record(mvdm_softpc_wow_window_borrow *borrow,
    int retained_owner)
{
    ULONG backing = borrow->backing;
    if (retained_owner ? (!borrow->retired || !borrow->retained_owner) :
            !borrow->references) return 0;
    /* Native scope/WW owners end here even if physical decommit fails.
     * Consume their reference exactly once and retain a failed allocation
     * in this existing list, not behind a caller that has already vanished. */
    if (retained_owner) borrow->retained_owner = 0;
    else --borrow->references;
    if (!borrow->retired) return 1; /* Identity metadata lives until withdrawal. */
    if (!borrow->references && !borrow->retained_owner) {
        if (borrow->retired && (page_domain_window_referenced(backing + domain.client_delta) ||
                VdmFreeVirtualMemory(backing) < 0)) return 1;
        /* Final physical unlink only. No native USER/guest/reentrant disposal
         * can run through this already-audited deallocation closure. */
        *window_borrow_link(backing) = borrow->next;
        dispose_window_borrow(borrow);
    }
    return 1;
}

static int release_window_backing(ULONG backing, int retained_owner)
{
    mvdm_softpc_wow_window_borrow *borrow;
    if (!page_domain_menu_context() || !backing ||
            !(borrow = *window_borrow_link(backing))) return 0;
    return release_window_record(borrow, retained_owner);
}

int mvdm_softpc_wow_page_domain_collect_retired_windows(void)
{
    mvdm_softpc_wow_window_borrow **link, *borrow;
    int complete = 1;
    if (!page_domain_menu_context()) return 0;
    for (link = &window_borrows; (borrow = *link) != NULL;) {
        if (!borrow->retired || borrow->references || borrow->retained_owner ||
                page_domain_window_referenced(borrow->backing + domain.client_delta)) {
            link = &borrow->next;
            continue;
        }
        if (VdmFreeVirtualMemory(borrow->backing) < 0) {
            complete = 0;
            link = &borrow->next;
            continue;
        }
        *link = borrow->next;
        dispose_window_borrow(borrow);
    }
    return complete;
}

int mvdm_softpc_wow_page_domain_release_window(unsigned long backing)
{
    return release_window_backing(backing, 0);
}

int mvdm_softpc_wow_page_domain_release_retained_window(unsigned long backing)
{
    return release_window_backing(backing, 1);
}

int mvdm_softpc_wow_page_domain_bind_window_host_storage(unsigned long backing,
    void *storage, mvdm_softpc_wow_host_window_dispose dispose)
{
    mvdm_softpc_wow_window_borrow *borrow;
    if (!page_domain_menu_context() || !storage || !dispose ||
            !(borrow = *window_borrow_link(backing)) || !borrow->references ||
            borrow->retired || borrow->host_storage) return 0;
    borrow->host_storage = storage;
    borrow->dispose_host_storage = dispose;
    return 1;
}

int mvdm_softpc_wow_page_domain_retain_host_window(unsigned long backing, void *storage)
{
    mvdm_softpc_wow_window_borrow *borrow;
    if (!page_domain_menu_context() || !storage ||
            !(borrow = *window_borrow_link(backing)) ||
            borrow->host_storage != storage || borrow->references == 0xffffffffu)
        return 0;
    ++borrow->references;
    return 1;
}

void *mvdm_softpc_wow_page_domain_window_host_storage(unsigned long backing)
{
    mvdm_softpc_wow_window_borrow *borrow;
    if (!page_domain_menu_context() ||
            !(borrow = *window_borrow_link(backing)) || !borrow->references)
        return NULL;
    return borrow->host_storage;
}

int mvdm_softpc_wow_page_domain_read_window_words(unsigned long backing,
    unsigned long words[3])
{
    unsigned i;
    mvdm_softpc_wow_window_borrow *borrow;
    if (!page_domain_menu_context() || !words ||
            !(borrow = *window_borrow_link(backing)) || !borrow->references) return 0;
    for (i = 0; i < 3u; ++i)
        words[i] = c_sas_dw_at(backing + MVDM_SOFTPC_WND_WOW_WORDS + i * 4u);
    return 1;
}

int mvdm_softpc_wow_page_domain_exchange_window_word(unsigned long backing,
    unsigned long index, unsigned long value, unsigned long *previous)
{
    mvdm_softpc_wow_window_borrow *borrow;
    ULONG address, old;
    if (!page_domain_menu_context() || !previous || index >= 3u ||
            !(borrow = *window_borrow_link(backing)) || !borrow->references ||
            borrow->retired) return 0;
    address = backing + MVDM_SOFTPC_WND_WOW_WORDS + index * 4u;
    old = c_sas_dw_at(address);
    c_sas_storedw(address, value);
    *previous = old;
    return 1;
}

int mvdm_softpc_wow_page_domain_read_window_identity(unsigned long backing,
    unsigned long *process_id, unsigned long *thread_id)
{
    mvdm_softpc_wow_window_borrow *borrow;
    if (!page_domain_menu_context() || !process_id || !thread_id ||
            !(borrow = *window_borrow_link(backing)) || !borrow->references ||
            !borrow->process_id || !borrow->thread_id) return 0;
    *process_id = borrow->process_id;
    *thread_id = borrow->thread_id;
    return 1;
}

/* ADAPTER-WOW-080: only for this domain's already validated allocated RAM.
 * Original nt_mem allocates page-rounded storage; c_GetLinAdd is the existing
 * original CCPU linear-to-native service. No guest/native callout or lifetime
 * transition may occur between resolving and copying. Do not retain the
 * alias, expose it to a caller, or use this for arbitrary guest/device spans.
 * A non-fitting span retains the original byte-wise SAS semantics. */
static void page_domain_copy_owned_ram(ULONG address, void *output, ULONG bytes)
{
    if (bytes <= MVDM_SOFTPC_PAGE_BYTES - (address & (MVDM_SOFTPC_PAGE_BYTES - 1u)))
        CopyMemory(output, c_GetLinAdd(address), bytes);
    else
        c_sas_loads(address, output, bytes);
}

static ULONG page_domain_read_owned_word(ULONG address)
{
    ULONG value;
    page_domain_copy_owned_ram(address, &value, sizeof(value));
    return value;
}

int mvdm_softpc_wow_page_domain_read_window_words_view(unsigned long backing,
    mvdm_softpc_wow_window_words_view *view)
{
    mvdm_softpc_wow_window_borrow *borrow;
    mvdm_softpc_wow_window_words_view value;
    ULONG fields[(MVDM_SOFTPC_WND_BYTES - MVDM_SOFTPC_WND_WOW_WORDS) / sizeof(ULONG)];
    unsigned i;
    if (!page_domain_menu_context() || !view ||
            !(borrow = *window_borrow_link(backing)) || !borrow->references) return 0;
    page_domain_copy_owned_ram(backing + MVDM_SOFTPC_WND_WOW_WORDS,
        fields, sizeof(fields));
    for (i = 0; i < 3u; ++i) value.words[i] = fields[i];
    value.style = fields[(MVDM_SOFTPC_WND_STYLE - MVDM_SOFTPC_WND_WOW_WORDS) / 4u];
    value.exstyle = fields[(MVDM_SOFTPC_WND_EXSTYLE - MVDM_SOFTPC_WND_WOW_WORDS) / 4u];
    value.module = borrow->module;
    *view = value;
    return 1;
}

/* Call-owned preparation, never a persistent handle table. Existing WNDs
 * retain their backing; fresh generations remain unpublished until commit. */
typedef struct page_domain_window_update {
    ULONG backing, class_server;
    mvdm_softpc_wow_window_borrow *record;
    ULONG class_style, window_extra;
    WINDOWINFO info;
    LONG control_id;
    DWORD process_id, thread_id, module, context_help_id;
    int fresh;
} page_domain_window_update;

static int page_domain_replace_window_links(ULONG count,
    const mvdm_softpc_wow_window_links *links, ULONG retired_count,
    const ULONG *retired, const page_domain_window_update *updates);

int mvdm_softpc_wow_page_domain_publish_window_links(ULONG count,
    const mvdm_softpc_wow_window_links *links)
{
    return mvdm_softpc_wow_page_domain_replace_window_links(count, links, 0, NULL);
}

int mvdm_softpc_wow_page_domain_replace_window_links(ULONG count,
    const mvdm_softpc_wow_window_links *links, ULONG retired_count,
    const ULONG *retired)
{
    return page_domain_replace_window_links(count, links, retired_count,
        retired, NULL);
}

static int page_domain_replace_window_links(ULONG count,
    const mvdm_softpc_wow_window_links *links, ULONG retired_count,
    const ULONG *retired, const page_domain_window_update *updates)
{
    ULONG i, j, *resolved = NULL;
    /* These are call-local validation indexes, not another object registry. */
    ULONG *updated = NULL;
    BYTE *withdrawn = NULL;
    mvdm_softpc_wow_window_borrow *prepared = NULL, *borrow;
    int result = 0;
    DWORD error, saved = GetLastError();
    if (!count || !links ||
            count >= MVDM_SOFTPC_HANDLE_ENTRY_COUNT ||
            retired_count >= MVDM_SOFTPC_HANDLE_ENTRY_COUNT ||
            (retired_count && !retired)) {
        SetLastError(ERROR_INVALID_PARAMETER); return 0;
    }
    if (!page_domain_menu_context()) {
        SetLastError(ERROR_INVALID_STATE); return 0;
    }
    /* Snapshot addresses in call-local storage. No guest callback/native
     * call can run between validation and publication. Failed allocation or
     * stale/unpublished identity leaves every previous edge unchanged. */
    resolved = HeapAlloc(GetProcessHeap(), 0, count * 5u * sizeof(ULONG));
    updated = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
        MVDM_SOFTPC_HANDLE_ENTRY_COUNT * sizeof(*updated));
    withdrawn = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
        MVDM_SOFTPC_HANDLE_ENTRY_COUNT);
    if (!resolved || !updated || !withdrawn) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY); goto done;
    }
    for (i = 0; i < retired_count; ++i) {
        ULONG backing = page_domain_window_lookup(retired[i]);
        WORD index = LOWORD(retired[i]);
        if (!backing) { SetLastError(ERROR_INVALID_STATE); goto done; }
        if (retired[i] == domain.desktop_handle || withdrawn[index]) {
            SetLastError(ERROR_INVALID_PARAMETER); goto done;
        }
        if (!page_domain_window_observed(backing) ||
                (!updates && IsWindow((HWND)(ULONG_PTR)retired[i]))) {
            SetLastError(ERROR_INVALID_STATE); goto done;
        }
        withdrawn[index] = 1;
        /* Reserve retention before commit so retirement cannot allocate or
         * decommit between edge replacement and handle withdrawal. */
        if (!*window_borrow_link(backing)) {
            borrow = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(*borrow));
            if (!borrow) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); goto done; }
            borrow->backing = backing;
            borrow->next = prepared;
            prepared = borrow;
        }
    }
    for (i = 0; i < count; ++i) {
        if (updated[LOWORD(links[i].window)]) {
            SetLastError(ERROR_INVALID_PARAMETER); goto done;
        }
        updated[LOWORD(links[i].window)] = i + 1u;
    }
    if (updates) for (i = 0; i < retired_count; ++i) {
        ULONG row = updated[LOWORD(retired[i])];
        if (row && links[row - 1u].window == retired[i]) {
            SetLastError(ERROR_INVALID_PARAMETER); goto done;
        }
    }
    for (i = 0; i < count; ++i) {
        ULONG identities[5];
        identities[0] = links[i].window;
        identities[1] = links[i].next;
        identities[2] = links[i].parent;
        identities[3] = links[i].child;
        identities[4] = links[i].owner;
        for (j = 0; j < 5u; ++j) {
            ULONG row = updated[LOWORD(identities[j])];
            ULONG client = identities[j] ? page_domain_window_lookup(identities[j]) : 0u;
            int replacement = updates && row &&
                links[row - 1u].window == identities[j];
            if (replacement) client = updates[row - 1u].backing;
            if (((!j || identities[j]) && !client) ||
                    (identities[j] && withdrawn[LOWORD(identities[j])] &&
                     !replacement)) {
                SetLastError(ERROR_INVALID_STATE); goto done;
            }
            resolved[i * 5u + j] = client ? client + domain.client_delta : 0u;
        }
    }
    /* Check future, not stale, incoming edges. Changed source rows are
     * already fully resolved above; retiring rows cease to be readers. */
    if (retired_count) for (i = 1; i < MVDM_SOFTPC_HANDLE_ENTRY_COUNT; ++i) {
        ULONG client = i == LOWORD(domain.desktop_handle) ?
            domain.guest_desktop_window : domain.guest_windows[i];
        if (!client || updated[i] || withdrawn[i]) continue;
        for (j = 0; j < 4u; ++j) {
            ULONG server = c_sas_dw_at(client + MVDM_SOFTPC_WND_NEXT + j * 4u);
            ULONG k;
            for (k = 0; server && k < retired_count; ++k)
                if (server == page_domain_window_lookup(retired[k]) + domain.client_delta) {
                    SetLastError(ERROR_INVALID_STATE); goto done;
                }
        }
    }
    /* Commit: no native queries, allocation, free or guest callback below. */
    while ((borrow = prepared) != NULL) {
        prepared = borrow->next;
        borrow->next = window_borrows;
        window_borrows = borrow;
    }
    for (i = 0; i < count; ++i) {
        ULONG client = resolved[i * 5u] - domain.client_delta;
        page_domain_store_owned_words(client + MVDM_SOFTPC_WND_NEXT,
            &resolved[i * 5u + 1u], 4u);
    }
    /* Dead observed nodes can reference each other. Drop only their outgoing
     * copied links after all surviving incoming links have been validated. */
    for (i = 0; i < retired_count; ++i) {
        const ULONG empty[4] = {0};
        ULONG client = page_domain_window_lookup(retired[i]);
        page_domain_store_owned_words(client + MVDM_SOFTPC_WND_NEXT, empty, 4u);
    }
    for (i = 0; i < retired_count; ++i) {
        ULONG client = page_domain_window_lookup(retired[i]);
        page_domain_withdraw_window(LOWORD(retired[i]), client,
            *window_borrow_link(client), 0);
    }
    if (updates) for (i = 0; i < count; ++i) {
        const page_domain_window_update *update = &updates[i];
        if (update->fresh)
            page_domain_commit_window(LOWORD(links[i].window), update->backing, 1u, 0u);
        c_sas_storedw(update->backing + MVDM_SOFTPC_WND_CLASS, update->class_server);
        /* Original SetClassData updates the existing CLS, including its
         * clones. Refresh only these numeric public fields in the same
         * commit; preserve class identity, logical procedure and WOW words. */
        c_sas_storedw(update->class_server - domain.client_delta +
            MVDM_SOFTPC_CLS_STYLE, update->class_style);
        c_sas_storedw(update->class_server - domain.client_delta +
            MVDM_SOFTPC_CLS_WINDOW_EXTRA, update->window_extra);
        page_domain_window_fields(update->backing, &update->info, update->control_id);
        borrow = update->record;
        borrow->process_id = update->process_id;
        borrow->thread_id = update->thread_id;
        borrow->module = update->module;
        borrow->context_help_id = update->context_help_id;
    }
    result = 1;
done:
    error = result ? saved : GetLastError();
    while ((borrow = prepared) != NULL) {
        prepared = borrow->next;
        dispose_window_borrow(borrow);
    }
    if (withdrawn) HeapFree(GetProcessHeap(), 0, withdrawn);
    if (updated) HeapFree(GetProcessHeap(), 0, updated);
    if (resolved) HeapFree(GetProcessHeap(), 0, resolved);
    SetLastError(error);
    return result;
}

static int page_domain_window_referenced(ULONG server)
{
    ULONG i, offset;
    for (i = 1; i < MVDM_SOFTPC_HANDLE_ENTRY_COUNT; ++i) {
        ULONG client = i == LOWORD(domain.desktop_handle) ?
            domain.guest_desktop_window : domain.guest_windows[i];
        if (!client || client + domain.client_delta == server) continue;
        for (offset = MVDM_SOFTPC_WND_NEXT; offset <= MVDM_SOFTPC_WND_OWNER; offset += 4u)
            if (page_domain_read_owned_word(client + offset) == server) return 1;
    }
    return 0;
}

/* Non-failing copied-state commit. Caller has already validated incoming
 * references and either reserved retention or physically freed the backing.
 * Both single retirement and graph replacement use this same disposition. */
static void page_domain_withdraw_window(WORD index, ULONG guest_window,
    mvdm_softpc_wow_window_borrow *borrow, int retained_owner)
{
    ULONG entry;
    if (borrow) {
        /* Original createw.c post-WM_NCDESTROY cleanup clears child/owner
         * and keeps the desktop parent until the final WND unlock. The
         * existing borrow owns this backing, not another object registry. */
        c_sas_storedw(guest_window + MVDM_SOFTPC_WND_NEXT, 0u);
        c_sas_storedw(guest_window + MVDM_SOFTPC_WND_PARENT,
            domain.guest_desktop_window + domain.client_delta);
        c_sas_storedw(guest_window + MVDM_SOFTPC_WND_CHILD, 0u);
        c_sas_storedw(guest_window + MVDM_SOFTPC_WND_OWNER, 0u);
        /* Original createw.c releases the non-child menu association before
         * destroying that menu. A retained callback WND must not retain the
         * copied edge after this terminal unlink either. Child IDs are data. */
        if (!page_domain_child_style(c_sas_dw_at(guest_window + MVDM_SOFTPC_WND_STYLE)))
            c_sas_storedw(guest_window + MVDM_SOFTPC_WND_MENU_OR_ID, 0u);
    }
    entry = domain.guest_handle_table +
        (ULONG)index * MVDM_SOFTPC_HANDLE_ENTRY_BYTES;
    c_sas_storedw(entry, (ULONG)index + 1u);
    c_sas_storedw(entry + 4u, 0u);
    c_sas_storedw(entry + 8u, 0x00010000u);
    domain.guest_windows[index] = 0u;
    if (borrow) {
        borrow->retired = 1;
        borrow->retained_owner = retained_owner;
    }
}

int mvdm_softpc_wow_page_domain_retire_handle(unsigned short index,
    unsigned long *retained_backing)
{
    ULONG guest_window;
    mvdm_softpc_wow_window_borrow *borrow;
    if (!domain.active || !index || index == 0xffffu ||
            index == LOWORD(domain.desktop_handle) ||
            domain.owner != session_thread_current() ||
            !(c_getCR0() & MVDM_SOFTPC_CR0_PG) || c_getCR3() != domain.directory)
        return 0;
    guest_window = domain.guest_windows[index];
    if (!guest_window) return 0;
    borrow = *window_borrow_link(guest_window);
    /* Terminal WW ownership can withdraw its identity while copied incoming
     * links still retain backing, just as original USER locks delay physical
     * destruction. The next relationship commit removes those stale links.
     * Unowned retirement cannot silently acquire that terminal authority. */
    if (!retained_backing &&
            page_domain_window_referenced(guest_window + domain.client_delta)) return 0;
    if (retained_backing && !borrow) {
        borrow = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(*borrow));
        if (!borrow) return 0;
        borrow->backing = guest_window;
        *window_borrow_link(guest_window) = borrow;
    }
    /* Original HMMarkObjectDestroy defers physical destruction while the
     * caller retains a WND borrow. The existing owner retains that backing. */
    if (!retained_backing && !borrow && VdmFreeVirtualMemory(guest_window) < 0) return 0;
    page_domain_withdraw_window(index, guest_window, borrow, retained_backing != NULL);
    if (retained_backing) *retained_backing = guest_window;
    else if (borrow && !borrow->references) {
        DWORD saved = GetLastError();
        (void)mvdm_softpc_wow_page_domain_collect_retired_windows();
        SetLastError(saved);
    }
    return 1;
}

unsigned long mvdm_softpc_wow_page_domain_publish_named_class(unsigned short atom,
    unsigned short fnid, unsigned long flags, unsigned long wow0, unsigned long wow1,
    unsigned long task, unsigned long style, unsigned long procedure,
    unsigned long window_extra, unsigned long module,
    const char *class_name, unsigned long name_bytes)
{
    ULONG client = 0u;
    ULONG server;
    ULONG size;
    mvdm_softpc_wow_class_allocation *allocation;

    if (!domain.active || !atom || !domain.client_delta ||
            domain.owner != session_thread_current() ||
            !(c_getCR0() & MVDM_SOFTPC_CR0_PG) || c_getCR3() != domain.directory)
        return 0u;
    if (!class_name || !HIWORD((ULONG_PTR)class_name) || !name_bytes ||
            name_bytes > 0xffffffffu - MVDM_SOFTPC_CLS_BYTES) return 0u;
    size = MVDM_SOFTPC_CLS_BYTES + name_bytes;
    if (size > 0xffffffffu - domain.client_delta ||
            class_name[name_bytes - 1u] != '\0') return 0u;
    allocation = HeapAlloc(GetProcessHeap(), 0u, sizeof(*allocation));
    if (allocation == NULL) return 0u;
    if (VdmAllocateVirtualMemory(&client, size, TRUE) < 0)
        goto fail;
    if (client > 0xffffffffu - domain.client_delta - size) {
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
    /* Original USER16 GetClassNameA rebases this pointer through WND.pSelf,
     * just as it rebases WND.pcls. Keep the bytes with CLS so existing class
     * retirement releases both; no native string pointer crosses the ABI. */
    c_sas_stores(client + MVDM_SOFTPC_CLS_BYTES,
        (uint8_t *)class_name, name_bytes);
    c_sas_storedw(client + MVDM_SOFTPC_CLS_ANSI_NAME,
        server + MVDM_SOFTPC_CLS_BYTES);
    page_domain_include_object(server, size);
    allocation->client = client;
    allocation->observed = 0;
    allocation->released = 0;
    allocation->next = class_allocations;
    class_allocations = allocation;
    return server;
fail:
    HeapFree(GetProcessHeap(), 0u, allocation);
    return 0u;
}

static int class_is_referenced(ULONG server)
{
    mvdm_softpc_wow_window_borrow *borrow;
    if (page_domain_read_owned_word(domain.guest_desktop_window +
            MVDM_SOFTPC_WND_CLASS) == server) return 1;
    /* ADAPTER-WOW-086: all ordinary WNDs have a lifetime record. Include
     * prepared and pinned retired storage, not just published identities. */
    for (borrow = window_borrows; borrow; borrow = borrow->next)
        if (page_domain_read_owned_word(borrow->backing + MVDM_SOFTPC_WND_CLASS) == server) return 1;
    return 0;
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
    /* A consumed final WW reference may leave physical reclamation pending.
     * Retry before checking CLS reachability; failures retain the WND/CLS. */
    (void)mvdm_softpc_wow_page_domain_collect_retired_windows();
    for (link = &class_allocations; (allocation = *link) != NULL;
            link = &allocation->next) {
        if (allocation->client == client) {
            /* Original class.c DestroyClass requires no WND associations.
             * Managed WW references already enforce that rule above this
             * carrier, but a sampled shared view must obey it as well. Do
             * not free a CLS still reachable through a published WND.pcls. */
            if (class_is_referenced(server)) return 0;
            if (VdmFreeVirtualMemory(client) < 0) return 0;
            *link = allocation->next;
            HeapFree(GetProcessHeap(), 0u, allocation);
            return 1;
        }
    }
    return 0;
}

static int __cdecl compare_class_reference(const void *left, const void *right)
{
    ULONG a = *(const ULONG *)left, b = *(const ULONG *)right;
    return (a > b) - (a < b);
}

int mvdm_softpc_wow_page_domain_collect_observed_classes(void)
{
    mvdm_softpc_wow_class_allocation *allocation, *next;
    mvdm_softpc_wow_window_borrow *borrow;
    ULONG *references, capacity = 1; /* Record-free desktop, plus all records. */
    ULONG count = 0, server;
    DWORD error;
    int result = 0;
    if (!page_domain_menu_context()) return 0;
    if (!mvdm_softpc_wow_page_domain_collect_retired_windows()) return 0;
    for (allocation = class_allocations; allocation; allocation = allocation->next)
        if (allocation->observed || allocation->released) break;
    if (!allocation) return 1;
    /* Existing WND backing is the authority. Derive its referenced classes
     * once for this call, not once per CLS. No native/guest callout can change
     * WND.pcls or borrower ownership during this collection. Final retirement
     * retains its independent strict reference check and failure behavior. */
    for (borrow = window_borrows; borrow; borrow = borrow->next) {
        if (capacity == MAXDWORD / sizeof(*references)) {
            SetLastError(ERROR_ARITHMETIC_OVERFLOW); return 0;
        }
        ++capacity;
    }
    references = HeapAlloc(GetProcessHeap(), 0, capacity * sizeof(*references));
    if (!references) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return 0; }
    server = page_domain_read_owned_word(domain.guest_desktop_window + MVDM_SOFTPC_WND_CLASS);
    if (server) references[count++] = server;
    for (borrow = window_borrows; borrow; borrow = borrow->next) {
        server = page_domain_read_owned_word(borrow->backing + MVDM_SOFTPC_WND_CLASS);
        if (server) references[count++] = server;
    }
    qsort(references, count, sizeof(*references), compare_class_reference);
    for (allocation = class_allocations; allocation; allocation = next) {
        server = allocation->client + domain.client_delta;
        next = allocation->next;
        if ((allocation->observed || allocation->released) &&
                !bsearch(&server, references, count, sizeof(*references),
                    compare_class_reference) &&
                !mvdm_softpc_wow_page_domain_retire_class(server)) goto done;
    }
    result = 1;
done:
    error = GetLastError();
    HeapFree(GetProcessHeap(), 0, references);
    SetLastError(error);
    return result;
}

int mvdm_softpc_wow_page_domain_release_class(unsigned long server)
{
    mvdm_softpc_wow_class_allocation *allocation;
    mvdm_softpc_wow_window_borrow *borrow;
    if (!page_domain_menu_context() ||
            !(allocation = page_domain_class_allocation(server))) return 0;
    /* A native registration and a copied reader have distinct lifetimes.
     * Never hide a still-published association; only withdrawn generations
     * may retain the released class. The collector already scans those pins. */
    if (page_domain_read_owned_word(domain.guest_desktop_window +
            MVDM_SOFTPC_WND_CLASS) == server) return 0;
    for (borrow = window_borrows; borrow; borrow = borrow->next)
        if (window_borrow_is_current(borrow) &&
                page_domain_read_owned_word(borrow->backing + MVDM_SOFTPC_WND_CLASS) == server) return 0;
    allocation->released = 1;
    /* Physical-free failure is retained by this allocation owner, just as
     * retired WND backing is. It cannot undo successful native unregister. */
    if (!class_is_referenced(server))
        (void)mvdm_softpc_wow_page_domain_retire_class(server);
    return 1;
}

int mvdm_softpc_wow_page_domain_observe_window(unsigned long window,
    unsigned short atom, unsigned long class_style, unsigned long window_extra,
    const char *class_name, unsigned long name_bytes)
{
    ULONG server, existing;
    WORD index = LOWORD(window);
    mvdm_softpc_wow_class_allocation *allocation;
    if (!page_domain_menu_context() || !index || index == 0xffffu)
        return 0;
    existing = window == domain.desktop_handle ?
        domain.guest_desktop_window : domain.guest_windows[index];
    if (existing && (window != domain.desktop_handle ||
            c_sas_dw_at(existing + MVDM_SOFTPC_WND_CLASS)))
        return c_sas_dw_at(existing) == window &&
            mvdm_softpc_wow_page_domain_refresh_window(window);
    if (window == domain.desktop_handle &&
            !mvdm_softpc_wow_page_domain_refresh_window(window)) return 0;
    /* A private sampled CLS is a query view, not a registration or a lookup
     * candidate. No class procedure, native module or task ownership is
     * fabricated. Native USER remains the class/window lifetime owner. */
    server = mvdm_softpc_wow_page_domain_publish_named_class(atom, 0u, 0u,
        0u, 0u, 0u, class_style, 0u, window_extra, 0u, class_name, name_bytes);
    if (!server) return 0;
    allocation = page_domain_class_allocation(server);
    allocation->observed = 1; /* publish_named_class registered this allocation. */
    if (window == domain.desktop_handle) {
        /* The desktop already owns its one WND in client_allocation. Add
         * only the missing query class, never another handle/backing owner. */
        c_sas_storedw(existing + MVDM_SOFTPC_WND_CLASS, server);
        return 1;
    }
    if (!page_domain_publish_handle(index, HIWORD(window),
            1u, 0u, window, server, NULL, 0)) {
        (void)mvdm_softpc_wow_page_domain_collect_observed_classes();
        return 0;
    }
    return 1;
}

static int copy_window_observation(ULONG backing,
    const mvdm_softpc_wow_window_borrow *borrow,
    mvdm_softpc_wow_window_observation *row)
{
    ULONG cls, targets[4], j, name;
    ULONG fields[MVDM_SOFTPC_WND_BYTES / sizeof(ULONG)];
    ULONG class_fields[MVDM_SOFTPC_CLS_BYTES / sizeof(ULONG)];
    int owned_class;
    ZeroMemory(row, sizeof(*row));
    if (borrow) {
        row->process_id = borrow->process_id;
        row->thread_id = borrow->thread_id;
        row->module = borrow->module;
        row->context_help_id = borrow->context_help_id;
    }
    /* Caller pins this exact backing, or enumerates the committed domain
     * with guest execution excluded. Never re-resolve a retained HWND. */
    page_domain_copy_owned_ram(backing, fields, sizeof(fields));
    row->links.window = fields[MVDM_SOFTPC_WND_HEAD_HANDLE / 4u];
    for (j = 0; j < 4u; ++j) {
        ULONG server = fields[MVDM_SOFTPC_WND_NEXT / 4u + j];
        /* Preserve full historical identity, not a fresh slot lookup. */
        targets[j] = server ? c_sas_dw_at(server - domain.client_delta) : 0u;
        row->window_rect[j] = (LONG)fields[MVDM_SOFTPC_WND_RECT / 4u + j];
        row->client_rect[j] = (LONG)fields[MVDM_SOFTPC_WND_CLIENT_RECT / 4u + j];
    }
    row->links.next = targets[0]; row->links.parent = targets[1];
    row->links.child = targets[2]; row->links.owner = targets[3];
    row->style = fields[MVDM_SOFTPC_WND_STYLE / 4u];
    row->exstyle = fields[MVDM_SOFTPC_WND_EXSTYLE / 4u];
    if (page_domain_child_style(row->style))
        row->control_id = fields[MVDM_SOFTPC_WND_MENU_OR_ID / 4u];
    cls = fields[MVDM_SOFTPC_WND_CLASS / 4u];
    if (!cls) return 1; /* Cold desktop has no observed descriptor yet. */
    owned_class = page_domain_class_allocation(cls) != NULL;
    cls -= domain.client_delta;
    if (owned_class) {
        page_domain_copy_owned_ram(cls, class_fields, sizeof(class_fields));
        row->descriptor.atom = (WORD)class_fields[MVDM_SOFTPC_CLS_ATOM / 4u];
        row->descriptor.class_style = class_fields[MVDM_SOFTPC_CLS_STYLE / 4u];
        row->descriptor.window_extra = class_fields[MVDM_SOFTPC_CLS_WINDOW_EXTRA / 4u];
        name = class_fields[MVDM_SOFTPC_CLS_ANSI_NAME / 4u];
    } else {
        row->descriptor.atom = (WORD)c_sas_dw_at(cls + MVDM_SOFTPC_CLS_ATOM);
        row->descriptor.class_style = c_sas_dw_at(cls + MVDM_SOFTPC_CLS_STYLE);
        row->descriptor.window_extra = c_sas_dw_at(cls + MVDM_SOFTPC_CLS_WINDOW_EXTRA);
        name = c_sas_dw_at(cls + MVDM_SOFTPC_CLS_ANSI_NAME);
    }
    if (!name) { SetLastError(ERROR_INVALID_DATA); return 0; }
    name -= domain.client_delta;
    /* The canonical inline name is inside the same page-rounded allocation.
     * Keep the existing scalar route for a noncanonical class-name pointer;
     * do not invent a new rejection or a host-side class-name authority. */
    if (owned_class && name == cls + MVDM_SOFTPC_CLS_BYTES &&
            (name & (MVDM_SOFTPC_PAGE_BYTES - 1u)) <=
                MVDM_SOFTPC_PAGE_BYTES - sizeof(row->descriptor.name)) {
        page_domain_copy_owned_ram(name, row->descriptor.name, sizeof(row->descriptor.name));
        for (j = 0; j < sizeof(row->descriptor.name) && row->descriptor.name[j]; ++j) {}
        if (j < sizeof(row->descriptor.name))
            ZeroMemory(row->descriptor.name + j + 1u, sizeof(row->descriptor.name) - j - 1u);
    } else {
        for (j = 0; j < sizeof(row->descriptor.name); ++j) {
            c_sas_loads(name + j, (BYTE *)&row->descriptor.name[j], 1u);
            if (!row->descriptor.name[j]) break;
        }
    }
    if (j == sizeof(row->descriptor.name)) {
        SetLastError(ERROR_INSUFFICIENT_BUFFER); return 0;
    }
    row->descriptor.name_bytes = j + 1u;
    return 1;
}

int mvdm_softpc_wow_page_domain_read_window_view(ULONG backing,
    mvdm_softpc_wow_window_observation *row)
{
    mvdm_softpc_wow_window_borrow *borrow;
    mvdm_softpc_wow_window_observation value;
    DWORD saved = GetLastError();
    if (!row) { SetLastError(ERROR_INVALID_PARAMETER); return 0; }
    if (!page_domain_menu_context() ||
            !(borrow = *window_borrow_link(backing)) || !borrow->references) {
        SetLastError(ERROR_INVALID_STATE); return 0;
    }
    if (!copy_window_observation(backing, borrow, &value)) return 0;
    *row = value;
    SetLastError(saved);
    return 1;
}

int mvdm_softpc_wow_page_domain_borrow_callback_window(ULONG window, ULONG *backing)
{
    ULONG client, pinned;
    mvdm_softpc_wow_window_borrow *borrow;
    DWORD saved = GetLastError();
    if (!backing) { SetLastError(ERROR_INVALID_PARAMETER); return 0; }
    if (!page_domain_menu_context()) { SetLastError(ERROR_INVALID_STATE); return 0; }
    if (!window || c_sas_dw_at(domain.guest_teb + MVDM_SOFTPC_TEB_CACHED_HWND) != window) {
        *backing = 0; SetLastError(saved); return 1;
    }
    client = c_sas_dw_at(domain.guest_teb + MVDM_SOFTPC_TEB_CACHED_WND);
    borrow = *window_borrow_link(client);
    /* Validate storage before reading a cached address. A retired callback
     * may intentionally differ from the current same-HWND generation. */
    if (borrow) {
        if (c_sas_dw_at(client + MVDM_SOFTPC_WND_HEAD_HANDLE) != window) {
            SetLastError(ERROR_INVALID_STATE); return 0;
        }
        if (borrow->references == 0xffffffffu) {
            SetLastError(ERROR_ARITHMETIC_OVERFLOW); return 0;
        }
        ++borrow->references;
    } else {
        if (!client || page_domain_window_lookup(window) != client) {
            SetLastError(ERROR_INVALID_STATE); return 0;
        }
        pinned = mvdm_softpc_wow_page_domain_borrow_window(window);
        if (!pinned) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return 0; }
    }
    *backing = client;
    SetLastError(saved);
    return 1;
}

int mvdm_softpc_wow_page_domain_borrow_window_relation(ULONG backing,
    ULONG relation, ULONG *target)
{
    mvdm_softpc_wow_window_borrow *source, *destination;
    ULONG server, client, pinned;
    DWORD saved = GetLastError();
    if (!target || relation >= 4u) {
        SetLastError(ERROR_INVALID_PARAMETER); return 0;
    }
    if (!page_domain_menu_context() ||
            !(source = *window_borrow_link(backing)) || !source->references) {
        SetLastError(ERROR_INVALID_STATE); return 0;
    }
    server = c_sas_dw_at(backing + MVDM_SOFTPC_WND_NEXT + relation * 4u);
    if (!server) { *target = 0; SetLastError(saved); return 1; }
    client = server - domain.client_delta;
    destination = *window_borrow_link(client);
    if (destination) {
        /* Incoming published links can retain a withdrawn generation. Do
         * not resolve its handle slot, which may already name a replacement. */
        if (destination->references == 0xffffffffu) {
            SetLastError(ERROR_ARITHMETIC_OVERFLOW); return 0;
        }
        ++destination->references;
    } else {
        ULONG handle = c_sas_dw_at(client + MVDM_SOFTPC_WND_HEAD_HANDLE);
        /* Cold live nodes (notably the desktop) may not yet have a borrow
         * record. Validate the full live identity and allocation together. */
        if (page_domain_window_lookup(handle) != client) {
            SetLastError(ERROR_INVALID_STATE); return 0;
        }
        pinned = mvdm_softpc_wow_page_domain_borrow_window(handle);
        if (!pinned) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return 0; }
    }
    *target = client;
    SetLastError(saved);
    return 1;
}

static int __cdecl compare_window_observation_slot(const void *left, const void *right)
{
    WORD a = LOWORD(((const mvdm_softpc_wow_window_observation *)left)->links.window);
    WORD b = LOWORD(((const mvdm_softpc_wow_window_observation *)right)->links.window);
    return (a > b) - (a < b);
}

static int __cdecl compare_window_menu_slot(const void *left, const void *right)
{
    WORD a = LOWORD(((const mvdm_softpc_wow_window_menu_observation *)left)->window);
    WORD b = LOWORD(((const mvdm_softpc_wow_window_menu_observation *)right)->window);
    return (a > b) - (a < b);
}

static int copy_window_projection(ULONG backing,
    const mvdm_softpc_wow_window_borrow *borrow, ULONG index,
    mvdm_softpc_wow_window_observation *rows,
    mvdm_softpc_wow_window_menu_observation *menus)
{
    if (!menus) return copy_window_observation(backing, borrow, &rows[index]);
    /* ADAPTER-WOW-089: read this exact committed generation, not its CLS or
     * unrelated relations/name. Guest execution and publication are excluded
     * for the entire two-pass call, as for the full observation view. */
    menus[index].window = page_domain_read_owned_word(backing + MVDM_SOFTPC_WND_HEAD_HANDLE);
    menus[index].style = page_domain_read_owned_word(backing + MVDM_SOFTPC_WND_STYLE);
    menus[index].process_id = borrow ? borrow->process_id : 0u;
    return 1;
}

static int copy_current_window_view(ULONG capacity,
    mvdm_softpc_wow_window_observation *rows,
    mvdm_softpc_wow_window_menu_observation *menus, ULONG *count)
{
    ULONG i, needed = 0, copied = 0;
    mvdm_softpc_wow_window_borrow *borrow;
    DWORD saved = GetLastError();
    if (!count || (capacity && !rows && !menus)) {
        SetLastError(ERROR_INVALID_PARAMETER); return 0;
    }
    if (!page_domain_menu_context()) {
        SetLastError(ERROR_INVALID_STATE); return 0;
    }
    for (i = 1; i < MVDM_SOFTPC_HANDLE_ENTRY_COUNT - 1u; ++i)
        if (i == LOWORD(domain.desktop_handle) || domain.guest_windows[i]) ++needed;
    *count = needed;
    if (!rows && !menus && !capacity) { SetLastError(saved); return 1; }
    if (capacity < needed) { SetLastError(ERROR_INSUFFICIENT_BUFFER); return 0; }
    /* ADAPTER-WOW-085: every ordinary published WND was prepared with a
     * lifetime record. Reuse it instead of searching the whole list for
     * each slot. Preparation and pinned retired generations coexist here;
     * only the slot's current backing is a published observation. */
    if (!copy_window_projection(domain.guest_desktop_window,
            *window_borrow_link(domain.guest_desktop_window), copied++, rows, menus)) return 0;
    for (borrow = window_borrows; borrow; borrow = borrow->next) {
        if (!window_borrow_is_current(borrow)) continue;
        if (copied >= needed) { SetLastError(ERROR_INVALID_STATE); return 0; }
        if (!copy_window_projection(borrow->backing, borrow, copied++, rows, menus)) return 0;
    }
    if (copied != needed) { SetLastError(ERROR_INVALID_STATE); return 0; }
    /* Existing callers retain row positions across a round trip. Preserve
     * the old ascending-slot order, without a second persistent index. */
    if (menus) qsort(menus, copied, sizeof(*menus), compare_window_menu_slot);
    else qsort(rows, copied, sizeof(*rows), compare_window_observation_slot);
    SetLastError(saved);
    return 1;
}

int mvdm_softpc_wow_page_domain_copy_window_view(ULONG capacity,
    mvdm_softpc_wow_window_observation *rows, ULONG *count)
{
    return copy_current_window_view(capacity, rows, NULL, count);
}

int mvdm_softpc_wow_page_domain_copy_window_menu_view(ULONG capacity,
    mvdm_softpc_wow_window_menu_observation *rows, ULONG *count)
{
    return copy_current_window_view(capacity, NULL, rows, count);
}

/* Validate copied reader structure, not whether native USER stayed unchanged.
 * Parent/child symmetry is deliberately not required: original creation and
 * destruction expose unlinked WNDs. Scratch belongs only to this transaction. */
static int page_domain_validate_window_graph(ULONG count,
    const mvdm_softpc_wow_window_links *links, ULONG *index)
{
    ULONG i, edge, row, next;
    BYTE *state;
    DWORD error = ERROR_INVALID_DATA, saved = GetLastError();
    int result = 0;
    state = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, count);
    if (!state) { error = ERROR_NOT_ENOUGH_MEMORY; goto done; }
    for (i = 0; i < count; ++i) {
        WORD slot = LOWORD(links[i].window);
        if (!slot || slot == 0xffffu || index[slot]) {
            error = ERROR_INVALID_PARAMETER; goto done;
        }
        index[slot] = i + 1u;
    }
    for (i = 0; i < count; ++i) {
        ULONG targets[4];
        if (links[i].window == domain.desktop_handle &&
                (links[i].next || links[i].parent || links[i].owner)) goto done;
        targets[0] = links[i].next; targets[1] = links[i].parent;
        targets[2] = links[i].child; targets[3] = links[i].owner;
        for (edge = 0; edge < 4u; ++edge) if (targets[edge]) {
            row = index[LOWORD(targets[edge])];
            if (!row || links[row - 1u].window != targets[edge]) goto done;
        }
    }
    /* Each relation is a functional graph. Mark a path grey, then black;
     * encountering grey is a cycle. This is linear, with no recursive stack
     * or per-node desktop traversal. Child-list traversal follows next. */
    for (edge = 0; edge < 3u; ++edge) {
        ZeroMemory(state, count);
        for (i = 0; i < count; ++i) {
            row = i + 1u;
            while (row && !state[row - 1u]) {
                const mvdm_softpc_wow_window_links *node = &links[row - 1u];
                state[row - 1u] = 1;
                next = edge == 0 ? node->next : edge == 1 ? node->parent : node->owner;
                row = next ? index[LOWORD(next)] : 0;
            }
            if (row && state[row - 1u] == 1) goto done;
            row = i + 1u;
            while (row && state[row - 1u] == 1) {
                const mvdm_softpc_wow_window_links *node = &links[row - 1u];
                state[row - 1u] = 2;
                next = edge == 0 ? node->next : edge == 1 ? node->parent : node->owner;
                row = next ? index[LOWORD(next)] : 0;
            }
        }
    }
    result = 1;
done:
    if (state) HeapFree(GetProcessHeap(), 0, state);
    SetLastError(result ? saved : error);
    return result;
}

/* Commit a captured observation view. Native observation belongs to WOW32;
 * a concurrent external mutation cannot invalidate this structural commit. */
int mvdm_softpc_wow_page_domain_project_window_graph(ULONG count,
    const mvdm_softpc_wow_window_observation *rows)
{
    page_domain_window_update *updates = NULL;
    mvdm_softpc_wow_window_links *links = NULL;
    mvdm_softpc_wow_window_borrow *record;
    ULONG *retired = NULL, *present = NULL, retired_count = 0, i;
    int result = 0;
    DWORD error, saved = GetLastError();
    if (!count || !rows ||
            count >= MVDM_SOFTPC_HANDLE_ENTRY_COUNT) {
        SetLastError(ERROR_INVALID_PARAMETER); return 0;
    }
    if (!page_domain_menu_context()) {
        SetLastError(ERROR_INVALID_STATE); return 0;
    }
    links = HeapAlloc(GetProcessHeap(), 0, count * sizeof(*links));
    present = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
        MVDM_SOFTPC_HANDLE_ENTRY_COUNT * sizeof(*present));
    updates = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, count * sizeof(*updates));
    retired = HeapAlloc(GetProcessHeap(), 0, MVDM_SOFTPC_HANDLE_ENTRY_COUNT * sizeof(*retired));
    if (!links || !present || !updates || !retired) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY); goto done;
    }
    for (i = 0; i < count; ++i) {
        links[i] = rows[i].links;
    }
    if (!page_domain_validate_window_graph(count, links, present)) goto done;
    /* Borrowed association, not cleanup ownership. Only backing!=0 below
     * means an actual acquired pin. Match the published full generation. */
    for (record = window_borrows; record; record = record->next) {
        ULONG full, row, current;
        if (record->retired) continue;
        full = c_sas_dw_at(record->backing + MVDM_SOFTPC_WND_HEAD_HANDLE);
        row = present[LOWORD(full)];
        current = full == domain.desktop_handle ?
            domain.guest_desktop_window : domain.guest_windows[LOWORD(full)];
        if (row && !updates[row - 1u].record && links[row - 1u].window == full &&
                record->backing == current) updates[row - 1u].record = record;
    }
    for (i = 0; i < count; ++i) {
        const mvdm_softpc_wow_window_descriptor *descriptor = &rows[i].descriptor;
        page_domain_window_update *update = &updates[i];
        ULONG window = links[i].window, existing;
        WORD index = LOWORD(window);
        mvdm_softpc_wow_class_allocation *cls;
        if (!index || index == 0xffffu || !descriptor->atom ||
                !descriptor->name_bytes || descriptor->name_bytes > sizeof(descriptor->name) ||
                descriptor->name[descriptor->name_bytes - 1u]) {
            SetLastError(ERROR_INVALID_PARAMETER); goto done;
        }
        update->info.cbSize = sizeof(update->info);
        CopyMemory(&update->info.rcWindow, rows[i].window_rect, sizeof(RECT));
        CopyMemory(&update->info.rcClient, rows[i].client_rect, sizeof(RECT));
        update->info.dwStyle = rows[i].style;
        update->info.dwExStyle = rows[i].exstyle;
        update->info.atomWindowType = descriptor->atom;
        update->control_id = (LONG)rows[i].control_id;
        update->process_id = rows[i].process_id;
        update->thread_id = rows[i].thread_id;
        update->module = rows[i].module;
        update->context_help_id = rows[i].context_help_id;
        update->class_style = descriptor->class_style;
        update->window_extra = descriptor->window_extra;
        existing = window == domain.desktop_handle ?
            domain.guest_desktop_window : domain.guest_windows[index];
        if (existing && c_sas_dw_at(existing + MVDM_SOFTPC_WND_HEAD_HANDLE) != window) {
            if (!page_domain_window_observed(existing))
            {
                SetLastError(ERROR_INVALID_STATE); goto done;
            }
            existing = 0; /* Old backing stays published until commit. */
        }
        if (existing) {
            mvdm_softpc_wow_window_borrow *pinned;
            SetLastError(ERROR_INVALID_STATE);
            pinned = borrow_window_backing(existing, update->record);
            if (!pinned) goto done;
            update->record = pinned;
            update->backing = existing;
            update->class_server = c_sas_dw_at(existing + MVDM_SOFTPC_WND_CLASS);
        }
        if (!update->class_server) {
            SetLastError(ERROR_INVALID_STATE);
            update->class_server = mvdm_softpc_wow_page_domain_publish_named_class(
                descriptor->atom, 0u, 0u, 0u, 0u, 0u, descriptor->class_style,
                0u, descriptor->window_extra, 0u, descriptor->name, descriptor->name_bytes);
            if (!update->class_server) goto done;
            cls = page_domain_class_allocation(update->class_server);
            cls->observed = 1;
        }
        if (!existing) {
            update->fresh = 1;
            SetLastError(ERROR_INVALID_STATE);
            update->backing = page_domain_prepare_window(window,
                (HWND)(ULONG_PTR)window, update->class_server, NULL, &update->info,
                &update->control_id, &update->record);
            if (!update->backing) goto done;
        }
    }
    /* An observation absent from the complete new view is withdrawn, whether
     * native USER has already destroyed it or merely changed our visibility.
     * This relinquishes copied storage only, never destroys a native window. */
    for (i = 1u; i < MVDM_SOFTPC_HANDLE_ENTRY_COUNT - 1u; ++i) {
        ULONG backing = domain.guest_windows[i], window;
        if (!backing) continue;
        window = c_sas_dw_at(backing + MVDM_SOFTPC_WND_HEAD_HANDLE);
        if (present[i] && links[present[i] - 1u].window == window) continue;
        if (!page_domain_window_observed(backing)) { SetLastError(ERROR_INVALID_STATE); goto done; }
        retired[retired_count++] = window;
    }
    if (!present[LOWORD(domain.desktop_handle)] ||
            links[present[LOWORD(domain.desktop_handle)] - 1u].window != domain.desktop_handle) {
        SetLastError(ERROR_INVALID_DATA); goto done;
    }
    result = page_domain_replace_window_links(count, links, retired_count, retired, updates);
done:
    error = result ? saved : GetLastError();
    if (updates) {
        for (i = 0; i < count; ++i) if (updates[i].backing) {
            if (!result && updates[i].fresh)
                updates[i].record->retired = 1;
            (void)release_window_record(updates[i].record, 0);
        }
        HeapFree(GetProcessHeap(), 0, updates);
    }
    /* Failure to physically free does not undo a completed logical commit.
     * P3's domain records retain that obligation for subsequent collection. */
    (void)mvdm_softpc_wow_page_domain_collect_observed_classes();
    if (retired) HeapFree(GetProcessHeap(), 0, retired);
    if (present) HeapFree(GetProcessHeap(), 0, present);
    if (links) HeapFree(GetProcessHeap(), 0, links);
    SetLastError(error);
    return result;
}

int mvdm_softpc_wow_page_domain_forget_observed_window(unsigned long window)
{
    ULONG backing;
    if (!page_domain_menu_context() || !(backing = page_domain_window_lookup(window)))
        return 0;
    if (page_domain_window_observed(backing))
        return mvdm_softpc_wow_page_domain_retire_handle(LOWORD(window), NULL);
    return 0; /* Never withdraw a managed WW's publication on this path. */
}

int mvdm_softpc_wow_page_domain_collect_observed_windows(void)
{
    ULONG index;
    DWORD saved = GetLastError();
    if (!page_domain_menu_context()) return 0;
    for (index = 1u; index < MVDM_SOFTPC_HANDLE_ENTRY_COUNT - 1u; ++index) {
        ULONG backing = domain.guest_windows[index], window;
        if (!backing) continue;
        if (!page_domain_window_observed(backing)) continue;
        window = c_sas_dw_at(backing + MVDM_SOFTPC_WND_HEAD_HANDLE);
        if (IsWindow((HWND)(ULONG_PTR)window) ||
                page_domain_window_referenced(backing + domain.client_delta)) continue;
        /* Native USER has already destroyed this generation. Retain the
         * existing WND/CLS if an outer reader still holds its backing. */
        if (!mvdm_softpc_wow_page_domain_retire_handle((WORD)index, NULL)) return 0;
    }
    if (!mvdm_softpc_wow_page_domain_collect_observed_classes()) return 0;
    SetLastError(saved);
    return 1;
}
