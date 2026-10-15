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
#include "wow32-dll/include/wow_user_task_lifecycle.h"
#include "wow32-dll/include/wow_user_private_access.h"

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
#include "wow32-dll/include/wow_dialog_creation_binding.h"
#include "wow32-dll/include/wow_window_creation_binding.h"
#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")
#include "opennt-abi/host-compat/include/wow32_provider_private.h"
#include "wow32-dll/include/wow_window_words_binding.h"
#include "wow32-dll/include/wow_task_order_bindings.h"
#include "wow32-dll/include/wow_class_client_bindings.h"
#include "wow32-dll/include/wow_standard_class_bindings.h"
#include "wow32-dll/include/wow_user_borrow_scope.h"
extern PWW WINAPI FindPWW(HANDLE, INT);
extern INT WINAPI GetStdClassNumber(LPSTR);
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
        BORROW_CHECK(wow_native_DestroyWindow(window));
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

static INT_PTR CALLBACK dialog_cache_callback(HWND window, UINT message,
    WPARAM wp, LPARAM lp)
{
    ULONG backing = c_sas_dw_at(callback_teb + 0x70u);
    (void)wp; (void)lp;
    BORROW_CHECK(c_sas_dw_at(callback_teb + 0x6cu) == (ULONG)(ULONG_PTR)window);
    BORROW_CHECK(backing && c_sas_dw_at(backing) == (ULONG)(ULONG_PTR)window);
    if (message == WM_APP + 99) {
        BORROW_CHECK(wow_native_DestroyWindow(window));
        BORROW_CHECK(c_sas_dw_at(callback_teb + 0x70u) == backing);
        BORROW_CHECK(c_sas_dw_at(backing) == (ULONG)(ULONG_PTR)window);
        BORROW_CHECK(c_sas_dw_at(backing + 28u) == 0u &&
            c_sas_dw_at(backing + 32u) == 0u);
    }
    return FALSE;
}

static ULONG verify_mutated_window(HWND window, ULONG delta)
{
    ULONG backing = mvdm_softpc_wow_page_domain_borrow_window((ULONG)(ULONG_PTR)window);
    ULONG cls = 0;
    WINDOWINFO info = {sizeof(info)};
    BORROW_CHECK(backing && GetWindowInfo(window, &info));
    if (backing) {
        BORROW_CHECK(c_sas_dw_at(backing + 168u) == info.dwStyle);
        BORROW_CHECK(c_sas_dw_at(backing + 164u) == info.dwExStyle);
        BORROW_CHECK(c_sas_dw_at(backing + 40u) == (ULONG)info.rcWindow.left &&
            c_sas_dw_at(backing + 44u) == (ULONG)info.rcWindow.top &&
            c_sas_dw_at(backing + 48u) == (ULONG)info.rcWindow.right &&
            c_sas_dw_at(backing + 52u) == (ULONG)info.rcWindow.bottom);
        BORROW_CHECK(c_sas_dw_at(backing + 56u) == (ULONG)info.rcClient.left &&
            c_sas_dw_at(backing + 60u) == (ULONG)info.rcClient.top &&
            c_sas_dw_at(backing + 64u) == (ULONG)info.rcClient.right &&
            c_sas_dw_at(backing + 68u) == (ULONG)info.rcClient.bottom);
        cls = c_sas_dw_at(backing + 76u);
        BORROW_CHECK(cls > delta);
        if (cls > delta) {
            BORROW_CHECK(c_sas_dw_at(cls - delta + 64u) == GetClassLongA(window, GCL_STYLE));
            BORROW_CHECK(c_sas_dw_at(cls - delta + 76u) == GetClassLongA(window, GCL_CBWNDEXTRA));
        }
        BORROW_CHECK(mvdm_softpc_wow_page_domain_release_window(backing));
    }
    return cls;
}

static void verify_native_mutation_group(ULONG delta)
{
    WNDCLASSA wc = {0};
    HWND parent = NULL, child = NULL, peer = NULL, scrollbar = NULL, popup = NULL;
    ULONG child_class, peer_class;
    DWORD previous_style;
    RECT before, after;
    unsigned errors_before = borrow_errors;
    wc.lpfnWndProc = DefWindowProcA;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "T447NumericClassMutation";
    wc.cbWndExtra = 16;
    BORROW_CHECK(RegisterClassA(&wc));
    parent = CreateWindowA(wc.lpszClassName, "Mutation frame", WS_OVERLAPPEDWINDOW,
        0, 0, 240, 180, NULL, NULL, wc.hInstance, NULL);
    if (parent) {
        child = CreateWindowA(wc.lpszClassName, "Child", WS_CHILD | WS_VISIBLE,
            10, 12, 24, 26, parent, (HMENU)51, wc.hInstance, NULL);
        peer = CreateWindowA(wc.lpszClassName, "Peer", WS_CHILD | WS_VISIBLE,
            50, 12, 24, 26, parent, (HMENU)52, wc.hInstance, NULL);
        scrollbar = CreateWindowA("SCROLLBAR", "", WS_CHILD | WS_VISIBLE,
            10, 90, 120, 18, parent, (HMENU)53, wc.hInstance, NULL);
        popup = CreateWindowA(wc.lpszClassName, "Owned popup", WS_POPUP | WS_VISIBLE,
            260, 0, 80, 80, parent, NULL, wc.hInstance, NULL);
    }
    BORROW_CHECK(parent && child && peer && scrollbar && popup);
    if (parent && child && peer && scrollbar && popup) {
        (void)wow_native_ShowWindow(parent, SW_SHOWNOACTIVATE);
        child_class = verify_mutated_window(child, delta);
        peer_class = verify_mutated_window(peer, delta);
        previous_style = GetClassLongA(child, GCL_STYLE);
        BORROW_CHECK(wow_user_set_class_longA(child, GCL_STYLE,
            previous_style ^ CS_DBLCLKS) == previous_style);
        BORROW_CHECK(GetClassLongA(child, GCL_STYLE) == (previous_style ^ CS_DBLCLKS));
        BORROW_CHECK(verify_mutated_window(child, delta) == child_class);
        BORROW_CHECK(verify_mutated_window(peer, delta) == peer_class);

        BORROW_CHECK(wow_native_ShowOwnedPopups(parent, FALSE));
        BORROW_CHECK(!IsWindowVisible(popup));
        (void)verify_mutated_window(popup, delta);
        BORROW_CHECK(wow_native_ShowOwnedPopups(parent, TRUE));
        BORROW_CHECK(IsWindowVisible(popup));
        (void)verify_mutated_window(popup, delta);
        BORROW_CHECK(wow_user_set_class_longA(child, GCL_STYLE, previous_style) ==
            (previous_style ^ CS_DBLCLKS));
        BORROW_CHECK(verify_mutated_window(peer, delta) == peer_class);

        BORROW_CHECK(GetWindowRect(child, &before));
        BORROW_CHECK(wow_native_ScrollWindow(parent, 3, 4, NULL, NULL));
        BORROW_CHECK(GetWindowRect(child, &after));
        BORROW_CHECK(after.left == before.left + 3 && after.top == before.top + 4);
        (void)verify_mutated_window(child, delta);
        before = after;
        BORROW_CHECK(wow_native_ScrollWindowEx(parent, -2, 5, NULL, NULL, NULL,
            NULL, SW_SCROLLCHILDREN | SW_INVALIDATE) != ERROR);
        BORROW_CHECK(GetWindowRect(child, &after));
        BORROW_CHECK(after.left == before.left - 2 && after.top == before.top + 5);
        (void)verify_mutated_window(child, delta);

        BORROW_CHECK(wow_native_ShowScrollBar(parent, SB_BOTH, TRUE));
        BORROW_CHECK((GetWindowLongA(parent, GWL_STYLE) & (WS_HSCROLL | WS_VSCROLL)) ==
            (WS_HSCROLL | WS_VSCROLL));
        (void)verify_mutated_window(parent, delta);
        BORROW_CHECK(wow_native_SetScrollRange(parent, SB_HORZ, 0, 100, FALSE));
        (void)wow_native_SetScrollPos(parent, SB_HORZ, 7, FALSE);
        BORROW_CHECK(GetScrollPos(parent, SB_HORZ) == 7);
        BORROW_CHECK(wow_native_SetScrollRange(parent, SB_HORZ, 0, 0, FALSE));
        BORROW_CHECK(!(GetWindowLongA(parent, GWL_STYLE) & WS_HSCROLL));
        (void)verify_mutated_window(parent, delta);
        BORROW_CHECK(wow_native_ShowScrollBar(parent, SB_BOTH, FALSE));
        (void)verify_mutated_window(parent, delta);
        BORROW_CHECK(wow_native_SetScrollRange(scrollbar, SB_CTL, 0, 50, FALSE));
        (void)wow_native_SetScrollPos(scrollbar, SB_CTL, 8, FALSE);
        BORROW_CHECK(GetScrollPos(scrollbar, SB_CTL) == 8);
        BORROW_CHECK(wow_native_EnableScrollBar(scrollbar, SB_CTL, ESB_DISABLE_BOTH));
        BORROW_CHECK(!IsWindowEnabled(scrollbar));
        (void)verify_mutated_window(scrollbar, delta);
        BORROW_CHECK(wow_native_EnableScrollBar(scrollbar, SB_CTL, ESB_ENABLE_BOTH));
        BORROW_CHECK(IsWindowEnabled(scrollbar));
        (void)verify_mutated_window(scrollbar, delta);
        BORROW_CHECK(wow_native_CloseWindow(parent));
        BORROW_CHECK(IsIconic(parent));
        (void)verify_mutated_window(parent, delta);
        (void)wow_native_ArrangeIconicWindows(GetDesktopWindow());
        (void)verify_mutated_window(parent, delta);
        BORROW_CHECK(wow_native_OpenIcon(parent));
        BORROW_CHECK(!IsIconic(parent));
        (void)verify_mutated_window(parent, delta);
        BORROW_CHECK(wow_native_OpenIcon(parent)); /* Already restored. */
        BORROW_CHECK(!wow_native_SetScrollRange(NULL, SB_HORZ, 0, 10, FALSE));
        BORROW_CHECK(wow_native_ScrollWindowEx(NULL, 1, 1, NULL, NULL, NULL, NULL, 0) == ERROR);
        BORROW_CHECK(verify_mutated_window(child, delta) == child_class);
    }
    if (popup) BORROW_CHECK(wow_native_DestroyWindow(popup));
    if (scrollbar) BORROW_CHECK(wow_native_DestroyWindow(scrollbar));
    if (peer) BORROW_CHECK(wow_native_DestroyWindow(peer));
    if (child) BORROW_CHECK(wow_native_DestroyWindow(child));
    if (parent) BORROW_CHECK(wow_native_DestroyWindow(parent));
    BORROW_CHECK(UnregisterClassA(wc.lpszClassName, wc.hInstance));
    fprintf(stderr, "WOW_GRAPH_MUTATION_GROUP_CCPU errors=%u class=1 scroll=1 bars=1 iconic=1 popups=1 invalid=1\n",
        borrow_errors - errors_before);
}

static int verify_callback_borrow(ULONG teb)
{
    static wow_user_object_table objects;
    wow_task_order_process process = {0};
    wow_task_order_thread thread = {0};
    wow_task_order_entry task = {0};
    wow_task_order_state task_state = {0};
    wow_user_runtime runtime = WOW_USER_RUNTIME_INITIALIZER;
    wow_user_task_lifecycle native_owner = {0};
    wow_user_runtime_thread runtime_binding = {0};
    wow_window_dispatch_target target = { 0x81234567u, destroy_in_callback };
    unsigned mode;
    process.objects = &objects;
    thread.ppi = &process;
    process.pwpi = &task_state;
    task_state.CSOwningThread = &thread;
    thread.ptdb = &task;
    task.hTaskWow = 0x4321u;
    runtime.lifecycle = &native_owner;
    if (!wow_user_runtime_bind(&runtime_binding, &runtime, &thread, NULL)) return 0;
    BORROW_CHECK(wow_user_window_refresh_graph(NULL));
    callback_teb = teb;
    verify_native_mutation_group(c_sas_dw_at(teb + 0x60u));
    {
        HWND first = CreateWindowA("STATIC", "Mutation parent", WS_POPUP,
            10, 20, 80, 80, NULL, NULL, GetModuleHandle(NULL), NULL);
        HWND second = CreateWindowA("STATIC", "New parent", WS_POPUP,
            100, 120, 80, 80, NULL, NULL, GetModuleHandle(NULL), NULL);
        HWND child = CreateWindowA("STATIC", "Child", WS_CHILD,
            1, 2, 16, 16, first, (HMENU)71, GetModuleHandle(NULL), NULL);
        ULONG held = 0, parent = 0, delta = c_sas_dw_at(teb + 0x60u);
        RECT rect;
        HDWP positions;
        unsigned before = borrow_errors;
        BORROW_CHECK(first && second && child);
        if (first && second && child) {
            BORROW_CHECK(wow_user_set_window_longA(child, GWL_ID, 91) == 71);
            held = mvdm_softpc_wow_page_domain_borrow_window((ULONG)(ULONG_PTR)child);
            BORROW_CHECK(held && c_sas_dw_at(held + 104u) == 91u);
            BORROW_CHECK(wow_native_SetParent(child, second) == first);
            parent = mvdm_softpc_wow_page_domain_borrow_window((ULONG)(ULONG_PTR)second);
            BORROW_CHECK(parent && held && c_sas_dw_at(held + 24u) == parent + delta);
            positions = BeginDeferWindowPos(1);
            if (positions) positions = DeferWindowPos(positions, child, NULL,
                7, 9, 28, 30, SWP_NOZORDER | SWP_NOACTIVATE);
            BORROW_CHECK(positions && wow_native_EndDeferWindowPos(positions));
            BORROW_CHECK(GetWindowRect(child, &rect));
            BORROW_CHECK(held && c_sas_dw_at(held + 40u) == (ULONG)rect.left &&
                c_sas_dw_at(held + 44u) == (ULONG)rect.top &&
                c_sas_dw_at(held + 48u) == (ULONG)rect.right &&
                c_sas_dw_at(held + 52u) == (ULONG)rect.bottom);
        }
        if (held) BORROW_CHECK(mvdm_softpc_wow_page_domain_release_window(held));
        if (parent) BORROW_CHECK(mvdm_softpc_wow_page_domain_release_window(parent));
        if (child) BORROW_CHECK(wow_native_DestroyWindow(child));
        if (first) BORROW_CHECK(wow_native_DestroyWindow(first));
        if (second) BORROW_CHECK(wow_native_DestroyWindow(second));
        fprintf(stderr, "WOW_GRAPH_NATIVE_MUTATION_CCPU errors=%u child_id=1 reparent=1 deferred_geometry=1\n",
            borrow_errors - before);
    }
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
    {
        DWORD template_words[16] = {0};
        DLGTEMPLATE *dialog = (DLGTEMPLATE *)template_words;
        HWND window;
        ULONG before, after, largest;
        dialog->style = WS_POPUP;
        dialog->cx = 100; dialog->cy = 40;
        /* Empty native template: menu, class and title are zero WORDs.
         * Production dialog provider, real page domain, explicit native
         * callback witness; this is not a substituted Win16 program. */
        BORROW_CHECK(VdmQueryFreeVirtualMemory(&before, &largest) >= 0);
        window = wow_user_server_create_dialog(GetModuleHandle(NULL), dialog,
            sizeof(template_words), NULL, dialog_cache_callback, 0,
            SCDLG_CLIENT | SCDLG_ANSI | SCDLG_NOREVALIDATE);
        BORROW_CHECK(window != NULL);
        if (window) {
            SendMessageA(window, WM_APP + 99, 0, 0);
            BORROW_CHECK(!IsWindow(window));
            BORROW_CHECK(objects.entries[LOWORD(window)].bType == TYPE_FREE);
            if (IsWindow(window)) DestroyWindow(window);
        }
        BORROW_CHECK(c_sas_dw_at(teb + 0x6cu) == 0u &&
            c_sas_dw_at(teb + 0x70u) == 0u);
        BORROW_CHECK(VdmQueryFreeVirtualMemory(&after, &largest) >= 0 && after == before);
        fprintf(stderr, "WOW_DIALOG_CACHE_CCPU errors=%u nested_destroy=1 released=1\n", borrow_errors);
    }
    {
        HWND window = CreateWindowExA(0, "STATIC", "Deferred terminal owner",
            WS_POPUP, 0, 0, 24, 24, NULL, NULL, GetModuleHandle(NULL), NULL);
        wow_window_words_binding *borrow;
        WW words = {0};
        ULONG before = 0, after = 0, largest;
        unsigned errors_before = borrow_errors;
        words.adwWOW[1] = 0x73abcdefu;
        BORROW_CHECK(window != NULL);
        BORROW_CHECK(VdmQueryFreeVirtualMemory(&before, &largest) >= 0);
        BORROW_CHECK(wow_window_words_attach(window, &words));
        borrow = wow_window_words_acquire(window);
        BORROW_CHECK(borrow != NULL);
        BORROW_CHECK(wow_window_words_publish_owner(window, borrow, &thread));
        mvdm_softpc_wow_page_domain_leave_protected();
        BORROW_CHECK(!wow_window_words_terminal(window, borrow));
        BORROW_CHECK(GetPropW(window, L"NTVDM.WOW.WindowWords") == NULL);
        BORROW_CHECK(objects.entries[LOWORD(window)].bType == TYPE_WINDOW);
        BORROW_CHECK(!wow_window_words_collect_retired(&objects));
        /* Drop the caller too: only the existing table now owns WW storage. */
        wow_window_words_release(borrow);
        BORROW_CHECK(DestroyWindow(window));
        BORROW_CHECK(mvdm_softpc_wow_page_domain_reenter_protected());
        BORROW_CHECK(wow_window_words_collect_retired(&objects));
        BORROW_CHECK(objects.entries[LOWORD(window)].bType == TYPE_FREE);
        BORROW_CHECK(wow_window_words_collect_retired(&objects));
        BORROW_CHECK(mvdm_softpc_wow_page_domain_collect_retired_windows());
        BORROW_CHECK(VdmQueryFreeVirtualMemory(&after, &largest) >= 0 && after == before);
        fprintf(stderr, "WOW_TERMINAL_RETRY_CCPU errors=%u property_removed=1 table_retained=1 native_gone=1 released=1\n",
            borrow_errors - errors_before);
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

static unsigned native_chain_calls;
static HWND vetoed_native;
static ATOM veto_native_atom;
static LRESULT CALLBACK native_chain(HWND w, UINT m, WPARAM p, LPARAM l,
    UINT_PTR id, DWORD_PTR data)
{
    (void)data;
    if (m == WM_APP + 65) ++native_chain_calls;
    if (m == WM_NCDESTROY) RemoveWindowSubclass(w, native_chain, id);
    return DefSubclassProc(w, m, p, l);
}
static LRESULT CALLBACK veto_native_creation(int code, WPARAM wp, LPARAM lp)
{
    if (code == HCBT_CREATEWND && (!veto_native_atom ||
            (ATOM)GetClassLongA((HWND)wp, GCW_ATOM) == veto_native_atom)) {
        vetoed_native = (HWND)wp;
        BORROW_CHECK(GetWindowLongA((HWND)wp, GWL_WNDPROC) == (LONG)wow_window_native_proc);
        return 1;
    }
    return CallNextHookEx(NULL, code, wp, lp);
}
static void verify_managed_slot_reuse(wow_class_lookup_context *classes)
{
    static wow_user_object_table objects;
    wow_task_order_process process = {0};
    wow_task_order_thread thread = {0};
    wow_standard_class_view descriptor;
    CREATESTRUCTA request = {0};
    WW words = {0};
    unsigned mode, before = borrow_errors;
    process.classes = classes; process.objects = &objects; thread.ppi = &process;
    BORROW_CHECK(wow_standard_class_describe("BUTTON", &descriptor));
    request.lpszClass = "BUTTON"; request.lpszName = "Managed reuse";
    request.hInstance = GetModuleHandle(NULL); request.style = WS_POPUP;
    request.cx = 24; request.cy = 24;
    words.adwWOW[0] = 0x13572468u;
    for (mode = 0; mode < 2; ++mode) {
        HWND old = NULL, window = NULL, desktop = GetDesktopWindow();
        ULONG old_full = 0, full = 0, held = 0, fresh = 0, previous, actual[3];
        ULONG retired = 0;
        unsigned attempts = 0;
        ULONGLONG started;
        old = CreateWindowExA(0, "STATIC", "Observed before creation", WS_POPUP,
            0, 0, 24, 24, NULL, NULL, GetModuleHandle(NULL), NULL);
        BORROW_CHECK(old != NULL);
        if (!old) continue;
        old_full = (ULONG)(ULONG_PTR)old;
        BORROW_CHECK(wow_user_window_project_graph(&old, 1));
        held = mvdm_softpc_wow_page_domain_borrow_window(old_full);
        BORROW_CHECK(held != 0);
        if (!held) { DestroyWindow(old); continue; }
        BORROW_CHECK(mvdm_softpc_wow_page_domain_exchange_window_word(
            held, 0, 0xabcdef12u, &previous));
        BORROW_CHECK(DestroyWindow(old));
        started = GetTickCount64();
        while (attempts < 131072u && GetTickCount64() - started < 15000u) {
            ++attempts;
            window = mode ? CreateWindowExA(0, "STATIC", "Null class carrier", WS_POPUP,
                0, 0, 24, 24, NULL, NULL, GetModuleHandle(NULL), NULL) :
                wow_window_create_boundA(&thread, descriptor.atom, &request, &words, NULL);
            if (!window) break;
            full = (ULONG)(ULONG_PTR)window;
            if (LOWORD(full) == LOWORD(old_full) && full != old_full) break;
            BORROW_CHECK(DestroyWindow(window)); window = NULL;
        }
        BORROW_CHECK(window && LOWORD(full) == LOWORD(old_full) && full != old_full);
        if (window && LOWORD(full) == LOWORD(old_full) && full != old_full) {
            /* Dialog roots reach this same producer with no CLS. This mode
             * tests that storage contract, not a guest dialog implementation. */
            if (mode) BORROW_CHECK(mvdm_softpc_wow_page_domain_publish_handle(
                LOWORD(full), HIWORD(full), 1, 0, full, 0, words.adwWOW));
            else BORROW_CHECK(objects.windows[LOWORD(full)] == window &&
                objects.entries[LOWORD(full)].bType == TYPE_WINDOW);
            fresh = mvdm_softpc_wow_page_domain_borrow_window(full);
            BORROW_CHECK(fresh && fresh != held);
            BORROW_CHECK(wow_user_full_handle(&objects, LOWORD(full)) == full);
            BORROW_CHECK(!mvdm_softpc_wow_page_domain_borrow_window(old_full));
            BORROW_CHECK(mvdm_softpc_wow_page_domain_read_window_words(held, actual) &&
                actual[0] == 0xabcdef12u && c_sas_dw_at(held) == old_full);
            BORROW_CHECK(fresh && mvdm_softpc_wow_page_domain_read_window_words(fresh, actual) &&
                actual[0] == words.adwWOW[0] && c_sas_dw_at(fresh) == full);
            if (mode && fresh) BORROW_CHECK(c_sas_dw_at(fresh + 76u) == 0);
            /* An existing managed/null-class publication is not replaceable
             * merely because the caller presents a different generation. */
            BORROW_CHECK(!mvdm_softpc_wow_page_domain_publish_handle(LOWORD(full),
                HIWORD(full) ^ 1u, 1, 0, full, 0, NULL));
            if (mode) {
                BORROW_CHECK(mvdm_softpc_wow_page_domain_retire_handle(LOWORD(full), &retired));
                if (retired) BORROW_CHECK(mvdm_softpc_wow_page_domain_release_retained_window(retired));
            }
        }
        if (window) BORROW_CHECK(DestroyWindow(window));
        BORROW_CHECK(wow_user_window_project_graph(&desktop, 1));
        if (fresh) BORROW_CHECK(mvdm_softpc_wow_page_domain_release_window(fresh));
        BORROW_CHECK(mvdm_softpc_wow_page_domain_release_window(held));
        BORROW_CHECK(mvdm_softpc_wow_page_domain_collect_observed_classes());
        BORROW_CHECK(!mvdm_softpc_wow_page_domain_read_window_words(held, actual));
        if (fresh) BORROW_CHECK(!mvdm_softpc_wow_page_domain_read_window_words(fresh, actual));
        fprintf(stderr, "WOW_MANAGED_REUSE_CASE mode=%u attempts=%u old=%08lx new=%08lx\n",
            mode, attempts, old_full, full);
    }
    fprintf(stderr, "WOW_MANAGED_SLOT_REUSE_CCPU errors=%u creation=1 null_class=1 retained=1 released=1\n",
        borrow_errors - before);
}

static void verify_native_class_creation(wow_class_lookup_context *classes)
{
    static wow_user_object_table objects;
    wow_task_order_process process = {0};
    wow_task_order_thread thread = {0};
    wow_standard_class_view native_class;
    CREATESTRUCTA request = {0};
    WW words = {0};
    HWND window;
    HHOOK veto;
    char text[32] = {0};
    unsigned before = borrow_errors;
    process.classes = classes; process.objects = &objects; thread.ppi = &process;
    verify_managed_slot_reuse(classes);
    BORROW_CHECK(wow_standard_class_describe("BUTTON", &native_class));
    request.lpszClass = "BUTTON"; request.lpszName = "Native original";
    request.hInstance = GetModuleHandle(NULL); request.style = WS_POPUP;
    request.cx = 100; request.cy = 30;
    window = wow_window_create_boundA(&thread, native_class.atom, &request, &words, NULL);
    BORROW_CHECK(window != NULL);
    if (window) {
        BORROW_CHECK(GetWindowLongA(window, GWL_WNDPROC) == (LONG)wow_window_native_proc);
        {
            HWND peer = wow_window_create_boundA(&thread, native_class.atom,
                &request, &words, NULL);
            wow_class_words_binding *owner = wow_class_words_acquire(classes,
                native_class.atom, request.hInstance);
            ULONG first = mvdm_softpc_wow_page_domain_borrow_window((ULONG)window);
            ULONG second = peer ? mvdm_softpc_wow_page_domain_borrow_window((ULONG)peer) : 0;
            BORROW_CHECK(peer && owner && first && second);
            if (owner && first && second) {
                ULONG shared = wow_class_words_entry(owner)->guest_server;
                BORROW_CHECK(shared && c_sas_dw_at(first + 76u) == shared &&
                    c_sas_dw_at(second + 76u) == shared);
            }
            if (second) BORROW_CHECK(mvdm_softpc_wow_page_domain_release_window(second));
            if (first) BORROW_CHECK(mvdm_softpc_wow_page_domain_release_window(first));
            wow_class_words_release(owner);
            if (peer) BORROW_CHECK(DestroyWindow(peer));
        }
        BORROW_CHECK(SetWindowSubclass(window, native_chain, 1, 0));
        SendMessageA(window, WM_APP + 65, 0, 0);
        BORROW_CHECK(native_chain_calls == 1);
        BORROW_CHECK(GetWindowTextA(window, text, sizeof(text)) > 0 &&
            !strcmp(text, "Native original"));
        {
            wow_standard_class_view combo_class;
            CREATESTRUCTA combo_request = {0};
            COMBOBOXINFO info = {sizeof(info)};
            HWND combo;
            wow_window_words_binding *child;
            unsigned i;
            HWND children[2] = {0};
            BORROW_CHECK(wow_standard_class_describe("COMBOBOX", &combo_class));
            combo_request.lpszClass = "COMBOBOX";
            combo_request.hInstance = request.hInstance;
            combo_request.style = WS_CHILD | CBS_DROPDOWN;
            combo_request.hwndParent = window;
            combo_request.cx = 100; combo_request.cy = 90;
            combo = wow_window_create_boundA(&thread, combo_class.atom,
                &combo_request, &words, NULL);
            BORROW_CHECK(combo != NULL);
            if (combo) {
                BORROW_CHECK(GetComboBoxInfo(combo, &info));
                children[0] = info.hwndItem; children[1] = info.hwndList;
                for (i = 0; i < ARRAYSIZE(children); ++i) {
                    child = wow_window_words_acquire(children[i]);
                    BORROW_CHECK(children[i] != NULL && child != NULL);
                    if (child) {
                        WW *view = wow_window_words_value(child);
                        BORROW_CHECK(view->adwWOW[0] == 0 &&
                            view->adwWOW[1] == 0 && view->adwWOW[2] == 0);
                        BORROW_CHECK(wow_window_words_cleanup_value(child)->thread == &thread);
                        BORROW_CHECK(wow_window_class_words_value(child) != NULL);
                        BORROW_CHECK(GetWindowLongA(children[i], GWL_WNDPROC) ==
                            (LONG)wow_window_native_proc);
                    }
                    wow_window_words_release(child);
                }
                /* A list may have been reparented by native combo creation.
                 * Its metadata is retained independently of parent membership. */
                BORROW_CHECK(SendMessageA(combo, CB_ADDSTRING, 0, (LPARAM)"item") == 0);
                BORROW_CHECK(SendMessageA(combo, CB_GETCOUNT, 0, 0) == 1);
                veto_native_atom = (ATOM)GetClassLongA(info.hwndList, GCW_ATOM);
                BORROW_CHECK(DestroyWindow(combo));
                BORROW_CHECK(objects.entries[LOWORD(combo)].bType == TYPE_FREE);
                for (i = 0; i < ARRAYSIZE(children); ++i)
                    BORROW_CHECK(children[i] && !IsWindow(children[i]) &&
                        objects.entries[LOWORD(children[i])].bType == TYPE_FREE);
                vetoed_native = NULL;
                veto = SetWindowsHookExA(WH_CBT, veto_native_creation, NULL,
                    GetCurrentThreadId());
                BORROW_CHECK(veto != NULL && veto_native_atom != 0);
                if (veto) {
                    combo = wow_window_create_boundA(&thread, combo_class.atom,
                        &combo_request, &words, NULL);
                    BORROW_CHECK(!combo && vetoed_native != NULL);
                    BORROW_CHECK(objects.entries[LOWORD(vetoed_native)].bType == TYPE_FREE);
                    BORROW_CHECK(UnhookWindowsHookEx(veto));
                    if (combo) DestroyWindow(combo);
                }
                veto_native_atom = 0;
            }
        }
        BORROW_CHECK(DestroyWindow(window));
        BORROW_CHECK(objects.entries[LOWORD(window)].bType == TYPE_FREE);
    }
    veto = SetWindowsHookExA(WH_CBT, veto_native_creation, NULL, GetCurrentThreadId());
    BORROW_CHECK(veto != NULL);
    if (veto) {
        window = wow_window_create_boundA(&thread, native_class.atom, &request, &words, NULL);
        BORROW_CHECK(!window && vetoed_native != NULL);
        BORROW_CHECK(objects.entries[LOWORD(vetoed_native)].bType == TYPE_FREE);
        BORROW_CHECK(UnhookWindowsHookEx(veto));
        if (window) DestroyWindow(window);
    }
    {
        DWORD template_words[32] = {0};
        DLGTEMPLATE *dialog = (DLGTEMPLATE *)template_words;
        DLGITEMTEMPLATE *item = (DLGITEMTEMPLATE *)((BYTE *)dialog +
            ((sizeof(*dialog) + 3 * sizeof(WORD) + 3) & ~3u));
        WORD *tail = (WORD *)(item + 1);
        wow_task_order_entry task = {0};
        wow_task_order_state task_state = {0};
        wow_user_runtime runtime = WOW_USER_RUNTIME_INITIALIZER;
        wow_user_task_lifecycle native_owner = {0};
        wow_user_runtime_thread binding = {0};
        HWND button = NULL;
        wow_window_words_binding *child;
        task.hTaskWow = 0x4321;
        runtime.lifecycle = &native_owner; /* Single-task native callback fixture. */
        thread.ptdb = &task; process.pwpi = &task_state;
        task_state.CSOwningThread = &thread;
        dialog->style = WS_POPUP; dialog->cdit = 1;
        dialog->cx = 100; dialog->cy = 40;
        item->style = WS_CHILD | BS_PUSHBUTTON;
        item->x = 5; item->y = 5; item->cx = 30; item->cy = 12; item->id = 101;
        tail[0] = 0xffff; tail[1] = 0x0080; /* Standard BUTTON ordinal. */
        tail[2] = 'O'; tail[3] = 'K'; /* NUL title and zero extra bytes follow. */
        BORROW_CHECK(wow_user_runtime_bind(&binding, &runtime, &thread, NULL));
        window = wow_user_server_create_dialog(GetModuleHandle(NULL), dialog,
            sizeof(template_words), NULL, NULL, 0,
            SCDLG_CLIENT | SCDLG_ANSI | SCDLG_NOREVALIDATE);
        BORROW_CHECK(window != NULL);
        if (window) {
            button = GetDlgItem(window, 101);
            child = wow_window_words_acquire(button);
            BORROW_CHECK(button != NULL && child != NULL);
            if (child) {
                BORROW_CHECK(wow_window_words_cleanup_value(child)->thread == &thread);
                BORROW_CHECK(wow_window_class_words_value(child) != NULL);
            }
            wow_window_words_release(child);
            BORROW_CHECK(wow_native_DestroyWindow(window));
            BORROW_CHECK(objects.entries[LOWORD(window)].bType == TYPE_FREE);
            BORROW_CHECK(button && !IsWindow(button) &&
                objects.entries[LOWORD(button)].bType == TYPE_FREE);
        }
        BORROW_CHECK(wow_user_runtime_unbind(&binding));
        thread.ptdb = NULL; process.pwpi = NULL;
    }
    fprintf(stderr, "WOW_NATIVE_CREATE_CCPU errors=%u forward=1 subclass=1 veto=1 retired=1 internal_combo=1 internal_dialog=1\n",
        borrow_errors - before);
}

static void verify_unowned_window_projection(ULONG teb)
{
    /* The foreign desktop already has the one real shared root. Never
     * publish a second ordinary WND for it. Exercise an unowned native
     * window separately; neither observation installs a WW property. */
    HWND desktop = GetDesktopWindow();
    HWND window = CreateWindowExA(0, "STATIC", "Unowned view", WS_POPUP,
        0, 0, 24, 24, NULL, NULL, GetModuleHandleA(NULL), NULL);
    WINDOWINFO info = {sizeof(info)};
    DWORD process_id = 0;
    ULONG server = 0, handle_table, client, cls, name;
    ULONG free_before = 0, free_after = 0, largest;
    ULONG delta = c_sas_dw_at(teb + 0x60u);
    char class_name[256] = {0}, copied[256] = {0};
    int count;
    BOOL published = FALSE;
    unsigned before = borrow_errors;
    BORROW_CHECK(GetWindowThreadProcessId(desktop, &process_id) != 0 &&
        process_id != GetCurrentProcessId());
    BORROW_CHECK(mvdm_softpc_wow_page_domain_window_handle(LOWORD(desktop)) ==
        (ULONG)(ULONG_PTR)desktop);
    BORROW_CHECK(!mvdm_softpc_wow_page_domain_publish_handle(LOWORD(desktop),
        HIWORD(desktop), 1u, 0u, (ULONG)(ULONG_PTR)desktop, 0u, NULL));
    BORROW_CHECK(window != NULL);
    if (!window) return;
    BORROW_CHECK(GetWindowInfo(window, &info));
    count = GetClassNameA(window, class_name, sizeof(class_name));
    BORROW_CHECK(count > 0 && count < sizeof(class_name) - 1);
    BORROW_CHECK(GetPropW(window, L"NTVDM.WOW.WindowWords") == NULL);
    BORROW_CHECK(VdmQueryFreeVirtualMemory(&free_before, &largest) >= 0);
    if (count > 0 && info.atomWindowType) {
        server = mvdm_softpc_wow_page_domain_publish_named_class(
            info.atomWindowType, 0, 0, 0, 0, 0,
            GetClassLongA(window, GCL_STYLE), 0,
            GetClassLongA(window, GCL_CBWNDEXTRA), 0,
            class_name, (ULONG)count + 1u);
    }
    BORROW_CHECK(server != 0);
    if (server) {
        published = mvdm_softpc_wow_page_domain_publish_handle(LOWORD(window),
            HIWORD(window), 1u, 0u, (ULONG)(ULONG_PTR)window, server, NULL);
        BORROW_CHECK(published);
        if (published) {
            handle_table = c_sas_dw_at(mvdm_softpc_wow_page_domain_guest_shared_info() + 4u);
            client = c_sas_dw_at(handle_table + LOWORD(window) * 16u) - delta;
            cls = c_sas_dw_at(client + 76u) - c_sas_dw_at(client + 12u) + client;
            name = c_sas_dw_at(cls + 100u) - c_sas_dw_at(client + 12u) + client;
            c_sas_loads(name, (uint8_t *)copied, (ULONG)count + 1u);
            BORROW_CHECK(!strcmp(copied, class_name));
            BORROW_CHECK(c_sas_dw_at(cls + 44u) == 0 &&
                c_sas_dw_at(cls + 68u) == 0 && c_sas_dw_at(cls + 80u) == 0);
            BORROW_CHECK(!mvdm_softpc_wow_page_domain_retire_class(server));
            BORROW_CHECK(c_sas_dw_at(client + 76u) == server);
            {
                wow_user_borrow_scope outer = {0}, inner = {0};
                PWW words;
                BOOL entered = wow_user_borrow_enter(&outer);
                BORROW_CHECK(entered);
                if (entered) {
                    words = FindPWW(window, 0); /* Retained original classifier. */
                    BORROW_CHECK(words && words->iClass == GetStdClassNumber(class_name));
                    BORROW_CHECK(words && words->dwStyle == info.dwStyle &&
                        words->dwExStyle == info.dwExStyle);
                    BORROW_CHECK(words && c_sas_dw_at(client + 144u) == words->adwWOW[0]);
                    entered = wow_user_borrow_enter(&inner);
                    BORROW_CHECK(entered);
                    if (entered) {
                        BORROW_CHECK(wow_user_borrow_window(window) == words);
                        BORROW_CHECK(wow_user_borrow_leave(&inner));
                    }
                    BORROW_CHECK(mvdm_softpc_wow_page_domain_retire_handle(LOWORD(window), NULL));
                    BORROW_CHECK(!mvdm_softpc_wow_page_domain_retire_class(server));
                    BORROW_CHECK(words && words->iClass == GetStdClassNumber(class_name));
                    BORROW_CHECK(wow_user_borrow_leave(&outer));
                }
            }
        }
        BORROW_CHECK(mvdm_softpc_wow_page_domain_retire_class(server));
    }
    BORROW_CHECK(IsWindow(window) && GetDesktopWindow() == desktop);
    BORROW_CHECK(GetPropW(window, L"NTVDM.WOW.WindowWords") == NULL);
    BORROW_CHECK(VdmQueryFreeVirtualMemory(&free_after, &largest) >= 0 &&
        free_after == free_before);
    BORROW_CHECK(DestroyWindow(window));
    fprintf(stderr, "WOW_UNOWNED_VIEW_CCPU errors=%u foreign_root=1 local_unowned=1 class_guard=1 native_untouched=1 released=1\n",
        borrow_errors - before);
}

static void verify_observed_window_promotion(ULONG teb)
{
    HWND window = CreateWindowExA(0, "STATIC", "Observed promotion", WS_POPUP,
        0, 0, 24, 24, NULL, NULL, GetModuleHandleA(NULL), NULL);
    WINDOWINFO info = {sizeof(info)};
    char name[256];
    int length;
    ULONG full = (ULONG)(ULONG_PTR)window, before_free = 0, after_free = 0, largest;
    ULONG backing, observed_class, managed_class, previous, words[3];
    const ULONG creator_words[3] = {0x87654321u, 0x11112222u, 0x33334444u};
    ULONG delta = c_sas_dw_at(teb + 0x60u);
    unsigned before = borrow_errors;
    BORROW_CHECK(window != NULL);
    if (!window) return;
    BORROW_CHECK(GetWindowInfo(window, &info));
    length = GetClassNameA(window, name, sizeof(name));
    BORROW_CHECK(length > 0 && length < sizeof(name) - 1);
    if (length <= 0 || length >= sizeof(name) - 1) {
        DestroyWindow(window);
        return;
    }
    BORROW_CHECK(VdmQueryFreeVirtualMemory(&before_free, &largest) >= 0);
    BORROW_CHECK(mvdm_softpc_wow_page_domain_observe_window(full, info.atomWindowType,
        GetClassLongA(window, GCL_STYLE), GetClassLongA(window, GCL_CBWNDEXTRA),
        name, (ULONG)length + 1u));
    backing = mvdm_softpc_wow_page_domain_borrow_window(full);
    BORROW_CHECK(backing != 0);
    if (backing) {
        observed_class = c_sas_dw_at(backing + 76u);
        BORROW_CHECK(mvdm_softpc_wow_page_domain_exchange_window_word(
            backing, 0, 0x12345678u, &previous) && previous == 0);
        BORROW_CHECK(mvdm_softpc_wow_page_domain_observe_window(full, info.atomWindowType,
            GetClassLongA(window, GCL_STYLE), GetClassLongA(window, GCL_CBWNDEXTRA),
            name, (ULONG)length + 1u));
        BORROW_CHECK(c_sas_dw_at(backing + 76u) == observed_class);
        managed_class = mvdm_softpc_wow_page_domain_publish_named_class(
            info.atomWindowType, 0, 0, 0, 0, 0,
            GetClassLongA(window, GCL_STYLE), 0, 0, 0,
            name, (ULONG)length + 1u);
        BORROW_CHECK(managed_class != 0);
        BORROW_CHECK(!mvdm_softpc_wow_page_domain_publish_handle(LOWORD(full),
            (WORD)(HIWORD(full) ^ 1u), 1, 0, full, managed_class, NULL));
        BORROW_CHECK(!mvdm_softpc_wow_page_domain_publish_handle(LOWORD(full),
            HIWORD(full), 1, 0, full, observed_class, NULL));
        BORROW_CHECK(c_sas_dw_at(backing + 76u) == observed_class);
        BORROW_CHECK(mvdm_softpc_wow_page_domain_publish_handle(LOWORD(full),
            HIWORD(full), 1, 0, full, managed_class, creator_words));
        BORROW_CHECK(c_sas_dw_at(backing) == full &&
            c_sas_dw_at(backing + 76u) == managed_class &&
            c_sas_dw_at(backing + 12u) == backing + delta);
        BORROW_CHECK(!mvdm_softpc_wow_page_domain_forget_observed_window(full));
        BORROW_CHECK(!mvdm_softpc_wow_page_domain_publish_handle(LOWORD(full),
            HIWORD(full), 1, 0, full, managed_class, NULL));
        BORROW_CHECK(mvdm_softpc_wow_page_domain_read_window_words(backing, words) &&
            memcmp(words, creator_words, sizeof(words)) == 0);
        BORROW_CHECK(mvdm_softpc_wow_page_domain_collect_observed_classes());
        BORROW_CHECK(mvdm_softpc_wow_page_domain_retire_handle(LOWORD(full), NULL));
        BORROW_CHECK(!mvdm_softpc_wow_page_domain_retire_class(managed_class));
        BORROW_CHECK(mvdm_softpc_wow_page_domain_read_window_words(backing, words) &&
            memcmp(words, creator_words, sizeof(words)) == 0);
        BORROW_CHECK(mvdm_softpc_wow_page_domain_release_window(backing));
        BORROW_CHECK(mvdm_softpc_wow_page_domain_retire_class(managed_class));
    }
    BORROW_CHECK(GetPropW(window, L"NTVDM.WOW.WindowWords") == NULL && IsWindow(window));
    BORROW_CHECK(VdmQueryFreeVirtualMemory(&after_free, &largest) >= 0 &&
        before_free == after_free);
    BORROW_CHECK(DestroyWindow(window));
    fprintf(stderr, "WOW_VIEW_PROMOTION_CCPU errors=%u same_backing=1 words=1 stale_rejected=1 ownership=1 released=1\n",
        borrow_errors - before);
}

static void verify_observed_window_retirement(void)
{
    static wow_user_object_table no_cleanup_owner;
    HWND window = CreateWindowExA(0, "STATIC", "Observed retirement", WS_POPUP,
        0, 0, 24, 24, NULL, NULL, GetModuleHandleA(NULL), NULL);
    WINDOWINFO info = {sizeof(info)};
    ULONG full = (ULONG)(ULONG_PTR)window, first, second, cls, previous, words[3];
    ULONG before_free = 0, after_free = 0, largest;
    unsigned before = borrow_errors;
    BORROW_CHECK(window != NULL);
    if (!window) return;
    BORROW_CHECK(GetWindowInfo(window, &info));
    BORROW_CHECK(VdmQueryFreeVirtualMemory(&before_free, &largest) >= 0);
    BORROW_CHECK(mvdm_softpc_wow_page_domain_observe_window(full,
        info.atomWindowType, GetClassLongA(window, GCL_STYLE),
        GetClassLongA(window, GCL_CBWNDEXTRA), "Static", sizeof("Static")));
    BORROW_CHECK(wow_user_full_handle(&no_cleanup_owner, LOWORD(full)) == full);
    BORROW_CHECK(!wow_user_window_full_handle(&no_cleanup_owner, LOWORD(full)));
    BORROW_CHECK(no_cleanup_owner.entries[LOWORD(full)].bType == TYPE_FREE);
    BORROW_CHECK(wow_user_full_handle(&no_cleanup_owner, LOWORD(GetDesktopWindow())) ==
        (ULONG)(ULONG_PTR)GetDesktopWindow());
    first = mvdm_softpc_wow_page_domain_borrow_window(full);
    second = mvdm_softpc_wow_page_domain_borrow_window(full);
    BORROW_CHECK(first && first == second);
    if (first && second) {
        cls = c_sas_dw_at(first + 76u);
        BORROW_CHECK(mvdm_softpc_wow_page_domain_collect_observed_windows());
        BORROW_CHECK(mvdm_softpc_wow_page_domain_window_handle(LOWORD(full)) == full);
        BORROW_CHECK(mvdm_softpc_wow_page_domain_exchange_window_word(
            first, 0, 0xabcdef12u, &previous));
        BORROW_CHECK(DestroyWindow(window));
        window = NULL;
        BORROW_CHECK(mvdm_softpc_wow_page_domain_collect_observed_windows());
        BORROW_CHECK(!mvdm_softpc_wow_page_domain_window_handle(LOWORD(full)));
        BORROW_CHECK(wow_user_full_handle(&no_cleanup_owner, LOWORD(full)) == LOWORD(full));
        BORROW_CHECK(!mvdm_softpc_wow_page_domain_forget_observed_window(full));
        BORROW_CHECK(mvdm_softpc_wow_page_domain_collect_observed_classes());
        BORROW_CHECK(!mvdm_softpc_wow_page_domain_retire_class(cls));
        BORROW_CHECK(mvdm_softpc_wow_page_domain_release_window(first));
        BORROW_CHECK(mvdm_softpc_wow_page_domain_collect_observed_classes());
        BORROW_CHECK(!mvdm_softpc_wow_page_domain_retire_class(cls));
        BORROW_CHECK(mvdm_softpc_wow_page_domain_read_window_words(second, words) &&
            words[0] == 0xabcdef12u);
        BORROW_CHECK(mvdm_softpc_wow_page_domain_release_window(second));
        BORROW_CHECK(!mvdm_softpc_wow_page_domain_release_window(second));
        BORROW_CHECK(mvdm_softpc_wow_page_domain_collect_observed_windows());
        BORROW_CHECK(!mvdm_softpc_wow_page_domain_retire_class(cls));
        BORROW_CHECK(mvdm_softpc_wow_page_domain_collect_observed_classes());
    }
    BORROW_CHECK(VdmQueryFreeVirtualMemory(&after_free, &largest) >= 0 &&
        before_free == after_free);
    if (window) DestroyWindow(window);
    fprintf(stderr, "WOW_VIEW_RETIREMENT_CCPU errors=%u nested_borrow=1 class_guard=1 consumed_once=1 released=1\n",
        borrow_errors - before);
}

#ifdef WOW_RECLAIM_FAILURE_FIXTURE
extern void __cdecl wow_fixture_fail_window_free(ULONG);
extern unsigned __cdecl wow_fixture_window_free_failures(void);
static void verify_failed_window_reclamation_case(BOOL retained_owner)
{
    HWND window = CreateWindowExA(0, "STATIC", "Deferred reclaim", WS_POPUP,
        0, 0, 24, 24, NULL, NULL, GetModuleHandleA(NULL), NULL);
    WINDOWINFO info = {sizeof(info)};
    ULONG full = (ULONG)(ULONG_PTR)window, backing, cls, words[3], retained = 0;
    ULONG before_free = 0, after_free = 0, largest;
    BORROW_CHECK(window != NULL);
    if (!window) return;
    BORROW_CHECK(GetWindowInfo(window, &info));
    BORROW_CHECK(VdmQueryFreeVirtualMemory(&before_free, &largest) >= 0);
    BORROW_CHECK(mvdm_softpc_wow_page_domain_observe_window(full,
        info.atomWindowType, 0, 0, "Static", sizeof("Static")));
    backing = mvdm_softpc_wow_page_domain_borrow_window(full);
    BORROW_CHECK(backing != 0);
    if (backing) {
        cls = c_sas_dw_at(backing + 76u);
        if (retained_owner) {
            BORROW_CHECK(mvdm_softpc_wow_page_domain_retire_handle(LOWORD(full), &retained));
            BORROW_CHECK(retained == backing);
            BORROW_CHECK(mvdm_softpc_wow_page_domain_release_window(backing));
        } else {
            BORROW_CHECK(mvdm_softpc_wow_page_domain_forget_observed_window(full));
        }
        wow_fixture_fail_window_free(backing);
        BORROW_CHECK(retained_owner ?
            mvdm_softpc_wow_page_domain_release_retained_window(backing) :
            mvdm_softpc_wow_page_domain_release_window(backing));
        BORROW_CHECK(wow_fixture_window_free_failures() == 1);
        BORROW_CHECK(!mvdm_softpc_wow_page_domain_release_window(backing));
        BORROW_CHECK(!mvdm_softpc_wow_page_domain_release_retained_window(backing));
        BORROW_CHECK(!mvdm_softpc_wow_page_domain_read_window_words(backing, words));
        BORROW_CHECK(!mvdm_softpc_wow_page_domain_collect_retired_windows());
        BORROW_CHECK(!mvdm_softpc_wow_page_domain_retire_class(cls));
        BORROW_CHECK(c_sas_dw_at(backing) == full);
        wow_fixture_fail_window_free(0);
        BORROW_CHECK(mvdm_softpc_wow_page_domain_collect_retired_windows());
        BORROW_CHECK(mvdm_softpc_wow_page_domain_collect_retired_windows());
        BORROW_CHECK(mvdm_softpc_wow_page_domain_collect_observed_classes());
    }
    BORROW_CHECK(VdmQueryFreeVirtualMemory(&after_free, &largest) >= 0 &&
        before_free == after_free);
    BORROW_CHECK(DestroyWindow(window));
}

static void verify_failed_window_reclamation(void)
{
    unsigned before = borrow_errors;
    verify_failed_window_reclamation_case(FALSE);
    verify_failed_window_reclamation_case(TRUE);
    fprintf(stderr, "WOW_DEFERRED_RECLAIM_CCPU errors=%u consumed_once=1 class_guard=1 retry=1 retained_owner=1 released=1\n",
        borrow_errors - before);
}
#endif

static void verify_dead_graph_retirement(ULONG teb)
{
    HWND a = CreateWindowExA(0, "STATIC", "Dead graph A", WS_POPUP,
        0, 0, 24, 24, NULL, NULL, GetModuleHandleA(NULL), NULL);
    HWND b = CreateWindowExA(0, "STATIC", "Dead graph B", WS_POPUP,
        0, 0, 24, 24, NULL, NULL, GetModuleHandleA(NULL), NULL);
    ULONG dead[2] = {(ULONG)(ULONG_PTR)a, (ULONG)(ULONG_PTR)b};
    ULONG desktop = (ULONG)(ULONG_PTR)GetDesktopWindow();
    ULONG held = 0, free_before = 0, free_after = 0, largest;
    ULONG delta = c_sas_dw_at(teb + 0x60u), root, i;
    mvdm_softpc_wow_window_links links[3] = {0}, replacement = {0};
    unsigned before = borrow_errors;
    BORROW_CHECK(a && b);
    if (!a || !b) goto cleanup;
    BORROW_CHECK(VdmQueryFreeVirtualMemory(&free_before, &largest) >= 0);
    for (i = 0; i < 2; ++i) {
        WINDOWINFO info = {sizeof(info)};
        BORROW_CHECK(GetWindowInfo((HWND)(ULONG_PTR)dead[i], &info));
        BORROW_CHECK(mvdm_softpc_wow_page_domain_observe_window(dead[i],
            info.atomWindowType, 0, 0, "Static", sizeof("Static")));
    }
    links[0].window = desktop; links[0].child = dead[0];
    links[1].window = dead[0]; links[1].parent = desktop; links[1].next = dead[1];
    links[2].window = dead[1]; links[2].parent = desktop; links[2].owner = dead[0];
    BORROW_CHECK(mvdm_softpc_wow_page_domain_publish_window_links(3, links));
    held = mvdm_softpc_wow_page_domain_borrow_window(dead[0]);
    BORROW_CHECK(held != 0);
    root = mvdm_softpc_wow_page_domain_borrow_window(desktop);
    BORROW_CHECK(root != 0);
    replacement.window = desktop;
    /* Live native identities cannot be withdrawn even with a valid new tree. */
    BORROW_CHECK(!mvdm_softpc_wow_page_domain_replace_window_links(1, &replacement, 2, dead));
    BORROW_CHECK(root && c_sas_dw_at(root + 28u) == held + delta);
    BORROW_CHECK(DestroyWindow(a)); a = NULL;
    BORROW_CHECK(DestroyWindow(b)); b = NULL;
    {
        ULONG stale[2] = {dead[0], dead[1] ^ 0x10000u};
        mvdm_softpc_wow_window_links duplicate[2];
        duplicate[0] = duplicate[1] = replacement;
        /* Failure after an earlier valid retirement must discard only its
         * prepared retention node, not consume the actual publication. */
        BORROW_CHECK(!mvdm_softpc_wow_page_domain_replace_window_links(1, &replacement, 2, stale));
        BORROW_CHECK(!mvdm_softpc_wow_page_domain_replace_window_links(2, duplicate, 2, dead));
        BORROW_CHECK(mvdm_softpc_wow_page_domain_window_handle(LOWORD(dead[0])) == dead[0] &&
            mvdm_softpc_wow_page_domain_window_handle(LOWORD(dead[1])) == dead[1]);
        BORROW_CHECK(root && c_sas_dw_at(root + 28u) == held + delta);
    }
    /* A source outside the replacement still points at B: refuse atomically. */
    BORROW_CHECK(!mvdm_softpc_wow_page_domain_replace_window_links(1, &replacement, 1, &dead[1]));
    BORROW_CHECK(root && c_sas_dw_at(root + 28u) == held + delta);
    BORROW_CHECK(mvdm_softpc_wow_page_domain_replace_window_links(1, &replacement, 2, dead));
    BORROW_CHECK(root && c_sas_dw_at(root + 28u) == 0);
    BORROW_CHECK(!mvdm_softpc_wow_page_domain_window_handle(LOWORD(dead[0])) &&
        !mvdm_softpc_wow_page_domain_window_handle(LOWORD(dead[1])));
    BORROW_CHECK(held && c_sas_dw_at(held) == dead[0] &&
        c_sas_dw_at(held + 24u) == root + delta && c_sas_dw_at(held + 20u) == 0);
    BORROW_CHECK(mvdm_softpc_wow_page_domain_release_window(held));
    BORROW_CHECK(mvdm_softpc_wow_page_domain_release_window(root));
    BORROW_CHECK(mvdm_softpc_wow_page_domain_collect_observed_classes());
    BORROW_CHECK(VdmQueryFreeVirtualMemory(&free_after, &largest) >= 0 && free_after == free_before);
cleanup:
    if (a) DestroyWindow(a);
    if (b) DestroyWindow(b);
    fprintf(stderr, "WOW_DEAD_GRAPH_CCPU errors=%u atomic=1 live_guard=1 retained=1 released=1\n",
        borrow_errors - before);
}

static void verify_graph_preparation_rollback(void)
{
    HWND windows[2];
    mvdm_softpc_wow_window_links links[2] = {0};
    mvdm_softpc_wow_window_descriptor descriptors[2] = {0};
    ULONG free_before = 0, free_after = 0, largest, i;
    unsigned before = borrow_errors;
    windows[0] = CreateWindowExA(0, "STATIC", "Prepare A", WS_POPUP,
        0, 0, 24, 24, NULL, NULL, GetModuleHandleA(NULL), NULL);
    windows[1] = CreateWindowExA(0, "STATIC", "Prepare B", WS_POPUP,
        0, 0, 24, 24, NULL, NULL, GetModuleHandleA(NULL), NULL);
    BORROW_CHECK(windows[0] && windows[1]);
    if (!windows[0] || !windows[1]) goto cleanup;
    BORROW_CHECK(VdmQueryFreeVirtualMemory(&free_before, &largest) >= 0);
    for (i = 0; i < 2u; ++i) {
        WINDOWINFO info = {sizeof(info)};
        int length;
        links[i].window = (ULONG)(ULONG_PTR)windows[i];
        BORROW_CHECK(!mvdm_softpc_wow_page_domain_window_handle(LOWORD(windows[i])));
        BORROW_CHECK(GetWindowInfo(windows[i], &info));
        descriptors[i].atom = info.atomWindowType;
        descriptors[i].class_style = GetClassLongA(windows[i], GCL_STYLE);
        descriptors[i].window_extra = GetClassLongA(windows[i], GCL_CBWNDEXTRA);
        length = GetClassNameA(windows[i], descriptors[i].name, sizeof(descriptors[i].name));
        BORROW_CHECK(length > 0);
        descriptors[i].name_bytes = (ULONG)length + 1u;
    }
    {
        WORD atom = descriptors[1].atom;
        descriptors[1].atom = 0; /* Fail after the first WND/CLS preparation. */
        BORROW_CHECK(!mvdm_softpc_wow_page_domain_project_window_graph(2, links, descriptors));
        descriptors[1].atom = atom;
    }
    for (i = 0; i < 2u; ++i)
        BORROW_CHECK(!mvdm_softpc_wow_page_domain_window_handle(LOWORD(windows[i])));
    BORROW_CHECK(VdmQueryFreeVirtualMemory(&free_after, &largest) >= 0 && free_after == free_before);
    /* Both descriptors are valid, but a top-level native parent cannot be
     * NULL. Reject the stale snapshot after preparing both unpublished WNDs. */
    BORROW_CHECK(!mvdm_softpc_wow_page_domain_project_window_graph(2, links, descriptors));
    for (i = 0; i < 2u; ++i)
        BORROW_CHECK(!mvdm_softpc_wow_page_domain_window_handle(LOWORD(windows[i])));
    BORROW_CHECK(VdmQueryFreeVirtualMemory(&free_after, &largest) >= 0 && free_after == free_before);
cleanup:
    for (i = 0; i < 2u; ++i) if (windows[i]) DestroyWindow(windows[i]);
    fprintf(stderr, "WOW_GRAPH_PREPARATION_CCPU errors=%u descriptor_failure=1 stale_snapshot=1 unpublished=1 released=1\n",
        borrow_errors - before);
}

#ifdef WOW_GRAPH_SNAPSHOT_FIXTURE
static void verify_native_slot_reuse(void)
{
    static wow_user_object_table no_cleanup_owner;
    HWND old_window = NULL, replacement = NULL, desktop = GetDesktopWindow();
    ULONG old_full = 0, new_full = 0, held = 0, fresh = 0, previous, words[3];
    unsigned attempts = 0, before = borrow_errors;
    ULONGLONG started;
    old_window = CreateWindowExA(0, "STATIC", "Reuse old", WS_POPUP,
        0, 0, 24, 24, NULL, NULL, GetModuleHandleA(NULL), NULL);
    BORROW_CHECK(old_window != NULL);
    if (!old_window) goto cleanup;
    old_full = (ULONG)(ULONG_PTR)old_window;
    BORROW_CHECK(wow_user_window_project_graph(&old_window, 1));
    held = mvdm_softpc_wow_page_domain_borrow_window(old_full);
    BORROW_CHECK(held != 0);
    if (!held) goto cleanup;
    BORROW_CHECK(mvdm_softpc_wow_page_domain_exchange_window_word(
        held, 0, 0x76543210u, &previous));
    BORROW_CHECK(DestroyWindow(old_window)); old_window = NULL;
    /* Keep the old publication and its actual CCPU-backed borrow until USER
     * supplies a different generation in the same slot. Never manufacture a
     * handle, and never count failure to witness reuse as a passing skip. */
    started = GetTickCount64();
    while (attempts < 131072u && GetTickCount64() - started < 30000u) {
        ++attempts;
        replacement = CreateWindowExA(0, "STATIC", "Reuse new", WS_POPUP,
            0, 0, 24, 24, NULL, NULL, GetModuleHandleA(NULL), NULL);
        if (!replacement) break;
        new_full = (ULONG)(ULONG_PTR)replacement;
        if (LOWORD(new_full) == LOWORD(old_full) && new_full != old_full) break;
        BORROW_CHECK(DestroyWindow(replacement)); replacement = NULL;
    }
    BORROW_CHECK(replacement && LOWORD(new_full) == LOWORD(old_full) &&
        new_full != old_full && !IsWindow((HWND)(ULONG_PTR)old_full));
    if (!replacement || LOWORD(new_full) != LOWORD(old_full) ||
            new_full == old_full) goto cleanup;
    BORROW_CHECK(wow_user_window_project_graph(&replacement, 1));
    BORROW_CHECK(wow_user_full_handle(&no_cleanup_owner, LOWORD(old_full)) == new_full);
    BORROW_CHECK(!mvdm_softpc_wow_page_domain_borrow_window(old_full));
    fresh = mvdm_softpc_wow_page_domain_borrow_window(new_full);
    BORROW_CHECK(fresh && fresh != held);
    BORROW_CHECK(mvdm_softpc_wow_page_domain_read_window_words(held, words) &&
        words[0] == 0x76543210u && c_sas_dw_at(held) == old_full);
    BORROW_CHECK(fresh && mvdm_softpc_wow_page_domain_read_window_words(fresh, words) &&
        words[0] == 0 && c_sas_dw_at(fresh) == new_full);
cleanup:
    if (old_window) BORROW_CHECK(DestroyWindow(old_window));
    if (replacement) BORROW_CHECK(DestroyWindow(replacement));
    BORROW_CHECK(wow_user_window_project_graph(&desktop, 1));
    if (fresh) BORROW_CHECK(mvdm_softpc_wow_page_domain_release_window(fresh));
    if (held) BORROW_CHECK(mvdm_softpc_wow_page_domain_release_window(held));
    BORROW_CHECK(mvdm_softpc_wow_page_domain_collect_observed_classes());
    if (fresh) BORROW_CHECK(!mvdm_softpc_wow_page_domain_read_window_words(fresh, words));
    if (held) BORROW_CHECK(!mvdm_softpc_wow_page_domain_read_window_words(held, words));
    fprintf(stderr, "WOW_NATIVE_SLOT_REUSE_CCPU errors=%u attempts=%u old=%08lx new=%08lx old_backing=%08lx new_backing=%08lx\n",
        borrow_errors - before, attempts, old_full, new_full, held, fresh);
}

typedef struct discovery_fixture {
    HWND parent, child;
    unsigned calls, children;
    BOOL stop, raise;
} discovery_fixture;

static BOOL CALLBACK discovery_child(HWND window, LPARAM parameter)
{
    discovery_fixture *scope = (discovery_fixture *)parameter;
    ULONG backing = mvdm_softpc_wow_page_domain_borrow_window((ULONG)(ULONG_PTR)window);
    BORROW_CHECK(!wow_user_runtime_current()->exclusive_held);
    BORROW_CHECK(window == scope->child && backing != 0);
    if (backing) {
        BORROW_CHECK(c_sas_dw_at(backing + 104u) == 173u);
        BORROW_CHECK(mvdm_softpc_wow_page_domain_release_window(backing));
    }
    ++scope->children;
    if (scope->raise) RaiseException(0xE0470012u, 0, 0, NULL);
    return !scope->stop;
}

static BOOL CALLBACK discovery_window(HWND window, LPARAM parameter)
{
    discovery_fixture *scope = (discovery_fixture *)parameter;
    BORROW_CHECK(!wow_user_runtime_current()->exclusive_held);
    BORROW_CHECK(mvdm_softpc_wow_page_domain_window_handle(LOWORD(window)) ==
        (ULONG)(ULONG_PTR)window);
    if (window != scope->parent) return TRUE;
    ++scope->calls;
    scope->child = CreateWindowExA(0, "STATIC", "Discovered child", WS_CHILD,
        0, 0, 16, 16, window, (HMENU)173, GetModuleHandle(NULL), NULL);
    BORROW_CHECK(scope->child != NULL);
    if (scope->child) {
        (void)wow_native_EnumChildWindows(window, discovery_child, parameter);
        BORROW_CHECK(DestroyWindow(scope->child));
        scope->child = NULL;
    }
    return TRUE;
}

static void verify_native_discovery(BOOL held)
{
    static wow_user_object_table objects;
    wow_task_order_process process = {0};
    wow_task_order_thread thread = {0};
    wow_user_runtime runtime = WOW_USER_RUNTIME_INITIALIZER;
    wow_user_runtime_thread binding = {0};
    discovery_fixture scope = {0};
    HWND desktop = GetDesktopWindow();
    DWORD native_error;
    BOOL caught = FALSE;
    unsigned before = borrow_errors;
    ZeroMemory(&objects, sizeof(objects));
    process.objects = &objects; thread.ppi = &process;
    BORROW_CHECK(wow_user_runtime_bind(&binding, &runtime, &thread, NULL));
    BORROW_CHECK(wow_user_window_project_graph(&desktop, 1));
    scope.parent = CreateWindowExA(0, "STATIC", "S2 discovery after snapshot",
        WS_POPUP, 0, 0, 32, 32, NULL, NULL, GetModuleHandle(NULL), NULL);
    BORROW_CHECK(scope.parent != NULL);
    if (!scope.parent) goto cleanup;
    BORROW_CHECK(mvdm_softpc_wow_page_domain_window_handle(LOWORD(scope.parent)) !=
        (ULONG)(ULONG_PTR)scope.parent);
    if (held) BORROW_CHECK(wow_user_runtime_enter(&binding));
    SetLastError(0x1234);
    BORROW_CHECK(wow_native_FindWindowA("STATIC", "S2 discovery after snapshot") == scope.parent);
    BORROW_CHECK(mvdm_softpc_wow_page_domain_window_handle(LOWORD(scope.parent)) ==
        (ULONG)(ULONG_PTR)scope.parent && binding.exclusive_held == held);
    BORROW_CHECK(wow_native_EnumWindows(discovery_window, (LPARAM)&scope));
    BORROW_CHECK(scope.calls == 1 && scope.children == 1 && binding.exclusive_held == held);
    BORROW_CHECK(wow_native_EnumThreadWindows(GetCurrentThreadId(), discovery_window, (LPARAM)&scope));
    BORROW_CHECK(scope.calls == 2 && scope.children == 2 && binding.exclusive_held == held);
    scope.child = CreateWindowExA(0, "STATIC", "Stop child", WS_CHILD,
        0, 0, 16, 16, scope.parent, (HMENU)173, GetModuleHandle(NULL), NULL);
    BORROW_CHECK(scope.child != NULL); scope.stop = TRUE;
    (void)wow_native_EnumChildWindows(scope.parent, discovery_child, (LPARAM)&scope);
    BORROW_CHECK(scope.children == 3);
    scope.raise = TRUE;
    __try { (void)wow_native_EnumChildWindows(scope.parent, discovery_child, (LPARAM)&scope); }
    __except(GetExceptionCode() == 0xE0470012u ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) { caught = TRUE; }
    BORROW_CHECK(caught && binding.exclusive_held == held);
    scope.raise = FALSE;
    thread.ppi = NULL; /* Deliberate publication failure, not a native failure. */
    BORROW_CHECK(!wow_native_EnumChildWindows(scope.parent, discovery_child, (LPARAM)&scope));
    BORROW_CHECK(GetLastError() == ERROR_INVALID_STATE && scope.children == 4);
    BORROW_CHECK(!wow_native_FindWindowA("STATIC", "S2 discovery after snapshot"));
    BORROW_CHECK(GetLastError() == ERROR_INVALID_STATE);
    thread.ppi = &process;
    SetLastError(0x1234); BORROW_CHECK(!FindWindowA("S2 absent class", NULL)); native_error = GetLastError();
    SetLastError(0x1234); BORROW_CHECK(!wow_native_FindWindowA("S2 absent class", NULL));
    BORROW_CHECK(GetLastError() == native_error);
    BORROW_CHECK(wow_native_FindWindowA("STATIC", "S2 discovery after snapshot") == scope.parent);
cleanup:
    if (binding.exclusive_held) BORROW_CHECK(wow_user_runtime_leave(&binding));
    if (scope.parent) BORROW_CHECK(DestroyWindow(scope.parent));
    BORROW_CHECK(wow_user_window_project_graph(&desktop, 1));
    BORROW_CHECK(wow_user_runtime_unbind(&binding));
    fprintf(stderr, "WOW_DISCOVERY_CCPU errors=%u held=%d fresh=1 nested=1 stop=1 exception=1 failure=1 recovery=1\n", borrow_errors - before, held);
}

static void verify_graph_snapshot_commit(ULONG teb)
{
    HWND desktop = GetDesktopWindow(), window = NULL, child = NULL;
    ULONG full = 0, held = 0, child_backing = 0;
    ULONG delta = c_sas_dw_at(teb + 0x60u), root;
    unsigned before = borrow_errors;
    BORROW_CHECK(wow_user_window_project_graph(&desktop, 1));
    window = CreateWindowExA(0, "STATIC", "Snapshot parent", WS_POPUP,
        0, 0, 32, 32, NULL, NULL, GetModuleHandleA(NULL), NULL);
    BORROW_CHECK(window != NULL);
    if (!window) goto cleanup;
    child = CreateWindowExA(0, "STATIC", "Snapshot child", WS_CHILD,
        1, 1, 16, 16, window, (HMENU)71, GetModuleHandleA(NULL), NULL);
    BORROW_CHECK(child != NULL);
    if (!child) goto cleanup;
    full = (ULONG)(ULONG_PTR)window;
    BORROW_CHECK(wow_user_window_project_graph(&window, 1));
    held = mvdm_softpc_wow_page_domain_borrow_window(full);
    child_backing = mvdm_softpc_wow_page_domain_borrow_window((ULONG)(ULONG_PTR)child);
    root = mvdm_softpc_wow_page_domain_borrow_window((ULONG)(ULONG_PTR)desktop);
    BORROW_CHECK(held && child_backing && root);
    if (held && child_backing && root) {
        BORROW_CHECK(c_sas_dw_at(held + 24u) == root + delta);
        BORROW_CHECK(c_sas_dw_at(held + 28u) == child_backing + delta);
        BORROW_CHECK(c_sas_dw_at(child_backing + 24u) == held + delta);
        BORROW_CHECK(c_sas_dw_at(child_backing + 104u) == 71u);
    }
    if (root) BORROW_CHECK(mvdm_softpc_wow_page_domain_release_window(root));
    if (child_backing) {
        BORROW_CHECK(mvdm_softpc_wow_page_domain_release_window(child_backing));
        child_backing = 0;
    }
    BORROW_CHECK(DestroyWindow(window)); window = child = NULL;
    BORROW_CHECK(wow_user_window_project_graph(&desktop, 1));
    BORROW_CHECK(!mvdm_softpc_wow_page_domain_window_handle(LOWORD(full)));
    if (held) {
        BORROW_CHECK(c_sas_dw_at(held) == full &&
            c_sas_dw_at(held + 20u) == 0 && c_sas_dw_at(held + 28u) == 0);
        BORROW_CHECK(mvdm_softpc_wow_page_domain_release_window(held));
        held = 0;
    }
    BORROW_CHECK(mvdm_softpc_wow_page_domain_collect_observed_classes());
cleanup:
    if (window) DestroyWindow(window);
    if (child_backing) mvdm_softpc_wow_page_domain_release_window(child_backing);
    if (held) mvdm_softpc_wow_page_domain_release_window(held);
    fprintf(stderr, "WOW_GRAPH_SNAPSHOT_CCPU errors=%u fresh=1 child_id=1 dead_withdrawal=1 retained_reader=1\n",
        borrow_errors - before);
}
#endif

static void verify_observed_words_enrollment(void)
{
    HWND window = CreateWindowExA(0, "STATIC", "Observed words", WS_POPUP,
        0, 0, 24, 24, NULL, NULL, GetModuleHandleA(NULL), NULL);
    WINDOWINFO info = {sizeof(info)};
    wow_user_borrow_scope outer = {0}, inner = {0};
    WW initial = {0}, *alias, *nested;
    ULONG full = (ULONG)(ULONG_PTR)window, before_free = 0, after_free = 0, largest;
    DWORD previous;
    unsigned before = borrow_errors;
    BORROW_CHECK(window != NULL);
    if (!window) return;
    BORROW_CHECK(GetWindowInfo(window, &info));
    BORROW_CHECK(VdmQueryFreeVirtualMemory(&before_free, &largest) >= 0);
    BORROW_CHECK(mvdm_softpc_wow_page_domain_observe_window(full,
        info.atomWindowType, GetClassLongA(window, GCL_STYLE),
        GetClassLongA(window, GCL_CBWNDEXTRA), "Static", sizeof("Static")));
    BORROW_CHECK(wow_user_borrow_enter(&outer));
    alias = wow_user_borrow_window(window);
    BORROW_CHECK(alias != NULL);
    BORROW_CHECK(GetPropW(window, L"NTVDM.WOW.WindowWords") == NULL);
    initial.adwWOW[0] = 0x12345678u;
    BORROW_CHECK(wow_window_words_attach(window, &initial));
    BORROW_CHECK(alias && alias->adwWOW[0] == initial.adwWOW[0]);
    BORROW_CHECK(wow_user_borrow_enter(&inner));
    nested = wow_user_borrow_window(window);
    BORROW_CHECK(alias && nested == alias);
    BORROW_CHECK(wow_user_borrow_window_exchange(window, 0, 0x12345678u, &previous) &&
        previous == 0x12345678u);
    BORROW_CHECK(wow_user_borrow_window_exchange(window, 1, 0x76543210u, &previous));
    BORROW_CHECK(alias && alias->adwWOW[1] == 0x76543210u);
    initial.adwWOW[0] = 0u;
    BORROW_CHECK(!wow_window_words_attach(window, &initial));
    BORROW_CHECK(alias && alias->adwWOW[0] == 0x12345678u);
    wow_window_words_detach(window);
    BORROW_CHECK(DestroyWindow(window));
    BORROW_CHECK(mvdm_softpc_wow_page_domain_forget_observed_window(full));
    BORROW_CHECK(wow_user_borrow_leave(&inner));
    BORROW_CHECK(alias && alias->adwWOW[1] == 0x76543210u);
    BORROW_CHECK(wow_user_borrow_leave(&outer));
    BORROW_CHECK(mvdm_softpc_wow_page_domain_collect_observed_classes());
    BORROW_CHECK(VdmQueryFreeVirtualMemory(&after_free, &largest) >= 0 &&
        before_free == after_free);
    fprintf(stderr, "WOW_VIEW_WORDS_CCPU errors=%u same_alias=1 creator_words=1 duplicate_safe=1 nested_retirement=1\n",
        borrow_errors - before);
}

static void verify_desktop_query_projection(ULONG teb)
{
    HWND desktop = GetDesktopWindow();
    ULONG full = (ULONG)(ULONG_PTR)desktop, backing, server, cls, name_address;
    ULONG delta = c_sas_dw_at(teb + 0x60u), free_before = 0, free_after = 0, largest;
    WINDOWINFO info = {sizeof(info)};
    char name[256], copied[256] = {0};
    int length;
    unsigned before = borrow_errors;
    BORROW_CHECK(GetWindowInfo(desktop, &info));
    length = GetClassNameA(desktop, name, sizeof(name));
    BORROW_CHECK(length > 0 && length < sizeof(name) - 1);
    if (length <= 0 || length >= sizeof(name) - 1) return;
    BORROW_CHECK(mvdm_softpc_wow_page_domain_observe_window(full,
        info.atomWindowType, GetClassLongA(desktop, GCL_STYLE),
        GetClassLongA(desktop, GCL_CBWNDEXTRA), name, (ULONG)length + 1u));
    backing = mvdm_softpc_wow_page_domain_borrow_window(full);
    BORROW_CHECK(backing != 0);
    if (!backing) return;
    server = c_sas_dw_at(backing + 76u);
    BORROW_CHECK(server > delta);
    if (server > delta) {
        cls = server - delta;
        name_address = c_sas_dw_at(cls + 100u);
        BORROW_CHECK(name_address > delta);
        if (name_address > delta) {
            c_sas_loads(name_address - delta, (uint8_t *)copied, (ULONG)length + 1u);
            BORROW_CHECK(strcmp(name, copied) == 0);
        }
    }
    BORROW_CHECK(!mvdm_softpc_wow_page_domain_forget_observed_window(full));
    BORROW_CHECK(!mvdm_softpc_wow_page_domain_retire_class(server));
    BORROW_CHECK(VdmQueryFreeVirtualMemory(&free_before, &largest) >= 0);
    BORROW_CHECK(mvdm_softpc_wow_page_domain_observe_window(full,
        info.atomWindowType, 0, 0, name, (ULONG)length + 1u));
    BORROW_CHECK(c_sas_dw_at(backing + 76u) == server);
    BORROW_CHECK(mvdm_softpc_wow_page_domain_release_window(backing));
    BORROW_CHECK(mvdm_softpc_wow_page_domain_collect_observed_classes());
    BORROW_CHECK(VdmQueryFreeVirtualMemory(&free_after, &largest) >= 0 &&
        free_before == free_after);
    fprintf(stderr, "WOW_DESKTOP_QUERY_CCPU errors=%u single_root=1 class_name=1 retained=1 idempotent=1\n",
        borrow_errors - before);
}

static int verify_native_class_projection(ULONG teb)
{
    wow_class_lookup_context classes = {0};
    wow_class_lookup_entry *entry;
    ULONG free_before = 0, free_after = 0, largest = 0;
    ULONG delta = c_sas_dw_at(teb + 0x60u);
    unsigned before = borrow_errors, projected = 0;
    classes.client_module = GetModuleHandleW(L"user32.dll");
    verify_desktop_query_projection(teb);
    {
        HWND desktop = GetDesktopWindow();
        /* Prior native menu tests leave observations, not managed owners.
         * Reconcile those completed native destructions before measuring
         * this independent class-allocation lifetime. */
        BORROW_CHECK(wow_user_window_project_graph(&desktop, 1));
    }
    BORROW_CHECK(VdmQueryFreeVirtualMemory(&free_before, &largest) >= 0);
    BORROW_CHECK(wow_standard_classes_prepare(&classes, class_projection_callback));
    mvdm_softpc_wow_page_domain_leave_protected();
    BORROW_CHECK(!wow_standard_classes_publish(&classes));
    for (entry = classes.pclsPublicList; entry; entry = entry->next)
        BORROW_CHECK(entry->guest_server == 0);
    BORROW_CHECK(mvdm_softpc_wow_page_domain_reenter_protected());
    BORROW_CHECK(wow_standard_classes_publish(&classes));
    for (entry = classes.pclsPublicList; entry; entry = entry->next) {
        wow_standard_class_view native_class, by_name;
        wow_class_words_binding *again;
        ULONG server, client, name, remaining_before = 0, remaining_after = 0;
        char text[64] = {0};
        unsigned i;
        BORROW_CHECK(wow_standard_class_describe(MAKEINTATOM(entry->atomClassName), &native_class));
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
    {
        ULONG remaining_before = 0, remaining_after = 0;
        BORROW_CHECK(VdmQueryFreeVirtualMemory(&remaining_before, &largest) >= 0);
        BORROW_CHECK(wow_standard_classes_publish(&classes));
        BORROW_CHECK(VdmQueryFreeVirtualMemory(&remaining_after, &largest) >= 0 &&
            remaining_after == remaining_before);
    }
    verify_native_class_creation(&classes);
    BORROW_CHECK(wow_class_words_forget_native(&classes));
    BORROW_CHECK(classes.pclsPublicList == NULL);
    BORROW_CHECK(VdmQueryFreeVirtualMemory(&free_after, &largest) >= 0 && free_after == free_before);
    fprintf(stderr, "WOW_CLASS_ALLOCATION before=%lu after=%lu\n", free_before, free_after);
    verify_unowned_window_projection(teb);
    verify_observed_window_promotion(teb);
    verify_observed_window_retirement();
    verify_dead_graph_retirement(teb);
    verify_graph_preparation_rollback();
#ifdef WOW_RECLAIM_FAILURE_FIXTURE
    verify_failed_window_reclamation();
#endif
    verify_observed_words_enrollment();
    fprintf(stderr, "WOW_NATIVE_CLASS_CCPU errors=%u projected=%u native_pointers=0 released=1\n",
        borrow_errors - before, projected);
    return borrow_errors == before;
}
#endif

static unsigned menu_callouts, menu_locked_callouts;
static BOOL cancel_menu_tracking;
static unsigned menu_tracking_callbacks;
static HMENU menu_tracking_other;
static wow_user_object_table *terminal_menu_objects;
static wow_cleanup_window *terminal_menu_window;
static BOOL terminal_menu_failed;
/* This low-level fixture owns the published WND directly rather than WW.
 * Match the production terminal owner's retained-backing contract now that
 * desktop relationships actually reference the published node. */
static BOOL retire_menu_fixture_window(wow_user_object_table *objects,
    HWND window, wow_cleanup_window *record)
{
    ULONG backing = 0;
    if (!wow_user_window_retire(objects, window, record, &backing)) return FALSE;
    return mvdm_softpc_wow_page_domain_release_retained_window(backing);
}

#ifdef WOW_WINDOW_BORROW_FIXTURE
static HWND rejected_menu_window;
static BOOL rejected_menu_published;

static LRESULT CALLBACK reject_menu_creation(HWND window, UINT message,
    WPARAM wp, LPARAM lp)
{
    if (message == WM_CREATE) {
        rejected_menu_window = window;
        rejected_menu_published = wow_menu_native_refresh_window(window);
        return -1;
    }
    return DefWindowProcA(window, message, wp, lp);
}

static BOOL verify_menu_creation_rollback(wow_user_runtime_thread *binding)
{
    static const WCHAR text[] = L"S2MenuCreationRollback";
    UNICODE_STRING name = {sizeof(text) - sizeof(WCHAR), sizeof(text), (PWSTR)text};
    wow_class_lookup_context classes = {0};
    wow_task_order_process *process = binding->thread->ppi;
    wow_class_lookup_context *previous = process->classes;
    WNDCLASSEXA cls = {sizeof(cls)};
    wow_window_dispatch_target target = {(DWORD)reject_menu_creation, NULL};
    CREATESTRUCTA request = {0};
    WW words = {0};
    DWORD class_words[2] = {0};
    ATOM atom = 0;
    HMENU menu = NULL;
    HWND window = NULL;
    unsigned mode;
    BOOL result = FALSE;
    process->classes = &classes;
    cls.hInstance = GetModuleHandleA(NULL);
    cls.lpfnWndProc = wow_window_native_proc;
    cls.lpszClassName = "S2MenuCreationRollback";
    atom = wow_class_words_register_target(&classes, &cls, &name,
        class_words, NULL, &target);
    if (!atom) goto cleanup;
    request.lpszClass = cls.lpszClassName;
    request.hInstance = cls.hInstance;
    request.style = WS_OVERLAPPEDWINDOW;
    request.cx = request.cy = 100;
    for (mode = 0; mode != 2; ++mode) {
        menu = wow_menu_native_CreateMenu();
        if (!menu) goto cleanup;
        request.hMenu = menu;
        rejected_menu_window = NULL;
        rejected_menu_published = FALSE;
        if (mode && !wow_user_runtime_enter(binding)) goto cleanup;
        window = wow_window_create_boundA(binding->thread, atom, &request, &words, NULL);
        if (window || !rejected_menu_window || !rejected_menu_published ||
                IsWindow(rejected_menu_window) || IsMenu(menu) ||
                process->objects->entries[LOWORD(rejected_menu_window)].bType != TYPE_FREE ||
                process->objects->entries[LOWORD(menu)].bType != TYPE_FREE ||
                binding->exclusive_held != (BOOL)mode) goto cleanup;
        if (mode && !wow_user_runtime_leave(binding)) goto cleanup;
        menu = NULL;
    }
    result = TRUE;
cleanup:
    if (binding->exclusive_held && !wow_user_runtime_leave(binding)) result = FALSE;
    if (window) wow_native_DestroyWindow(window);
    if (menu && IsMenu(menu)) wow_menu_native_DestroyMenu(menu);
    if (atom && !wow_class_words_unregister(&classes, atom, cls.hInstance, NULL))
        result = FALSE;
    process->classes = previous;
    fprintf(stderr, "WOW_MENU_CREATION_ROLLBACK_CCPU result=%u modes=2\n", result);
    return result;
}
#endif

static LRESULT CALLBACK menu_callout_window(HWND window, UINT message,
    WPARAM wp, LPARAM lp)
{
    if (message == WM_NCCALCSIZE) {
        wow_user_runtime_thread *binding = wow_user_runtime_current();
        ++menu_callouts;
        if (binding && binding->exclusive_held) ++menu_locked_callouts;
    }
    if (message == WM_INITMENUPOPUP && cancel_menu_tracking) {
        wow_user_runtime_thread *binding = wow_user_runtime_current();
        ++menu_tracking_callbacks;
        if (binding && binding->exclusive_held) ++menu_locked_callouts;
        /* Native tracking reenters before any user interaction. Mutate the
         * menu, then cancel synchronously; FALSE must not skip publication. */
        if (wow_menu_native_CheckMenuItem((HMENU)wp, 71u, MF_UNCHECKED) == (DWORD)-1)
            ++menu_locked_callouts;
        /* Native code can change a different, detached menu without going
         * through any WOW mutation wrapper. Only the common return boundary
         * can make that change visible to USER16's local queries. */
        if (CheckMenuItem(menu_tracking_other, 99u, MF_CHECKED) == (DWORD)-1)
            ++menu_locked_callouts;
        if (!EndMenu()) ++menu_locked_callouts;
        return 0;
    }
    if (message == WM_NCDESTROY && terminal_menu_objects) {
        if (!retire_menu_fixture_window(terminal_menu_objects, window,
                terminal_menu_window)) terminal_menu_failed = TRUE;
    }
    return DefWindowProcA(window, message, wp, lp);
}

static BOOL verify_native_menu_entry(wow_task_order_process *process,
    ULONG handles, ULONG delta)
{
    wow_user_runtime runtime = WOW_USER_RUNTIME_INITIALIZER;
    wow_user_task_lifecycle native_owner = {0};
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
    runtime.lifecycle = &native_owner; /* No second task/handoff in this fixture. */
    if (!wow_user_runtime_bind(&binding, &runtime, &thread, NULL)) return FALSE;
    {
        wow_user_native_call call;
        BOOL accepted;
        DWORD error;
        step = "missing-lifecycle-rejected";
        runtime.lifecycle = NULL;
        accepted = wow_user_native_call_begin(&call, NULL);
        error = GetLastError();
        runtime.lifecycle = &native_owner;
        if (accepted || error != ERROR_INVALID_STATE || binding.exclusive_held)
            goto cleanup;
    }
#ifdef WOW_RESOURCE_CLIENT_FIXTURE
    {
        extern int __cdecl wow_original_menu_resource_fixture(void);
        extern int __cdecl wow_original_menu_thunk_fixture(void);
        step = "bound-original-resource-client";
        if (wow_original_menu_resource_fixture()) goto cleanup;
        step = "bound-original-menu-thunks";
        if (wow_original_menu_thunk_fixture()) goto cleanup;
    }
#endif
    {
        /* The selected provider ABI maps original FASTCALL to stdcall. */
        extern ULONG WINAPI WU32CreateMenu(PVOID);
        extern ULONG WINAPI WU32CreatePopupMenu(PVOID);
        ULONG menu16, popup16;
        step = "original-menu-thunk-creation";
        menu16 = WU32CreateMenu(NULL);
        popup16 = WU32CreatePopupMenu(NULL);
        root = wow_menu_client_full_handle(process->objects, (HMENU)(ULONG_PTR)menu16);
        child = wow_menu_client_full_handle(process->objects, (HMENU)(ULONG_PTR)popup16);
        if (!menu16 || !popup16 || !root || !child ||
                *(DWORD *)process->objects->entries[LOWORD(root)].phead != task.hTaskWow ||
                *(DWORD *)process->objects->entries[LOWORD(child)].phead != task.hTaskWow)
            goto cleanup;
        if (!wow_menu_native_DestroyMenu(root)) goto cleanup;
        root = NULL;
        if (!wow_menu_native_DestroyMenu(child)) goto cleanup;
        child = NULL;
        fprintf(stderr, "WOW_ORIGINAL_MENU_ENTRY_CCPU create=1 popup=1 registered=1\n");
    }
    {
        extern HMENU WINAPI CreateMenuFromResource(LPBYTE, BOOL);
        WORD resource[] = {0, 0, MF_POPUP | MF_END, 'P', 0,
            MF_END, 31, 'C', 0};
        /* Extended root succeeds in allocating before its invalid item is
         * rejected; the original parser must retire that allocation too. */
        DWORD invalid[] = {0x00040001, 0, 0x80000000, 0, 23, 0x80, 0};
        DWORD invalid_child[] = {0x00040001,0,0,0,23,0x00500081,0,
            0,0x80000000,0,24,0x80,0};
        WORD version[] = {2,0};
        DWORD extended[] = {0x00040001,77,0,0,51,0x00580080,0};
        WORD empty_owner[] = {0,0,MF_OWNERDRAW | MF_END,41,0};
        WORD owner_resource[] = {0,0,MF_END | MF_OWNERDRAW,23,0x5678,0x1234,0};
        MENUITEMINFOW info = {sizeof(info)};
        ULONG i, before = 0, after = 0;
        step = "original-resource-creation";
        root = CreateMenuFromResource((LPBYTE)resource, TRUE);
        child = root ? GetSubMenu(root, 0) : NULL;
        if (!root || !child ||
                process->objects->entries[LOWORD(root)].bType != TYPE_MENU ||
                process->objects->entries[LOWORD(child)].bType != TYPE_MENU ||
                *(DWORD *)process->objects->entries[LOWORD(root)].phead != task.hTaskWow ||
                *(DWORD *)process->objects->entries[LOWORD(child)].phead != task.hTaskWow)
            goto cleanup;
        root_client = c_sas_dw_at(handles + LOWORD(root) * 16u) - delta;
        child_client = c_sas_dw_at(handles + LOWORD(child) * 16u) - delta;
        items = c_sas_dw_at(root_client + 56u) - delta;
        if (c_sas_dw_at(root_client + 40u) != 1u ||
                c_sas_dw_at(items + 12u) != child_client + delta ||
                c_sas_dw_at(child_client + 40u) != 1u)
            goto cleanup;
        if (!wow_menu_native_DestroyMenu(root) || IsMenu(child) ||
                process->objects->entries[LOWORD(root)].bType != TYPE_FREE ||
                process->objects->entries[LOWORD(child)].bType != TYPE_FREE)
            goto cleanup;
        root = child = NULL;
        step = "original-resource-extended-and-ownerdraw";
        root = CreateMenuFromResource((LPBYTE)extended, FALSE);
        if (!root || GetMenuItemID(root, 0) != 51 ||
                !wow_menu_native_DestroyMenu(root)) goto cleanup;
        root = CreateMenuFromResource((LPBYTE)empty_owner, TRUE);
        info.fMask = MIIM_DATA | MIIM_FTYPE;
        if (!root || !GetMenuItemInfoW(root, 0, TRUE, &info) ||
                info.dwItemData || !(info.fType & MFT_OWNERDRAW) ||
                !wow_menu_native_DestroyMenu(root)) goto cleanup;
        root = CreateMenuFromResource((LPBYTE)owner_resource, FALSE);
        if (!root || !GetMenuItemInfoW(root, 0, TRUE, &info) ||
                info.dwItemData != (ULONG_PTR)&owner_resource[4] ||
                !wow_menu_native_DestroyMenu(root)) goto cleanup;
        root = NULL;
        for (i = 0; i <= process->objects->last_handle; ++i)
            if (process->objects->entries[i].bType != TYPE_FREE) ++before;
        step = "original-resource-failure-cleanup";
        if (CreateMenuFromResource((LPBYTE)version, TRUE) != NULL) goto cleanup;
        for (i = 0; i != 64; ++i) {
            if (CreateMenuFromResource((LPBYTE)invalid, TRUE) != NULL ||
                    GetLastError() != ERROR_INVALID_DATA ||
                    CreateMenuFromResource((LPBYTE)invalid_child, TRUE) != NULL)
                goto cleanup;
        }
        for (i = 0; i <= process->objects->last_handle; ++i)
            if (process->objects->entries[i].bType != TYPE_FREE) ++after;
        if (before != after) goto cleanup;
        fprintf(stderr, "WOW_RESOURCE_MENU_CCPU registered=1 nested=1 rollback=1\n");
    }
    {
        UINT messages[] = { WM_INITMENU, WM_INITMENUPOPUP,
            WM_MENUSELECT, WM_MENUCHAR };
        unsigned n;
        step = "message-carried-borrowed-menu";
        /* Deliberately not created through the WOW wrapper and not attached
         * to a window: the message is the first observation of this tree. */
        root = CreatePopupMenu();
        child = CreatePopupMenu();
        if (!root || !child || !AppendMenuA(child, MF_STRING, 61, "Native") ||
                !AppendMenuA(root, MF_POPUP, (UINT_PTR)child, "Popup"))
            goto cleanup;
        for (n = 0; n < ARRAYSIZE(messages); ++n) {
            BOOL in_wp = messages[n] == WM_INITMENU || messages[n] == WM_INITMENUPOPUP;
            SetLastError(0x1234);
            if (!wow_menu_native_refresh_message(messages[n],
                    in_wp ? (WPARAM)root : MAKEWPARAM(0, MF_POPUP),
                    in_wp ? 0 : (LPARAM)root) || GetLastError() != 0x1234 ||
                    process->objects->entries[LOWORD(root)].bType != TYPE_MENU ||
                    process->objects->entries[LOWORD(root)].pOwner ||
                    process->objects->entries[LOWORD(child)].pOwner)
                goto cleanup;
            root_client = c_sas_dw_at(handles + LOWORD(root) * 16u) - delta;
            child_client = c_sas_dw_at(handles + LOWORD(child) * 16u) - delta;
            items = c_sas_dw_at(root_client + 56u) - delta;
            if (c_sas_dw_at(items + 12u) != child_client + delta ||
                    c_sas_dw_at(child_client + 40u) != 1u) goto cleanup;
        }
        SetLastError(0x4321);
        if (!wow_menu_native_refresh_message(WM_MENUSELECT, MAKEWPARAM(0, 0xffff), 0) ||
                GetLastError() != 0x4321 ||
                !wow_menu_native_refresh_message(WM_INITMENU, (WPARAM)0x12340001, 0) ||
                GetLastError() != 0x4321 ||
                !wow_menu_native_DestroyMenu(root) || IsMenu(child)) goto cleanup;
        root = child = NULL;
        fprintf(stderr, "WOW_MENU_MESSAGE_CCPU borrowed=1 detached=1 terminal=1 invalid_passthrough=1\n");
    }
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
    step = "draw-menu-callout";
    if (!wow_menu_native_DrawMenuBar(window) || !binding.exclusive_held ||
            menu_locked_callouts) goto cleanup;
    step = "private-system-menu-observation-and-revert";
    {
        HMENU system_menu = wow_menu_native_GetSystemMenu(window, FALSE);
        if (!system_menu || !binding.exclusive_held ||
                process->objects->entries[LOWORD(system_menu)].bType != TYPE_MENU ||
                process->objects->entries[LOWORD(system_menu)].pOwner)
            goto cleanup;
        /* Querying a second time from another task is not proof that this
         * task allocated the private native copy. Revert remains USER-owned. */
        task.hTaskWow = 0x9876u;
        if (wow_menu_native_GetSystemMenu(window, FALSE) != system_menu ||
                process->objects->entries[LOWORD(system_menu)].pOwner)
            goto cleanup;
        task.hTaskWow = 0x4321u;
        if (wow_menu_native_GetSystemMenu(window, TRUE) != NULL ||
                !binding.exclusive_held || IsMenu(system_menu) ||
                process->objects->entries[LOWORD(system_menu)].bType != TYPE_FREE)
            goto cleanup;
        fprintf(stderr, "WOW_SYSTEM_MENU_CCPU borrowed=1 observer_preserved=1 revert_retired=1\n");
    }
    step = "popup-tracking-cancelled-mutation";
    menu_tracking_other = wow_menu_native_CreatePopupMenu();
    if (!menu_tracking_other || !wow_menu_native_AppendMenuA(menu_tracking_other,
            MF_STRING, 99u, "Detached callback target")) goto cleanup;
    cancel_menu_tracking = TRUE;
    menu_tracking_callbacks = 0;
    {
        BOOL tracked = wow_menu_native_TrackPopupMenu(alias, TPM_RETURNCMD,
            0, 0, 0, window, NULL);
        cancel_menu_tracking = FALSE;
        if (tracked || menu_tracking_callbacks != 1 || menu_locked_callouts ||
                !binding.exclusive_held) goto cleanup;
        items = c_sas_dw_at(child_client + 56u) - delta;
        if (c_sas_dw_at(items + 4u) & MF_CHECKED) goto cleanup;
        {
            ULONG other = c_sas_dw_at(handles + LOWORD(menu_tracking_other) * 16u) - delta;
            ULONG other_items = c_sas_dw_at(other + 56u) - delta;
            if (!(c_sas_dw_at(other_items + 4u) & MF_CHECKED)) goto cleanup;
        }
        if (!wow_menu_native_DestroyMenu(menu_tracking_other)) goto cleanup;
        menu_tracking_other = NULL;
        fprintf(stderr, "WOW_MENU_TRACKING_CCPU cancelled=1 reentered=1 unlocked=1 mutation_published=1 detached_published=1\n");
    }
    step = "post-tracking-window-menu";
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
    step = "post-tracking-mutation-lock";
    if (wow_menu_native_CheckMenuItem(alias, 71u, MF_UNCHECKED) == (DWORD)-1 ||
            !binding.exclusive_held ||
            wow_menu_native_CheckMenuItem(alias, 999u, MF_CHECKED) != (DWORD)-1 ||
            !binding.exclusive_held ||
            wow_menu_native_CheckMenuItem(alias, 71u, MF_CHECKED) == (DWORD)-1 ||
            !binding.exclusive_held) goto cleanup;
    step = "post-tracking-detach";
    if (!wow_menu_native_SetMenu(window, NULL) || !binding.exclusive_held ||
            !wow_user_runtime_leave(&binding)) goto cleanup;
    step = "post-tracking-retirement";
    if (!retire_menu_fixture_window(process->objects, window, &window_record))
        goto cleanup;
    published = FALSE;
    if (!wow_native_DestroyWindow(window)) goto cleanup;
    window = NULL;
    items = c_sas_dw_at(child_client + 56u) - delta;
    step = "delete-submenu-publication-before-retirement";
    {
        HMENU nested = wow_menu_native_CreatePopupMenu();
        if (!nested) goto cleanup;
        if (!wow_menu_native_AppendMenuA(child, MF_POPUP, (UINT_PTR)nested, "Nested")) {
            wow_menu_native_DestroyMenu(nested);
            goto cleanup;
        }
        if (!wow_menu_native_DeleteMenu(child, 1u, MF_BYPOSITION) || IsMenu(nested) ||
                process->objects->entries[LOWORD(nested)].bType != TYPE_FREE ||
                c_sas_dw_at(child_client + 40u) != 1u) goto cleanup;
        items = c_sas_dw_at(child_client + 56u) - delta;
    }
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
    step = "implicit-window-destruction-menu-retirement";
    {
        unsigned mode;
        for (mode = 0; mode != 2; ++mode) {
            MSG message = {0};
            root = wow_menu_native_CreateMenu();
            if (!root) goto cleanup;
            window = CreateWindowA(window_class.lpszClassName, "Implicit close",
                WS_OVERLAPPEDWINDOW, 0, 0, 100, 100, NULL, root,
                window_class.hInstance, NULL);
            if (!window) goto cleanup;
            published = wow_user_window_publish(process->objects, window, &window_record, &thread);
            if (!published || !wow_menu_native_refresh_window(window)) goto cleanup;
            terminal_menu_objects = process->objects;
            terminal_menu_window = &window_record;
            terminal_menu_failed = FALSE;
            message.hwnd = window; message.message = WM_CLOSE;
            if (mode) (void)wow_native_DispatchMessageA(&message);
            else (void)wow_native_DefWindowProcA(window, WM_CLOSE, 0, 0);
            terminal_menu_objects = NULL;
            if (terminal_menu_failed || IsWindow(window) || IsMenu(root) ||
                    process->objects->entries[LOWORD(window)].bType != TYPE_FREE ||
                    process->objects->entries[LOWORD(root)].bType != TYPE_FREE ||
                    binding.exclusive_held) goto cleanup;
            published = FALSE;
            root = NULL; window = NULL;
        }
        fprintf(stderr, "WOW_MENU_NATIVE_RETURN_CCPU default_close=1 dispatch_close=1 retired=1 lock_restored=1\n");
    }
#ifdef WOW_WINDOW_BORROW_FIXTURE
    step = "implicit-creation-failure-menu-retirement";
    if (!verify_menu_creation_rollback(&binding)) goto cleanup;
#endif
    result = TRUE;
cleanup:
    cancel_menu_tracking = FALSE;
    if (menu_tracking_other) DestroyMenu(menu_tracking_other);
    menu_tracking_other = NULL;
    terminal_menu_objects = NULL;
    if (!result) fprintf(stderr, "WOW_NATIVE_MENU_ENTRY_FAILED step=%s error=%lu\n",
        step, GetLastError());
    if (binding.exclusive_held && !wow_user_runtime_leave(&binding)) result = FALSE;
    if (published && !retire_menu_fixture_window(process->objects, window,
            &window_record)) result = FALSE;
    if (window && !wow_native_DestroyWindow(window)) result = FALSE;
    if (window_class.lpszClassName)
        UnregisterClassA(window_class.lpszClassName, window_class.hInstance);
    if (root && IsMenu(root)) DestroyMenu(root);
    if (child && IsMenu(child)) DestroyMenu(child);
    if (!wow_menu_client_retire_destroyed(process)) result = FALSE;
    if (!wow_user_runtime_unbind(&binding)) result = FALSE;
    fprintf(stderr, "WOW_NATIVE_MENU_ENTRY_CCPU result=%u\n", result);
    return result;
}

static int run_fixture(void)
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
    {
        ULONG native_desktop = (ULONG)(ULONG_PTR)GetDesktopWindow();
        ULONG desktop_entry = handles + LOWORD(native_desktop) * 16u;
        uint32_t root;
        if (!mvdm_softpc_wow_page_domain_client_desktop_ready() ||
            c_sas_dw_at(teb + 0x5cu) == 0u || c_sas_dw_at(teb + 0x60u) == 0u ||
            c_sas_dw_at(desktop_entry + 8u) != ((native_desktop & 0xffff0000u) | 1u) ||
            c_sas_dw_at(c_sas_dw_at(desktop_entry) - c_sas_dw_at(teb + 0x60u)) !=
                native_desktop) goto done;
        root = c_sas_dw_at(desktop_entry) - c_sas_dw_at(teb + 0x60u);
        /* Original WND.fnid is a WORD at 122, between unrelated fields.
         * Original _IsWindowVisible stops at FNID_DESKTOP, not a NULL parent.
         * Verify actual CCPU backing and avoid a DWORD write corrupting its
         * neighbour. This checks the producer, not a full graph traversal. */
        stage = "desktop-root-fnid";
        if (c_sas_dw_at(root + 120u) != 0x029d0000u ||
                c_sas_dw_at(root + 124u) != 0u ||
                c_sas_dw_at(root + 24u) != 0u ||
                !(c_sas_dw_at(root + 168u) & WS_VISIBLE)) goto done;
        if (mvdm_softpc_wow_page_domain_publish_handle(LOWORD(native_desktop),
                HIWORD(native_desktop), 1u, 0u, native_desktop, 0u, NULL) ||
                mvdm_softpc_wow_page_domain_retire_handle(LOWORD(native_desktop), NULL))
            goto done;
        /* Index one is ordinary unless it really belongs to the desktop.
         * No hidden permanent reservation survives the identity repair. */
        if (LOWORD(native_desktop) != 1u) {
            if (!mvdm_softpc_wow_page_domain_publish_handle(1u, 0x4567u,
                    1u, 0u, 0u, 0u, NULL) ||
                    mvdm_softpc_wow_page_domain_window_handle(1u) != 0x45670001u ||
                    !mvdm_softpc_wow_page_domain_retire_handle(1u, NULL)) goto done;
        }
        fprintf(stderr, "WOW_DESKTOP_FNID_CCPU value=029d adjacent=0 root=1\n");
    }
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
            0u, guest_class, NULL) ||
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
        mvdm_softpc_wow_window_links links[4] = {
            { 0u, 0u, 0u, 0x23450002u, 0u },
            { 0x23450002u, 0u, 0u, 0x34560005u, 0u },
            { 0x34560005u, 0x34560006u, 0x23450002u, 0u, 0u },
            { 0x34560006u, 0u, 0x23450002u, 0u, 0x23450002u }
        };
        ULONG delta = c_sas_dw_at(teb + 0x60u);
        ULONG parent = c_sas_dw_at(handles + 32u) - delta;
        ULONG native_desktop = (ULONG)(ULONG_PTR)GetDesktopWindow();
        ULONG root = c_sas_dw_at(handles + LOWORD(native_desktop) * 16u) - delta;
        ULONG child, sibling, retired = 0u;
        links[0].window = links[1].parent = native_desktop;
        stage = "window-links-publication";
        if (!mvdm_softpc_wow_page_domain_publish_handle(5u, 0x3456u, 1u, 0u, 0u, 0u, NULL) ||
            !mvdm_softpc_wow_page_domain_publish_handle(6u, 0x3456u, 1u, 0u, 0u, 0u, NULL) ||
            !mvdm_softpc_wow_page_domain_publish_window_links(4u, links)) goto done;
        child = c_sas_dw_at(parent + 28u) - c_sas_dw_at(parent + 12u) + parent;
        sibling = c_sas_dw_at(child + 20u) - c_sas_dw_at(child + 12u) + child;
        if (c_sas_dw_at(child) != 0x34560005u || c_sas_dw_at(sibling) != 0x34560006u ||
            c_sas_dw_at(child + 24u) != parent + delta ||
            c_sas_dw_at(sibling + 32u) != parent + delta ||
            c_sas_dw_at(parent + 24u) != root + delta) goto done;
        stage = "window-view-borrow";
        if (mvdm_softpc_wow_page_domain_borrow_window(0x34560005u) != child ||
            mvdm_softpc_wow_page_domain_borrow_window(0x34560005u) != child ||
            mvdm_softpc_wow_page_domain_borrow_window(0x34560006u) != sibling ||
            mvdm_softpc_wow_page_domain_borrow_window(0x99990005u)) goto done;
        stage = "window-links-failure-atomicity";
        links[0].child = 0u; /* Earlier valid change must not commit on failure. */
        links[3].owner = 0x98760002u; /* Full generation, not just slot. */
        if (mvdm_softpc_wow_page_domain_publish_window_links(4u, links) ||
            c_sas_dw_at(root + 28u) != parent + delta ||
            c_sas_dw_at(sibling + 32u) != parent + delta ||
            mvdm_softpc_wow_page_domain_retire_handle(2u, NULL) ||
            mvdm_softpc_wow_page_domain_retire_handle(5u, NULL)) goto done;
        stage = "window-links-update-and-retire";
        /* Represent original pre-link creation: a parent backpointer does
         * not require membership in that parent's child list yet. */
        links[1].child = 0u;
        links[2].next = 0u;
        links[3].owner = 0u;
        if (!mvdm_softpc_wow_page_domain_publish_window_links(4u, links) ||
            c_sas_dw_at(parent + 28u) != 0u ||
            c_sas_dw_at(child + 24u) != parent + delta ||
            !mvdm_softpc_wow_page_domain_retire_handle(5u, NULL) ||
            !mvdm_softpc_wow_page_domain_retire_handle(6u, &retired)) goto done;
        if (retired != sibling || c_sas_dw_at(retired + 20u) != 0u ||
            c_sas_dw_at(retired + 24u) != root + delta ||
            c_sas_dw_at(retired + 28u) != 0u ||
            c_sas_dw_at(retired + 32u) != 0u ||
            !mvdm_softpc_wow_page_domain_release_retained_window(retired) ||
            c_sas_dw_at(retired) != 0x34560006u ||
            !mvdm_softpc_wow_page_domain_release_window(retired)) goto done;
        stage = "window-links-reused-slot";
        if (!mvdm_softpc_wow_page_domain_publish_handle(5u, 0x3457u, 1u, 0u, 0u, 0u, NULL)) goto done;
        if (c_sas_dw_at(handles + 5u * 16u) - delta == child ||
            c_sas_dw_at(child) != 0x34560005u ||
            !mvdm_softpc_wow_page_domain_release_window(child) ||
            c_sas_dw_at(child) != 0x34560005u ||
            !mvdm_softpc_wow_page_domain_release_window(child) ||
            mvdm_softpc_wow_page_domain_release_window(child)) goto done;
        links[1].child = 0x34560005u;
        if (mvdm_softpc_wow_page_domain_publish_window_links(2u, links) ||
            c_sas_dw_at(parent + 28u) != 0u) goto done;
        links[1].child = 0x34570005u;
        if (!mvdm_softpc_wow_page_domain_publish_window_links(2u, links)) goto done;
        links[1].child = links[1].parent = 0u;
        if (!mvdm_softpc_wow_page_domain_publish_window_links(2u, links) ||
            !mvdm_softpc_wow_page_domain_retire_handle(5u, NULL)) goto done;
        fprintf(stderr, "WOW_WINDOW_LINKS_CCPU atomic=1 rebase=1 intermediate=1 retirement=1 reuse=1\n");
    }
    {
        ULONG sample_class, sample, retained = 0, words[3], previous = 0xabcdef01u;
        static const char sample_name[] = "Borrowed class";
        stage = "window-view-retained-class";
        sample_class = mvdm_softpc_wow_page_domain_publish_named_class(
            0xc123u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
            sample_name, sizeof(sample_name));
        if (!sample_class ||
            !mvdm_softpc_wow_page_domain_publish_handle(7u, 0x4567u,
                1u, 0u, 0u, sample_class, NULL)) goto done;
        sample = mvdm_softpc_wow_page_domain_borrow_window(0x45670007u);
        if (!sample ||
            !mvdm_softpc_wow_page_domain_read_window_words(sample, words) ||
            words[0] || words[1] || words[2] ||
            !mvdm_softpc_wow_page_domain_exchange_window_word(sample, 0u, 0x12340002u, &previous) ||
            previous || c_sas_dw_at(sample + 144u) != 0x12340002u ||
            mvdm_softpc_wow_page_domain_exchange_window_word(sample, 3u, 1u, &previous) ||
            previous ||
            !mvdm_softpc_wow_page_domain_retire_handle(7u, &retained) ||
            sample != retained ||
            !mvdm_softpc_wow_page_domain_read_window_words(sample, words) ||
            words[0] != 0x12340002u || words[1] || words[2] ||
            mvdm_softpc_wow_page_domain_exchange_window_word(sample, 0u, 1u, &previous) ||
            mvdm_softpc_wow_page_domain_retire_class(sample_class) ||
            !mvdm_softpc_wow_page_domain_release_window(sample) ||
            mvdm_softpc_wow_page_domain_retire_class(sample_class) ||
            !mvdm_softpc_wow_page_domain_release_retained_window(retained) ||
            mvdm_softpc_wow_page_domain_release_retained_window(retained) ||
            mvdm_softpc_wow_page_domain_read_window_words(sample, words) ||
            !mvdm_softpc_wow_page_domain_retire_class(sample_class)) goto done;
        /* Retained ownership alone must also be registered and validated. */
        if (!mvdm_softpc_wow_page_domain_publish_handle(7u, 0x4568u,
                1u, 0u, 0u, 0u, NULL) ||
            !mvdm_softpc_wow_page_domain_retire_handle(7u, &retained) ||
            !mvdm_softpc_wow_page_domain_release_retained_window(retained) ||
            mvdm_softpc_wow_page_domain_release_retained_window(retained)) goto done;
        fprintf(stderr, "WOW_WINDOW_VIEW_BORROWS_CCPU retained_class=1 release_orders=2 duplicate=1 words=1\n");
    }
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
        stage = "guest-menu-unchanged-publication";
        if (mvdm_softpc_wow_page_domain_publish_menu(0x45670003u,
                0x4321u, 2u, entries) != root_menu ||
                c_sas_dw_at(root_client + 56u) != item_array + delta) goto done;
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
        stage = "guest-menu-incoming-reference-retention";
        if (mvdm_softpc_wow_page_domain_retire_menu(0x45670003u) ||
                GetLastError() != ERROR_BUSY ||
                mvdm_softpc_wow_page_domain_retire_menu(0x45670004u) ||
                GetLastError() != ERROR_BUSY ||
                c_sas_dw_at(handles + 48u) != root_menu ||
                c_sas_dw_at(root_client + 40u) != 2u ||
                c_sas_dw_at(item_array + 12u) != child_menu ||
                c_sas_dw_at(window_client + 104u) != root_menu) goto done;
        stage = "guest-menu-rejection-preserves-graph";
        entries[0].submenu = 0x98760004u; /* stale identity, same slot */
        if (mvdm_softpc_wow_page_domain_publish_menu(0x45670003u, 0x4321u, 2u, entries) ||
                mvdm_softpc_wow_page_domain_publish_menu(0x45670003u, 1u, 0u, NULL) ||
                mvdm_softpc_wow_page_domain_publish_menu(0x98760003u, 0x4321u, 0u, NULL) ||
                mvdm_softpc_wow_page_domain_publish_menu(0x23450002u, 0x4321u, 0u, NULL) ||
                mvdm_softpc_wow_page_domain_publish_handle(3u, 0x4567u, 1u, 0u, 0u, 0u, NULL) ||
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
            !wow_menu_client_register_created(&menu_process, 0x4321u, native_root) ||
            !wow_menu_client_register_created(&menu_process, 0x4321u, native_submenu) ||
            !AppendMenuA(native_submenu, MF_STRING | MF_CHECKED, 71u, "Leaf") ||
            !AppendMenuA(native_root, MF_POPUP, (UINT_PTR)native_submenu, "Root") ||
            !wow_menu_client_publish_tree(&menu_process, native_root))
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
                !wow_menu_client_publish_tree(&menu_process, native_root))
            goto done;
        sub_items = c_sas_dw_at(sub_client + 56u) - delta;
        if ((c_sas_dw_at(sub_items + 4u) & MF_CHECKED) ||
                c_sas_dw_at(sub_client + 20u) != 0x4321u) goto done;
        stage = "native-menu-remove-retains-submenu";
        if (!RemoveMenu(native_root, 0, MF_BYPOSITION) ||
                !wow_menu_client_publish_tree(&menu_process, native_root) ||
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
                !wow_menu_client_register_created(&menu_process, 0x4321u, native_root) ||
                !wow_menu_client_register_created(&menu_process, 0x4321u, native_submenu) ||
                !AppendMenuA(native_root, MF_POPUP, (UINT_PTR)native_submenu, "Sub") ||
                !wow_menu_client_publish_tree(&menu_process, native_root) ||
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
                !wow_menu_client_register_created(&menu_process, 0x4321u, native_root) ||
                !wow_menu_client_register_created(&menu_process, 0x9876u, native_submenu) ||
                !wow_menu_client_publish_tree(&menu_process, native_submenu) ||
                wow_menu_client_forget_borrowed(&menu_process) ||
                GetLastError() != ERROR_BUSY ||
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
        stage = "borrowed-menu-cleanup";
        native_root = CreateMenu();
        native_submenu = CreatePopupMenu();
        if (!native_root || !native_submenu ||
                !AppendMenuA(native_root, MF_POPUP, (UINT_PTR)native_submenu, "Native") ||
                !wow_menu_client_publish_tree(&menu_process, native_root) ||
                object_table.entries[LOWORD(native_root)].pOwner ||
                object_table.entries[LOWORD(native_submenu)].pOwner ||
                wow_menu_client_register_created(&menu_process, 0x4321u, native_root) ||
                GetLastError() != ERROR_ALREADY_EXISTS ||
                !wow_user_cleanup_bound(NULL, 0x4321u, NULL, 0, &cleanup) ||
                !IsMenu(native_root) || !IsMenu(native_submenu) ||
                !wow_menu_client_forget_borrowed(&menu_process) ||
                object_table.entries[LOWORD(native_root)].bType != TYPE_FREE ||
                object_table.entries[LOWORD(native_submenu)].bType != TYPE_FREE ||
                !IsMenu(native_root) || !IsMenu(native_submenu) ||
                !DestroyMenu(native_root)) goto done;
        native_root = native_submenu = NULL;
        fprintf(stderr, "WOW_MENU_OBSERVATION_CCPU unowned=1 cleanup_skips=1 native_survives=1 released=1\n");
        menu_process.classes = NULL;
    }
    stage = "live-window-geometry";
    geometry_window = CreateWindowExA(0, "STATIC", "S2 geometry fixture",
        WS_OVERLAPPEDWINDOW, 120, 140, 280, 190, NULL, NULL,
        GetModuleHandle(NULL), NULL);
    if (!geometry_window || !mvdm_softpc_wow_page_domain_publish_handle(
            LOWORD(geometry_window), HIWORD(geometry_window), 1u, 0u,
            (ULONG)(ULONG_PTR)geometry_window, 0u, NULL)) goto done;
    stage = "child-control-id-publication";
    id_window = CreateWindowExA(0, "STATIC", "S2 control ID fixture",
        WS_CHILD, 0, 0, 20, 20, geometry_window, (HMENU)(ULONG_PTR)0x8123u,
        GetModuleHandle(NULL), NULL);
    if (!id_window || !mvdm_softpc_wow_page_domain_publish_handle(
            LOWORD(id_window), HIWORD(id_window), 1u, 0u,
            (ULONG)(ULONG_PTR)id_window, 0u, NULL)) goto done;
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
        !mvdm_softpc_wow_page_domain_release_retained_window(retained_backing)) goto done;
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
#ifdef WOW_GRAPH_SNAPSHOT_FIXTURE
    stage = "whole-graph-snapshot";
    verify_graph_snapshot_commit(teb);
    verify_native_slot_reuse();
    verify_native_discovery(FALSE);
    verify_native_discovery(TRUE);
    if (borrow_errors) goto done;
#endif
#endif
#ifdef WOW_NATIVE_CREATION_FIXTURE
    {
        extern int __cdecl wow_native_creation_fixture(void);
        stage = "native-creation-owner";
        if (wow_native_creation_fixture()) goto done;
    }
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

#ifdef WOW_WINDOW_BORROW_FIXTURE
static DWORD WINAPI run_graph_desktop(void *desktop)
{
    if (!SetThreadDesktop((HDESK)desktop)) return 2;
    return (DWORD)run_fixture();
}
#endif

int main(void)
{
#ifdef WOW_WINDOW_BORROW_FIXTURE
    WCHAR name[64];
    HDESK desktop;
    HANDLE thread;
    DWORD result = 3;
    wsprintfW(name, L"T447Graph-%lu", GetCurrentProcessId());
    desktop = CreateDesktopW(name, NULL, NULL, 0, GENERIC_ALL, NULL);
    if (!desktop) return 2;
    thread = CreateThread(NULL, 0, run_graph_desktop, desktop, 0, NULL);
    if (thread) {
        WaitForSingleObject(thread, INFINITE); /* Outer harness owns deadline. */
        GetExitCodeThread(thread, &result);
        CloseHandle(thread);
    }
    if (!CloseDesktop(desktop)) result = 4;
    return (int)result;
#else
    return run_fixture();
#endif
}
