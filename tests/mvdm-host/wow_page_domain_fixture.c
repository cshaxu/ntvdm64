#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "ntvdm-exe/session/session.h"
#include "ntvdm-exe/softpc/include/mvdm_cvidc_vector_binding.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_guest_memory.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_physical_mapping.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_wow_page_domain.h"
#include "wow32-dll/include/wow_user_object_bindings.h"
#include "wow32-dll/include/wow_window_dispatch_bindings.h"
#include "wow32-dll/include/wow_menu_client_bindings.h"
#include "wow32-dll/include/wow_menu_native_bindings.h"
#include "ntvdm-exe/wow/include/wow_user_runtime.h"
#include "wow32-dll/include/wow_cleanup_bindings.h"

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
extern void c_sas_loads(uint32_t source, uint8_t *destination, uint32_t byte_count);
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
#include "wow32-dll/include/wow_class_client_bindings.h"
#include "wow32-dll/include/wow_standard_class_bindings.h"
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
        {
            ULONG menu = c_sas_dw_at(callback_backing + 104u);
            ULONG delta = c_sas_dw_at(callback_teb + 0x60u);
            BORROW_CHECK(menu && c_sas_dw_at(menu - delta) ==
                (ULONG)(ULONG_PTR)GetMenu(window));
            BORROW_CHECK(menu && c_sas_dw_at(menu - delta + 40u) == 1u);
        }
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
    wow_task_order_entry task = {0};
    wow_task_order_state task_state = {0};
    wow_user_runtime runtime = WOW_USER_RUNTIME_INITIALIZER;
    wow_user_runtime_thread runtime_binding = {0};
    wow_window_dispatch_target target = { 0x81234567u, destroy_in_callback };
    unsigned mode;
    process.objects = &objects;
    thread.ppi = &process;
    process.pwpi = &task_state;
    task_state.CSOwningThread = &thread;
    thread.ptdb = &task;
    task.hTaskWow = 0x4321u;
    if (!wow_user_runtime_bind(&runtime_binding, &runtime, &thread, NULL)) return 0;
    callback_teb = teb;
    for (mode = 0; mode < 2; ++mode) {
        HWND window = CreateWindowExA(0, "STATIC", "S2 callback borrow",
            0, 20, 30, 180, 120, NULL, NULL, GetModuleHandle(NULL), NULL);
        wow_window_words_binding *borrow;
        WW words = {0};
        LONG result = 0x123456;
        ULONG free_before, free_after, largest;
        BOOL caught = FALSE;
        HMENU menu;
        words.adwWOW[1] = 0x12345678u;
        if (!window) {
            (void)wow_user_runtime_unbind(&runtime_binding);
            return 0;
        }
        BORROW_CHECK(VdmQueryFreeVirtualMemory(&free_before, &largest) >= 0);
        menu = CreateMenu();
        BORROW_CHECK(menu && AppendMenuA(menu, MF_STRING, 17u, "Callback menu"));
        BORROW_CHECK(SetMenu(window, menu));
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
        BORROW_CHECK(!IsMenu(menu));
        BORROW_CHECK(wow_menu_native_refresh_menu(NULL));
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
    BORROW_CHECK(wow_user_runtime_unbind(&runtime_binding));
    fprintf(stderr, "WOW_WINDOW_BORROW_CCPU errors=%u normal=1 exception=1\n", borrow_errors);
    return borrow_errors == 0;
}

static DWORD WINAPI class_projection_version(HANDLE module)
{
    (void)module;
    return 0x030au;
}

static DWORD WINAPI class_projection_callback(HWND window, UINT message,
    UINT wp, LONG lp, DWORD procedure, PVOID words)
{
    (void)window; (void)message; (void)wp; (void)lp;
    (void)procedure; (void)words;
    BORROW_CHECK(FALSE); /* Register/query/unregister must not call a WNDPROC. */
    return 0;
}

static int verify_class_registration(ULONG teb)
{
    wow_class_lookup_context classes = {0};
    wow_task_order_thread thread = {0};
    wow_task_order_entry task = {0};
    wow_class_client_context context = {&classes, 0x030a,
        class_projection_version, class_projection_callback, &thread, NULL};
    WNDCLASSA wc = {0}, native_view;
    DWORD words[2] = {0x12345678u, 0x87654321u};
    unsigned mode, before = borrow_errors;
    classes.client_module = GetModuleHandleW(L"user32.dll");
    thread.ptdb = &task;
    task.hTaskWow = 0x4321u;
    wc.hInstance = (HINSTANCE)0x43211234u;
    wc.lpfnWndProc = (WNDPROC)0x81234567u;
    wc.lpszClassName = "S2_CCPU_CLASS_COMMIT";
    for (mode = 0; mode < 2; ++mode) {
        wow_class_words_binding *binding;
        wow_class_lookup_entry *entry;
        ULONG free_before = 0, free_after = 0, largest = 0;
        ATOM atom;
        thread.TIF_flags = mode ? 0 : WOW_TASK_TIF_16BIT;
        wc.style = mode ? CS_GLOBALCLASS : 0;
        BORROW_CHECK(VdmQueryFreeVirtualMemory(&free_before, &largest) >= 0);
        atom = wow_class_client_register(&context, &wc, words);
        BORROW_CHECK(atom != 0);
        if (!atom) continue;
        binding = wow_class_words_acquire(&classes, atom, wc.hInstance);
        BORROW_CHECK(binding != NULL);
        entry = wow_class_words_entry(binding);
        BORROW_CHECK(entry && entry->guest_server);
        if (entry && entry->guest_server) {
            ULONG client = entry->guest_server - c_sas_dw_at(teb + 0x60u);
            /* Real CCPU reads of original CLS.adwWOW / hTaskWow. No mocked
             * parent export: this case links the selected worker page owner. */
            BORROW_CHECK(c_sas_dw_at(client + 36u) == words[0]);
            BORROW_CHECK(c_sas_dw_at(client + 40u) == words[1]);
            BORROW_CHECK(c_sas_dw_at(client + 44u) == (mode ? 0u : task.hTaskWow));
            BORROW_CHECK(c_sas_dw_at(client + 68u) == (ULONG)wc.lpfnWndProc);
            BORROW_CHECK(entry->adwWOW[0] == words[0] && entry->adwWOW[1] == words[1]);
        }
        wow_class_words_release(binding);
        BORROW_CHECK(wow_class_client_unregister(&context, wc.lpszClassName, wc.hInstance));
        BORROW_CHECK(!classes.pclsPrivateList && !classes.pclsPublicList);
        BORROW_CHECK(!GetClassInfoA(wc.hInstance, wc.lpszClassName, &native_view));
        BORROW_CHECK(VdmQueryFreeVirtualMemory(&free_after, &largest) >= 0 &&
            free_after == free_before);
    }
    fprintf(stderr, "WOW_CLASS_COMMIT_CCPU errors=%u cases=2 original_words=1 original_task=1 released=1\n",
        borrow_errors - before);
    return borrow_errors == before;
}

static int verify_native_class_projection(ULONG teb)
{
    wow_class_lookup_context classes = {0};
    wow_class_lookup_entry *entry;
    ULONG free_before = 0, free_after = 0, largest = 0;
    ULONG delta = c_sas_dw_at(teb + 0x60u);
    unsigned before = borrow_errors, projected = 0;
    classes.client_module = GetModuleHandleW(L"user32.dll");
    BORROW_CHECK(VdmQueryFreeVirtualMemory(&free_before, &largest) >= 0);
    BORROW_CHECK(wow_standard_classes_prepare(&classes, class_projection_callback));
    for (entry = classes.pclsPublicList; entry; entry = entry->next) {
        wow_standard_class_view native_class, by_name;
        wow_class_words_binding *again;
        ULONG server, client, name, remaining_before = 0, remaining_after = 0;
        char text[64] = {0};
        unsigned i;
        BORROW_CHECK(wow_standard_class_describe(MAKEINTATOM(entry->atomClassName), &native_class));
        BORROW_CHECK(wow_class_words_publish_guest(entry, &native_class.native));
        server = entry->guest_server;
        BORROW_CHECK(server != 0);
        if (!server) continue;
        client = server - delta;
        BORROW_CHECK((WORD)c_sas_dw_at(client + 4u) == entry->atomClassName);
        BORROW_CHECK((WORD)(c_sas_dw_at(client + 4u) >> 16) == entry->fnid);
        BORROW_CHECK(c_sas_dw_at(client + 36u) == 0 && c_sas_dw_at(client + 40u) == 0);
        BORROW_CHECK(c_sas_dw_at(client + 44u) == 0);
        BORROW_CHECK(c_sas_dw_at(client + 64u) == native_class.native.style);
        BORROW_CHECK(c_sas_dw_at(client + 68u) == 0); /* No native/CPD address. */
        BORROW_CHECK(c_sas_dw_at(client + 76u) == (ULONG)native_class.native.cbWndExtra);
        BORROW_CHECK(c_sas_dw_at(client + 80u) == 0); /* No native module. */
        name = c_sas_dw_at(client + 100u);
        BORROW_CHECK(name == server + 108u);
        for (i = 0; i < sizeof(text) - 1u; ++i) {
            c_sas_loads(name - delta + i, (uint8_t *)&text[i], 1);
            if (!text[i]) break;
        }
        BORROW_CHECK(i < sizeof(text) - 1u);
        BORROW_CHECK(wow_standard_class_describe(text, &by_name));
        BORROW_CHECK(by_name.atom == entry->atomClassName);
        again = wow_class_words_acquire(&classes, entry->atomClassName, (HANDLE)0x12345678u);
        BORROW_CHECK(wow_class_words_entry(again) == entry);
        wow_class_words_release(again);
        BORROW_CHECK(VdmQueryFreeVirtualMemory(&remaining_before, &largest) >= 0);
        BORROW_CHECK(!wow_class_words_publish_guest(entry, &native_class.native));
        BORROW_CHECK(GetLastError() == ERROR_ALREADY_EXISTS && entry->guest_server == server);
        BORROW_CHECK(VdmQueryFreeVirtualMemory(&remaining_after, &largest) >= 0 &&
            remaining_after == remaining_before);
        ++projected;
    }
    BORROW_CHECK(projected == 9);
    BORROW_CHECK(wow_class_words_forget_native(&classes));
    BORROW_CHECK(classes.pclsPublicList == NULL);
    BORROW_CHECK(VdmQueryFreeVirtualMemory(&free_after, &largest) >= 0 && free_after == free_before);
    fprintf(stderr, "WOW_NATIVE_CLASS_CCPU errors=%u projected=%u native_pointers=0 released=1\n",
        borrow_errors - before, projected);
    return borrow_errors == before;
}
#endif

static unsigned menu_callouts, menu_locked_callouts;

static LRESULT CALLBACK menu_callout_window(HWND window, UINT message,
    WPARAM wp, LPARAM lp)
{
    if (message == WM_NCCALCSIZE) {
        wow_user_runtime_thread *binding = wow_user_runtime_current();
        ++menu_callouts;
        if (binding && binding->exclusive_held) ++menu_locked_callouts;
    }
    return DefWindowProcA(window, message, wp, lp);
}

static BOOL verify_native_menu_entry(wow_task_order_process *process,
    ULONG handles, ULONG delta)
{
    wow_user_runtime runtime = WOW_USER_RUNTIME_INITIALIZER;
    wow_user_runtime_thread binding = {0};
    wow_task_order_thread thread = {0};
    wow_task_order_entry task = {0};
    HMENU root = NULL, child = NULL, alias;
    HWND window = NULL;
    wow_cleanup_window window_record = {0};
    WNDCLASSA window_class = {0};
    BOOL published = FALSE;
    ULONG root_client, child_client, items;
    BOOL result = FALSE;
    const char *step = "bind";
    thread.ppi = process;
    thread.ptdb = &task;
    task.hTaskWow = 0x4321u;
    if (!wow_user_runtime_bind(&binding, &runtime, &thread, NULL)) return FALSE;
    step = "create";
    root = wow_menu_native_CreateMenu();
    child = wow_menu_native_CreatePopupMenu();
    if (!root || !child) goto cleanup;
    alias = (HMENU)(LONG_PTR)(SHORT)LOWORD(child);
    /* Original HMENU32 sign-extends, including popup operands, not just roots. */
    step = "append";
    if (!wow_menu_native_AppendMenuA(alias, MF_STRING, 71u, "Leaf") ||
            !wow_menu_native_AppendMenuA(root, MF_POPUP, (UINT_PTR)alias, "Root"))
        goto cleanup;
    root_client = c_sas_dw_at(handles + LOWORD(root) * 16u) - delta;
    child_client = c_sas_dw_at(handles + LOWORD(child) * 16u) - delta;
    items = c_sas_dw_at(root_client + 56u) - delta;
    step = "published-tree-and-check";
    if (c_sas_dw_at(root_client + 40u) != 1u ||
            c_sas_dw_at(items + 12u) != child_client + delta ||
            wow_menu_native_CheckMenuItem(alias, 71u, MF_CHECKED) == (DWORD)-1)
        goto cleanup;
    step = "set-menu-callout";
    window_class.lpfnWndProc = menu_callout_window;
    window_class.hInstance = GetModuleHandle(NULL);
    window_class.lpszClassName = "T447MenuCallout";
    if (!RegisterClassA(&window_class)) goto cleanup;
    window = CreateWindowA(window_class.lpszClassName, "Menu callout",
        WS_OVERLAPPEDWINDOW, 0, 0, 240, 160, NULL, NULL,
        window_class.hInstance, NULL);
    if (!window) goto cleanup;
    published = wow_user_window_publish(process->objects, window, &window_record, &thread);
    if (!published || !wow_user_runtime_enter(&binding)) goto cleanup;
    menu_callouts = menu_locked_callouts = 0;
    if (!wow_menu_native_SetMenu(window, root) || !binding.exclusive_held ||
            !menu_callouts || menu_locked_callouts ||
            c_sas_dw_at(c_sas_dw_at(handles + LOWORD(window) * 16u) - delta + 104u)
                != root_client + delta) goto cleanup;
    {
        ULONG wnd = c_sas_dw_at(handles + LOWORD(window) * 16u) - delta;
        ULONG query = c_sas_dw_at(wnd + 104u);
        /* USER16 _GetMenu@4 uses REBASEPTR, not an unconditional delta
         * subtraction. Compare before dereference so a bad projection
         * fails here rather than faulting the fixture's CCPU data access. */
        if (query > 0x7ffeffffu) query = query - c_sas_dw_at(wnd + 12u) + wnd;
        if (query != root_client || c_sas_dw_at(query) != (ULONG)(ULONG_PTR)root)
            goto cleanup;
    }
    /* Mutation wrappers must also preserve an inherited lock on success
     * and a native lookup failure, without changing task ownership. */
    if (wow_menu_native_CheckMenuItem(alias, 71u, MF_UNCHECKED) == (DWORD)-1 ||
            !binding.exclusive_held ||
            wow_menu_native_CheckMenuItem(alias, 999u, MF_CHECKED) != (DWORD)-1 ||
            !binding.exclusive_held ||
            wow_menu_native_CheckMenuItem(alias, 71u, MF_CHECKED) == (DWORD)-1 ||
            !binding.exclusive_held) goto cleanup;
    if (!wow_menu_native_SetMenu(window, NULL) || !binding.exclusive_held ||
            !wow_user_runtime_leave(&binding)) goto cleanup;
    if (!wow_user_window_retire(process->objects, window, &window_record, NULL))
        goto cleanup;
    published = FALSE;
    DestroyWindow(window);
    window = NULL;
    UnregisterClassA(window_class.lpszClassName, window_class.hInstance);
    items = c_sas_dw_at(child_client + 56u) - delta;
    step = "remove-and-destroy";
    if (!(c_sas_dw_at(items + 4u) & MF_CHECKED) ||
            wow_menu_native_CheckMenuItem(alias, 999u, MF_CHECKED) != (DWORD)-1 ||
            !(c_sas_dw_at(items + 4u) & MF_CHECKED) ||
            !wow_menu_native_RemoveMenu(root, 0u, MF_BYPOSITION) ||
            c_sas_dw_at(root_client + 40u) != 0u || !IsMenu(child) ||
            !wow_menu_native_DestroyMenu(root)) goto cleanup;
    root = NULL;
    if (!wow_menu_native_DestroyMenu(alias) || IsMenu(child) ||
            process->objects->entries[LOWORD(child)].bType != TYPE_FREE)
        goto cleanup;
    child = NULL;
    result = TRUE;
cleanup:
    if (!result) fprintf(stderr, "WOW_NATIVE_MENU_ENTRY_FAILED step=%s error=%lu\n",
        step, GetLastError());
    if (binding.exclusive_held && !wow_user_runtime_leave(&binding)) result = FALSE;
    if (published && !wow_user_window_retire(process->objects, window,
            &window_record, NULL)) result = FALSE;
    if (window) DestroyWindow(window);
    if (window_class.lpszClassName)
        UnregisterClassA(window_class.lpszClassName, window_class.hInstance);
    if (root && IsMenu(root)) DestroyMenu(root);
    if (child && IsMenu(child)) DestroyMenu(child);
    if (!wow_menu_client_retire_destroyed(process)) result = FALSE;
    if (!wow_user_runtime_unbind(&binding)) result = FALSE;
    fprintf(stderr, "WOW_NATIVE_MENU_ENTRY_CCPU result=%u\n", result);
    return result;
}

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
    char class_name[] = "S2 Class Name";
    HWND geometry_window = NULL, id_window = NULL;
    HMENU native_root = NULL, native_submenu = NULL;
    static ULONG native_bases[0x1fff];
    static wow_user_object_table object_table;
    wow_task_order_process menu_process = { 0 };
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
    if (mvdm_softpc_wow_page_domain_publish_named_class(
            0xc123u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, NULL, 1u) ||
        mvdm_softpc_wow_page_domain_publish_named_class(
            0xc123u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, class_name, 0u) ||
        mvdm_softpc_wow_page_domain_publish_named_class(
            0xc123u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, class_name, 2u) ||
        mvdm_softpc_wow_page_domain_publish_named_class(
            0xc123u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, class_name, 0xffffffffu))
        goto done;
    guest_class = (uint32_t)mvdm_softpc_wow_page_domain_publish_named_class(
        0xc123u, 0x0042u, 0x00000002u, 0x11223344u, 0x55667788u,
        0x00004321u, 0x10000000u, 0x0000f00du, 8u, 0u,
        class_name, sizeof(class_name));
    class_name[0] = 'X'; /* Call-local source must no longer own guest bytes. */
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
    stage = "guest-class-name-rebase";
    {
        uint32_t window = c_sas_dw_at(handles + 32u) - c_sas_dw_at(teb + 0x60u);
        uint32_t self = c_sas_dw_at(window + 12u);
        /* Exact original GetClassNameA pointer chain, not an assumed delta
         * subtraction: WND.pcls-self+WND, then CLS.name-self+WND. */
        uint32_t cls = c_sas_dw_at(window + 76u) - self + window;
        uint32_t name = c_sas_dw_at(cls + 100u) - self + window;
        char copied[sizeof(class_name)];
        if (name != cls + 108u) goto guest_window_failure;
        c_sas_loads(name, (uint8_t *)copied, sizeof(copied));
        if (memcmp(copied, "S2 Class Name", sizeof(copied))) goto guest_window_failure;
        fprintf(stderr, "WOW_CLASS_NAME_CCPU copied=1 rebase=1 bytes=%u\n",
            (unsigned)sizeof(copied));
    }
    stage = "guest-window-desktop-membership";
    {
        uint32_t desktop = c_sas_dw_at(teb + 0x5cu);
        uint32_t server_window = c_sas_dw_at(handles + 32u);
        if (server_window < c_sas_dw_at(desktop) ||
                server_window + 176u > c_sas_dw_at(desktop + 4u) ||
                guest_class < c_sas_dw_at(desktop) ||
                guest_class + 108u > c_sas_dw_at(desktop + 4u))
            goto guest_window_failure;
        /* Execute the original range-conditioned rebase, with no callback
         * cache. The returned WND must be readable by the CCPU SAS path. */
        if (server_window >= c_sas_dw_at(desktop) &&
                server_window < c_sas_dw_at(desktop + 4u))
            server_window -= c_sas_dw_at(teb + 0x60u);
        if (c_sas_dw_at(server_window) != 0x23450002u)
            goto guest_window_failure;
    }
    stage = "guest-window-retirement";
    {
        mvdm_softpc_wow_menu_query_item leaf = { 0u, MF_CHECKED, 7u, 0u };
        mvdm_softpc_wow_menu_query_item entries[2] = {
            { 0u, 0u, 4u, 0x45670004u }, { MF_SEPARATOR, MF_DISABLED, 8u, 0u }
        };
        ULONG child_menu, root_menu, root_client, item_array, window_client;
        ULONG delta = c_sas_dw_at(teb + 0x60u);
        stage = "guest-menu-graph-publication";
        child_menu = mvdm_softpc_wow_page_domain_publish_menu(0x45670004u,
            0x4321u, 1u, &leaf);
        root_menu = mvdm_softpc_wow_page_domain_publish_menu(0x45670003u,
            0x4321u, 2u, entries);
        if (!child_menu || !root_menu) goto done;
        root_client = root_menu - delta;
        item_array = c_sas_dw_at(root_client + 56u) - delta;
        window_client = c_sas_dw_at(handles + 32u) - delta;
        if (!child_menu || !root_menu || c_sas_dw_at(root_client) != 0x45670003u ||
                c_sas_dw_at(root_client + 12u) != root_menu ||
                c_sas_dw_at(root_client + 20u) != 0x4321u ||
                c_sas_dw_at(root_client + 40u) != 2u ||
                c_sas_dw_at(item_array + 12u) != child_menu ||
                c_sas_dw_at(item_array + 64u) != MF_SEPARATOR ||
                c_sas_dw_at(item_array + 68u) != MF_DISABLED ||
                c_sas_dw_at(handles + 48u) != root_menu ||
                c_sas_dw_at(handles + 56u) != 0x45670002u ||
                !mvdm_softpc_wow_page_domain_window_menu(0x23450002u, 0x45670003u) ||
                c_sas_dw_at(window_client + 104u) != root_menu) goto done;
        stage = "guest-menu-rejection-preserves-graph";
        entries[0].submenu = 0x98760004u; /* stale identity, same slot */
        if (mvdm_softpc_wow_page_domain_publish_menu(0x45670003u, 0x4321u, 2u, entries) ||
                mvdm_softpc_wow_page_domain_publish_menu(0x45670003u, 1u, 0u, NULL) ||
                mvdm_softpc_wow_page_domain_publish_menu(0x98760003u, 0x4321u, 0u, NULL) ||
                mvdm_softpc_wow_page_domain_publish_menu(0x23450002u, 0x4321u, 0u, NULL) ||
                mvdm_softpc_wow_page_domain_publish_handle(3u, 0x4567u, 1u, 0u, 0u, 0u) ||
                mvdm_softpc_wow_page_domain_publish_menu(0x45670003u, 0x4321u, 0x4000000u, entries) ||
                mvdm_softpc_wow_page_domain_window_menu(0x98760002u, 0x45670003u) ||
                mvdm_softpc_wow_page_domain_window_menu(0x23450002u, 0x98760003u) ||
                c_sas_dw_at(root_client + 40u) != 2u ||
                c_sas_dw_at(root_client + 56u) != item_array + delta ||
                c_sas_dw_at(item_array + 12u) != child_menu) goto done;
        entries[0].submenu = 0x45670003u;
        if (mvdm_softpc_wow_page_domain_publish_menu(0x45670003u,
                0x4321u, 2u, entries) ||
                c_sas_dw_at(item_array + 12u) != child_menu) goto done;
        stage = "guest-menu-update-and-detach";
        /* Detached submenu remains independently published, as RemoveMenu
         * requires. This fixture checks carrier lifetime, not native policy. */
        if (mvdm_softpc_wow_page_domain_publish_menu(0x45670003u, 0x4321u, 0u, NULL)
                    != root_menu || c_sas_dw_at(root_client + 40u) != 0u ||
                c_sas_dw_at(root_client + 56u) != 0u ||
                c_sas_dw_at(child_menu - delta) != 0x45670004u ||
                !mvdm_softpc_wow_page_domain_window_menu(0x23450002u, 0u) ||
                c_sas_dw_at(window_client + 104u) != 0u ||
                !mvdm_softpc_wow_page_domain_retire_menu(0x45670003u) ||
                !mvdm_softpc_wow_page_domain_retire_menu(0x45670004u)) goto done;
        stage = "guest-menu-slot-reuse";
        if (!mvdm_softpc_wow_page_domain_publish_menu(0x98760003u, 0x4321u, 0u, NULL) ||
                mvdm_softpc_wow_page_domain_retire_menu(0x45670003u) ||
                !mvdm_softpc_wow_page_domain_retire_menu(0x98760003u) ||
                c_sas_dw_at(handles + 56u) != 0x00010000u) goto done;
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
    stage = "native-menu-entry-publication";
    menu_process.objects = &object_table;
    if (!verify_native_menu_entry(&menu_process, handles,
            c_sas_dw_at(teb + 0x60u))) goto done;
    stage = "native-menu-graph-enrollment";
    native_root = CreateMenu();
    native_submenu = CreatePopupMenu();
    if (!native_root || !native_submenu ||
            !AppendMenuA(native_submenu, MF_STRING | MF_CHECKED, 71u, "Leaf") ||
            !AppendMenuA(native_root, MF_POPUP, (UINT_PTR)native_submenu, "Root") ||
            !wow_menu_client_publish_tree(&menu_process, 0x4321u, native_root))
        goto done;
    {
        ULONG delta = c_sas_dw_at(teb + 0x60u);
        ULONG root_client = c_sas_dw_at(handles + LOWORD(native_root) * 16u) - delta;
        ULONG sub_client = c_sas_dw_at(handles + LOWORD(native_submenu) * 16u) - delta;
        ULONG sub_items = c_sas_dw_at(sub_client + 56u) - delta;
        if (object_table.entries[LOWORD(native_root)].bType != TYPE_MENU ||
                object_table.entries[LOWORD(native_root)].pOwner != &menu_process ||
                c_sas_dw_at(root_client + 40u) != 1u ||
                c_sas_dw_at(sub_client + 40u) != 1u ||
                c_sas_dw_at(sub_items + 8u) != 71u ||
                !(c_sas_dw_at(sub_items + 4u) & MF_CHECKED) ||
                wow_menu_client_full_handle(&object_table,
                    (HMENU)(LONG_PTR)(SHORT)LOWORD(native_submenu)) != native_submenu)
            goto done;
        stage = "native-menu-mutation";
        if (CheckMenuItem(native_submenu, 71u, MF_BYCOMMAND | MF_UNCHECKED) == (DWORD)-1 ||
                !wow_menu_client_publish_tree(&menu_process, 0x9999u, native_root))
            goto done;
        sub_items = c_sas_dw_at(sub_client + 56u) - delta;
        if ((c_sas_dw_at(sub_items + 4u) & MF_CHECKED) ||
                c_sas_dw_at(sub_client + 20u) != 0x4321u) goto done;
        stage = "native-menu-remove-retains-submenu";
        if (!RemoveMenu(native_root, 0, MF_BYPOSITION) ||
                !wow_menu_client_publish_tree(&menu_process, 0x4321u, native_root) ||
                !wow_menu_client_destroy_object(&menu_process,
                    &object_table.entries[LOWORD(native_root)])) goto done;
        native_root = NULL;
        if (!IsMenu(native_submenu) ||
                object_table.entries[LOWORD(native_submenu)].bType != TYPE_MENU ||
                !wow_menu_client_destroy_object(&menu_process,
                    &object_table.entries[LOWORD(native_submenu)])) goto done;
        native_submenu = NULL;
        stage = "native-menu-recursive-destruction";
        native_root = CreateMenu();
        native_submenu = CreatePopupMenu();
        if (!native_root || !native_submenu ||
                !AppendMenuA(native_root, MF_POPUP, (UINT_PTR)native_submenu, "Sub") ||
                !wow_menu_client_publish_tree(&menu_process, 0x4321u, native_root) ||
                !wow_menu_client_destroy_object(&menu_process,
                    &object_table.entries[LOWORD(native_root)])) goto done;
        native_root = NULL;
        if (IsMenu(native_submenu) ||
                object_table.entries[LOWORD(native_submenu)].bType != TYPE_FREE)
            goto done;
        native_submenu = NULL;
    }
    stage = "original-menu-cleanup-phases";
    {
        wow_class_lookup_context classes = {0};
        wow_task_order_thread thread = {0};
        wow_task_order_entry task = {0};
        wow_input_desktop desktop = {0};
        wow_cleanup_desktop_info deskinfo = {0};
        wow_cleanup_shared cleanup_shared = {0};
        wow_cleanup_context cleanup = {0};
        menu_process.classes = &classes;
        thread.ppi = &menu_process;
        thread.ptdb = &task;
        task.hTaskWow = 0x4321u;
        thread.rpdesk = &desktop;
        desktop.pDeskInfo = &deskinfo;
        cleanup.thread = &thread;
        cleanup.shared = &cleanup_shared;
        cleanup.destroy_object = wow_menu_client_cleanup;
        native_root = CreateMenu();
        native_submenu = CreateMenu(); /* Independent other-task menu. */
        if (!native_root || !native_submenu ||
                !wow_menu_client_publish_tree(&menu_process, 0x4321u, native_root) ||
                !wow_menu_client_publish_tree(&menu_process, 0x9876u, native_submenu) ||
                !wow_user_cleanup_bound((HANDLE)0x4321u, 0x4321u, NULL, 0, &cleanup) ||
                !IsMenu(native_root) || !IsMenu(native_submenu) ||
                !wow_user_cleanup_bound(NULL, 0x4321u, NULL, 0, &cleanup) ||
                IsMenu(native_root) || !IsMenu(native_submenu) ||
                object_table.entries[LOWORD(native_root)].bType != TYPE_FREE ||
                object_table.entries[LOWORD(native_submenu)].bType != TYPE_MENU)
            goto done;
        native_root = NULL;
        if (!wow_user_cleanup_bound(NULL, 0x9876u, NULL, 0, &cleanup) ||
                IsMenu(native_submenu) ||
                object_table.entries[LOWORD(native_submenu)].bType != TYPE_FREE)
            goto done;
        native_submenu = NULL;
        menu_process.classes = NULL;
    }
    stage = "live-window-geometry";
    geometry_window = CreateWindowExA(0, "STATIC", "S2 geometry fixture",
        WS_OVERLAPPEDWINDOW, 120, 140, 280, 190, NULL, NULL,
        GetModuleHandle(NULL), NULL);
    if (!geometry_window || !mvdm_softpc_wow_page_domain_publish_handle(
            LOWORD(geometry_window), HIWORD(geometry_window), 1u, 0u,
            (ULONG)(ULONG_PTR)geometry_window, 0u)) goto done;
    stage = "child-control-id-publication";
    id_window = CreateWindowExA(0, "STATIC", "S2 control ID fixture",
        WS_CHILD, 0, 0, 20, 20, geometry_window, (HMENU)(ULONG_PTR)0x8123u,
        GetModuleHandle(NULL), NULL);
    if (!id_window || !mvdm_softpc_wow_page_domain_publish_handle(
            LOWORD(id_window), HIWORD(id_window), 1u, 0u,
            (ULONG)(ULONG_PTR)id_window, 0u)) goto done;
    {
        ULONG projection = c_sas_dw_at(handles + LOWORD(id_window) * 16u)
            - c_sas_dw_at(teb + 0x60u);
        ULONG parent_projection = c_sas_dw_at(
            handles + LOWORD(geometry_window) * 16u) - c_sas_dw_at(teb + 0x60u);
        ULONG saved[2];
        if (c_sas_dw_at(projection + 104u) != 0x8123u ||
                mvdm_softpc_wow_page_domain_window_menu((ULONG)(ULONG_PTR)id_window, 0u) ||
                c_sas_dw_at(parent_projection + 104u) != 0u) goto done;
        /* Control IDs are DWORD data: neither server relocation nor handle
         * alias conversion applies, including zero and high-bit values. */
        SetWindowLongA(id_window, GWL_ID, (LONG)0xfedcba98u);
        if (!mvdm_softpc_wow_page_domain_callback_window(
                (ULONG)(ULONG_PTR)id_window, saved) ||
                c_sas_dw_at(projection + 104u) != 0xfedcba98u ||
                !mvdm_softpc_wow_page_domain_restore_callback(saved)) goto done;
        SetWindowLongA(id_window, GWL_ID, 0);
        if (!mvdm_softpc_wow_page_domain_refresh_window(
                (ULONG)(ULONG_PTR)id_window) ||
                c_sas_dw_at(projection + 104u) != 0u ||
                !mvdm_softpc_wow_page_domain_retire_handle(
                    LOWORD(id_window), NULL)) goto done;
    }
    if (!DestroyWindow(id_window)) goto done;
    id_window = NULL;
    stage = "live-window-geometry";
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
    ULONG expected_last = object_table.last_handle > index ? object_table.last_handle : index;
    ULONG entry = handles + index * 16u;
    ULONG retained_backing = 0u;
    ULONG outer_callback[2], inner_callback[2];
    ULONG desktop = c_sas_dw_at(teb + 0x5cu);
    ULONG desktop_base, desktop_limit;
    if (!wow_user_window_publish(&object_table,
            geometry_window,
            &object_storage,
            (struct wow_task_order_thread *)&thread_storage) ||
        object_table.entries[index].phead != &object_storage ||
        object_table.entries[index].pOwner != &thread_storage ||
        object_table.entries[index].bType != 1u ||
        object_table.entries[index].wUniq != HIWORD(geometry_window) ||
        wow_user_window_full_handle(&object_table, index) != geometry_window ||
        object_table.last_handle != expected_last ||
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
    desktop_base = c_sas_dw_at(desktop);
    desktop_limit = c_sas_dw_at(desktop + 4u);
    if (c_sas_dw_at(teb + 0x5cu) != desktop ||
        c_sas_dw_at(entry) >= desktop_base) goto done;
    if (!DestroyWindow(geometry_window)) goto done;
    if (wow_user_window_publish(&object_table, geometry_window,
            &object_storage, (struct wow_task_order_thread *)&thread_storage) ||
        object_table.entries[index].bType != TYPE_FREE ||
        object_table.entries[index].phead != NULL ||
        object_table.windows[index] != NULL ||
        c_sas_dw_at(entry) != (ULONG)index + 1u ||
        c_sas_dw_at(entry + 8u) != 0x00010000u ||
        c_sas_dw_at(desktop) != desktop_base ||
        c_sas_dw_at(desktop + 4u) != desktop_limit) goto done;
    geometry_window = NULL;
    }
#ifdef WOW_WINDOW_BORROW_FIXTURE
    stage = "production-class-registration";
    if (!verify_class_registration(teb)) goto done;
    stage = "native-class-projection";
    if (!verify_native_class_projection(teb)) goto done;
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
    if (native_root && IsMenu(native_root)) DestroyMenu(native_root);
    if (native_submenu && IsMenu(native_submenu)) DestroyMenu(native_submenu);
    if (id_window) DestroyWindow(id_window);
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
