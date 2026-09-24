#include <stdint.h>
#include <stdio.h>
#include <windows.h>

#include "ntvdm-exe/session/session.h"
#include "ntvdm-exe/softpc/include/mvdm_cvidc_vector_binding.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_guest_memory.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_physical_mapping.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_wow_page_domain.h"
#include "wow32-dll/include/wow_user_object_bindings.h"
#include "wow32-dll/include/wow_window_dispatch_bindings.h"

extern void sas_init(uint32_t size);
extern void sas_term(void);
extern void c_sas_enable_20_bit_wrapping(void);
extern void c_sas_disable_20_bit_wrapping(void);
extern uint32_t c_sas_memory_size(void);
extern uint8_t *c_GetPhyAdd(uint32_t address);
extern void c_cpu_init(void);
extern void load_sw_cpu_access_functions(void);
extern uint32_t c_getCR0(void);
extern uint32_t c_getCR3(void);
extern void c_setCR3(uint32_t value);
extern void c_setCR0(uint32_t value);
extern uint16_t c_getMSW(void);
extern void c_setMSW(uint16_t value);
extern void c_setCPL(uint32_t value);
extern int32_t c_getCPL(void);
extern int xtrn2phy(uint32_t linear, uint32_t access, uint32_t *physical);
extern uint32_t c_sas_dw_at(uint32_t address);
extern void c_sas_fills(uint32_t address, uint8_t value, uint32_t byte_count);
extern void phy_w32(uint32_t address, uint32_t value);
extern uint32_t c_getGDT_BASE(void);
extern uint16_t c_getGDT_LIMIT(void);
extern void c_setGDT_BASE_LIMIT(uint32_t base, uint16_t limit);
extern uint32_t c_getLDT_BASE(void);
extern uint16_t c_getLDT_LIMIT(void);
extern void c_setLDT_BASE_LIMIT(uint32_t base, uint16_t limit);
extern LONG VdmAllocateVirtualMemory(ULONG *address, ULONG size, BOOL commit);
extern LONG VdmFreeVirtualMemory(ULONG address);
extern LONG VdmQueryFreeVirtualMemory(ULONG *bytes, ULONG *largest);

/* CCPU's access request bits are not x86 page-table bits. */
#define PG_R 0x00u
#define PG_W 0x01u
#define PG_U 0x02u
#define CR0_PE 0x00000001u
#define CR0_PG 0x80000000u
#define SHARED_CLOCK_LINEAR 0x7ffe0000u

#ifdef WOW_WINDOW_BORROW_FIXTURE
#include "wow32-dll/include/wow_window_words_binding.h"
#include "wow32-dll/include/wow_task_order_bindings.h"
typedef VOID (APIENTRY *PFNW32ET)(VOID);
#include "opennt-abi/source/public/internal/windows/inc/wowuserp.h"

static unsigned borrow_errors;
static ULONG callback_backing, callback_teb;
#define BORROW_CHECK(value) do { if (!(value)) { ++borrow_errors; \
    fprintf(stderr, "WINDOW_BORROW_FAIL line=%d error=%lu\n", __LINE__, GetLastError()); } } while (0)

static DWORD WINAPI destroy_in_callback(HWND window, UINT message, UINT wp,
    LONG lp, DWORD procedure, PVOID words)
{
    BORROW_CHECK(procedure == 0x81234567u && words != NULL);
    if (message == WM_APP + 71 || message == WM_APP + 72) {
        callback_backing = c_sas_dw_at(callback_teb + 0x70u);
        BORROW_CHECK(c_sas_dw_at(callback_teb + 0x6cu) == (ULONG)(ULONG_PTR)window);
        BORROW_CHECK(callback_backing != 0u);
        BORROW_CHECK(DestroyWindow(window));
        /* WM_DESTROY/WM_NCDESTROY recursively dispatch through production
         * bindings. Their restoration must preserve this outer borrow. */
        BORROW_CHECK(!IsWindow(window));
        BORROW_CHECK(c_sas_dw_at(callback_teb + 0x70u) == callback_backing);
        BORROW_CHECK(c_sas_dw_at(callback_backing) == (ULONG)(ULONG_PTR)window);
        BORROW_CHECK(((WW *)words)->adwWOW[1] == 0x12345678u);
        if (message == WM_APP + 72) RaiseException(0xe0420057u, 0, 0, NULL);
        return 0x55aau;
    }
    return (DWORD)DefWindowProcA(window, message, wp, lp);
}

static int verify_callback_borrow(ULONG teb)
{
    static wow_user_object_table objects;
    wow_task_order_process process = {0};
    wow_task_order_thread thread = {0};
    wow_window_dispatch_target target = { 0x81234567u, destroy_in_callback };
    unsigned mode;
    process.objects = &objects;
    thread.ppi = &process;
    callback_teb = teb;
    for (mode = 0; mode < 2; ++mode) {
        HWND window = CreateWindowExA(0, "STATIC", "S2 callback borrow",
            0, 20, 30, 180, 120, NULL, NULL, GetModuleHandle(NULL), NULL);
        wow_window_words_binding *borrow;
        WW words = {0};
        LONG result = 0x123456;
        ULONG free_before, free_after, largest;
        BOOL caught = FALSE;
        words.adwWOW[1] = 0x12345678u;
        if (!window) return 0;
        BORROW_CHECK(VdmQueryFreeVirtualMemory(&free_before, &largest) >= 0);
        BORROW_CHECK(wow_window_words_attach_target(window, &words, NULL, &target));
        borrow = wow_window_words_acquire(window);
        BORROW_CHECK(borrow != NULL);
        BORROW_CHECK(wow_window_words_publish_owner(window, borrow, &thread));
        if (mode == 0) {
            wow_window_words_binding *retry_borrow;
            mvdm_softpc_wow_page_domain_leave_protected();
            BORROW_CHECK(!wow_window_words_detach_if(window, borrow));
            retry_borrow = wow_window_words_acquire(window);
            BORROW_CHECK(retry_borrow == borrow);
            wow_window_words_release(retry_borrow);
            BORROW_CHECK(objects.entries[LOWORD(window)].bType == TYPE_WINDOW);
            BORROW_CHECK(mvdm_softpc_wow_page_domain_reenter_protected());
        }
        wow_window_words_release(borrow);
        BORROW_CHECK(SetWindowLongA(window, GWL_WNDPROC, (LONG)wow_window_native_proc) != 0);
        callback_backing = 0u;
        __try {
            BORROW_CHECK(wow_window_dispatch_bound(window, WM_APP + 71 + mode,
                0, 0, target.procedure, target.callback, &result));
            BORROW_CHECK(mode == 0 && result == 0x55aa);
        } __except (GetExceptionCode() == 0xe0420057u ?
                EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
            caught = TRUE;
        }
        BORROW_CHECK(caught == (mode != 0));
        BORROW_CHECK(!IsWindow(window));
        BORROW_CHECK(objects.entries[LOWORD(window)].bType == TYPE_FREE);
        BORROW_CHECK(c_sas_dw_at(teb + 0x6cu) == 0u &&
            c_sas_dw_at(teb + 0x70u) == 0u);
        /* Query the original allocator instead of attempting a double free.
         * Native window creation has no other guest allocation in this test. */
        BORROW_CHECK(callback_backing &&
            VdmQueryFreeVirtualMemory(&free_after, &largest) >= 0 &&
            free_after == free_before);
        if (IsWindow(window)) DestroyWindow(window);
    }
    fprintf(stderr, "WOW_WINDOW_BORROW_CCPU errors=%u normal=1 exception=1\n", borrow_errors);
    return borrow_errors == 0;
}
#endif

int main(void)
{
    session owner;
    uint32_t first_physical = 0u;
    uint32_t second_physical = 0u;
    uint32_t first_tick;
    uint32_t original_gdt = 0u;
    uint16_t original_gdt_limit = 0u;
    ULONG dosx_gdt = 0u;
    ULONG client_view = 0u;
    uint32_t shared = 0u;
    uint32_t wow_gdt;
    uint32_t low, high, teb, handles;
    uint32_t csr_flag;
    uint32_t server_info;
    uint32_t saved_cr3;
    uint32_t guest_class = 0u;
    HWND geometry_window = NULL;
    static ULONG native_bases[0x1fff];
    static wow_user_object_table object_table;
    static wow_cleanup_window object_storage;
    static BYTE thread_storage;
    int sas_started = 0;
    int guest_memory_started = 0;
    int physical_mapping_started = 0;
    int passed = 0;
    char const *stage = "initial";

    /* A worker domain is never a process-global fallback: without a bound
     * session or protected mode, admission must leave no published view. */
    stage = "unbound-rejection";
    if (mvdm_softpc_wow_page_domain_enter() ||
        mvdm_softpc_wow_page_domain_active() ||
        mvdm_softpc_wow_page_domain_set_client_desktop(0x1000u, 0u) ||
        mvdm_softpc_wow_page_domain_clear_client_desktop() ||
        mvdm_softpc_wow_page_domain_guest_shared_info() != 0u) goto done;
    stage = "session";
    session_initialize(&owner, 41u);
    if (!session_activate(&owner) || !session_thread_bind(&owner)) goto done;
    /* The immutable USER16 profile addresses a 64K-entry, 16-byte shared
     * handle table.  Two MiB was sufficient for S41's header-only carrier,
     * but cannot hold that original table alongside CCPU page structures and
     * the test's physical mapping.  Eight MiB remains a small VDM while
     * exercising the actual source-shaped allocation path. */
    sas_init(0x00800000u);
    sas_started = 1;
    c_cpu_init();
    load_sw_cpu_access_functions();
    stage = "guest-memory";
    if (!mvdm_softpc_guest_memory_begin(&owner)) goto done;
    guest_memory_started = 1;
    mvdm_cvidc_bind_vectors();
    /* This narrow fixture does not run the original nt_mem bootstrap. Reset
     * any inherited fixture binding, then establish the same source-shaped
     * physical record that nt_mem owns in the product before VdmAdd is used. */
    mvdm_softpc_physical_mapping_release();
    stage = "physical-mapping";
    if (!mvdm_softpc_physical_mapping_initialize(c_GetPhyAdd(0u),
            c_sas_memory_size())) goto done;
    physical_mapping_started = 1;
    stage = "real-mode-rejection";
    if (mvdm_softpc_wow_page_domain_enter() ||
        mvdm_softpc_wow_page_domain_active()) goto done;
    original_gdt = c_getGDT_BASE();
    original_gdt_limit = c_getGDT_LIMIT();
    stage = "dosx-gdt";
    if (VdmAllocateVirtualMemory(&dosx_gdt, 4096u, TRUE) < 0) goto done;
    c_sas_fills(dosx_gdt, 0u, 4096u);
    c_setGDT_BASE_LIMIT(dosx_gdt, 0xfffu);
    c_setMSW((uint16_t)(c_getMSW() | CR0_PE));
    c_setCR0(CR0_PE);
    c_setCPL(3u);
    /* The page domain must reject the real-mode A20 view rather than silently
     * wrap its high clock page. DOSX owns the subsequent A20 transition. */
    c_sas_enable_20_bit_wrapping();
    stage = "a20-rejection";
    if (mvdm_softpc_wow_page_domain_enter() ||
        mvdm_softpc_wow_page_domain_active()) goto done;
    c_sas_disable_20_bit_wrapping();
    /* A20 toggling changes the high-memory alias. Seed the source view only
     * after DOSX's A20 state is established, as in the real startup path. */
    phy_w32(dosx_gdt + 0x20u, 0x11112222u);
    phy_w32(dosx_gdt + 0x24u, 0x33334444u);
    phy_w32(dosx_gdt + 0x38u, 0x55556666u);
    phy_w32(dosx_gdt + 0x3cu, 0x77778888u);
    phy_w32(dosx_gdt + 0x18u, 0x0000ffffu);
    phy_w32(dosx_gdt + 0x1cu, 0x0000fa00u);
    /* Deliberately retain DOSX's conflicting selectors. The product must
     * copy, not overwrite them, before it activates the WOW descriptor view. */
    if (c_sas_dw_at(dosx_gdt + 0x20u) != 0x11112222u ||
        c_sas_dw_at(dosx_gdt + 0x38u) != 0x55556666u) goto done;
    stage = "domain-admission";
    {
        int entered = mvdm_softpc_wow_page_domain_enter();
        int readable = xtrn2phy(SHARED_CLOCK_LINEAR, PG_U | PG_R,
            &first_physical);
        int writable = xtrn2phy(SHARED_CLOCK_LINEAR, PG_U | PG_W,
            &second_physical);

        if (!entered ||
            (c_getCR0() & (CR0_PE | CR0_PG)) != (CR0_PE | CR0_PG) ||
            c_getCR3() == 0u || !readable || writable) {
            fprintf(stderr, "WOW_PAGE_DOMAIN_ADMISSION entered=%d cr0=%08lx cr3=%08lx read=%d write=%d read_phy=%08lx write_phy=%08lx\n",
                entered, (unsigned long)c_getCR0(),
                (unsigned long)c_getCR3(), readable, writable,
                (unsigned long)first_physical, (unsigned long)second_physical);
            goto done;
        }
    }
    stage = "client-view";
    shared = (uint32_t)mvdm_softpc_wow_page_domain_guest_shared_info();
    teb = (uint32_t)mvdm_softpc_wow_page_domain_guest_teb();
    handles = c_sas_dw_at(shared + 4u);
    server_info = c_sas_dw_at(shared);
    if (shared == 0u || teb == 0u || shared >= c_sas_memory_size() ||
        server_info != shared + 4096u ||
        handles != shared + 8192u ||
        c_sas_dw_at(server_info + 4u) != 0x10000u ||
        c_sas_dw_at(server_info + 0x170u) !=
            (uint32_t)GetSystemMetrics(SM_CXSCREEN) ||
        c_sas_dw_at(server_info + 0x2a0u + COLOR_WINDOW * 4u) !=
            (uint32_t)GetSysColor(COLOR_WINDOW) ||
        c_sas_dw_at(handles) != 1u ||
        c_sas_dw_at(handles + 4u) != 0u ||
        c_sas_dw_at(handles + 8u) != 0x00010000u ||
        c_getGDT_BASE() != dosx_gdt) {
        fprintf(stderr, "WOW_PAGE_DOMAIN_CLIENT_VIEW shared=%08lx sas=%08lx dosx=%08lx psi=%08lx ahe=%08lx first=%08lx second=%08lx\n",
            (unsigned long)shared, (unsigned long)c_sas_memory_size(),
            (unsigned long)dosx_gdt,
            (unsigned long)c_sas_dw_at(shared),
            (unsigned long)c_sas_dw_at(shared + 4u),
            0ul, 0ul);
        goto done;
    }
    stage = "flat-address-domains";
    {
        uint32_t saved_ldt = c_getLDT_BASE();
        uint16_t saved_limit = c_getLDT_LIMIT();
        uint32_t table;
        /* Deliberately unrelated native value: the guest must receive the
         * descriptor base, not a copied pointer or IntelBase subtraction. */
        native_bases[0x100] = 0xdeadbeefu;
        phy_w32(dosx_gdt + 0x800u, 0x5000ffffu);
        phy_w32(dosx_gdt + 0x804u, 0x1200f234u);
        c_setLDT_BASE_LIMIT(dosx_gdt, 0xfffu);
        table = mvdm_softpc_wow_page_domain_flat_address_array(native_bases);
        c_setLDT_BASE_LIMIT(saved_ldt, saved_limit);
        if (!table || c_sas_dw_at(table + 0x400u) != 0x12345000u ||
                c_sas_dw_at(table + 0x404u) != 0u) goto done;
        mvdm_softpc_wow_page_domain_update_flat_address(0x807u, 0x65432000u);
        if (c_sas_dw_at(table + 0x400u) != 0x65432000u) goto done;
        mvdm_softpc_wow_page_domain_update_flat_address(0xffffu, 0x11111111u);
        if (c_sas_dw_at(table + 0x400u) != 0x65432000u) goto done;
    }
    stage = "system-colour-publication";
    if (!mvdm_softpc_wow_page_domain_publish_system_color(COLOR_WINDOW,
            0x00112233u) ||
        c_sas_dw_at(server_info + 0x2a0u + COLOR_WINDOW * 4u) !=
            0x00112233u ||
        mvdm_softpc_wow_page_domain_publish_system_color(25u,
            0x00445566u) ||
        c_sas_dw_at(server_info + 0x2a0u + COLOR_WINDOW * 4u) !=
            0x00112233u) goto done;
    stage = "unselected-reentry";
    mvdm_softpc_wow_page_domain_leave_protected();
    if (!mvdm_softpc_wow_page_domain_reenter_protected() ||
        c_getGDT_BASE() != dosx_gdt) goto done;
    stage = "desktop-root-publication";
    if (!mvdm_softpc_wow_page_domain_client_desktop_ready() ||
        c_sas_dw_at(teb + 0x5cu) == 0u || c_sas_dw_at(teb + 0x60u) == 0u ||
        c_sas_dw_at(handles + 16u) == 2u ||
        c_sas_dw_at(handles + 16u + 8u) != 0x00010001u ||
        c_sas_dw_at(c_sas_dw_at(handles + 16u) - c_sas_dw_at(teb + 0x60u)) !=
            0x00010001u) goto done;
    stage = "guest-window-publication";
    guest_class = (uint32_t)mvdm_softpc_wow_page_domain_publish_class(
        0xc123u, 0x0042u, 0x00000002u, 0x11223344u, 0x55667788u,
        0x00004321u, 0x10000000u, 0x0000f00du, 8u, 0u);
    stage = "guest-class-fields";
    if (!guest_class ||
        (c_sas_dw_at(guest_class - c_sas_dw_at(teb + 0x60u) + 4u) & 0xffffu) !=
            0xc123u ||
        (c_sas_dw_at(guest_class - c_sas_dw_at(teb + 0x60u) + 4u) >> 16) !=
            0x0042u ||
        c_sas_dw_at(guest_class - c_sas_dw_at(teb + 0x60u) + 24u) != 2u ||
        c_sas_dw_at(guest_class - c_sas_dw_at(teb + 0x60u) + 36u) != 0x11223344u ||
        c_sas_dw_at(guest_class - c_sas_dw_at(teb + 0x60u) + 40u) != 0x55667788u ||
        c_sas_dw_at(guest_class - c_sas_dw_at(teb + 0x60u) + 44u) != 0x4321u ||
        c_sas_dw_at(guest_class - c_sas_dw_at(teb + 0x60u) + 64u) != 0x10000000u ||
        c_sas_dw_at(guest_class - c_sas_dw_at(teb + 0x60u) + 68u) != 0x0000f00du ||
        c_sas_dw_at(guest_class - c_sas_dw_at(teb + 0x60u) + 76u) != 8u ||
        c_sas_dw_at(guest_class - c_sas_dw_at(teb + 0x60u) + 80u) != 0u) {
        goto guest_window_failure;
    }
    stage = "guest-window-binding";
    if (!mvdm_softpc_wow_page_domain_publish_handle(2u, 0x2345u, 1u, 3u,
            0u, guest_class) ||
        c_sas_dw_at(handles + 32u + 8u) != 0x23450301u ||
        c_sas_dw_at(c_sas_dw_at(handles + 32u) - c_sas_dw_at(teb + 0x60u)) !=
            0x23450002u ||
        c_sas_dw_at(c_sas_dw_at(handles + 32u) - c_sas_dw_at(teb + 0x60u) +
            76u) != guest_class) {
        goto guest_window_failure;
    }
    /* Original HMValidateHandle rebases only pointers inside pDeskInfo's
     * server interval. Unconditionally subtracting delta above hid a broken
     * general (non-callback-cache) USER16 lookup. */
    stage = "guest-window-desktop-membership";
    {
        uint32_t desktop = c_sas_dw_at(teb + 0x5cu);
        uint32_t server_window = c_sas_dw_at(handles + 32u);
        if (server_window < c_sas_dw_at(desktop) ||
                server_window >= c_sas_dw_at(desktop + 4u))
            goto guest_window_failure;
    }
    stage = "guest-window-retirement";
    if (!mvdm_softpc_wow_page_domain_retire_handle(2u, NULL) ||
        !mvdm_softpc_wow_page_domain_retire_class(guest_class) ||
        mvdm_softpc_wow_page_domain_retire_class(guest_class) ||
        c_sas_dw_at(handles + 32u) != 3u ||
        c_sas_dw_at(handles + 32u + 4u) != 0u ||
        c_sas_dw_at(handles + 32u + 8u) != 0x00010000u) {
guest_window_failure:
        uint32_t client_class = guest_class ?
            guest_class - c_sas_dw_at(teb + 0x60u) : 0u;
        uint32_t guest_window = c_sas_dw_at(handles + 32u) ?
            c_sas_dw_at(handles + 32u) - c_sas_dw_at(teb + 0x60u) : 0u;

        fprintf(stderr,
            "CLS_WND guest=%08lx client=%08lx atom=%08lx fnid=%08lx flags=%08lx wow0=%08lx wow1=%08lx task=%08lx style=%08lx proc=%08lx extra=%08lx module=%08lx wnd=%08lx pcls=%08lx entry=%08lx\n",
            (unsigned long)guest_class, (unsigned long)client_class,
            (unsigned long)c_sas_dw_at(client_class + 4u),
            (unsigned long)c_sas_dw_at(client_class + 6u),
            (unsigned long)c_sas_dw_at(client_class + 24u),
            (unsigned long)c_sas_dw_at(client_class + 36u),
            (unsigned long)c_sas_dw_at(client_class + 40u),
            (unsigned long)c_sas_dw_at(client_class + 44u),
            (unsigned long)c_sas_dw_at(client_class + 64u),
            (unsigned long)c_sas_dw_at(client_class + 68u),
            (unsigned long)c_sas_dw_at(client_class + 76u),
            (unsigned long)c_sas_dw_at(client_class + 80u),
            (unsigned long)guest_window,
            (unsigned long)c_sas_dw_at(guest_window + 76u),
            (unsigned long)c_sas_dw_at(handles + 32u + 8u));
        goto done;
    }
    stage = "live-window-geometry";
    geometry_window = CreateWindowExA(0, "STATIC", "S2 geometry fixture",
        WS_OVERLAPPEDWINDOW, 120, 140, 280, 190, NULL, NULL,
        GetModuleHandle(NULL), NULL);
    if (!geometry_window || !mvdm_softpc_wow_page_domain_publish_handle(
            LOWORD(geometry_window), HIWORD(geometry_window), 1u, 0u,
            (ULONG)(ULONG_PTR)geometry_window, 0u)) goto done;
    {
        WINDOWINFO info = { sizeof(info) };
        ULONG projection = c_sas_dw_at(handles + LOWORD(geometry_window) * 16u)
            - c_sas_dw_at(teb + 0x60u);
        unsigned iteration, coordinate;
        for (iteration = 0; iteration < 2; ++iteration) {
            if (iteration && (!SetWindowPos(geometry_window, NULL,
                    230, 260, 330, 240, SWP_NOZORDER | SWP_NOACTIVATE) ||
                    !mvdm_softpc_wow_page_domain_refresh_window(
                        (ULONG)(ULONG_PTR)geometry_window))) goto done;
            if (!GetWindowInfo(geometry_window, &info)) goto done;
            for (coordinate = 0; coordinate < 4u; ++coordinate) {
                if (c_sas_dw_at(projection + 40u + coordinate * 4u) !=
                        (ULONG)(&info.rcWindow.left)[coordinate] ||
                    c_sas_dw_at(projection + 56u + coordinate * 4u) !=
                        (ULONG)(&info.rcClient.left)[coordinate]) goto done;
            }
            if (c_sas_dw_at(projection + 164u) != info.dwExStyle ||
                c_sas_dw_at(projection + 168u) != info.dwStyle) goto done;
        }
        if (mvdm_softpc_wow_page_domain_refresh_window(
                (ULONG)(ULONG_PTR)geometry_window ^ 0x10000u) ||
            !mvdm_softpc_wow_page_domain_retire_handle(LOWORD(geometry_window), NULL) ||
            mvdm_softpc_wow_page_domain_refresh_window(
                (ULONG)(ULONG_PTR)geometry_window)) goto done;
    }
    stage = "window-publication-lifecycle";
    {
    WORD index = LOWORD(geometry_window);
    ULONG entry = handles + index * 16u;
    ULONG retained_backing = 0u;
    ULONG outer_callback[2], inner_callback[2];
    if (!wow_user_window_publish(&object_table,
            geometry_window,
            &object_storage,
            (struct wow_task_order_thread *)&thread_storage) ||
        object_table.entries[index].phead != &object_storage ||
        object_table.entries[index].pOwner != &thread_storage ||
        object_table.entries[index].bType != 1u ||
        object_table.entries[index].wUniq != HIWORD(geometry_window) ||
        wow_user_window_full_handle(&object_table, index) != geometry_window ||
        object_table.last_handle != index ||
        c_sas_dw_at(entry + 8u) != ((ULONG)HIWORD(geometry_window) << 16 | 1u) ||
        !mvdm_softpc_wow_page_domain_callback_window(
            (ULONG)(ULONG_PTR)geometry_window, outer_callback) ||
        !wow_user_window_retire(&object_table,
            geometry_window, &object_storage, &retained_backing) ||
        wow_user_window_full_handle(&object_table, index) != NULL ||
        c_sas_dw_at(entry) != (ULONG)index + 1u ||
        c_sas_dw_at(entry + 8u) != 0x00010000u) goto done;
    stage = "retired-window-borrow";
    if (!retained_backing || c_sas_dw_at(retained_backing) !=
            (ULONG)(ULONG_PTR)geometry_window ||
        c_sas_dw_at(teb + 0x6cu) != (ULONG)(ULONG_PTR)geometry_window ||
        c_sas_dw_at(teb + 0x70u) != retained_backing ||
        !wow_user_window_publish(&object_table, geometry_window,
            &object_storage, (struct wow_task_order_thread *)&thread_storage) ||
        c_sas_dw_at(entry) - c_sas_dw_at(teb + 0x60u) == retained_backing ||
        !mvdm_softpc_wow_page_domain_callback_window(
            (ULONG)(ULONG_PTR)geometry_window, inner_callback) ||
        c_sas_dw_at(teb + 0x70u) == retained_backing ||
        !mvdm_softpc_wow_page_domain_restore_callback(inner_callback) ||
        c_sas_dw_at(teb + 0x70u) != retained_backing ||
        !wow_user_window_retire(&object_table, geometry_window,
            &object_storage, NULL) ||
        c_sas_dw_at(retained_backing) != (ULONG)(ULONG_PTR)geometry_window ||
        !mvdm_softpc_wow_page_domain_restore_callback(outer_callback) ||
        c_sas_dw_at(teb + 0x6cu) != 0u || c_sas_dw_at(teb + 0x70u) != 0u ||
        VdmFreeVirtualMemory(retained_backing) < 0) goto done;
    stage = "dead-window-publication-rejection";
    if (!DestroyWindow(geometry_window)) goto done;
    if (wow_user_window_publish(&object_table, geometry_window,
            &object_storage, (struct wow_task_order_thread *)&thread_storage) ||
        object_table.entries[index].bType != TYPE_FREE ||
        object_table.entries[index].phead != NULL ||
        object_table.windows[index] != NULL ||
        c_sas_dw_at(entry) != (ULONG)index + 1u ||
        c_sas_dw_at(entry + 8u) != 0x00010000u) goto done;
    geometry_window = NULL;
    }
#ifdef WOW_WINDOW_BORROW_FIXTURE
    stage = "production-window-borrow";
    if (!verify_callback_borrow(teb)) goto done;
#endif
    stage = "wow-gdt";
    if (!mvdm_softpc_wow_page_domain_activate_wow_context() ||
        (wow_gdt = c_getGDT_BASE()) == dosx_gdt) goto done;
    low = c_sas_dw_at(wow_gdt + 0x20u);
    high = c_sas_dw_at(wow_gdt + 0x24u);
    if (low != 0x0000ffffu || high != 0x00cff200u) goto done;
    low = c_sas_dw_at(wow_gdt + 0x38u);
    high = c_sas_dw_at(wow_gdt + 0x3cu);
    teb = (low >> 16) | ((high & 0xffu) << 16) | (high & 0xff000000u);
    if (teb != (uint32_t)mvdm_softpc_wow_page_domain_guest_teb() ||
        teb <= handles || (low & 0xffffu) != 0x0fffu ||
        (high & 0x00ffff00u) != 0x0000f200u ||
        c_sas_dw_at(teb + 0x18u) != teb ||
        c_sas_dw_at(teb + 0x44u) != 0u || c_sas_dw_at(teb + 0x50u) != 0u ||
        c_sas_dw_at(teb + 0x5cu) == 0u || c_sas_dw_at(teb + 0x60u) == 0u ||
        c_sas_dw_at(teb + 0x6cu) != 0u || c_sas_dw_at(teb + 0x70u) != 0u) goto done;
    stage = "expected-windows-version";
    if (!mvdm_softpc_wow_page_domain_set_expected_windows_version(0x0000030au) ||
        c_sas_dw_at(teb + 0x50u) != 0x0000030au ||
        !mvdm_softpc_wow_page_domain_set_expected_windows_version(0u) ||
        c_sas_dw_at(teb + 0x50u) != 0u) goto done;
    stage = "shared-cursor-publication";
    if (!mvdm_softpc_wow_page_domain_publish_cursor(319, 199) ||
        c_sas_dw_at(server_info + 0x304u) != 319u ||
        c_sas_dw_at(server_info + 0x308u) != 199u) goto done;
    stage = "call-server-flag";
    csr_flag = (uint32_t)mvdm_softpc_wow_page_domain_guest_csr_flag();
    if (csr_flag != teb + 4096u || c_sas_dw_at(csr_flag) != 0u ||
        !xtrn2phy(csr_flag, PG_U | PG_W, &second_physical)) goto done;
    phy_w32(second_physical, 1u);
    if (c_sas_dw_at(csr_flag) != 1u) goto done;
    phy_w32(second_physical, 0u);
    if (c_sas_dw_at(csr_flag) != 0u) goto done;
    stage = "client-desktop-publication";
    if (VdmAllocateVirtualMemory(&client_view, 4096u, TRUE) < 0) goto done;
    c_sas_fills(client_view, 0u, 4096u);
    /* Serialized fixture data only: reproduce the original consumer's
     * relocation chain without inventing a production desktop owner. */
    phy_w32(client_view + 8u, client_view + 128u + 0x80000000u);
    phy_w32(client_view + 128u, 0x12340002u);
    phy_w32(teb + 0x6cu, 0x11112222u);
    phy_w32(teb + 0x70u, 0x33334444u);
    if (!mvdm_softpc_wow_page_domain_set_client_desktop(client_view, 0x80000000u) ||
        c_sas_dw_at(teb + 0x5cu) != client_view ||
        c_sas_dw_at(teb + 0x60u) != 0x80000000u ||
        c_sas_dw_at(teb + 0x6cu) || c_sas_dw_at(teb + 0x70u) ||
        c_sas_dw_at(c_sas_dw_at(c_sas_dw_at(teb + 0x5cu) + 8u) -
            c_sas_dw_at(teb + 0x60u)) != 0x12340002u) goto done;
    stage = "client-desktop-null-rejection";
    if (mvdm_softpc_wow_page_domain_set_client_desktop(0u, 0u) ||
        c_sas_dw_at(teb + 0x5cu) != client_view) goto done;
    stage = "client-desktop-zero-delta-rejection";
    if (mvdm_softpc_wow_page_domain_set_client_desktop(client_view, 0u) ||
        c_sas_dw_at(teb + 0x5cu) != client_view ||
        c_sas_dw_at(teb + 0x60u) != 0x80000000u) goto done;
    stage = "client-desktop-wrong-address-space";
    saved_cr3 = c_getCR3();
    c_setCR3(dosx_gdt);
    if (mvdm_softpc_wow_page_domain_set_client_desktop(client_view, 0u) ||
        mvdm_softpc_wow_page_domain_clear_client_desktop()) {
        c_setCR3(saved_cr3);
        goto done;
    }
    c_setCR3(saved_cr3);
    if (c_sas_dw_at(teb + 0x5cu) != client_view ||
        c_sas_dw_at(teb + 0x60u) != 0x80000000u) goto done;
    stage = "client-desktop-withdrawal";
    if (!mvdm_softpc_wow_page_domain_clear_client_desktop() ||
        !mvdm_softpc_wow_page_domain_clear_client_desktop() ||
        mvdm_softpc_wow_page_domain_client_desktop_ready() ||
        c_sas_dw_at(teb + 0x5cu) || c_sas_dw_at(teb + 0x60u) ||
        c_sas_dw_at(teb + 0x6cu) || c_sas_dw_at(teb + 0x70u)) goto done;
    stage = "clock-instruction";
    first_tick = c_sas_dw_at(SHARED_CLOCK_LINEAR);
    Sleep(100u);
    stage = "clock-update";
    if (!xtrn2phy(SHARED_CLOCK_LINEAR, PG_U | PG_R, &second_physical) ||
        second_physical != first_physical ||
        c_sas_dw_at(SHARED_CLOCK_LINEAR) == first_tick) {
        goto done;
    }
    stage = "withdrawal";
    mvdm_softpc_wow_page_domain_leave_protected();
    if ((c_getCR0() & CR0_PG) != 0u || c_getGDT_BASE() != dosx_gdt) goto done;
    if (mvdm_softpc_wow_page_domain_set_client_desktop(client_view, 0u) ||
        mvdm_softpc_wow_page_domain_publish_cursor(1, 1) ||
        mvdm_softpc_wow_page_domain_publish_system_color(COLOR_WINDOW,
            0x00778899u)) goto done;
    stage = "reentry";
    if (!mvdm_softpc_wow_page_domain_reenter_protected() ||
        (c_getCR0() & (CR0_PE | CR0_PG)) != (CR0_PE | CR0_PG) ||
        !xtrn2phy(SHARED_CLOCK_LINEAR, PG_U | PG_R, &second_physical) ||
        c_getGDT_BASE() != wow_gdt ||
        c_sas_dw_at(wow_gdt + 0x20u) != 0x0000ffffu ||
        c_sas_dw_at(wow_gdt + 0x24u) != 0x00cff200u ||
        c_sas_dw_at(dosx_gdt + 0x20u) != 0x11112222u ||
        c_sas_dw_at(dosx_gdt + 0x24u) != 0x33334444u) {
        fprintf(stderr, "WOW_PAGE_DOMAIN_REENTRY cr0=%08lx gdt=%08lx expected=%08lx flat=%08lx:%08lx dosx=%08lx:%08lx\n",
            (unsigned long)c_getCR0(), (unsigned long)c_getGDT_BASE(),
            (unsigned long)wow_gdt,
            (unsigned long)c_sas_dw_at(wow_gdt + 0x24u),
            (unsigned long)c_sas_dw_at(wow_gdt + 0x20u),
            (unsigned long)c_sas_dw_at(dosx_gdt + 0x24u),
            (unsigned long)c_sas_dw_at(dosx_gdt + 0x20u));
        goto done;
    }
    stage = "selected-entry";
    mvdm_softpc_wow_page_domain_leave_protected();
    if (!mvdm_softpc_wow_page_domain_enter() ||
        c_getGDT_BASE() != wow_gdt) goto done;
    stage = "repeated-withdrawal";
    mvdm_softpc_wow_page_domain_leave_protected();
    mvdm_softpc_wow_page_domain_leave_protected();
    if (c_getGDT_BASE() != dosx_gdt ||
        !mvdm_softpc_wow_page_domain_reenter_protected() ||
        c_getGDT_BASE() != wow_gdt) goto done;
    passed = 1;
done:
    if (geometry_window) DestroyWindow(geometry_window);
    mvdm_softpc_wow_page_domain_leave_protected();
    if (original_gdt != 0u) c_setGDT_BASE_LIMIT(original_gdt, original_gdt_limit);
    if (guest_memory_started) mvdm_softpc_guest_memory_end(&owner);
    if (session_thread_current() == &owner) (void)session_thread_unbind(&owner);
    if (session_valid(&owner) && !session_dispose(&owner)) passed = 0;
    if (mvdm_softpc_wow_page_domain_active() ||
        mvdm_softpc_wow_page_domain_guest_shared_info() != 0u ||
        mvdm_softpc_wow_page_domain_clear_client_desktop() ||
        mvdm_softpc_wow_page_domain_guest_csr_flag() != 0u) passed = 0;
    if (dosx_gdt != 0u && VdmFreeVirtualMemory(dosx_gdt) < 0) passed = 0;
    if (client_view != 0u && VdmFreeVirtualMemory(client_view) < 0) passed = 0;
    if (physical_mapping_started) mvdm_softpc_physical_mapping_release();
    if (sas_started) sas_term();
    if (!passed) {
        fprintf(stderr, "WOW_PAGE_DOMAIN_LIFECYCLE_FAILED stage=%s\n", stage);
        return 1;
    }
    /* Original fprt.c intentionally discards puts in PROD; keep this fixture
     * marker on the CRT stderr route already used for failure evidence. */
    fprintf(stderr, "WOW_PAGE_DOMAIN_LIFECYCLE_OK\n");
    return 0;
}
