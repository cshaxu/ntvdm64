#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>

#ifdef WOW_WINDOW_BORROW_FIXTURE
/* Original header assumes its /Gz compilation environment. This CCPU
 * fixture uses /Gd; spell the existing provider ABI on that declaration. */
#define UserRegisterWowHandlers WINAPI UserRegisterWowHandlers
#include "wow32-dll/include/wow_shared_user_abi.h"
#undef UserRegisterWowHandlers
#endif

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
#ifdef WOW_WINDOW_BORROW_FIXTURE
#include "wow32-dll/include/wow_title_policy.h"
#include "common/protocol/window_style.h"
#endif

extern BOOL WINAPI WOWCallback16Ex(DWORD, DWORD, DWORD, PVOID, PDWORD);

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
extern void c_sas_storedw(uint32_t address, uint32_t value);
extern volatile LONG mvdm_softpc_wow_source_descriptor_trace_active;
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
#include "wow32-dll/include/wow_window_procedure_entry.h"
#include "wow32-dll/include/wow_task_order_bindings.h"
#include "wow32-dll/include/wow_class_client_bindings.h"
#include "wow32-dll/include/wow_standard_class_bindings.h"
#include "wow32-dll/include/wow_user_borrow_scope.h"
#include "wow32-dll/include/wow_window_client_state_bindings.h"
#include "wow32-dll/include/wow_edit_entry_bindings.h"
#include "wow32-dll/include/wow_dialog_state_bindings.h"
#include "wow32-dll/include/wow_edit_lookaside_bindings.h"
#define WOW_ORIGINAL_COMBO_STATE_LAYOUT
#define WOW_ORIGINAL_CONTROL_EXTENT_LAYOUT
#include "opennt-host/windows/core/ntuser/client/usercli.h"
#undef WOW_ORIGINAL_CONTROL_EXTENT_LAYOUT
#undef WOW_ORIGINAL_COMBO_STATE_LAYOUT
#include "wow32-dll/include/wow_user_registered_callbacks.h"
extern HANDLE WINAPI W32LocalAlloc(UINT, UINT, HANDLE);
extern HANDLE WINAPI W32LocalReAlloc(HANDLE, UINT, UINT, HANDLE, PVOID *);
extern LPSTR WINAPI W32LocalLock(HANDLE, HANDLE);
extern BOOL WINAPI W32LocalUnlock(HANDLE, HANDLE);
extern DWORD WINAPI W32LocalSize(HANDLE, HANDLE);
extern HANDLE WINAPI W32LocalFree(HANDLE, HANDLE);
extern ULONG WINAPI W32GetExpWinVer(HANDLE);
extern DWORD WINAPI W32InitDlg(HWND, LONG);
extern WORD WINAPI W32GlobalAlloc16(UINT, DWORD);
extern VOID WINAPI W32GlobalFree16(WORD);
extern VOID WINAPI W32EmptyClipboard(void);
extern int WINAPI W32EditNextWord(LPSTR, int, int, int, DWORD);
extern PWW WINAPI FindPWW(HANDLE, INT);
extern INT WINAPI GetStdClassNumber(LPSTR);
static unsigned borrow_errors;
static ULONG callback_backing, callback_teb;
static unsigned callback_zero_count;
static const char *native_failure_signal;
static const char *callback_failure_kind;
static const char *class_failure_signal;
static const char *callback_cache_signal;
static BOOL dialog_cache_failure;
#define BORROW_CHECK(value) do { if (!(value)) { ++borrow_errors; \
    fprintf(stderr, "WINDOW_BORROW_FAIL line=%d error=%lu\n", __LINE__, GetLastError()); } } while (0)

static DWORD WINAPI destroy_in_callback(HWND window, UINT message, UINT wp,
    LONG lp, DWORD procedure, PVOID words)
{
    BORROW_CHECK(procedure == 0x81234567u && words != NULL);
    if (message == WM_APP + 70) {
        if (callback_cache_signal && !dialog_cache_failure) {
            HANDLE signal = OpenEventA(EVENT_MODIFY_STATE, FALSE, callback_cache_signal);
            if (!signal) ExitProcess(7);
            SetEvent(signal); CloseHandle(signal);
            /* Break only the domain identity checked by the actual WND
             * finally; do not access guest memory again in this child. */
            c_setCR3(c_getCR3() ^ 0x1000u);
        }
        ++callback_zero_count;
        SetLastError(0x55bbu);
        return 0;
    }
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
    if (callback_cache_signal && dialog_cache_failure) {
        HANDLE signal = OpenEventA(EVENT_MODIFY_STATE, FALSE, callback_cache_signal);
        if (!signal) ExitProcess(7);
        SetEvent(signal); CloseHandle(signal);
        c_setCR3(c_getCR3() ^ 0x1000u);
        return FALSE; /* Production dialog finally must reject this return. */
    }
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

static BOOL mutation_callout_expected;
static unsigned mutation_erase_calls, mutation_scroll_calls;
static LRESULT CALLBACK mutation_window_proc(HWND window, UINT message,
    WPARAM wp, LPARAM lp)
{
    if (mutation_callout_expected &&
            (message == WM_ERASEBKGND || message == SBM_SETSCROLLINFO)) {
        wow_user_runtime_thread *runtime = wow_user_runtime_current();
        BORROW_CHECK(runtime && !runtime->exclusive_held);
        if (message == SBM_SETSCROLLINFO) {
            const SCROLLINFO *info = (const SCROLLINFO *)lp;
            BORROW_CHECK(info && info->cbSize == sizeof(*info) && info->nPos == 17);
            ++mutation_scroll_calls;
            SetLastError(0x7448);
            return 77;
        }
        ++mutation_erase_calls;
    }
    return DefWindowProcA(window, message, wp, lp);
}

static void verify_native_mutation_group(ULONG delta)
{
    WNDCLASSA wc = {0};
    HWND parent = NULL, child = NULL, peer = NULL, scrollbar = NULL, popup = NULL;
    ULONG child_class, peer_class;
    DWORD previous_style;
    RECT before, after;
    unsigned errors_before = borrow_errors;
    wc.lpfnWndProc = mutation_window_proc;
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
        {
            SCROLLINFO info = {sizeof(info), SIF_RANGE | SIF_PAGE | SIF_POS,
                0, 100, 10, 17, 0};
            SCROLLINFO observed = {sizeof(observed), SIF_RANGE | SIF_PAGE | SIF_POS};
            PAINTSTRUCT paint = {0};
            HDC dc;
            POINT caret;
            wow_user_runtime_thread *runtime = wow_user_runtime_current();
            BOOL held = runtime->exclusive_held;
            unsigned erase_before = mutation_erase_calls;
            unsigned scroll_before = mutation_scroll_calls;
            mutation_callout_expected = TRUE;
            BORROW_CHECK(wow_native_SetScrollInfo(child, SB_CTL, &info, FALSE) == 77);
            BORROW_CHECK(GetLastError() == 0x7448);
            BORROW_CHECK(mutation_scroll_calls == scroll_before + 1);
            BORROW_CHECK(wow_native_SetScrollInfo(scrollbar, SB_CTL, &info, FALSE) == 17);
            BORROW_CHECK(GetScrollInfo(scrollbar, SB_CTL, &observed) &&
                observed.nMin == 0 && observed.nMax == 100 &&
                observed.nPage == 10 && observed.nPos == 17);
            BORROW_CHECK(wow_native_InvalidateRect(child, NULL, TRUE));
            dc = wow_native_BeginPaint(child, &paint);
            BORROW_CHECK(dc != NULL && mutation_erase_calls > erase_before);
            if (dc) BORROW_CHECK(wow_native_EndPaint(child, &paint));
            mutation_callout_expected = FALSE;
            BORROW_CHECK(!GetUpdateRect(child, NULL, FALSE));
            BORROW_CHECK(wow_native_CreateCaret(child, NULL, 1, 8));
            BORROW_CHECK(wow_native_SetCaretPos(2, 3));
            BORROW_CHECK(GetCaretPos(&caret) && caret.x == 2 && caret.y == 3);
            BORROW_CHECK(wow_native_ShowCaret(child));
            BORROW_CHECK(wow_native_HideCaret(child));
            BORROW_CHECK(wow_native_DestroyCaret());
            {
                HCURSOR cursor = GetCursor();
                BORROW_CHECK(wow_native_SetCursor(cursor) == cursor && GetCursor() == cursor);
            }
            SetLastError(ERROR_SUCCESS);
            BORROW_CHECK(wow_native_SetScrollInfo(NULL, SB_CTL, &info, FALSE) == 0 &&
                GetLastError() == ERROR_INVALID_WINDOW_HANDLE);
            BORROW_CHECK(runtime->exclusive_held == held);
            (void)verify_mutated_window(child, delta);
            (void)verify_mutated_window(scrollbar, delta);
            fprintf(stderr, "WOW_CONTROL_NATIVE_RETURN_CCPU errors=%u paint=1 scroll=1 caret=1 lock=1\n",
                borrow_errors - errors_before);
        }
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

#include <dde.h>
static void verify_message_window_operands(void)
{
    HWND roots[4], first = (HWND)0x12340112u, second = (HWND)0x56780224u;
    WINDOWPOS position = {0};
    NCCALCSIZE_PARAMS bounds = {0};
    DRAWITEMSTRUCT draw = {0};
    DELETEITEMSTRUCT deleted = {0};
    COMPAREITEMSTRUCT compare = {0};
    MSG nested = {0};
    DROPSTRUCT drop = {0};
    MDINEXTMENU next = {0};
    unsigned before = borrow_errors;
    ULONG count;
    count = wow_user_message_window_roots(WM_SETFOCUS, (WPARAM)first, 0, roots);
    BORROW_CHECK(count == 1 && roots[0] == first);
    count = wow_user_message_window_roots(WM_MDIACTIVATE, (WPARAM)first, (LPARAM)second, roots);
    BORROW_CHECK(count == 2 && roots[0] == first && roots[1] == second);
    BORROW_CHECK(!wow_user_message_window_roots(WM_USER + 3, (WPARAM)first, (LPARAM)second, roots));
    BORROW_CHECK(!wow_user_message_window_roots(WM_PARENTNOTIFY, WM_LBUTTONDOWN, (LPARAM)first, roots));
    BORROW_CHECK(!wow_user_message_window_roots(WM_NEXTDLGCTL, (WPARAM)first, 0, roots));
    BORROW_CHECK(wow_user_message_window_roots(WM_PARENTNOTIFY, WM_CREATE, (LPARAM)first, roots) == 1 && roots[0] == first);
    draw.CtlType = deleted.CtlType = compare.CtlType = ODT_MENU;
    draw.hwndItem = deleted.hwndItem = compare.hwndItem = first;
    BORROW_CHECK(!wow_user_message_window_roots(WM_DRAWITEM, 0, (LPARAM)&draw, roots));
    BORROW_CHECK(!wow_user_message_window_roots(WM_DELETEITEM, 0, (LPARAM)&deleted, roots));
    BORROW_CHECK(!wow_user_message_window_roots(WM_COMPAREITEM, 0, (LPARAM)&compare, roots));
    draw.CtlType = deleted.CtlType = compare.CtlType = ODT_LISTBOX;
    BORROW_CHECK(wow_user_message_window_roots(WM_DRAWITEM, 0, (LPARAM)&draw, roots) == 1 && roots[0] == first);
    BORROW_CHECK(wow_user_message_window_roots(WM_DELETEITEM, 0, (LPARAM)&deleted, roots) == 1 && roots[0] == first);
    BORROW_CHECK(wow_user_message_window_roots(WM_COMPAREITEM, 0, (LPARAM)&compare, roots) == 1 && roots[0] == first);
    position.hwnd = first; position.hwndInsertAfter = HWND_TOPMOST;
    BORROW_CHECK(wow_user_message_window_roots(WM_WINDOWPOSCHANGING, 0, (LPARAM)&position, roots) == 1);
    position.hwndInsertAfter = second;
    BORROW_CHECK(wow_user_message_window_roots(WM_WINDOWPOSCHANGED, 0, (LPARAM)&position, roots) == 2 && roots[1] == second);
    position.flags = SWP_NOZORDER;
    BORROW_CHECK(wow_user_message_window_roots(WM_WINDOWPOSCHANGING, 0, (LPARAM)&position, roots) == 2);
    bounds.lppos = &position;
    BORROW_CHECK(!wow_user_message_window_roots(WM_NCCALCSIZE, 0, (LPARAM)&bounds, roots));
    BORROW_CHECK(wow_user_message_window_roots(WM_NCCALCSIZE, 1, (LPARAM)&bounds, roots) == 2);
    nested.hwnd = first; nested.message = WM_COMMAND; nested.lParam = (LPARAM)second;
    BORROW_CHECK(wow_user_message_window_roots(WM_GETDLGCODE, 0, (LPARAM)&nested, roots) == 2 && roots[1] == second);
    nested.message = WM_GETDLGCODE; nested.lParam = 1; /* Never recursively dereference. */
    BORROW_CHECK(wow_user_message_window_roots(WM_GETDLGCODE, 0, (LPARAM)&nested, roots) == 1);
    drop.hwndSource = first; drop.hwndSink = second;
    BORROW_CHECK(wow_user_message_window_roots(0x022a, 0, (LPARAM)&drop, roots) == 2 && roots[1] == second);
    BORROW_CHECK(wow_user_message_window_roots(0x0038, (WPARAM)first, 0, roots) == 1 && roots[0] == first);
    BORROW_CHECK(wow_user_queue_window_roots(&nested, roots) == 1); /* No lp=1 dereference. */
    nested.message = WM_DDE_ACK; nested.wParam = (WPARAM)second;
    BORROW_CHECK(wow_user_queue_window_roots(&nested, roots) == 2 && roots[1] == second);
    next.hwndNext = second;
    BORROW_CHECK(wow_user_message_return_roots(WM_NEXTMENU, 0, (LPARAM)&next, 0, roots) == 1 && roots[0] == second);
    BORROW_CHECK(wow_user_message_return_roots(WM_MDIGETACTIVE, 0, 0, (LRESULT)first, roots) == 1 && roots[0] == first);
    BORROW_CHECK(!wow_user_message_return_roots(WM_USER, 0, 1, (LRESULT)first, roots));
    fprintf(stderr, "WOW_MESSAGE_OPERANDS_CCPU errors=%u typed=1 numeric=1 menu=1 position=1 nested=1\n", borrow_errors - before);
}

static HWND returned_message_window;
static LRESULT CALLBACK returned_message_proc(HWND window, UINT message, WPARAM wp, LPARAM lp)
{
    if (message == WM_MDIGETACTIVE) return (LRESULT)returned_message_window;
    return DefWindowProcA(window, message, wp, lp);
}

static void verify_returned_windows(void)
{
    HWND first, second, child, old, dead, message_window;
    HWND payload[4];
    WNDCLASSA message_class = {0};
    DWORD_PTR message_result = 0;
    ULONG backing;
    unsigned before = borrow_errors;
    first = CreateWindowA("STATIC", "Return owner", WS_POPUP,
        0, 0, 40, 40, NULL, NULL, GetModuleHandle(NULL), NULL);
    second = CreateWindowA("STATIC", "Return owned", WS_POPUP,
        0, 0, 40, 40, first, NULL, GetModuleHandle(NULL), NULL);
    child = CreateWindowA("STATIC", "Return child", WS_CHILD,
        0, 0, 10, 10, first, (HMENU)189, GetModuleHandle(NULL), NULL);
    BORROW_CHECK(first && second && child);
    if (first && second && child) {
        BORROW_CHECK((HWND)wow_user_get_window_longA(second, GWL_HWNDPARENT) == first);
        BORROW_CHECK((HWND)wow_user_get_window_longA(child, GWL_HWNDPARENT) == first);
        BORROW_CHECK(wow_user_get_window_longA(child, GWL_ID) == 189);
        BORROW_CHECK(wow_native_SetParent(child, second) == first);
        BORROW_CHECK((HWND)wow_user_get_window_longA(child, GWL_HWNDPARENT) == second);
        BORROW_CHECK((HWND)wow_user_set_window_longA(second, GWL_HWNDPARENT, 0) == first);
        BORROW_CHECK(!wow_user_get_window_longA(second, GWL_HWNDPARENT));
        BORROW_CHECK(wow_native_ReleaseCapture());
        BORROW_CHECK(!wow_native_SetCapture(first));
        BORROW_CHECK(GetCapture() == first);
        old = wow_native_SetCapture(second);
        BORROW_CHECK(old == first && GetCapture() == second);
        backing = mvdm_softpc_wow_page_domain_borrow_window((ULONG)(ULONG_PTR)old);
        BORROW_CHECK(backing != 0);
        if (backing) BORROW_CHECK(mvdm_softpc_wow_page_domain_release_window(backing));
        BORROW_CHECK(wow_native_ReleaseCapture() && !GetCapture());
        message_class.lpfnWndProc = returned_message_proc;
        message_class.hInstance = GetModuleHandle(NULL);
        message_class.lpszClassName = "T447ReturnedMessage";
        BORROW_CHECK(RegisterClassA(&message_class));
        message_window = CreateWindowA(message_class.lpszClassName, "", WS_POPUP,
            0, 0, 10, 10, NULL, NULL, message_class.hInstance, NULL);
        BORROW_CHECK(message_window != NULL);
        if (message_window) {
            returned_message_window = second;
            BORROW_CHECK((HWND)wow_native_SendMessageA(message_window, WM_MDIGETACTIVE, 0, 0) == second);
            BORROW_CHECK(wow_native_SendMessageTimeoutA(message_window, WM_MDIGETACTIVE,
                0, 0, SMTO_ABORTIFHUNG, 1000, &message_result) && (HWND)message_result == second);
            BORROW_CHECK(DestroyWindow(message_window));
        }
        BORROW_CHECK(UnregisterClassA(message_class.lpszClassName, message_class.hInstance));
        dead = first;
        BORROW_CHECK(wow_native_DestroyWindow(first)); first = NULL;
        BORROW_CHECK(wow_user_window_refresh_return(dead));
        BORROW_CHECK(!mvdm_softpc_wow_page_domain_borrow_window((ULONG)(ULONG_PTR)dead));
        payload[0] = dead; payload[1] = second;
        payload[2] = (HWND)(LONG)(SHORT)LOWORD(child); payload[3] = NULL;
        BORROW_CHECK(wow_user_window_refresh_payload(NULL, payload, 4));
        backing = mvdm_softpc_wow_page_domain_borrow_window((ULONG)(ULONG_PTR)child);
        BORROW_CHECK(backing != 0);
        if (backing) BORROW_CHECK(mvdm_softpc_wow_page_domain_release_window(backing));
        BORROW_CHECK(!wow_user_window_refresh_payload(NULL, NULL, 1));
    }
    if (child) BORROW_CHECK(wow_native_DestroyWindow(child));
    if (second) BORROW_CHECK(wow_native_DestroyWindow(second));
    if (first) BORROW_CHECK(wow_native_DestroyWindow(first));
    fprintf(stderr, "WOW_RETURNED_WINDOWS_CCPU errors=%u parent=1 owner=1 numeric=1 previous=1 capture=1 dead=1\n",
        borrow_errors - before);
}

/* Original dialog creator requires the same initialized task/class/resource
 * context as controls. Keep this assertion in that fixture scope, not in the
 * generic callback fixture which intentionally has only an object table. */
static void verify_dialog_cache(ULONG teb, wow_user_object_table *objects)
{
    DWORD template_words[16] = {0};
    DLGTEMPLATE *dialog = (DLGTEMPLATE *)template_words;
    HWND window;
    ULONG before, after, largest;
    unsigned errors_before = borrow_errors;
    callback_teb = teb;
    dialog->style = WS_POPUP;
    dialog->cx = 100; dialog->cy = 40;
    BORROW_CHECK(VdmQueryFreeVirtualMemory(&before, &largest) >= 0);
    window = wow_user_server_create_dialog(GetModuleHandle(NULL), dialog,
        sizeof(template_words), NULL, dialog_cache_callback, 0,
        SCDLG_CLIENT | SCDLG_ANSI | SCDLG_NOREVALIDATE);
    BORROW_CHECK(window != NULL);
    if (window) {
        SendMessageA(window, WM_APP + 99, 0, 0);
        BORROW_CHECK(!IsWindow(window));
        BORROW_CHECK(objects->entries[LOWORD(window)].bType == TYPE_FREE);
        if (IsWindow(window)) DestroyWindow(window);
    }
    BORROW_CHECK(c_sas_dw_at(teb + 0x6cu) == 0u &&
        c_sas_dw_at(teb + 0x70u) == 0u);
    BORROW_CHECK(VdmQueryFreeVirtualMemory(&after, &largest) >= 0 && after == before);
    fprintf(stderr, "WOW_DIALOG_CACHE_CCPU errors=%u nested_destroy=1 released=1\n",
        borrow_errors - errors_before);
}

static int verify_callback_scope(ULONG teb)
{
    static wow_user_object_table objects;
    wow_task_order_process process = {0};
    wow_task_order_thread thread = {0};
    wow_task_order_entry task = {0};
    wow_task_order_state task_state = {0};
    wow_user_runtime runtime = WOW_USER_RUNTIME_INITIALIZER;
    wow_user_task_lifecycle native_owner = {0};
    wow_user_runtime_thread runtime_binding = {0};
    wow_window_dispatch_target target = { 0x81234567u, destroy_in_callback, TRUE };
    WNDPROC entry;
    HHOOK terminal;
    unsigned mode;
    process.objects = &objects;
    thread.ppi = &process;
    process.pwpi = &task_state;
    task_state.CSOwningThread = &thread;
    thread.ptdb = &task;
    task.hTaskWow = 0x4321u;
    runtime.lifecycle = &native_owner;
    if (!wow_user_runtime_bind(&runtime_binding, &runtime, &thread, NULL)) return 0;
    terminal = SetWindowsHookExA(WH_CALLWNDPROCRET,
        wow_window_terminal_observer,NULL,GetCurrentThreadId());
    entry = wow_window_entry_for_guest(&target);
    BORROW_CHECK(terminal && entry);
    if (!terminal || !entry) {
        if (terminal) BORROW_CHECK(UnhookWindowsHookEx(terminal));
        BORROW_CHECK(wow_user_runtime_unbind(&runtime_binding));
        return 0;
    }
    BORROW_CHECK(wow_user_window_refresh_graph(NULL));
    callback_teb = teb;
    verify_native_mutation_group(c_sas_dw_at(teb + 0x60u));
    verify_returned_windows();
    verify_message_window_operands();
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
            BORROW_CHECK(UnhookWindowsHookEx(terminal));
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
        BORROW_CHECK(SetWindowLongA(window, GWL_WNDPROC, (LONG)entry) != 0);
        if (mode == 0) {
            /* Force only the graph owner lookup to fail. A zero native result
             * must not be evidence that the original callback ran. */
            callback_zero_count = 0;
            process.objects = NULL;
            SetLastError(0x1234);
            BORROW_CHECK(CallWindowProcA(entry,window, WM_APP + 70, 0, 0) == 0);
            BORROW_CHECK(GetLastError() == ERROR_INVALID_STATE);
            BORROW_CHECK(callback_zero_count == 0 && IsWindow(window));
            process.objects = &objects;
            BORROW_CHECK(CallWindowProcA(entry,window, WM_APP + 70, 0, 0) == 0);
            BORROW_CHECK(GetLastError() == 0x55bbu);
            BORROW_CHECK(callback_zero_count == 1 && IsWindow(window));
            BORROW_CHECK(CallWindowProcA(entry,(HWND)(ULONG_PTR)LOWORD(window),
                WM_APP+70,0,0)==0 && GetLastError()==0x55bbu);
            BORROW_CHECK(CallWindowProcA(entry,(HWND)(LONG_PTR)(SHORT)LOWORD(window),
                WM_APP+70,0,0)==0 && GetLastError()==0x55bbu);
            BORROW_CHECK(callback_zero_count==3 && IsWindow(window));
            fprintf(stderr,"WOW_IMMUTABLE_CARRIERS callbacks=%u full=1 zero16=1 signed16=1 live=%d\n",
                callback_zero_count,IsWindow(window));
        }
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
    BORROW_CHECK(UnhookWindowsHookEx(terminal));
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
    if (class_failure_signal) {
        /* Test-owned stale publication after native destruction. Do not
         * mutate an original guest or insert a production failure hook. */
        WNDCLASSEXA native = {sizeof(native)};
        WCHAR class_name[] = L"T447_CLASS_RELEASE_FAILURE";
        UNICODE_STRING name = {sizeof(class_name) - sizeof(WCHAR),
            sizeof(class_name), class_name};
        wow_class_words_binding *storage;
        wow_class_lookup_entry *entry;
        HWND window;
        HANDLE signal;
        ATOM atom;
        native.hInstance = GetModuleHandleA(NULL);
        native.lpfnWndProc = DefWindowProcA;
        native.lpszClassName = "T447_CLASS_RELEASE_FAILURE";
        atom = wow_class_words_register(&classes, &native, &name, words, NULL);
        if (!atom) return FALSE;
        storage = wow_class_words_acquire(&classes, atom, native.hInstance);
        entry = wow_class_words_entry(storage);
        if (!entry || !wow_class_words_publish_guest(entry, &native)) return FALSE;
        window = CreateWindowExA(0, MAKEINTATOM(atom), "", WS_POPUP,
            0, 0, 16, 16, NULL, NULL, native.hInstance, NULL);
        if (!window || !mvdm_softpc_wow_page_domain_publish_handle(LOWORD(window),
                HIWORD(window), 1, 0, (ULONG)(ULONG_PTR)window,
                entry->guest_server, NULL) || !DestroyWindow(window)) return FALSE;
        wow_class_words_release(storage);
        signal = OpenEventA(EVENT_MODIFY_STATE, FALSE, class_failure_signal);
        if (!signal) return FALSE;
        SetEvent(signal); CloseHandle(signal);
        (void)wow_class_words_destroy_native(&classes.pclsPrivateList,
            native.hInstance, TRUE);
        return FALSE; /* Native success plus failed release cannot return. */
    }
    for (mode = 0; mode < 2; ++mode) {
        wow_class_words_binding *binding;
        wow_class_lookup_entry *entry;
        ULONG free_before = 0, free_after = 0, largest = 0;
        ATOM atom;
        thread.TIF_flags = mode ? 0 : WOW_TASK_TIF_16BIT;
        wc.style = mode ? CS_GLOBALCLASS : 0;
        BORROW_CHECK(VdmQueryFreeVirtualMemory(&free_before, &largest) >= 0);
        atom = wow_class_client_register_guest(&context, &wc, words);
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
static DWORD WINAPI combo_query_wrong_thread(void *parameter)
{
    COMBOBOXINFO info={sizeof(info)};
    return !wow_control_get_combo_info((HWND)parameter,&info) &&
        GetLastError()==ERROR_INVALID_THREAD_ID ? 0 : 1;
}
static HWND vetoed_native;
static ATOM veto_native_atom;
static BOOL selected_native_control(HWND window)
{
    DWORD saved=GetLastError();
    wow_window_words_binding *binding=wow_window_words_acquire(window);
    wow_cleanup_window *view=binding?wow_window_words_cleanup_value(binding):NULL;
    PWND state=binding?wow_window_words_client_state_value(binding):NULL;
    WNDPROC actual=(WNDPROC)GetWindowLongA(window,GWL_WNDPROC);
    WNDPROC narrow=state?wow_control_client_procedure(GETFNID(state),FALSE):NULL;
    WNDPROC wide=state?wow_control_client_procedure(GETFNID(state),TRUE):NULL;
    BOOL matches=view && !view->procedure_is_guest && actual &&
        actual==view->lpfnWndProc && (!(narrow || wide) || actual==narrow || actual==wide);
    wow_window_words_release(binding);
    SetLastError(saved);
    return matches;
}
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
        BORROW_CHECK(selected_native_control((HWND)wp));
        return 1;
    }
    return CallNextHookEx(NULL, code, wp, lp);
}
static BOOL CALLBACK trace_reuse_owner(HWND window,LPARAM original)
{
    if(LOWORD((ULONG_PTR)window)==LOWORD(original)){
        char name[80]={0};GetClassNameA(window,name,sizeof(name));
        fprintf(stderr,"WOW_REUSE_SLOT_OWNER old=%08lx current=%p class=%s thread=%lu\n",
            (ULONG)original,window,name,GetWindowThreadProcessId(window,NULL));
    }
    return TRUE;
}
static BOOL CALLBACK trace_reuse_tree(HWND window,LPARAM original)
{trace_reuse_owner(window,original);EnumChildWindows(window,trace_reuse_owner,original);return TRUE;}

/* Native USER need not reuse one chosen dead slot. Re-arm the full observed
 * old generation after a miss; a pass still requires actual same-slot reuse
 * and every retained-generation assertion below, within the original bound. */
static BOOL rearm_observed_reuse_source(ULONG *full, ULONG *held, ULONG tag)
{
    HWND desktop = GetDesktopWindow(), old = NULL;
    ULONG previous;
    BOOL result = FALSE;
    if (*held) {
        if (!wow_user_window_project_graph(&desktop, 1) ||
                !mvdm_softpc_wow_page_domain_release_window(*held)) return FALSE;
        *held = 0;
        if (!mvdm_softpc_wow_page_domain_collect_observed_classes()) return FALSE;
    }
    old = CreateWindowExA(0, "STATIC", "Rearmed observed reuse source", WS_POPUP,
        0, 0, 24, 24, NULL, NULL, GetModuleHandle(NULL), NULL);
    if (!old) return FALSE;
    *full = (ULONG)(ULONG_PTR)old;
    if (!wow_user_window_project_graph(&old, 1)) goto done;
    *held = mvdm_softpc_wow_page_domain_borrow_window(*full);
    if (!*held || !mvdm_softpc_wow_page_domain_exchange_window_word(
            *held, 0, tag, &previous)) goto done;
    result = TRUE;
done:
    if (!DestroyWindow(old)) result = FALSE;
    return result;
}
static void verify_managed_slot_reuse(wow_task_order_thread *thread)
{
    wow_user_object_table *objects = thread->ppi->objects;
    wow_standard_class_view descriptor;
    CREATESTRUCTA request = {0};
    WW words = {0};
    unsigned mode, before = borrow_errors;
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
                wow_window_create_boundA(thread, descriptor.atom, &request, &words, NULL);
            if (!window) break;
            full = (ULONG)(ULONG_PTR)window;
            if(attempts==1 && LOWORD(full)!=LOWORD(old_full))
                EnumDesktopWindows(GetThreadDesktop(GetCurrentThreadId()),trace_reuse_tree,old_full);
            if (LOWORD(full) == LOWORD(old_full) && full != old_full) break;
            BORROW_CHECK(DestroyWindow(window)); window = NULL;
            if (!rearm_observed_reuse_source(&old_full, &held, 0xabcdef12u)) {
                BORROW_CHECK(FALSE); break;
            }
        }
        BORROW_CHECK(window && LOWORD(full) == LOWORD(old_full) && full != old_full);
        if (window && LOWORD(full) == LOWORD(old_full) && full != old_full) {
            /* Dialog roots reach this same producer with no CLS. This mode
             * tests that storage contract, not a guest dialog implementation. */
            if (mode) BORROW_CHECK(mvdm_softpc_wow_page_domain_publish_handle(
                LOWORD(full), HIWORD(full), 1, 0, full, 0, words.adwWOW));
            else BORROW_CHECK(objects->windows[LOWORD(full)] == window &&
                objects->entries[LOWORD(full)].bType == TYPE_WINDOW);
            fresh = mvdm_softpc_wow_page_domain_borrow_window(full);
            BORROW_CHECK(fresh && fresh != held);
            BORROW_CHECK(wow_user_full_handle(objects, LOWORD(full)) == full);
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

static unsigned terminal_dialog_callbacks;
static unsigned modal_dialog_calls;
static BOOL modal_style_expected;
static wow_window_words_binding *modal_retained;
static unsigned modal_handle_form;
static INT_PTR CALLBACK original_modal_witness(HWND window,UINT message,
    WPARAM wp,LPARAM lp)
{
    (void)wp;(void)lp;
    if(message==WM_INITDIALOG){
        PWND state;
        fprintf(stderr,"WOW_MODAL_STAGE init hwnd=%p\n",window);
        ++modal_dialog_calls;
        modal_retained=wow_window_words_acquire(window);
        state=wow_window_words_client_state_value(modal_retained);
        BORROW_CHECK(state && GETFNID(state)==FNID_DIALOG &&
            TestWF(state,WFDIALOGWINDOW) && ((PDIALOG)state)->pdlg);
        BORROW_CHECK(SetTimer(window,1,10,NULL)!=0);
        return TRUE;
    }
    if(message==WM_TIMER){
        fprintf(stderr,"WOW_MODAL_STAGE timer hwnd=%p\n",window);
        HMODULE module=GetModuleHandleW(L"ntw16s32.dll");
        BOOL (__cdecl *attached)(HWND)=module?
            (BOOL (__cdecl *)(HWND))(void *)GetProcAddress(module,"Ntw16sWindowAttached"):NULL;
        BORROW_CHECK((attached && attached(window))==modal_style_expected);
        BORROW_CHECK(KillTimer(window,1));
        HWND carrier=modal_handle_form==1?(HWND)(ULONG_PTR)LOWORD(window):
            modal_handle_form==2?(HWND)(LONG_PTR)(SHORT)LOWORD(window):window;
        BORROW_CHECK(wow_user_end_dialog(carrier,49));
        return TRUE;
    }
    return FALSE;
}
static INT_PTR CALLBACK terminal_dialog_witness(HWND window, UINT message,
    WPARAM wp, LPARAM lp)
{
    UNREFERENCED_PARAMETER(window);
    UNREFERENCED_PARAMETER(wp);
    UNREFERENCED_PARAMETER(lp);
    if (message == WM_DESTROY || message == WM_NCDESTROY)
        ++terminal_dialog_callbacks;
    return FALSE;
}

static VOID WINAPI modal_task_ended(void) { BORROW_CHECK(FALSE); }
#ifdef WOW_REAL_DIALOG_LIFECYCLE_FIXTURE
#include "wow_dialog_real_lifecycle.h"
#endif
static void verify_original_modal_task(void)
{
    wow_user_runtime runtime=WOW_USER_RUNTIME_INITIALIZER;
    wow_user_runtime_thread binding={0};
    wow_user_task_lifecycle *lifecycle;
    PFNWOWHANDLERSIN inputs=*wow_user_registered_callbacks();
    PFNWOWHANDLERSOUT outputs={0};
    DWORD resource[32]={0};DLGTEMPLATE *dialog=(DLGTEMPLATE *)resource;
    HANDLE wake=CreateEventW(NULL,FALSE,FALSE,NULL);
    unsigned before=borrow_errors,mode,encoding,carrier,modes=1;
    /* Modal Peek/Wait require InitTask's actual host_context/owner, not just
     * the controlled task words used by the native creation-only matrix. */
    BORROW_CHECK(wake && wow_user_runtime_initialize(&runtime));
    fprintf(stderr,"WOW_MODAL_STAGE runtime\n");
    BORROW_CHECK(wow_user_runtime_bind(&binding,&runtime,NULL,NULL));
    /* The original modal queue calls the registered bridge. Its owner must
     * be that bridge's lifecycle, not an unrelated directly-created model. */
    inputs.pfnLocalAlloc=W32LocalAlloc;inputs.pfnLocalReAlloc=W32LocalReAlloc;
    inputs.pfnLocalLock=W32LocalLock;inputs.pfnLocalUnlock=W32LocalUnlock;
    inputs.pfnLocalSize=W32LocalSize;inputs.pfnLocalFree=W32LocalFree;
    inputs.pfnGetExpWinVer=W32GetExpWinVer;inputs.pfnInitDlgCb=W32InitDlg;
    inputs.pfn16GlobalAlloc=W32GlobalAlloc16;inputs.pfn16GlobalFree=W32GlobalFree16;
    inputs.pfnEmptyCB=W32EmptyClipboard;inputs.pfnWowEditNextWord=W32EditNextWord;
    inputs.pfnWowWndProcEx=destroy_in_callback;
    BORROW_CHECK(UserRegisterWowHandlers(&inputs,&outputs)!=0);
    BORROW_CHECK(outputs.pfnRegisterUserHungAppHandlers(modal_task_ended,wake));
    lifecycle=runtime.lifecycle;
    BORROW_CHECK(lifecycle && lifecycle->initialized);
    fprintf(stderr,"WOW_MODAL_STAGE registered\n");
    /* Launcher metadata is outside this native-only modal row. Enter the
     * actual registered owner's original InitTask without inventing a launch. */
    BORROW_CHECK(wow_user_task_lifecycle_init(lifecycle,0x0400,"MODAL.EXE",
        0x4323,0,0,0,0,200,100,SW_HIDE));
    {
        wow_task_order_thread second = {0};
        wow_task_client_info client = {0};
        ULONG teb = mvdm_softpc_wow_page_domain_guest_teb();
        unsigned before_context = borrow_errors;
        /* Controlled second client view, not a second registered task or
         * scheduler witness. Exercise the actual production host operation
         * against the CCPU projection; original scheduling is tested apart. */
        client.dwExpWinVer = 0x030au;
        second.ppi = binding.thread->ppi;
        second.pClientInfo = &client;
        BORROW_CHECK(binding.thread->pClientInfo->dwExpWinVer == 0x0400u &&
            c_sas_dw_at(teb + 0x50u) == 0x0400u);
        BORROW_CHECK(wow_user_runtime_enter(&binding));
        binding.thread->host->client_context(binding.thread, &second);
        BORROW_CHECK(c_sas_dw_at(teb + 0x50u) == 0x030au);
        binding.thread->host->client_context(&second, binding.thread);
        BORROW_CHECK(c_sas_dw_at(teb + 0x50u) == 0x0400u);
        BORROW_CHECK(wow_user_runtime_leave(&binding));
        fprintf(stderr,"WOW_CLIENT_CONTEXT_BINDING_CCPU errors=%u production_op=1 controlled_views=2 scheduling=separate\n",
            borrow_errors - before_context);
    }
    fprintf(stderr,"WOW_MODAL_STAGE admitted\n");
#ifdef WOW_REAL_DIALOG_LIFECYCLE_FIXTURE
    verify_real_dialog_lifecycle(lifecycle);
#else
#ifdef WOW_STYLED_TERMINAL_FIXTURE
    modes=3;
#endif
    dialog->style=WS_POPUP|WS_CAPTION|DS_MODALFRAME;dialog->cx=100;dialog->cy=40;
    for(mode=0;mode<modes;++mode)for(encoding=0;encoding<2;++encoding)for(carrier=0;carrier<3;++carrier){
        INT_PTR result;PWND state;unsigned calls=modal_dialog_calls;
        modal_retained=NULL;modal_style_expected=mode!=WINDOW_STYLE_OFF;
        modal_handle_form=carrier;
        fprintf(stderr,"WOW_MODAL_STAGE enter mode=%u encoding=%u\n",mode,encoding);
        result=wow_title_modal(GetModuleHandleW(NULL),dialog,NULL,
            original_modal_witness,0,encoding?0:SCDLG_ANSI,mode);
        fprintf(stderr,"WOW_MODAL_STAGE returned result=%ld error=%lu\n",(LONG)result,GetLastError());
        BORROW_CHECK(result==49 && modal_dialog_calls==calls+1 && modal_retained);
        state=wow_window_words_client_state_value(modal_retained);
        BORROW_CHECK(state && (state->fnid&FNID_CLEANEDUP_BIT) &&
            TestWF(state,WFDESTROYED) && !TestWF(state,WFDIALOGWINDOW) &&
            ((PDIALOG)state)->pdlg==NULL && !IsWindow(state->head.h));
        wow_window_words_release(modal_retained);modal_retained=NULL;
    }
#endif
    BORROW_CHECK(outputs.pfnWOWCleanup((HANDLE)0x4323,0x4323,NULL,0));
    BORROW_CHECK(wow_user_runtime_unbind(&binding));
    BORROW_CHECK(wow_user_task_lifecycle_dispose(lifecycle));
    BORROW_CHECK(CloseHandle(wake));
#ifndef WOW_REAL_DIALOG_LIFECYCLE_FIXTURE
    fprintf(stderr,"WOW_ORIGINAL_MODAL_CARRIERS errors=%u full=1 zero16=1 signed16=1 original_end=1\n",borrow_errors-before);
    fprintf(stderr,"WOW_ORIGINAL_MODAL_CCPU errors=%u modes=%u aw=2 returned=49 pdlg_retired=1\n",
        borrow_errors-before,modes);
#else
    (void)before;(void)mode;(void)encoding;(void)modes;(void)carrier;
#endif
}

static void verify_native_class_creation(wow_class_lookup_context *classes, ULONG teb)
{
    static wow_user_object_table objects;
    wow_user_task_lifecycle native_owner = {0};
    wow_user_runtime runtime = WOW_USER_RUNTIME_INITIALIZER;
    wow_user_runtime_thread binding = {0};
    wow_task_order_entry task = {0};
    wow_task_order_state task_state = {0};
    wow_task_client_info client = {0};
    wow_class_client_context class_client = {0};
    PFNWOWHANDLERSIN callbacks = *wow_user_registered_callbacks();
    PFNWOWHANDLERSIN saved_callbacks = callbacks;
    PFNWOWHANDLERSOUT outputs = {0};
    wow_task_order_thread thread = {0};
    wow_standard_class_view native_class;
    CREATESTRUCTA request = {0};
    WW words = {0};
    HWND window;
    HHOOK veto, terminal;
    char text[32] = {0};
    unsigned before = borrow_errors;
    /* Supplemental native-module row: use the real original W32 callbacks,
     * not guest task initialization or native replacement control bodies. */
    callbacks.pfnLocalAlloc = W32LocalAlloc;
    callbacks.pfnLocalReAlloc = W32LocalReAlloc;
    callbacks.pfnLocalLock = W32LocalLock;
    callbacks.pfnLocalUnlock = W32LocalUnlock;
    callbacks.pfnLocalSize = W32LocalSize;
    callbacks.pfnLocalFree = W32LocalFree;
    callbacks.pfnGetExpWinVer = W32GetExpWinVer;
    callbacks.pfnInitDlgCb = W32InitDlg;
    callbacks.pfn16GlobalAlloc = W32GlobalAlloc16;
    callbacks.pfn16GlobalFree = W32GlobalFree16;
    callbacks.pfnEmptyCB = W32EmptyClipboard;
    callbacks.pfnWowEditNextWord = W32EditNextWord;
    BORROW_CHECK(UserRegisterWowHandlers(&callbacks, &outputs) != 0);
    native_owner.process.classes = classes;
    native_owner.process.objects = &objects;
    native_owner.process.pwpi = &task_state;
    native_owner.cleanup.shared = &native_owner.cleanup_shared;
    BORROW_CHECK(wow_standard_classes_bind_cleanup(&native_owner.cleanup));
    native_owner.initialized = TRUE;
    runtime.lifecycle = &native_owner;
    task.hTaskWow = 0x4321;
    thread.ppi = &native_owner.process;
    thread.ptdb = &task;
    thread.pClientInfo = &client;
    thread.dwExpWinVer = client.dwExpWinVer = W32GetExpWinVer(GetModuleHandle(NULL));
    task_state.CSOwningThread = &thread;
    class_client.classes = classes;
    class_client.thread = &thread;
    class_client.dwExpWinVer = thread.dwExpWinVer;
    class_client.get_version = W32GetExpWinVer;
    if (!wow_user_runtime_bind(&binding, &runtime, &thread, &class_client)) {
        BORROW_CHECK(FALSE);
        BORROW_CHECK(UserRegisterWowHandlers(&saved_callbacks, &outputs) != 0);
        return;
    }
    terminal = SetWindowsHookExA(WH_CALLWNDPROCRET,
        wow_window_terminal_observer,NULL,GetCurrentThreadId());
    BORROW_CHECK(terminal != NULL);
    if (!terminal) {
        BORROW_CHECK(wow_user_runtime_unbind(&binding));
        BORROW_CHECK(UserRegisterWowHandlers(&saved_callbacks,&outputs) != 0);
        return;
    }
    verify_managed_slot_reuse(&thread);
    BORROW_CHECK(wow_standard_class_describe("BUTTON", &native_class));
    request.lpszClass = "BUTTON"; request.lpszName = "Native original";
    request.hInstance = GetModuleHandle(NULL); request.style = WS_POPUP;
    request.cx = 100; request.cy = 30;
    window = wow_window_create_boundA(&thread, native_class.atom, &request, &words, NULL);
    BORROW_CHECK(window != NULL);
    if (window) {
        BORROW_CHECK(selected_native_control(window));
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
                wow_task_callback_scope scope;
                BOOL entered = wow_task_callback_enter(&scope);
                BORROW_CHECK(entered);
                if (entered) {
                    wow_window_words_binding *root = wow_window_words_acquire(combo);
                    PCOMBOWND state = root ? (PCOMBOWND)
                        wow_window_words_client_state_value(root) : NULL;
                    BORROW_CHECK(state && state->pcbox &&
                        state->pcbox->spwndEdit && state->pcbox->spwndList);
                    if (state && state->pcbox) {
                        COMBOBOXINFO info={sizeof(info)},unchanged;
                        HANDLE other;DWORD code=1;WORD fnid=state->wnd.fnid;
                        SetLastError(0x44730074);
                        BORROW_CHECK(wow_control_get_combo_info(combo,&info) &&
                            GetLastError()==0x44730074 && info.hwndCombo==combo &&
                            EqualRect(&info.rcItem,&state->pcbox->editrc) &&
                            EqualRect(&info.rcButton,&state->pcbox->buttonrc) &&
                            info.hwndItem==state->pcbox->spwndEdit->head.h &&
                            info.hwndList==state->pcbox->spwndList->head.h);
                        unchanged=info;info.cbSize=0;
                        BORROW_CHECK(!wow_control_get_combo_info(combo,&info) &&
                            GetLastError()==ERROR_INVALID_PARAMETER && !info.cbSize);
                        info=unchanged;state->wnd.fnid|=FNID_CLEANEDUP_BIT;
                        BORROW_CHECK(!wow_control_get_combo_info(combo,&info) &&
                            GetLastError()==ERROR_INVALID_STATE &&
                            !memcmp(&info,&unchanged,sizeof(info)));
                        state->wnd.fnid=fnid;
                        other=CreateThread(NULL,0,combo_query_wrong_thread,combo,0,NULL);
                        BORROW_CHECK(other!=NULL);
                        if(other){
                            BORROW_CHECK(WaitForSingleObject(other,5000)==WAIT_OBJECT_0 &&
                                GetExitCodeThread(other,&code) && !code);
                            CloseHandle(other);
                        }
                        if (state->pcbox->spwndEdit)
                            children[0] = (HWND)state->pcbox->spwndEdit->head.h;
                        if (state->pcbox->spwndList)
                            children[1] = (HWND)state->pcbox->spwndList->head.h;
                    }
                    wow_window_words_release(root);
                    BORROW_CHECK(wow_task_callback_leave(&scope));
                }
                for (i = 0; i < ARRAYSIZE(children); ++i) {
                    child = wow_window_words_acquire(children[i]);
                    BORROW_CHECK(children[i] != NULL && child != NULL);
                    if (child) {
                        WW *view = wow_window_words_value(child);
                        BORROW_CHECK(view->adwWOW[0] == 0 &&
                            view->adwWOW[1] == 0 && view->adwWOW[2] == 0);
                        BORROW_CHECK(wow_window_words_cleanup_value(child)->thread == &thread);
                        BORROW_CHECK(wow_window_class_words_value(child) != NULL);
                        BORROW_CHECK(selected_native_control(children[i]));
                    }
                    wow_window_words_release(child);
                }
                /* A list may have been reparented by native combo creation.
                 * Its metadata is retained independently of parent membership. */
                BORROW_CHECK(SendMessageA(combo, CB_ADDSTRING, 0, (LPARAM)"item") == 0);
                BORROW_CHECK(SendMessageA(combo, CB_GETCOUNT, 0, 0) == 1);
                veto_native_atom = (ATOM)GetClassLongA(children[1], GCW_ATOM);
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
        HWND button = NULL;
        wow_window_words_binding *child;
        unsigned terminal_case;
        dialog->style = WS_POPUP; dialog->cdit = 1;
        dialog->cx = 100; dialog->cy = 40;
        item->style = WS_CHILD | BS_PUSHBUTTON;
        item->x = 5; item->y = 5; item->cx = 30; item->cy = 12; item->id = 101;
        tail[0] = 0xffff; tail[1] = 0x0080; /* Standard BUTTON ordinal. */
        tail[2] = 'O'; tail[3] = 'K'; /* NUL title and zero extra bytes follow. */
        for (terminal_case = 0; terminal_case != 2; ++terminal_case) {
#ifdef WOW_STYLED_TERMINAL_FIXTURE
        void *style_scope=NULL;
        DWORD style_gdi=0;
        BOOL (__cdecl *query_style)(HWND)=NULL;
        if (terminal_case) {
            HMODULE style;
            BORROW_CHECK(wow_style_creation_begin(WINDOW_STYLE_WIN16,&style_scope));
            style=GetModuleHandleW(L"ntw16s32.dll");
            query_style=style?(BOOL (__cdecl *)(HWND))(void *)GetProcAddress(style,"Ntw16sWindowAttached"):NULL;
            BORROW_CHECK(style_scope && query_style);
            style_gdi=GetGuiResources(GetCurrentProcess(),GR_GDIOBJECTS);
        }
#endif
        window = wow_user_server_create_dialog(GetModuleHandle(NULL), dialog,
            sizeof(template_words), NULL, terminal_dialog_witness, 0,
            SCDLG_CLIENT | SCDLG_ANSI | SCDLG_NOREVALIDATE);
#ifdef WOW_STYLED_TERMINAL_FIXTURE
        if (terminal_case) BORROW_CHECK(wow_style_creation_end(style_scope)==ERROR_SUCCESS);
#endif
        BORROW_CHECK(window != NULL);
        if (window) {
            button = GetDlgItem(window, 101);
            child = wow_window_words_acquire(button);
            BORROW_CHECK(button != NULL && child != NULL);
#ifdef WOW_STYLED_TERMINAL_FIXTURE
            if (terminal_case) BORROW_CHECK(query_style && query_style(window) && query_style(button));
#endif
            if (child) {
                BORROW_CHECK(wow_window_words_cleanup_value(child)->thread == &thread);
                BORROW_CHECK(wow_window_class_words_value(child) != NULL);
            }
            if (terminal_case) {
                wow_window_words_binding *root = wow_window_words_acquire(window);
                wow_task_callback_scope rejected;
                unsigned callbacks = terminal_dialog_callbacks;
                BORROW_CHECK(root != NULL);
                BORROW_CHECK(wow_user_runtime_enter(&binding));
                thread.TIF_flags |= WOW_TASK_TIF_INCLEANUP;
                if (child) {
                    DWORD target = 0;
                    WNDCLASSEXA narrow = {sizeof(narrow)};
                    WNDCLASSEXW wide = {sizeof(wide)};
                    HMODULE provider = NULL;
                    char provider_path[MAX_PATH] = {0};
                    wow_window_procedure_read(button, &target);
                    if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            (LPCSTR)(ULONG_PTR)target, &provider))
                        GetModuleFileNameA(provider, provider_path, sizeof(provider_path));
                    GetClassInfoExA(NULL, "BUTTON", &narrow);
                    GetClassInfoExW(NULL, L"BUTTON", &wide);
                    fprintf(stderr, "WOW_TERMINAL_NATIVE_TARGET hwnd=%p target=%08lx classA=%p classW=%p fnid=%04x cleanup=%p\n",
                        button, target, narrow.lpfnWndProc, wide.lpfnWndProc,
                        wow_window_words_cleanup_value(child)->fnid,
                        wow_window_words_cleanup_value(child)->native_cleanup);
                    fprintf(stderr, "WOW_TERMINAL_NATIVE_PROVIDER module=%s rva=%08lx\n",
                        provider_path, provider ? target - (DWORD)(ULONG_PTR)provider : 0);
                    /* Original controls clean up by retained client FNID;
                     * only native owners require the native fallback. */
                    BORROW_CHECK(wow_control_client_procedure(GETFNID(
                        wow_window_words_client_state_value(child)),FALSE) != NULL ||
                        wow_window_words_cleanup_value(child)->native_cleanup != NULL);
                    {
                        WNDPROC retained = wow_window_words_cleanup_value(child)->native_cleanup;
                        WORD retained_fnid = wow_window_words_cleanup_value(child)->fnid;
                        SetLastError(0x447a3002);
                        wow_window_note_native_procedure(button, (WNDPROC)(ULONG_PTR)1, FALSE);
                        BORROW_CHECK(GetLastError() == 0x447a3002 && binding.exclusive_held);
                        BORROW_CHECK(wow_window_words_cleanup_value(child)->native_cleanup == retained &&
                            wow_window_words_cleanup_value(child)->fnid == retained_fnid);
                    }
                    BORROW_CHECK(wow_window_words_prepare_exit(button, child));
                }
                if (root) BORROW_CHECK(wow_window_words_prepare_exit(window, root));
#ifdef WOW_STYLED_TERMINAL_FIXTURE
                BORROW_CHECK(query_style && query_style(window) && query_style(button));
#endif
                BORROW_CHECK(wow_user_runtime_leave(&binding));
                BORROW_CHECK(!wow_task_callback_enter(&rejected));
                BORROW_CHECK(GetLastError() == ERROR_INVALID_STATE && !binding.exclusive_held);
                BORROW_CHECK(!wow_task_resource_cleanup_enter(&rejected));
                BORROW_CHECK(GetLastError() == ERROR_INVALID_STATE);
                binding.resource_cleanup_live = TRUE;
                BORROW_CHECK(!wow_task_callback_enter(&rejected));
                BORROW_CHECK(wow_task_resource_cleanup_enter(&rejected));
                BORROW_CHECK(wow_task_callback_leave(&rejected));
                binding.resource_cleanup_live = FALSE;
                BORROW_CHECK(!wow_task_resource_cleanup_enter(&rejected));
                BORROW_CHECK(!wow_window_create_boundA(&thread, native_class.atom,
                    &request, &words, NULL));
                BORROW_CHECK(GetLastError() == ERROR_INVALID_STATE);
                /* Match prepare_exit's whole destruction scope. Application
                 * callbacks remain denied, but original resource finalizers
                 * must run before their owning task storage disappears. */
                binding.resource_cleanup_live = TRUE;
                __try { BORROW_CHECK(wow_native_DestroyWindow(window)); }
                __finally { binding.resource_cleanup_live = FALSE; }
                BORROW_CHECK(terminal_dialog_callbacks == callbacks);
                wow_window_words_release(root);
                thread.TIF_flags &= ~WOW_TASK_TIF_INCLEANUP;
            } else {
                unsigned callbacks = terminal_dialog_callbacks;
                BORROW_CHECK(wow_native_DestroyWindow(window));
                BORROW_CHECK(terminal_dialog_callbacks == callbacks + 2);
            }
            wow_window_words_release(child);
            BORROW_CHECK(wow_window_words_collect_retired(&objects));
            BORROW_CHECK(mvdm_softpc_wow_page_domain_collect_observed_classes());
            BORROW_CHECK(objects.entries[LOWORD(window)].bType == TYPE_FREE);
            BORROW_CHECK(button && !IsWindow(button) &&
                objects.entries[LOWORD(button)].bType == TYPE_FREE);
#ifdef WOW_STYLED_TERMINAL_FIXTURE
            if (terminal_case) {
                DWORD after;
                GdiFlush();after=GetGuiResources(GetCurrentProcess(),GR_GDIOBJECTS);
                BORROW_CHECK(after==style_gdi);
                fprintf(stderr,"WOW_STYLED_TERMINAL_CCPU errors=%u style_before=1 neutralized=1 dialog=1 button=1 gdi=%lu/%lu\n",
                    borrow_errors-before,style_gdi,after);
            }
#endif
        }
        }
        fprintf(stderr, "WOW_TERMINAL_PREPARE_CCPU errors=%u dialog_suppressed=1 native_button=1 creation_denied=1 callback_denied=1\n",
            borrow_errors - before);
    }
    verify_dialog_cache(teb, &objects);
    BORROW_CHECK(UnhookWindowsHookEx(terminal));
    BORROW_CHECK(wow_user_runtime_unbind(&binding));
    BORROW_CHECK(UserRegisterWowHandlers(&saved_callbacks, &outputs) != 0);
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
                PWW previous_words = NULL;
                ULONG held;
                wow_window_words_binding *stored;
                BOOL entered;
                BOOL first_scope = wow_user_borrow_enter(&inner);
                BORROW_CHECK(first_scope);
                if (first_scope) {
                    previous_words = FindPWW(window, 0);
                    BORROW_CHECK(previous_words != NULL);
                    BORROW_CHECK(wow_user_borrow_leave(&inner));
                }
                held = mvdm_softpc_wow_page_domain_borrow_window((ULONG)(ULONG_PTR)window);
                BORROW_CHECK(held != 0);
                if (held) {
                    stored = mvdm_softpc_wow_page_domain_window_host_storage(held);
                    BORROW_CHECK(stored && wow_window_words_value(stored) == previous_words);
                    BORROW_CHECK(mvdm_softpc_wow_page_domain_release_window(held));
                }
                entered = wow_user_borrow_enter(&outer);
                BORROW_CHECK(entered);
                if (entered) {
                    words = FindPWW(window, 0); /* Retained original classifier. */
                    BORROW_CHECK(words == previous_words);
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

static unsigned host_storage_disposals;
static unsigned host_storage_cookie;
static void __cdecl dispose_host_storage_witness(void *storage)
{
    BORROW_CHECK(storage == &host_storage_cookie);
    ++host_storage_disposals;
    SetLastError(0x7447); /* Deallocation must not replace caller error. */
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
    ULONG identity_process = 0, identity_thread = 0;
    mvdm_softpc_wow_window_words_view view, sentinel;
    const ULONG creator_words[3] = {0x87654321u, 0x11112222u, 0x33334444u};
    ULONG delta = c_sas_dw_at(teb + 0x60u);
    unsigned before = borrow_errors;
    unsigned disposals_before = host_storage_disposals;
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
        BORROW_CHECK(!mvdm_softpc_wow_page_domain_window_host_storage(backing));
        BORROW_CHECK(!mvdm_softpc_wow_page_domain_bind_window_host_storage(backing,
            NULL, dispose_host_storage_witness));
        BORROW_CHECK(!mvdm_softpc_wow_page_domain_bind_window_host_storage(backing,
            &host_storage_cookie, NULL));
        BORROW_CHECK(mvdm_softpc_wow_page_domain_bind_window_host_storage(backing,
            &host_storage_cookie, dispose_host_storage_witness));
        BORROW_CHECK(!mvdm_softpc_wow_page_domain_retain_host_window(backing, NULL));
        BORROW_CHECK(!mvdm_softpc_wow_page_domain_retain_host_window(backing, &host_storage_disposals));
        BORROW_CHECK(mvdm_softpc_wow_page_domain_retain_host_window(backing, &host_storage_cookie));
        BORROW_CHECK(mvdm_softpc_wow_page_domain_release_window(backing));
        BORROW_CHECK(!mvdm_softpc_wow_page_domain_bind_window_host_storage(backing,
            &host_storage_cookie, dispose_host_storage_witness));
        BORROW_CHECK(mvdm_softpc_wow_page_domain_window_host_storage(backing) ==
            &host_storage_cookie && host_storage_disposals == disposals_before);
        BORROW_CHECK(mvdm_softpc_wow_page_domain_release_window(backing));
        BORROW_CHECK(!mvdm_softpc_wow_page_domain_window_host_storage(backing));
        BORROW_CHECK(host_storage_disposals == disposals_before);
        BORROW_CHECK(mvdm_softpc_wow_page_domain_borrow_window(full) == backing);
        BORROW_CHECK(mvdm_softpc_wow_page_domain_window_host_storage(backing) ==
            &host_storage_cookie);
        observed_class = c_sas_dw_at(backing + 76u);
        BORROW_CHECK(mvdm_softpc_wow_page_domain_read_window_identity(backing,
            &identity_process, &identity_thread));
        BORROW_CHECK(identity_process == GetCurrentProcessId() &&
            identity_thread == GetCurrentThreadId());
        BORROW_CHECK(mvdm_softpc_wow_page_domain_read_window_words_view(backing, &view));
        BORROW_CHECK(view.style == info.dwStyle && view.exstyle == info.dwExStyle &&
            view.module == (ULONG)(ULONG_PTR)GetModuleHandleA(NULL));
        {
            /* Actual CCPU-owned projection, not a host observation mock.
             * Bulk reads must see backing edits immediately and preserve
             * retained identity/LastError and rejection output semantics. */
            mvdm_softpc_wow_window_observation row, rejected, untouched;
            ULONG cls = observed_class - delta, directory = c_getCR3();
            ULONG old_left = c_sas_dw_at(backing + 40u);
            ULONG old_name = c_sas_dw_at(cls + 100u), alternate = 0;
            BORROW_CHECK(mvdm_softpc_wow_page_domain_read_window_view(backing, &row));
            BORROW_CHECK(row.links.window == full && row.process_id == GetCurrentProcessId() &&
                row.descriptor.atom == info.atomWindowType && !strcmp(row.descriptor.name, name));
            c_sas_storedw(backing + 40u, 0x74478080u);
            SetLastError(0x74478001u);
            BORROW_CHECK(mvdm_softpc_wow_page_domain_read_window_view(backing, &row));
            BORROW_CHECK((ULONG)row.window_rect[0] == 0x74478080u && GetLastError() == 0x74478001u);
            c_sas_storedw(backing + 40u, old_left);
            BORROW_CHECK(VdmAllocateVirtualMemory(&alternate, 8192u, TRUE) >= 0);
            if (alternate) {
                c_sas_loads(cls + 108u, (BYTE *)row.descriptor.name, (ULONG)length + 1u);
                /* Noncanonical pointer crossing a page retains scalar SAS.
                 * This test edits only its independently allocated view. */
                { extern void c_sas_stores(uint32_t, uint8_t *, uint32_t);
                    c_sas_stores(alternate + 4094u, (BYTE *)"XYZ", 4u); }
                c_sas_storedw(cls + 100u, alternate + 4094u + delta);
                BORROW_CHECK(mvdm_softpc_wow_page_domain_read_window_view(backing, &row));
                BORROW_CHECK(row.descriptor.name_bytes == 4u && !strcmp(row.descriptor.name, "XYZ") &&
                    row.descriptor.name[4] == 0);
                c_sas_storedw(cls + 100u, old_name);
                BORROW_CHECK(VdmFreeVirtualMemory(alternate) >= 0);
            }
            memset(&untouched, 0xa5, sizeof(untouched));rejected = untouched;
            c_setCR3(0);
            BORROW_CHECK(!mvdm_softpc_wow_page_domain_read_window_view(backing, &rejected));
            BORROW_CHECK(!memcmp(&rejected, &untouched, sizeof(rejected)));
            c_setCR3(directory);
            BORROW_CHECK(mvdm_softpc_wow_page_domain_read_window_view(backing, &row));
            BORROW_CHECK(row.window_rect[0] == (LONG)old_left && !strcmp(row.descriptor.name, name));
        }
        {
            /* Publish exactly the original public fields, not an entire WND
             * overwrite. Exercise immediate RAM stores and trace fallback
             * using the actual original CCPU domain in this fixture. */
            const ULONG offsets[4] = {36u, 72u, 160u, 172u};
            ULONG saved_fields[4], pass, k;
            LONG trace = mvdm_softpc_wow_source_descriptor_trace_active;
            mvdm_softpc_wow_window_observation row;
            for (k = 0; k < 4u; ++k) {
                saved_fields[k] = c_sas_dw_at(backing + offsets[k]);
                c_sas_storedw(backing + offsets[k], 0x74478100u + k);
            }
            for (pass = 0; pass < 2u; ++pass) {
                InterlockedExchange(&mvdm_softpc_wow_source_descriptor_trace_active, pass);
                BORROW_CHECK(SetWindowPos(window, NULL, 11 + pass, 21 + pass,
                    31 + pass, 41 + pass, SWP_NOZORDER | SWP_NOACTIVATE));
                BORROW_CHECK(GetWindowInfo(window, &info));
                BORROW_CHECK(mvdm_softpc_wow_page_domain_refresh_window(full));
                BORROW_CHECK(mvdm_softpc_wow_page_domain_read_window_view(backing, &row));
                BORROW_CHECK(!memcmp(row.window_rect, &info.rcWindow, sizeof(RECT)) &&
                    !memcmp(row.client_rect, &info.rcClient, sizeof(RECT)) &&
                    row.style == info.dwStyle && row.exstyle == info.dwExStyle &&
                    row.links.window == full && c_sas_dw_at(backing + 76u) == observed_class);
                for (k = 0; k < 4u; ++k)
                    BORROW_CHECK(c_sas_dw_at(backing + offsets[k]) == 0x74478100u + k);
            }
            InterlockedExchange(&mvdm_softpc_wow_source_descriptor_trace_active, trace);
            for (k = 0; k < 4u; ++k)
                c_sas_storedw(backing + offsets[k], saved_fields[k]);
        }
        /* Native mutation is not a new committed observation. Reading the
         * retained carrier must not secretly resample half of the fields. */
        SetLastError(ERROR_SUCCESS);
        BORROW_CHECK(SetWindowLongA(window, GWL_EXSTYLE,
            info.dwExStyle | WS_EX_TOOLWINDOW) != 0 || GetLastError() == ERROR_SUCCESS);
        BORROW_CHECK(mvdm_softpc_wow_page_domain_read_window_words_view(backing, &view));
        BORROW_CHECK(view.exstyle == info.dwExStyle);
        BORROW_CHECK(mvdm_softpc_wow_page_domain_exchange_window_word(
            backing, 0, 0x12345678u, &previous) && previous == 0);
        BORROW_CHECK(mvdm_softpc_wow_page_domain_observe_window(full, info.atomWindowType,
            GetClassLongA(window, GCL_STYLE), GetClassLongA(window, GCL_CBWNDEXTRA),
            name, (ULONG)length + 1u));
        BORROW_CHECK(c_sas_dw_at(backing + 76u) == observed_class);
        BORROW_CHECK(mvdm_softpc_wow_page_domain_read_window_words_view(backing, &view));
        BORROW_CHECK(view.exstyle == (info.dwExStyle | WS_EX_TOOLWINDOW) &&
            view.module == (ULONG)(ULONG_PTR)GetModuleHandleA(NULL) &&
            view.words[0] == 0x12345678u);
        managed_class = mvdm_softpc_wow_page_domain_publish_named_class(
            info.atomWindowType, 0, 0, 0, 0, 0,
            GetClassLongA(window, GCL_STYLE), 0, 0, 0,
            name, (ULONG)length + 1u);
        BORROW_CHECK(managed_class != 0);
        BORROW_CHECK(!mvdm_softpc_wow_page_domain_publish_handle(LOWORD(full),
            (WORD)(HIWORD(full) ^ 1u), 1, 0, full, managed_class, NULL));
        BORROW_CHECK(!mvdm_softpc_wow_page_domain_publish_handle(LOWORD(full),
            HIWORD(full), 1, 0, full, 0xdeadbeefu, NULL));
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
        BORROW_CHECK(mvdm_softpc_wow_page_domain_retain_host_window(backing, &host_storage_cookie));
        BORROW_CHECK(mvdm_softpc_wow_page_domain_window_host_storage(backing) ==
            &host_storage_cookie && host_storage_disposals == disposals_before);
        BORROW_CHECK(!mvdm_softpc_wow_page_domain_bind_window_host_storage(backing,
            &host_storage_cookie, dispose_host_storage_witness));
        BORROW_CHECK(!mvdm_softpc_wow_page_domain_retire_class(managed_class));
        identity_process = identity_thread = 0;
        BORROW_CHECK(mvdm_softpc_wow_page_domain_read_window_identity(backing,
            &identity_process, &identity_thread));
        BORROW_CHECK(identity_process == GetCurrentProcessId() &&
            identity_thread == GetCurrentThreadId());
        BORROW_CHECK(mvdm_softpc_wow_page_domain_read_window_words(backing, words) &&
            memcmp(words, creator_words, sizeof(words)) == 0);
        SetLastError(0x4473);
        BORROW_CHECK(mvdm_softpc_wow_page_domain_release_window(backing));
        /* The independent host scope pin must outlive the earlier ordinary
         * backing borrow, even after logical retirement. */
        BORROW_CHECK(host_storage_disposals == disposals_before);
        BORROW_CHECK(mvdm_softpc_wow_page_domain_window_host_storage(backing) ==
            &host_storage_cookie);
        BORROW_CHECK(mvdm_softpc_wow_page_domain_read_window_words(backing, words));
        BORROW_CHECK(mvdm_softpc_wow_page_domain_release_window(backing));
        BORROW_CHECK(host_storage_disposals == disposals_before + 1 &&
            GetLastError() != 0x7447);
        BORROW_CHECK(!mvdm_softpc_wow_page_domain_retain_host_window(backing, &host_storage_cookie));
        BORROW_CHECK(!mvdm_softpc_wow_page_domain_window_host_storage(backing));
        BORROW_CHECK(!mvdm_softpc_wow_page_domain_release_window(backing));
        BORROW_CHECK(host_storage_disposals == disposals_before + 1);
        FillMemory(&sentinel, sizeof(sentinel), 0xa5);
        view = sentinel;
        BORROW_CHECK(!mvdm_softpc_wow_page_domain_read_window_words_view(backing, &view));
        BORROW_CHECK(memcmp(&view, &sentinel, sizeof(view)) == 0);
        identity_process = 0x1234;
        identity_thread = 0x5678;
        BORROW_CHECK(!mvdm_softpc_wow_page_domain_read_window_identity(backing,
            &identity_process, &identity_thread));
        BORROW_CHECK(identity_process == 0x1234 && identity_thread == 0x5678);
        BORROW_CHECK(mvdm_softpc_wow_page_domain_retire_class(managed_class));
    }
    BORROW_CHECK(GetPropW(window, L"NTVDM.WOW.WindowWords") == NULL && IsWindow(window));
    BORROW_CHECK(VdmQueryFreeVirtualMemory(&after_free, &largest) >= 0 &&
        before_free == after_free);
    BORROW_CHECK(DestroyWindow(window));
    fprintf(stderr, "WOW_VIEW_PROMOTION_CCPU errors=%u same_backing=1 words=1 stale_rejected=1 ownership=1 released=1\n",
        borrow_errors - before);
}

static unsigned adopted_callback_calls;
static DWORD WINAPI adopted_guest_callback(HWND window, UINT message, UINT wp,
    LONG lp, DWORD procedure, PVOID words)
{
    /* Explicit guest callback double. Real CCPU publication and original
     * dispatch boundary are exercised, not execution of guest instructions. */
    if (message == WM_APP + 47) {
        BORROW_CHECK(((PWW)words)->vpfnWndProc == 0x12340056u);
        ++adopted_callback_calls;
        return procedure;
    }
    return (DWORD)DefWindowProcA(window, message, wp, lp);
}
static LRESULT CALLBACK adoption_native_proc(HWND window, UINT message, WPARAM wp, LPARAM lp)
{
    if (message == WM_APP + 47) return 73;
    return DefWindowProcA(window, message, wp, lp);
}
static void verify_native_window_first_subclass_case(BOOL existing_subclass)
{
    static wow_user_object_table objects;
    wow_user_runtime runtime = WOW_USER_RUNTIME_INITIALIZER;
    wow_user_runtime_thread runtime_binding = {0};
    wow_task_order_process process = {0};
    wow_task_order_thread thread = {0};
    wow_task_order_entry task = {0};
    wow_task_order_state task_state = {0};
    wow_user_task_lifecycle native_owner = {0};
    wow_class_client_context context = {0};
    wow_user_borrow_scope scope;
    WNDCLASSEXA cls = {sizeof(cls)};
    HWND window;
    PWW words;
    ULONG backing, class_server, previous, native_predecessor;
    BOOL previous_guest;
    HHOOK terminal = NULL;
    unsigned before = borrow_errors, callbacks = adopted_callback_calls;
    cls.lpfnWndProc = adoption_native_proc;
    cls.hInstance = GetModuleHandleA(NULL);
    cls.lpszClassName = "S3_NATIVE_FIRST_SUBCLASS";
    BORROW_CHECK(RegisterClassExA(&cls));
    window = CreateWindowExA(0, cls.lpszClassName, "", 0, 0, 0, 16, 16,
        NULL, NULL, cls.hInstance, NULL);
    BORROW_CHECK(window != NULL);
    if (!window) goto class_cleanup;
    if (existing_subclass) BORROW_CHECK(SetWindowSubclass(window, native_chain, 47, 0));
    native_predecessor = (ULONG)GetWindowLongA(window, GWL_WNDPROC);
    BORROW_CHECK(native_predecessor != 0);
    process.objects = &objects;
    /* Match the real callback/menu boundary: an enrolled WOW thread owns a
     * task and execution state, not just an object table. */
    process.pwpi = &task_state;
    task_state.CSOwningThread = &thread;
    thread.ppi = &process;
    thread.ptdb = &task;
    task.hTaskWow = 0x4321u;
    runtime.lifecycle = &native_owner;
    context.thread = &thread;
    context.callback = adopted_guest_callback;
    if (!wow_user_runtime_bind(&runtime_binding, &runtime, &thread, &context)) {
        BORROW_CHECK(FALSE); goto window_cleanup;
    }
    terminal = SetWindowsHookExA(WH_CALLWNDPROCRET,
        wow_window_terminal_observer,NULL,GetCurrentThreadId());
    BORROW_CHECK(terminal != NULL);
    if (!terminal) goto unbind;
    BORROW_CHECK(wow_user_window_refresh_return(window));
    if (!wow_user_borrow_enter(&scope)) {
        BORROW_CHECK(FALSE); goto unbind;
    }
    words = wow_user_borrow_window(window);
    backing = mvdm_softpc_wow_page_domain_borrow_window((ULONG)(ULONG_PTR)window);
    BORROW_CHECK(words && backing);
    if (words && backing) {
        class_server = c_sas_dw_at(backing + 76u);
        /* Fail ownership publication before native installation. The same
         * borrowed WW and observed identity must remain usable for retry. */
        objects.entries[LOWORD(window)].bType = TYPE_MENU;
        BORROW_CHECK(!wow_user_set_procedure(window,FALSE,0x92340056u,TRUE,&previous_guest));
        ZeroMemory(&objects.entries[LOWORD(window)], sizeof(objects.entries[0]));
        BORROW_CHECK(!GetPropW(window, L"NTVDM.WOW.WindowWords"));
        BORROW_CHECK((ULONG)GetWindowLongA(window, GWL_WNDPROC) == native_predecessor);
        BORROW_CHECK(c_sas_dw_at(backing + 76u) == class_server);
        BORROW_CHECK(wow_user_borrow_window(window) == words);
        previous = wow_user_set_procedure(window,FALSE,0x92340056u,TRUE,&previous_guest);
        BORROW_CHECK(previous == native_predecessor && !previous_guest);
        BORROW_CHECK(wow_user_borrow_window(window) == words);
        BORROW_CHECK(c_sas_dw_at(backing + 76u) == class_server);
        BORROW_CHECK(objects.entries[LOWORD(window)].pOwner == &thread);
        /* Original walias.h: GWL_WOWvpfnWndProc aliases GWL_WOWDWORD2. */
        (void)wow_user_set_window_longA(window, GWL_WOWDWORD2, 0x12340056u);
        BORROW_CHECK((ULONG)SendMessageA(window, WM_APP + 47, 0, 0) == 0x92340056u);
        BORROW_CHECK(adopted_callback_calls == callbacks + 1);
        BORROW_CHECK(wow_user_set_procedure(window,FALSE,previous,FALSE,
            &previous_guest) == 0x92340056u && previous_guest);
        BORROW_CHECK(SendMessageA(window, WM_APP + 47, 0, 0) == 73);
        if (existing_subclass) {
            DWORD_PTR data = 1;
            unsigned chain_calls = native_chain_calls;
            /* Retaining the comctl predecessor must retain its chain, not
             * substitute the raw leaf procedure. This is a carrier test;
             * it does not claim NTW16S rendering or guest execution. */
            BORROW_CHECK(GetWindowSubclass(window, native_chain, 47, &data) && data == 0);
            (void)SendMessageA(window, WM_APP + 65, 0, 0);
            BORROW_CHECK(native_chain_calls == chain_calls + 1);
        }
    }
    BORROW_CHECK(DestroyWindow(window));
    BORROW_CHECK(objects.entries[LOWORD(window)].bType == TYPE_FREE);
    window = NULL;
    if (backing) BORROW_CHECK(mvdm_softpc_wow_page_domain_release_window(backing));
    BORROW_CHECK(wow_user_borrow_leave(&scope));
    /* Native destruction completed outside a native-call wrapper. Reconcile
     * its copied graph before the next independent allocation-balance case. */
    BORROW_CHECK(wow_user_window_refresh_graph(NULL));
unbind:
    BORROW_CHECK(wow_user_runtime_unbind(&runtime_binding));
window_cleanup:
    if (window) BORROW_CHECK(DestroyWindow(window));
    if (terminal) BORROW_CHECK(UnhookWindowsHookEx(terminal));
class_cleanup:
    BORROW_CHECK(UnregisterClassA(cls.lpszClassName, cls.hInstance));
    fprintf(stderr, "WOW_NATIVE_FIRST_SUBCLASS_CCPU errors=%u existing_subclass=%u callback_double=1 rollback_retry=1 identity_retained=1\n",
        borrow_errors - before, existing_subclass);
}

static void verify_native_window_first_subclass(void)
{
    verify_native_window_first_subclass_case(FALSE);
    verify_native_window_first_subclass_case(TRUE);
}

static void verify_native_class_window_ownership(void)
{
    unsigned mode, before = borrow_errors;
    /* Both explicit native CLS and preserve-current-CLS publication must
     * retain the same WND generation. This is not a WNDPROC dispatch test. */
    for (mode = 0; mode != 2; ++mode) {
        HWND window = CreateWindowExA(0, "STATIC", "Native class owner", WS_POPUP,
            0, 0, 24, 24, NULL, NULL, GetModuleHandleA(NULL), NULL);
        WINDOWINFO info = {sizeof(info)};
        ULONG full = (ULONG)(ULONG_PTR)window, backing, cls, words[3];
        const ULONG replacement[3] = {0x12345678u, 0x23456789u, 0x34567890u};
        char name[256];
        int length;
        BORROW_CHECK(window != NULL);
        if (!window) continue;
        BORROW_CHECK(GetWindowInfo(window, &info));
        length = GetClassNameA(window, name, sizeof(name));
        BORROW_CHECK(length > 0);
        BORROW_CHECK(mvdm_softpc_wow_page_domain_observe_window(full, info.atomWindowType,
            GetClassLongA(window, GCL_STYLE), GetClassLongA(window, GCL_CBWNDEXTRA),
            name, (ULONG)length + 1u));
        backing = mvdm_softpc_wow_page_domain_borrow_window(full);
        BORROW_CHECK(backing != 0);
        if (backing) {
            cls = c_sas_dw_at(backing + 76u);
            BORROW_CHECK(cls != 0);
            BORROW_CHECK(mvdm_softpc_wow_page_domain_publish_handle(LOWORD(full),
                HIWORD(full), 1, 0, full, mode ? cls : 0, replacement));
            BORROW_CHECK(c_sas_dw_at(backing + 76u) == cls);
            BORROW_CHECK(GetClassLongA(window, GCW_ATOM) == info.atomWindowType);
            BORROW_CHECK(mvdm_softpc_wow_page_domain_window_handle(LOWORD(full)) == full);
            BORROW_CHECK(!mvdm_softpc_wow_page_domain_forget_observed_window(full));
            BORROW_CHECK(!mvdm_softpc_wow_page_domain_publish_handle(LOWORD(full),
                HIWORD(full), 1, 0, full, cls, NULL));
            BORROW_CHECK(mvdm_softpc_wow_page_domain_read_window_words(backing, words) &&
                memcmp(words, replacement, sizeof(words)) == 0);
            BORROW_CHECK(!mvdm_softpc_wow_page_domain_cancel_native_adoption(full, backing + 4));
            BORROW_CHECK(mvdm_softpc_wow_page_domain_cancel_native_adoption(full, backing));
            BORROW_CHECK(!mvdm_softpc_wow_page_domain_cancel_native_adoption(full, backing));
            BORROW_CHECK(c_sas_dw_at(backing + 76u) == cls);
            BORROW_CHECK(mvdm_softpc_wow_page_domain_read_window_words(backing, words) &&
                memcmp(words, replacement, sizeof(words)) == 0);
            BORROW_CHECK(mvdm_softpc_wow_page_domain_publish_handle(LOWORD(full),
                HIWORD(full), 1, 0, full, 0, NULL));
            BORROW_CHECK(DestroyWindow(window));
            window = NULL;
            /* Even after native destruction the explicit managed owner,
             * not an observation sweep, must retire the shared identity. */
            BORROW_CHECK(mvdm_softpc_wow_page_domain_collect_observed_windows());
            BORROW_CHECK(mvdm_softpc_wow_page_domain_window_handle(LOWORD(full)) == full);
            BORROW_CHECK(mvdm_softpc_wow_page_domain_retire_handle(LOWORD(full), NULL));
            BORROW_CHECK(mvdm_softpc_wow_page_domain_read_window_words(backing, words));
            BORROW_CHECK(mvdm_softpc_wow_page_domain_release_window(backing));
            BORROW_CHECK(!mvdm_softpc_wow_page_domain_read_window_words(backing, words));
            BORROW_CHECK(mvdm_softpc_wow_page_domain_collect_observed_classes());
        }
        if (window) BORROW_CHECK(DestroyWindow(window));
    }
    fprintf(stderr, "WOW_NATIVE_CLASS_WND_OWNER_CCPU errors=%u same_class=1 same_generation=1 managed_retirement=1 modes=2\n",
        borrow_errors - before);
}

static void verify_released_class_borrow(ULONG teb)
{
    HWND window = CreateWindowExA(0, "STATIC", "Class release", WS_POPUP,
        0, 0, 24, 24, NULL, NULL, GetModuleHandleA(NULL), NULL);
    WINDOWINFO info = {sizeof(info)};
    ULONG full = (ULONG)(ULONG_PTR)window, cls, backing;
    ULONG delta = c_sas_dw_at(teb + 0x60u), free_before, free_after, largest;
    unsigned before = borrow_errors;
    BORROW_CHECK(window && GetWindowInfo(window, &info));
    if (!window) return;
    BORROW_CHECK(VdmQueryFreeVirtualMemory(&free_before, &largest) >= 0);
    cls = mvdm_softpc_wow_page_domain_publish_named_class(info.atomWindowType,
        0, 0, 0, 0, 0, 0, 0, 0, 0, "STATIC", 7);
    BORROW_CHECK(cls != 0);
    BORROW_CHECK(mvdm_softpc_wow_page_domain_publish_handle(LOWORD(full),
        HIWORD(full), 1, 0, full, cls, NULL));
    backing = mvdm_softpc_wow_page_domain_borrow_window(full);
    BORROW_CHECK(backing != 0);
    BORROW_CHECK(!mvdm_softpc_wow_page_domain_release_class(cls));
    BORROW_CHECK(mvdm_softpc_wow_page_domain_retire_handle(LOWORD(full), NULL));
    BORROW_CHECK(!mvdm_softpc_wow_page_domain_retire_class(cls));
    BORROW_CHECK(mvdm_softpc_wow_page_domain_release_class(cls));
    BORROW_CHECK(mvdm_softpc_wow_page_domain_collect_observed_classes());
    BORROW_CHECK(c_sas_dw_at(backing + 76u) == cls &&
        (WORD)c_sas_dw_at(cls - delta + 4u) == info.atomWindowType);
    BORROW_CHECK(!mvdm_softpc_wow_page_domain_publish_handle(LOWORD(full),
        HIWORD(full), 1, 0, full, cls, NULL));
    BORROW_CHECK(mvdm_softpc_wow_page_domain_release_window(backing));
    BORROW_CHECK(mvdm_softpc_wow_page_domain_collect_observed_classes());
    BORROW_CHECK(!mvdm_softpc_wow_page_domain_release_class(cls));
    BORROW_CHECK(VdmQueryFreeVirtualMemory(&free_after, &largest) >= 0 &&
        free_before == free_after);
    BORROW_CHECK(DestroyWindow(window));
    fprintf(stderr, "WOW_CLASS_RELEASE_CCPU errors=%u live_rejected=1 borrowed_safe=1 no_resurrection=1 reclaimed=1\n",
        borrow_errors - before);
}

static void verify_class_collection_reference_set(ULONG teb)
{
    enum { count = 32 };
    HWND windows[count] = {0};
    ULONG classes[count] = {0}, backings[count] = {0};
    ULONG delta = c_sas_dw_at(teb + 0x60u), before_free = 0, after_free = 0, largest;
    unsigned i, iteration, before = borrow_errors;
    BORROW_CHECK(VdmQueryFreeVirtualMemory(&before_free, &largest) >= 0);
    for (i = 0; i < count; ++i) {
        WINDOWINFO info = {sizeof(info)};
        ULONG full;
        windows[i] = CreateWindowExA(0, "STATIC", "Class set", WS_POPUP,
            0, 0, 24, 24, NULL, NULL, GetModuleHandleA(NULL), NULL);
        BORROW_CHECK(windows[i] && GetWindowInfo(windows[i], &info));
        if (!windows[i]) break;
        full = (ULONG)(ULONG_PTR)windows[i];
        classes[i] = mvdm_softpc_wow_page_domain_publish_named_class(info.atomWindowType,
            0, 0, 0, 0, 0, 0, 0, 0, 0, "STATIC", 7);
        BORROW_CHECK(classes[i] && (!i || classes[i] != classes[i - 1]));
        BORROW_CHECK(mvdm_softpc_wow_page_domain_publish_handle(LOWORD(full),
            HIWORD(full), 1, 0, full, classes[i], NULL));
        backings[i] = mvdm_softpc_wow_page_domain_borrow_window(full);
        BORROW_CHECK(backings[i] != 0);
    }
    BORROW_CHECK(i == count);
    for (i = 0; i < count; ++i) if (classes[i]) {
        if (!(i & 1u)) {
            BORROW_CHECK(mvdm_softpc_wow_page_domain_retire_handle(LOWORD(windows[i]), NULL));
            BORROW_CHECK(mvdm_softpc_wow_page_domain_release_class(classes[i]));
        } else BORROW_CHECK(!mvdm_softpc_wow_page_domain_release_class(classes[i]));
    }
    for (iteration = 0; iteration < 16u; ++iteration)
        BORROW_CHECK(mvdm_softpc_wow_page_domain_collect_observed_classes());
    for (i = 0; i < count; ++i) if (backings[i]) {
        BORROW_CHECK(c_sas_dw_at(backings[i] + 76u) == classes[i]);
        BORROW_CHECK((WORD)c_sas_dw_at(classes[i] - delta + 4u) != 0);
        if (i & 1u) {
            BORROW_CHECK(mvdm_softpc_wow_page_domain_retire_handle(LOWORD(windows[i]), NULL));
            BORROW_CHECK(mvdm_softpc_wow_page_domain_release_class(classes[i]));
        }
        BORROW_CHECK(mvdm_softpc_wow_page_domain_release_window(backings[i]));
    }
    BORROW_CHECK(mvdm_softpc_wow_page_domain_collect_observed_classes());
    for (i = 0; i < count; ++i) {
        if (classes[i]) BORROW_CHECK(!mvdm_softpc_wow_page_domain_release_class(classes[i]));
        if (windows[i]) BORROW_CHECK(DestroyWindow(windows[i]));
    }
    BORROW_CHECK(VdmQueryFreeVirtualMemory(&after_free, &largest) >= 0 && before_free == after_free);
    fprintf(stderr, "WOW_CLASS_REFERENCE_SET_CCPU errors=%u classes=32 live=16 retired_borrowed=16 retained=1 reclaimed=1\n",
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
    mvdm_softpc_wow_window_words_view view;
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
        BORROW_CHECK(mvdm_softpc_wow_page_domain_read_window_words_view(second, &view));
        BORROW_CHECK(view.words[0] == 0xabcdef12u && view.style == info.dwStyle &&
            view.exstyle == info.dwExStyle &&
            view.module == (ULONG)(ULONG_PTR)GetModuleHandleA(NULL));
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
    unsigned disposals_before = host_storage_disposals;
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
        BORROW_CHECK(mvdm_softpc_wow_page_domain_bind_window_host_storage(backing,
            &host_storage_cookie, dispose_host_storage_witness));
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
        BORROW_CHECK(host_storage_disposals == disposals_before);
        BORROW_CHECK(!mvdm_softpc_wow_page_domain_release_window(backing));
        BORROW_CHECK(!mvdm_softpc_wow_page_domain_release_retained_window(backing));
        BORROW_CHECK(!mvdm_softpc_wow_page_domain_read_window_words(backing, words));
        BORROW_CHECK(!mvdm_softpc_wow_page_domain_collect_retired_windows());
        BORROW_CHECK(host_storage_disposals == disposals_before);
        BORROW_CHECK(!mvdm_softpc_wow_page_domain_retire_class(cls));
        BORROW_CHECK(c_sas_dw_at(backing) == full);
        wow_fixture_fail_window_free(0);
        BORROW_CHECK(mvdm_softpc_wow_page_domain_collect_retired_windows());
        BORROW_CHECK(host_storage_disposals == disposals_before + 1);
        BORROW_CHECK(mvdm_softpc_wow_page_domain_collect_retired_windows());
        BORROW_CHECK(host_storage_disposals == disposals_before + 1);
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

static void verify_failed_menu_reclamation(ULONG delta)
{
    mvdm_softpc_wow_menu_query_item item = {0, 0, 71, 0};
    ULONG server, client, old_items, metadata, before_free, after_free, largest;
    unsigned before = borrow_errors;
    BORROW_CHECK(VdmQueryFreeVirtualMemory(&before_free, &largest) >= 0);
    server = mvdm_softpc_wow_page_domain_publish_menu(0x45670003u, 0x4321u, 1, &item);
    BORROW_CHECK(server != 0);
    if (!server) return;
    client = server - delta;
    old_items = c_sas_dw_at(client + 56u) - delta;
    wow_fixture_fail_window_free(old_items);
    item.id = 72;
    BORROW_CHECK(mvdm_softpc_wow_page_domain_publish_menu(0x45670003u, 0x4321u, 1, &item) == server);
    BORROW_CHECK(wow_fixture_window_free_failures() == 1 && c_sas_dw_at(old_items + 8u) == 71);
    BORROW_CHECK(c_sas_dw_at(c_sas_dw_at(client + 56u) - delta + 8u) == 72);
    wow_fixture_fail_window_free(0);
    BORROW_CHECK(mvdm_softpc_wow_page_domain_publish_menu(0x45670003u, 0x4321u, 1, &item) == server);
    wow_fixture_fail_window_free(client);
    BORROW_CHECK(mvdm_softpc_wow_page_domain_retire_menu(0x45670003u));
    BORROW_CHECK(wow_fixture_window_free_failures() == 1 && c_sas_dw_at(client) == 0x45670003u);
    BORROW_CHECK(mvdm_softpc_wow_page_domain_handle_metadata(3, &metadata) && !(metadata & 0xffu));
    BORROW_CHECK(!mvdm_softpc_wow_page_domain_retire_menu(0x45670003u));
    wow_fixture_fail_window_free(0);
    BORROW_CHECK(mvdm_softpc_wow_page_domain_publish_menu(0x98760003u, 0x4321u, 0, NULL));
    BORROW_CHECK(mvdm_softpc_wow_page_domain_retire_menu(0x98760003u));
    BORROW_CHECK(VdmQueryFreeVirtualMemory(&after_free, &largest) >= 0 && before_free == after_free);
    fprintf(stderr, "WOW_MENU_DEFERRED_RECLAIM_CCPU errors=%u atomic_update=1 logical_retirement=1 retry=1 released=1\n",
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
    unsigned disposals_before = host_storage_disposals;
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
    if (held) BORROW_CHECK(mvdm_softpc_wow_page_domain_bind_window_host_storage(held,
        &host_storage_cookie, dispose_host_storage_witness));
    root = mvdm_softpc_wow_page_domain_borrow_window(desktop);
    BORROW_CHECK(root != 0);
    replacement.window = desktop;
    SetLastError(0x1234);
    BORROW_CHECK(!mvdm_softpc_wow_page_domain_replace_window_links(0, &replacement, 0, NULL));
    BORROW_CHECK(GetLastError() == ERROR_INVALID_PARAMETER);
    BORROW_CHECK(!mvdm_softpc_wow_page_domain_replace_window_links(1, &replacement, 1, NULL));
    BORROW_CHECK(GetLastError() == ERROR_INVALID_PARAMETER);
    BORROW_CHECK(root && c_sas_dw_at(root + 28u) == held + delta);
    /* Live native identities cannot be withdrawn even with a valid new tree. */
    SetLastError(0x1234);
    BORROW_CHECK(!mvdm_softpc_wow_page_domain_replace_window_links(1, &replacement, 2, dead));
    BORROW_CHECK(GetLastError() == ERROR_INVALID_STATE);
    BORROW_CHECK(root && c_sas_dw_at(root + 28u) == held + delta);
    BORROW_CHECK(DestroyWindow(a)); a = NULL;
    BORROW_CHECK(DestroyWindow(b)); b = NULL;
    {
        ULONG stale[2] = {dead[0], dead[1] ^ 0x10000u};
        mvdm_softpc_wow_window_links duplicate[2];
        duplicate[0] = duplicate[1] = replacement;
        /* Failure after an earlier valid retirement must discard only its
         * prepared retention node, not consume the actual publication. */
        SetLastError(0x1234);
        BORROW_CHECK(!mvdm_softpc_wow_page_domain_replace_window_links(1, &replacement, 2, stale));
        BORROW_CHECK(GetLastError() == ERROR_INVALID_STATE);
        BORROW_CHECK(!mvdm_softpc_wow_page_domain_replace_window_links(2, duplicate, 2, dead));
        BORROW_CHECK(GetLastError() == ERROR_INVALID_PARAMETER);
        BORROW_CHECK(mvdm_softpc_wow_page_domain_window_handle(LOWORD(dead[0])) == dead[0] &&
            mvdm_softpc_wow_page_domain_window_handle(LOWORD(dead[1])) == dead[1]);
        BORROW_CHECK(root && c_sas_dw_at(root + 28u) == held + delta);
    }
    /* A source outside the replacement still points at B: refuse atomically. */
    SetLastError(0x1234);
    BORROW_CHECK(!mvdm_softpc_wow_page_domain_replace_window_links(1, &replacement, 1, &dead[1]));
    BORROW_CHECK(GetLastError() == ERROR_INVALID_STATE);
    BORROW_CHECK(root && c_sas_dw_at(root + 28u) == held + delta);
    SetLastError(0x55bb);
    BORROW_CHECK(mvdm_softpc_wow_page_domain_replace_window_links(1, &replacement, 2, dead));
    BORROW_CHECK(GetLastError() == 0x55bb);
    BORROW_CHECK(root && c_sas_dw_at(root + 28u) == 0);
    BORROW_CHECK(!mvdm_softpc_wow_page_domain_window_handle(LOWORD(dead[0])) &&
        !mvdm_softpc_wow_page_domain_window_handle(LOWORD(dead[1])));
    BORROW_CHECK(held && c_sas_dw_at(held) == dead[0] &&
        c_sas_dw_at(held + 24u) == root + delta && c_sas_dw_at(held + 20u) == 0);
    BORROW_CHECK(host_storage_disposals == disposals_before &&
        mvdm_softpc_wow_page_domain_window_host_storage(held) == &host_storage_cookie);
    BORROW_CHECK(mvdm_softpc_wow_page_domain_release_window(held));
    BORROW_CHECK(host_storage_disposals == disposals_before + 1);
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
    mvdm_softpc_wow_window_observation observations[2] = {0};
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
        CopyMemory(observations[i].window_rect, &info.rcWindow, sizeof(RECT));
        CopyMemory(observations[i].client_rect, &info.rcClient, sizeof(RECT));
        observations[i].style = info.dwStyle;
        observations[i].exstyle = info.dwExStyle;
        observations[i].thread_id = GetWindowThreadProcessId(windows[i],
            &observations[i].process_id);
    }
    {
        WORD atom = descriptors[1].atom;
        descriptors[1].atom = 0; /* Fail after the first WND/CLS preparation. */
        SetLastError(0x1234);
        observations[0].links = links[0]; observations[1].links = links[1];
        observations[0].descriptor = descriptors[0]; observations[1].descriptor = descriptors[1];
        BORROW_CHECK(!mvdm_softpc_wow_page_domain_project_window_graph(2, observations));
        BORROW_CHECK(GetLastError() == ERROR_INVALID_PARAMETER);
        descriptors[1].atom = atom;
    }
    for (i = 0; i < 2u; ++i)
        BORROW_CHECK(!mvdm_softpc_wow_page_domain_window_handle(LOWORD(windows[i])));
    BORROW_CHECK(VdmQueryFreeVirtualMemory(&free_after, &largest) >= 0 && free_after == free_before);
    /* Native stability cannot prove copied traversal safety. Reject cycles
     * in each reader chain before allocating/publishing any guest object. */
    for (i = 0; i < 3u; ++i) {
        if (i == 0) { links[0].next = links[1].window; links[1].next = links[0].window; }
        if (i == 1) { links[0].parent = links[1].window; links[1].parent = links[0].window; }
        if (i == 2) { links[0].owner = links[1].window; links[1].owner = links[0].window; }
        observations[0].links = links[0]; observations[1].links = links[1];
        observations[0].descriptor = descriptors[0]; observations[1].descriptor = descriptors[1];
        BORROW_CHECK(!mvdm_softpc_wow_page_domain_project_window_graph(2, observations));
        BORROW_CHECK(GetLastError() == ERROR_INVALID_DATA);
        links[0].next = links[1].next = 0;
        links[0].parent = links[1].parent = 0;
        links[0].owner = links[1].owner = 0;
    }
    links[0].child = 0x12340001u;
    observations[0].links = links[0]; observations[1].links = links[1];
    BORROW_CHECK(!mvdm_softpc_wow_page_domain_project_window_graph(2, observations));
    BORROW_CHECK(GetLastError() == ERROR_INVALID_DATA);
    links[0].child = 0;
    BORROW_CHECK(VdmQueryFreeVirtualMemory(&free_after, &largest) >= 0 && free_after == free_before);
    /* Both descriptors are valid, but a complete view must include desktop.
     * Reject after preparation, without testing later native freshness. */
    SetLastError(0x1234);
    observations[0].links = links[0]; observations[1].links = links[1];
    BORROW_CHECK(!mvdm_softpc_wow_page_domain_project_window_graph(2, observations));
    BORROW_CHECK(GetLastError() == ERROR_INVALID_DATA);
    for (i = 0; i < 2u; ++i)
        BORROW_CHECK(!mvdm_softpc_wow_page_domain_window_handle(LOWORD(windows[i])));
    BORROW_CHECK(VdmQueryFreeVirtualMemory(&free_after, &largest) >= 0 && free_after == free_before);
cleanup:
    for (i = 0; i < 2u; ++i) if (windows[i]) DestroyWindow(windows[i]);
    fprintf(stderr, "WOW_GRAPH_PREPARATION_CCPU errors=%u descriptor_failure=1 missing_desktop=1 unpublished=1 released=1\n",
        borrow_errors - before);
}

#ifdef WOW_GRAPH_SNAPSHOT_FIXTURE
static void verify_terminal_failure(const char *option, const char *marker)
{
    char exe[MAX_PATH], command[4 * MAX_PATH], name[96], error_log[2 * MAX_PATH];
    HANDLE event;
    STARTUPINFOA startup = {sizeof(startup)};
    PROCESS_INFORMATION process = {0};
    DWORD code = 0;
    unsigned before = borrow_errors;
    sprintf_s(name, sizeof(name), "Local\\T447RemovedMessage-%lu", GetCurrentProcessId());
    event = CreateEventA(NULL, TRUE, FALSE, name);
    BORROW_CHECK(event != NULL);
    if (!event) return;
    GetModuleFileNameA(NULL, exe, sizeof(exe));
    sprintf_s(error_log, sizeof(error_log), "%s.%s.stderr.txt", exe, option + 2);
    sprintf_s(command, sizeof(command), "\"%s\" --terminal-log \"%s\" %s %s",
        exe, error_log, option, name);
    BORROW_CHECK(CreateProcessA(exe, command, NULL, NULL, FALSE, CREATE_NO_WINDOW,
        NULL, NULL, &startup, &process));
    if (process.hProcess) {
        if (WaitForSingleObject(process.hProcess, 20000) != WAIT_OBJECT_0) {
            BORROW_CHECK(FALSE); TerminateProcess(process.hProcess, 4);
            WaitForSingleObject(process.hProcess, 10000);
        }
        GetExitCodeProcess(process.hProcess, &code);
        fprintf(stderr, "WOW_TERMINAL_CHILD option=%s reached=%d exit=%08lx log=%s\n",
            option, WaitForSingleObject(event, 0) == WAIT_OBJECT_0, code, error_log);
        BORROW_CHECK(code == 0xc0000420u && WaitForSingleObject(event, 0) == WAIT_OBJECT_0);
        CloseHandle(process.hThread); CloseHandle(process.hProcess);
    }
    CloseHandle(event);
    fprintf(stderr, "%s errors=%u consumed_once=1 worker_exit=%08lx\n",
        marker, borrow_errors - before, code);
}

static void verify_class_retirement_failure(void)
{
    char path[MAX_PATH + 32], previous[32768], line[256];
    DWORD count;
    FILE *trace = NULL;
    BOOL native_removed = FALSE, terminal_release = FALSE;
    unsigned before = borrow_errors;
    if (!GetModuleFileNameA(NULL, path, MAX_PATH)) { BORROW_CHECK(FALSE); return; }
    strcat_s(path, sizeof(path), ".class-retirement.txt");
    DeleteFileA(path); /* This executable's disposable diagnostic only. */
    count = GetEnvironmentVariableA("MVDM_WOW_WINDOW_TRACE_PATH", previous, sizeof(previous));
    BORROW_CHECK(count < sizeof(previous));
    if (count >= sizeof(previous)) return;
    BORROW_CHECK(SetEnvironmentVariableA("MVDM_WOW_WINDOW_TRACE_PATH", path));
    verify_terminal_failure("--class-retirement-failure", "WOW_CLASS_RETIREMENT_TERMINAL_CCPU");
    BORROW_CHECK(SetEnvironmentVariableA("MVDM_WOW_WINDOW_TRACE_PATH", count ? previous : NULL));
    if (!fopen_s(&trace, path, "r") && trace) {
        while (fgets(line, sizeof(line), trace)) {
            if (strstr(line, "ClassRetirement") && strstr(line, "native-unregistered=1")) native_removed = TRUE;
            if (strstr(line, "stage=release-class-after-native-unregister")) terminal_release = TRUE;
        }
        fclose(trace);
    }
    BORROW_CHECK(native_removed && terminal_release);
    fprintf(stderr, "WOW_CLASS_RETIREMENT_FAILURE_CCPU errors=%u native_removed=1 stale_projection_rejected=1 terminal=1\n",
        borrow_errors - before);
}

static int external_window_churn(const char *stop_name, const char *ready_name, BOOL hold)
{
    HANDLE stop = OpenEventA(SYNCHRONIZE, FALSE, stop_name);
    HANDLE ready = OpenEventA(EVENT_MODIFY_STATE, FALSE, ready_name);
    ULONGLONG deadline = GetTickCount64() + 120000u;
    if (!stop || !ready) return 2;
    do {
        HWND window = CreateWindowExA(0, "STATIC", "External observation churn",
            WS_POPUP, 0, 0, 24, 24, NULL, NULL, GetModuleHandle(NULL), NULL);
        HWND child;
        if (!window) return 3;
        child = CreateWindowExA(0, "STATIC", "External child", WS_CHILD,
            0, 0, 8, 8, window, (HMENU)19, GetModuleHandle(NULL), NULL);
        SetEvent(ready);
        if (hold) WaitForSingleObject(stop, 120000);
        SetWindowPos(window, HWND_TOP, 4, 6, 32, 32, SWP_NOACTIVATE);
        if (child) DestroyWindow(child);
        DestroyWindow(window);
    } while (WaitForSingleObject(stop, 1) == WAIT_TIMEOUT && GetTickCount64() < deadline);
    CloseHandle(ready); CloseHandle(stop);
    return 0;
}

static void verify_external_observation_churn(BOOL hold)
{
    char exe[MAX_PATH], command[2 * MAX_PATH + 256], desktop[128], stop_name[96], ready_name[96];
    STARTUPINFOA startup = {sizeof(startup)};
    PROCESS_INFORMATION process = {0};
    HANDLE stop = NULL, ready = NULL;
    HWND local = NULL, root = GetDesktopWindow();
    DWORD needed, exit_code = 1;
    unsigned before = borrow_errors, iteration;
    sprintf_s(stop_name, sizeof(stop_name), "Local\\T447ObservationStop-%lu", GetCurrentProcessId());
    sprintf_s(ready_name, sizeof(ready_name), "Local\\T447ObservationReady-%lu", GetCurrentProcessId());
    stop = CreateEventA(NULL, TRUE, FALSE, stop_name);
    ready = CreateEventA(NULL, TRUE, FALSE, ready_name);
    BORROW_CHECK(stop && ready);
    if (!stop || !ready) goto cleanup;
    BORROW_CHECK(GetModuleFileNameA(NULL, exe, sizeof(exe)) != 0);
    BORROW_CHECK(GetUserObjectInformationA(GetThreadDesktop(GetCurrentThreadId()),
        UOI_NAME, desktop, sizeof(desktop), &needed));
    startup.lpDesktop = desktop;
    sprintf_s(command, sizeof(command), "\"%s\" %s %s %s", exe,
        hold ? "--external-window-hold" : "--external-window-churn", stop_name, ready_name);
    BORROW_CHECK(CreateProcessA(exe, command, NULL, NULL, FALSE, CREATE_NO_WINDOW,
        NULL, NULL, &startup, &process));
    if (!process.hProcess) goto cleanup;
    BORROW_CHECK(WaitForSingleObject(ready, 10000) == WAIT_OBJECT_0);
    local = CreateWindowExA(0, "STATIC", "Required local observation", WS_POPUP,
        0, 0, 24, 24, NULL, NULL, GetModuleHandle(NULL), NULL);
    BORROW_CHECK(local != NULL);
    if (!local) goto cleanup;
    for (iteration = 0; iteration < 32u; ++iteration) {
        ULONG backing;
        mvdm_softpc_wow_window_observation help_view;
        BORROW_CHECK(SetWindowContextHelpId(local, 1000u + iteration));
        BORROW_CHECK(SetWindowPos(local, NULL, 10 + iteration, 20, 24, 24,
            SWP_NOZORDER | SWP_NOACTIVATE));
        BORROW_CHECK(wow_user_window_project_graph(&local, 1));
        backing = mvdm_softpc_wow_page_domain_borrow_window((ULONG)(ULONG_PTR)local);
        BORROW_CHECK(backing && c_sas_dw_at(backing + 40u) == 10u + iteration);
        if (backing) {
            BORROW_CHECK(mvdm_softpc_wow_page_domain_read_window_view(backing, &help_view));
            BORROW_CHECK(help_view.context_help_id == 1000u + iteration);
            BORROW_CHECK(SetWindowContextHelpId(local, 2000u + iteration));
            /* A retained read cannot requery native HWND behind the
             * transaction. Only the next publication changes this value. */
            BORROW_CHECK(mvdm_softpc_wow_page_domain_read_window_view(backing, &help_view));
            BORROW_CHECK(help_view.context_help_id == 1000u + iteration);
            BORROW_CHECK(mvdm_softpc_wow_page_domain_release_window(backing));
        }
    }
    if (hold) {
        mvdm_softpc_wow_window_observation *view;
        ULONG count = 0, at, external = 0;
        BORROW_CHECK(mvdm_softpc_wow_page_domain_copy_window_view(0, NULL, &count));
        view = HeapAlloc(GetProcessHeap(), 0, count * sizeof(*view));
        BORROW_CHECK(view != NULL);
        if (view) {
            BORROW_CHECK(mvdm_softpc_wow_page_domain_copy_window_view(count, view, &count));
            for (at = 0; at < count; ++at) if (view[at].process_id == process.dwProcessId)
                ++external;
            BORROW_CHECK(external >= 2u);
            for (at = 0; at < count; ++at) {
                HWND foreign;
                ULONG backing, previous;
                mvdm_softpc_wow_window_observation retained;
                if (view[at].process_id != process.dwProcessId ||
                        (view[at].style & WS_CHILD)) continue;
                foreign = (HWND)(ULONG_PTR)view[at].links.window;
                previous = view[at].context_help_id;
                /* Original route-E contract: native resource authority may
                 * change without becoming a global freshness gate. Local
                 * publication retains the committed external observation;
                 * explicit discovery then synchronously refreshes it. */
                BORROW_CHECK(SetWindowContextHelpId(foreign, 0x74478201u));
                BORROW_CHECK(GetWindowContextHelpId(foreign) == 0x74478201u);
                BORROW_CHECK(wow_user_window_project_graph(&local, 1));
                backing = mvdm_softpc_wow_page_domain_borrow_window((ULONG)(ULONG_PTR)foreign);
                BORROW_CHECK(backing != 0);
                if (backing) {
                    BORROW_CHECK(mvdm_softpc_wow_page_domain_read_window_view(backing, &retained));
                    BORROW_CHECK(retained.context_help_id == previous);
                    BORROW_CHECK(mvdm_softpc_wow_page_domain_release_window(backing));
                }
                BORROW_CHECK(wow_user_window_project_graph(&foreign, 1));
                backing = mvdm_softpc_wow_page_domain_borrow_window((ULONG)(ULONG_PTR)foreign);
                BORROW_CHECK(backing != 0);
                if (backing) {
                    BORROW_CHECK(mvdm_softpc_wow_page_domain_read_window_view(backing, &retained));
                    BORROW_CHECK(retained.context_help_id == 0x74478201u &&
                        retained.process_id == process.dwProcessId);
                    BORROW_CHECK(mvdm_softpc_wow_page_domain_release_window(backing));
                }
                {
                    static wow_user_object_table objects;
                    wow_task_order_process owner = {0};
                    wow_task_order_thread thread = {0};
                    wow_task_order_entry task = {0};
                    wow_user_runtime runtime = WOW_USER_RUNTIME_INITIALIZER;
                    wow_user_task_lifecycle lifecycle = {0};
                    wow_user_runtime_thread binding = {0};
                    ZeroMemory(&objects, sizeof(objects));
                    owner.objects = &objects;thread.ppi = &owner;thread.ptdb = &task;
                    task.hTaskWow = 0x4321u;runtime.lifecycle = &lifecycle;
                    BORROW_CHECK(wow_user_runtime_bind(&binding, &runtime, &thread, NULL));
                    /* Explicit native mutation of a foreign operand must
                     * publish on return without a separate discovery call. */
                    BORROW_CHECK(wow_native_SetWindowContextHelpId(foreign, 0x74478202u));
                    backing = mvdm_softpc_wow_page_domain_borrow_window((ULONG)(ULONG_PTR)foreign);
                    BORROW_CHECK(backing != 0);
                    if (backing) {
                        BORROW_CHECK(mvdm_softpc_wow_page_domain_read_window_view(backing, &retained));
                        BORROW_CHECK(retained.context_help_id == 0x74478202u);
                        BORROW_CHECK(mvdm_softpc_wow_page_domain_release_window(backing));
                    }
                    BORROW_CHECK(wow_user_runtime_unbind(&binding));
                }
                break;
            }
            BORROW_CHECK(at < count);
            fprintf(stderr, "WOW_EXTERNAL_REFRESH_BOUNDARY_CCPU errors=%u retained=1 explicit_refresh=1 native_mutation=1 local_fresh=1\n",
                borrow_errors - before);
            SetEvent(stop);
            BORROW_CHECK(WaitForSingleObject(process.hProcess, 10000) == WAIT_OBJECT_0);
            /* Every captured external HWND is now dead. Committing this
             * observation must not query native USER and deny the local view. */
            for (at = 0; at < count; ++at) if (view[at].process_id == process.dwProcessId)
                BORROW_CHECK(!IsWindow((HWND)(ULONG_PTR)view[at].links.window));
            BORROW_CHECK(mvdm_softpc_wow_page_domain_project_window_graph(count, view));
            HeapFree(GetProcessHeap(), 0, view);
        }
    }
    {
        ULONG backing = mvdm_softpc_wow_page_domain_borrow_window((ULONG)(ULONG_PTR)local);
        mvdm_softpc_wow_window_observation help_view;
        BORROW_CHECK(backing != 0);
        BORROW_CHECK(DestroyWindow(local));
        local = NULL;
        BORROW_CHECK(wow_user_window_project_graph(&root, 1));
        if (backing) {
            /* Old generation remains readable after native destruction and
             * withdrawal; there is no native lookup in this retained read. */
            BORROW_CHECK(mvdm_softpc_wow_page_domain_read_window_view(backing, &help_view));
            BORROW_CHECK(help_view.context_help_id == 1031u);
            BORROW_CHECK(mvdm_softpc_wow_page_domain_release_window(backing));
        }
    }
cleanup:
    if (stop) SetEvent(stop);
    if (process.hProcess) {
        if (WaitForSingleObject(process.hProcess, 10000) != WAIT_OBJECT_0) {
            BORROW_CHECK(FALSE); TerminateProcess(process.hProcess, 4);
            WaitForSingleObject(process.hProcess, 10000);
        }
        GetExitCodeProcess(process.hProcess, &exit_code);
        BORROW_CHECK(exit_code == 0);
        CloseHandle(process.hThread); CloseHandle(process.hProcess);
    }
    if (local) DestroyWindow(local);
    BORROW_CHECK(wow_user_window_project_graph(&root, 1));
    if (stop) CloseHandle(stop);
    if (ready) CloseHandle(ready);
    if (hold) fprintf(stderr, "WOW_EXTERNAL_CAPTURE_CCPU errors=%u dead_native=1 committed_observation=1\n",
        borrow_errors - before);
    else fprintf(stderr, "WOW_EXTERNAL_OBSERVATION_CCPU errors=%u separate_process=1 local_updates=32\n",
        borrow_errors - before);
}

static void verify_local_observation_commit(void)
{
    HWND windows[2] = {0}, desktop = GetDesktopWindow();
    wow_window_words_binding *bindings[2] = {0};
    ULONG backings[2] = {0}, indices[2] = {~0u, ~0u};
    DWORD styles[2] = {0}, modules[2] = {0};
    mvdm_softpc_wow_window_observation *view = NULL;
    ULONG count = 0, i, j;
    WW words = {0};
    unsigned before = borrow_errors;
    for (i = 0; i < 2; ++i) {
        windows[i] = CreateWindowExA(0, "STATIC", "Local commit", WS_POPUP,
            0, 0, 24, 24, NULL, NULL, GetModuleHandleA(NULL), NULL);
        BORROW_CHECK(windows[i] != NULL);
        if (!windows[i]) goto done;
        BORROW_CHECK(wow_window_words_attach(windows[i], &words));
        bindings[i] = wow_window_words_acquire(windows[i]);
        BORROW_CHECK(bindings[i] != NULL);
        if (!bindings[i]) goto done;
    }
    BORROW_CHECK(wow_user_window_project_graph(windows, 2));
    BORROW_CHECK(mvdm_softpc_wow_page_domain_copy_window_view(0, NULL, &count));
    if (!count) goto done;
    view = HeapAlloc(GetProcessHeap(), 0, count * sizeof(*view));
    BORROW_CHECK(view != NULL);
    if (!view) goto done;
    BORROW_CHECK(mvdm_softpc_wow_page_domain_copy_window_view(count, view, &count));
    for (i = 0; i < 2; ++i) {
        for (j = 0; j < count; ++j)
            if (view[j].links.window == (ULONG)(ULONG_PTR)windows[i]) indices[i] = j;
        BORROW_CHECK(indices[i] != ~0u);
        if (indices[i] == ~0u) goto done;
        styles[i] = wow_window_words_value(bindings[i])->dwStyle;
        modules[i] = (DWORD)(ULONG_PTR)wow_window_words_value(bindings[i])->hInstance;
        backings[i] = mvdm_softpc_wow_page_domain_borrow_window((ULONG)(ULONG_PTR)windows[i]);
        BORROW_CHECK(backings[i] != 0);
        if (!backings[i]) goto done;
        /* Synthetic observations test the commit, not native style policy. */
        view[indices[i]].style ^= WS_DISABLED;
        view[indices[i]].module = 0x12340000u + i;
    }
    {
        WORD atom = view[indices[1]].descriptor.atom;
        view[indices[1]].descriptor.atom = 0;
        BORROW_CHECK(!wow_window_words_project_observed(count, view));
        for (i = 0; i < 2; ++i) {
            BORROW_CHECK(wow_window_words_value(bindings[i])->dwStyle == styles[i]);
            BORROW_CHECK((DWORD)(ULONG_PTR)wow_window_words_value(bindings[i])->hInstance == modules[i]);
            BORROW_CHECK(c_sas_dw_at(backings[i] + 168u) == styles[i]);
        }
        view[indices[1]].descriptor.atom = atom;
    }
    {
        unsigned position, repeat, pin_before = borrow_errors;
        /* Direct parent entry, not the managed wrapper's own validation.
         * Fail before any row is pinned and after earlier rows were pinned.
         * Repetition exposes an accidental release of raw, unowned record
         * associations by exhausting the independently held readers. */
        for (position = 0; position < 2u; ++position) {
            ULONG invalid = position ? count - 1u : 0u;
            WORD atom = view[invalid].descriptor.atom;
            view[invalid].descriptor.atom = 0;
            for (repeat = 0; repeat < 16u; ++repeat) {
                SetLastError(0x1234);
                BORROW_CHECK(!mvdm_softpc_wow_page_domain_project_window_graph(count, view));
                BORROW_CHECK(GetLastError() == ERROR_INVALID_PARAMETER);
                for (i = 0; i < 2u; ++i) {
                    mvdm_softpc_wow_window_observation held = {0};
                    BORROW_CHECK(mvdm_softpc_wow_page_domain_read_window_view(backings[i], &held));
                    BORROW_CHECK(held.links.window == (ULONG)(ULONG_PTR)windows[i] &&
                        held.style == styles[i]);
                    BORROW_CHECK(wow_window_words_value(bindings[i])->dwStyle == styles[i]);
                }
            }
            view[invalid].descriptor.atom = atom;
        }
        fprintf(stderr, "WOW_KNOWN_RECORD_ROLLBACK_CCPU errors=%u failures=32 positions=2 retained=2\n",
            borrow_errors - pin_before);
    }
    SetLastError(0x4473);
    BORROW_CHECK(wow_window_words_project_observed(count, view));
    BORROW_CHECK(GetLastError() == 0x4473);
    for (i = 0; i < 2; ++i) {
        BORROW_CHECK(wow_window_words_value(bindings[i])->dwStyle == view[indices[i]].style);
        BORROW_CHECK((DWORD)(ULONG_PTR)wow_window_words_value(bindings[i])->hInstance == view[indices[i]].module);
        BORROW_CHECK(c_sas_dw_at(backings[i] + 168u) == view[indices[i]].style);
    }
done:
    if (view) HeapFree(GetProcessHeap(), 0, view);
    for (i = 0; i < 2; ++i) {
        if (windows[i]) wow_window_words_detach(windows[i]);
        wow_window_words_release(bindings[i]);
        if (backings[i]) BORROW_CHECK(mvdm_softpc_wow_page_domain_release_window(backings[i]));
        if (windows[i]) BORROW_CHECK(DestroyWindow(windows[i]));
    }
    BORROW_CHECK(wow_user_window_project_graph(&desktop, 1));
    fprintf(stderr, "WOW_LOCAL_OBSERVATION_COMMIT_CCPU errors=%u rows=2 rollback=1 commit=1\n",
        borrow_errors - before);
}

static int verify_menu_projection(ULONG expected, ULONG excluded)
{
    mvdm_softpc_wow_window_observation *full = NULL;
    mvdm_softpc_wow_window_menu_observation *menus = NULL, sentinel = {0};
    ULONG count = 0, menu_count = 0, returned = 0, row;
    unsigned before = borrow_errors;
    BOOL found = FALSE;
    BORROW_CHECK(sizeof(sentinel) == 12u);
    BORROW_CHECK(mvdm_softpc_wow_page_domain_copy_window_view(0, NULL, &count));
    SetLastError(0x55bbu);
    BORROW_CHECK(mvdm_softpc_wow_page_domain_copy_window_menu_view(0, NULL, &menu_count));
    BORROW_CHECK(count == menu_count && count > 0 && GetLastError() == 0x55bbu);
    if (!count || count != menu_count) return 0;
    sentinel.window = 0x76543210u;
    sentinel.style = 0x12345678u; sentinel.process_id = 0x23456789u;
    BORROW_CHECK(!mvdm_softpc_wow_page_domain_copy_window_menu_view(0, &sentinel, &returned));
    BORROW_CHECK(GetLastError() == ERROR_INSUFFICIENT_BUFFER && returned == count);
    BORROW_CHECK(!mvdm_softpc_wow_page_domain_copy_window_menu_view(count - 1u, &sentinel, &returned));
    BORROW_CHECK(GetLastError() == ERROR_INSUFFICIENT_BUFFER && returned == count &&
        sentinel.window == 0x76543210u && sentinel.style == 0x12345678u &&
        sentinel.process_id == 0x23456789u);
    returned = 0x4479u;
    BORROW_CHECK(!mvdm_softpc_wow_page_domain_copy_window_menu_view(1, NULL, &returned));
    BORROW_CHECK(GetLastError() == ERROR_INVALID_PARAMETER && returned == 0x4479u);
    BORROW_CHECK(!mvdm_softpc_wow_page_domain_copy_window_menu_view(0, NULL, NULL));
    BORROW_CHECK(GetLastError() == ERROR_INVALID_PARAMETER);
    full = HeapAlloc(GetProcessHeap(), 0, count * sizeof(*full));
    menus = HeapAlloc(GetProcessHeap(), 0, (count + 1u) * sizeof(*menus));
    BORROW_CHECK(full && menus);
    if (full && menus) {
        menus[count] = sentinel;
        BORROW_CHECK(mvdm_softpc_wow_page_domain_copy_window_view(count, full, &returned));
        BORROW_CHECK(returned == count);
        SetLastError(0x55bbu);
        BORROW_CHECK(mvdm_softpc_wow_page_domain_copy_window_menu_view(count, menus, &returned));
        BORROW_CHECK(returned == count && GetLastError() == 0x55bbu &&
            !memcmp(&menus[count], &sentinel, sizeof(sentinel)));
        for (row = 0; row < count; ++row) {
            BORROW_CHECK(menus[row].window == full[row].links.window &&
                menus[row].style == full[row].style &&
                menus[row].process_id == full[row].process_id);
            if (row) BORROW_CHECK(LOWORD(menus[row - 1u].window) < LOWORD(menus[row].window));
            if (menus[row].window == expected) found = TRUE;
            if (excluded) BORROW_CHECK(menus[row].window != excluded);
        }
        BORROW_CHECK(found);
    }
    if (menus) HeapFree(GetProcessHeap(), 0, menus);
    if (full) HeapFree(GetProcessHeap(), 0, full);
    return borrow_errors == before;
}

static void verify_native_slot_reuse(void)
{
    static wow_user_object_table no_cleanup_owner;
    HWND old_window = NULL, replacement = NULL, desktop = GetDesktopWindow();
    ULONG old_full = 0, new_full = 0, held = 0, fresh = 0, previous, words[3];
    unsigned attempts = 0, before = borrow_errors, menu_phases = 0;
    ULONGLONG started;
    old_window = CreateWindowExA(0, "STATIC", "Reuse old", WS_POPUP,
        0, 0, 24, 24, NULL, NULL, GetModuleHandleA(NULL), NULL);
    BORROW_CHECK(old_window != NULL);
    if (!old_window) goto cleanup;
    old_full = (ULONG)(ULONG_PTR)old_window;
    BORROW_CHECK(wow_user_window_project_graph(&old_window, 1));
    {
        mvdm_softpc_wow_window_observation *view, sentinel;
        ULONG count = 0, returned = 0, row;
        BOOL found = FALSE;
        SetLastError(0x55bbu);
        BORROW_CHECK(mvdm_softpc_wow_page_domain_copy_window_view(0, NULL, &count));
        BORROW_CHECK(count > 1 && GetLastError() == 0x55bbu);
        ZeroMemory(&sentinel, sizeof(sentinel));
        sentinel.links.window = 0x76543210u;
        BORROW_CHECK(!mvdm_softpc_wow_page_domain_copy_window_view(1, &sentinel, &returned));
        BORROW_CHECK(GetLastError() == ERROR_INSUFFICIENT_BUFFER && returned == count &&
            sentinel.links.window == 0x76543210u);
        view = HeapAlloc(GetProcessHeap(), 0, count * sizeof(*view));
        BORROW_CHECK(view != NULL);
        if (view) {
            SetLastError(0x55bbu);
            BORROW_CHECK(mvdm_softpc_wow_page_domain_copy_window_view(count, view, &returned));
            BORROW_CHECK(returned == count && GetLastError() == 0x55bbu);
            for (row = 1; row < returned; ++row)
                BORROW_CHECK(LOWORD(view[row - 1u].links.window) < LOWORD(view[row].links.window));
            for (row = 0; row < returned; ++row) if (view[row].links.window == old_full) {
                found = TRUE;
                BORROW_CHECK(view[row].descriptor.atom != 0 &&
                    _stricmp(view[row].descriptor.name, "STATIC") == 0);
                BORROW_CHECK(view[row].style & WS_POPUP);
                BORROW_CHECK(view[row].process_id == GetCurrentProcessId() &&
                    view[row].thread_id == GetCurrentThreadId());
                BORROW_CHECK(view[row].module == (ULONG)(ULONG_PTR)GetModuleHandleA(NULL));
                BORROW_CHECK(view[row].links.parent == (ULONG)(ULONG_PTR)desktop);
                BORROW_CHECK(view[row].window_rect[2] - view[row].window_rect[0] == 24);
                {
                    DWORD original_style = view[row].style;
                    ULONG backing;
                    /* Synthetic copied observation, not a mutation of native
                     * window styles: original TestwndChild excludes a popup
                     * even when WS_CHILD is also set. Its menu slot must not
                     * be overwritten or exported as an arbitrary child ID. */
                    view[row].style |= WS_CHILD | WS_POPUP;
                    view[row].control_id = 0xfedcba98u;
                    BORROW_CHECK(mvdm_softpc_wow_page_domain_project_window_graph(returned, view));
                    backing = mvdm_softpc_wow_page_domain_borrow_window(old_full);
                    BORROW_CHECK(backing != 0);
                    if (backing) {
                        BORROW_CHECK(c_sas_dw_at(backing + 104u) == 0u);
                        BORROW_CHECK(mvdm_softpc_wow_page_domain_release_window(backing));
                    }
                    BORROW_CHECK(mvdm_softpc_wow_page_domain_window_menu(old_full, 0));
                    BORROW_CHECK(mvdm_softpc_wow_page_domain_copy_window_view(count, view, &returned));
                    BORROW_CHECK(view[row].links.window == old_full && view[row].control_id == 0);
                    view[row].style = original_style;
                    BORROW_CHECK(mvdm_softpc_wow_page_domain_project_window_graph(returned, view));
                }
            }
            BORROW_CHECK(found);
            HeapFree(GetProcessHeap(), 0, view);
        }
    }
    menu_phases += verify_menu_projection(old_full, 0);
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
        if (!rearm_observed_reuse_source(&old_full, &held, 0x76543210u)) {
            BORROW_CHECK(FALSE); break;
        }
    }
    BORROW_CHECK(replacement && LOWORD(new_full) == LOWORD(old_full) &&
        new_full != old_full && !IsWindow((HWND)(ULONG_PTR)old_full));
    if (!replacement || LOWORD(new_full) != LOWORD(old_full) ||
            new_full == old_full) goto cleanup;
    BORROW_CHECK(wow_user_window_project_graph(&replacement, 1));
    menu_phases += verify_menu_projection(new_full, old_full);
    BORROW_CHECK(wow_user_full_handle(&no_cleanup_owner, LOWORD(old_full)) == new_full);
    BORROW_CHECK(!mvdm_softpc_wow_page_domain_borrow_window(old_full));
    fresh = mvdm_softpc_wow_page_domain_borrow_window(new_full);
    BORROW_CHECK(fresh && fresh != held);
    BORROW_CHECK(mvdm_softpc_wow_page_domain_read_window_words(held, words) &&
        words[0] == 0x76543210u && c_sas_dw_at(held) == old_full);
    BORROW_CHECK(fresh && mvdm_softpc_wow_page_domain_read_window_words(fresh, words) &&
        words[0] == 0 && c_sas_dw_at(fresh) == new_full);
    {
        mvdm_softpc_wow_window_observation *view;
        ULONG count = 0, returned = 0, row;
        BOOL found = FALSE;
        /* Pinned retired storage must not appear beside its replacement in
         * the copied live view, even though both lifetime records still exist. */
        BORROW_CHECK(mvdm_softpc_wow_page_domain_copy_window_view(0, NULL, &count));
        view = HeapAlloc(GetProcessHeap(), 0, (count + 1u) * sizeof(*view));
        BORROW_CHECK(view != NULL);
        if (view) {
            ZeroMemory(&view[count], sizeof(*view));
            view[count].links.window = 0x76543210u;
            SetLastError(0x55bbu);
            BORROW_CHECK(mvdm_softpc_wow_page_domain_copy_window_view(count, view, &returned));
            BORROW_CHECK(returned == count && GetLastError() == 0x55bbu &&
                view[count].links.window == 0x76543210u);
            for (row = 0; row < returned; ++row) {
                BORROW_CHECK(view[row].links.window != old_full);
                if (view[row].links.window == new_full) found = TRUE;
                if (row) BORROW_CHECK(LOWORD(view[row - 1u].links.window) <
                    LOWORD(view[row].links.window));
            }
            BORROW_CHECK(found);
            HeapFree(GetProcessHeap(), 0, view);
        }
    }
cleanup:
    if (old_window) BORROW_CHECK(DestroyWindow(old_window));
    if (replacement) BORROW_CHECK(DestroyWindow(replacement));
    BORROW_CHECK(wow_user_window_project_graph(&desktop, 1));
    menu_phases += verify_menu_projection((ULONG)(ULONG_PTR)desktop, new_full);
    if (fresh) BORROW_CHECK(mvdm_softpc_wow_page_domain_release_window(fresh));
    if (held) BORROW_CHECK(mvdm_softpc_wow_page_domain_release_window(held));
    BORROW_CHECK(mvdm_softpc_wow_page_domain_collect_observed_classes());
    if (fresh) BORROW_CHECK(!mvdm_softpc_wow_page_domain_read_window_words(fresh, words));
    if (held) BORROW_CHECK(!mvdm_softpc_wow_page_domain_read_window_words(held, words));
    fprintf(stderr, "WOW_NATIVE_SLOT_REUSE_CCPU errors=%u attempts=%u old=%08lx new=%08lx old_backing=%08lx new_backing=%08lx\n",
        borrow_errors - before, attempts, old_full, new_full, held, fresh);
    fprintf(stderr, "WOW_MENU_PROJECTION_CCPU errors=%u phases=%u fields=3 capacity=1 retired=1\n",
        borrow_errors - before, menu_phases);
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

typedef struct property_fixture {
    HWND window;
    unsigned calls, atoms, strings;
    BOOL stop, raise, remove;
} property_fixture;

static int CALLBACK property_callback(HWND window, LPCSTR name, HANDLE data)
{
    property_fixture *scope = (property_fixture *)data;
    BORROW_CHECK(!wow_user_runtime_current()->exclusive_held);
    BORROW_CHECK(LOWORD(window) == LOWORD(scope->window));
    BORROW_CHECK(mvdm_softpc_wow_page_domain_window_handle(LOWORD(window)) ==
        (ULONG)(ULONG_PTR)scope->window);
    ++scope->calls;
    if (HIWORD(name)) {
        BORROW_CHECK(strcmp(name, "T447 property string") == 0);
        ++scope->strings;
    } else {
        BORROW_CHECK(LOWORD(name) == 0x1234);
        ++scope->atoms;
    }
    if (scope->remove) BORROW_CHECK(RemovePropA(window, name) == data);
    if (scope->raise) RaiseException(0xE0470014u, 0, 0, NULL);
    return scope->stop ? 0 : -7;
}

static void verify_property_discovery(BOOL held)
{
    property_fixture scope = {0};
    unsigned before = borrow_errors;
    BOOL caught = FALSE;
    DWORD error;
    int native;
    scope.window = CreateWindowA("STATIC", "Property discovery", WS_POPUP,
        0, 0, 32, 32, NULL, NULL, GetModuleHandle(NULL), NULL);
    BORROW_CHECK(scope.window != NULL);
    if (!scope.window) return;
    SetLastError(0x4567);
    native = EnumPropsA(scope.window, property_callback); error = GetLastError();
    SetLastError(0x4567);
    BORROW_CHECK(wow_native_EnumPropsA(scope.window, property_callback) == native);
    BORROW_CHECK(GetLastError() == error && scope.calls == 0);
    BORROW_CHECK(SetPropA(scope.window, "T447 property string", (HANDLE)&scope));
    BORROW_CHECK(SetPropA(scope.window, MAKEINTATOM(0x1234), (HANDLE)&scope));
    BORROW_CHECK(wow_native_EnumPropsA(scope.window, property_callback) == -7);
    BORROW_CHECK(scope.calls == 2 && scope.atoms == 1 && scope.strings == 1);
    BORROW_CHECK(wow_user_runtime_current()->exclusive_held == held);
    /* Negative witness: a short carrier cannot itself be a full graph root.
     * The discovery entrance must recover its already-published identity. */
    BORROW_CHECK(!wow_user_window_refresh_graph((HWND)(LONG_PTR)(SHORT)LOWORD(scope.window)));
    scope.stop = TRUE;
    BORROW_CHECK(wow_native_EnumPropsA((HWND)(LONG_PTR)(SHORT)LOWORD(scope.window), property_callback) == 0);
    BORROW_CHECK(scope.calls == 3 && wow_user_runtime_current()->exclusive_held == held);
    scope.stop = FALSE; scope.raise = TRUE;
    __try { (void)wow_native_EnumPropsA(scope.window, property_callback); }
    __except(GetExceptionCode() == 0xE0470014u ?
        EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) { caught = TRUE; }
    BORROW_CHECK(caught && scope.calls == 4 &&
        wow_user_runtime_current()->exclusive_held == held);
    scope.raise = FALSE; scope.remove = TRUE;
    BORROW_CHECK(wow_native_EnumPropsA(scope.window, property_callback) == -7);
    BORROW_CHECK(scope.calls == 6 && !GetPropA(scope.window, "T447 property string") &&
        !GetPropA(scope.window, MAKEINTATOM(0x1234)));
    BORROW_CHECK(DestroyWindow(scope.window));
    SetLastError(0x4567);
    native = EnumPropsA(scope.window, property_callback); error = GetLastError();
    SetLastError(0x4567);
    BORROW_CHECK(wow_native_EnumPropsA(scope.window, property_callback) == native);
    BORROW_CHECK(GetLastError() == error && scope.calls == 6 &&
        wow_user_runtime_current()->exclusive_held == held);
    fprintf(stderr, "WOW_PROPERTY_DISCOVERY_CCPU errors=%u held=%d atom=1 string=1 signed=1 stop=1 exception=1 removal=1 invalid=1\n",
        borrow_errors - before, held);
}

static void verify_native_window_thread_query(HWND window, BOOL dead, BOOL held)
{
    HWND forms[3] = {window, (HWND)(ULONG_PTR)LOWORD(window),
        (HWND)(LONG_PTR)(SHORT)LOWORD(window)};
    DWORD expected_pid = 0xcccccccc, expected_tid, expected_error;
    unsigned i, before = borrow_errors;
    SetLastError(0x4567);
    expected_tid = GetWindowThreadProcessId(window, &expected_pid);
    expected_error = GetLastError();
    if (dead) BORROW_CHECK(!expected_tid && expected_pid == 0xcccccccc);
    else BORROW_CHECK(expected_tid == GetCurrentThreadId() &&
        expected_pid == GetCurrentProcessId());
    for (i = 0; i < 3; ++i) {
        DWORD process = 0xcccccccc;
        SetLastError(0x4567);
        BORROW_CHECK(wow_native_GetWindowThreadProcessId(forms[i], &process) == expected_tid);
        BORROW_CHECK(process == expected_pid && GetLastError() == expected_error &&
            wow_user_runtime_current()->exclusive_held == held);
    }
    fprintf(stderr, "WOW_WINDOW_THREAD_QUERY_CCPU errors=%u held=%d dead=%d full=1 unsigned16=1 signed16=1 native_output=1\n",
        borrow_errors - before, held, dead);
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
    verify_property_discovery(held);
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
    verify_native_window_thread_query(scope.parent, FALSE, held);
cleanup:
    if (binding.exclusive_held) BORROW_CHECK(wow_user_runtime_leave(&binding));
    if (scope.parent) {
        BORROW_CHECK(DestroyWindow(scope.parent));
        if (held) BORROW_CHECK(wow_user_runtime_enter(&binding));
        verify_native_window_thread_query(scope.parent, TRUE, held);
        if (binding.exclusive_held) BORROW_CHECK(wow_user_runtime_leave(&binding));
    }
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
    {
        HWND roots[2] = {window, window};
        BORROW_CHECK(wow_user_window_project_graph(roots, 2));
    }
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

static BOOL verify_callback_borrow(HWND window, ULONG expected);
static void verify_observed_words_enrollment(void)
{
    wow_user_runtime runtime = WOW_USER_RUNTIME_INITIALIZER;
    wow_user_runtime_thread runtime_binding = {0};
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
    /* Public validation requires the same real execution-domain carrier
     * as its production caller. This row has no original task/callback,
     * so do not invent a TDB or register a success-only control provider. */
    if (!wow_user_runtime_bind(&runtime_binding, &runtime, NULL, NULL)) {
        BORROW_CHECK(FALSE);
        DestroyWindow(window);
        return;
    }
    BORROW_CHECK(GetWindowInfo(window, &info));
    BORROW_CHECK(VdmQueryFreeVirtualMemory(&before_free, &largest) >= 0);
    BORROW_CHECK(mvdm_softpc_wow_page_domain_observe_window(full,
        info.atomWindowType, GetClassLongA(window, GCL_STYLE),
        GetClassLongA(window, GCL_CBWNDEXTRA), "Static", sizeof("Static")));
    BORROW_CHECK(wow_user_borrow_enter(&outer));
    alias = wow_user_borrow_window(window);
    BORROW_CHECK(alias != NULL);
    BORROW_CHECK(GetPropW(window, L"NTVDM.WOW.WindowWords") == NULL);
    {
        struct tagWND *state;
        ULONG stale = ((HIWORD(full) == 1u ? 2u : 1u) << 16) | LOWORD(full);
        SetLastError(0x7654);
        state = wow_user_borrow_window_state(window);
        BORROW_CHECK(state != NULL && GetLastError() == 0x7654);
        BORROW_CHECK(wow_window_client_validate_window(window) == state);
        BORROW_CHECK(GetLastError() == 0x7654);
        BORROW_CHECK(wow_window_client_validate_window_no_rip(window) == state);
        BORROW_CHECK(wow_window_client_desktop() != NULL);
        BORROW_CHECK(wow_window_client_desktop() ==
            wow_window_client_validate_window(GetDesktopWindow()));
        BORROW_CHECK(!wow_window_words_owns_client_state(
            wow_user_borrow_observed_binding(window), state));
        BORROW_CHECK(wow_user_borrow_window_state((HWND)(ULONG_PTR)LOWORD(full)) == state);
        BORROW_CHECK(wow_user_borrow_window_state(
            (HWND)(ULONG_PTR)(0xffff0000u | LOWORD(full))) == state);
        BORROW_CHECK(wow_user_borrow_window_state((HWND)(ULONG_PTR)stale) == NULL);
        BORROW_CHECK(GetLastError() == ERROR_INVALID_WINDOW_HANDLE);
        BORROW_CHECK(wow_user_borrow_window_state(NULL) == NULL);
        BORROW_CHECK(GetLastError() == ERROR_INVALID_WINDOW_HANDLE);
        SetLastError(0x7654);
        BORROW_CHECK(wow_window_client_validate_window_no_rip((HWND)(ULONG_PTR)stale) == NULL);
        BORROW_CHECK(GetLastError() == 0x7654);
        BORROW_CHECK(wow_window_client_validate_window((HWND)(ULONG_PTR)stale) == NULL);
        BORROW_CHECK(GetLastError() == ERROR_INVALID_WINDOW_HANDLE);
    }
    initial.adwWOW[0] = 0x12345678u;
    BORROW_CHECK(wow_window_words_attach(window, &initial));
    BORROW_CHECK(alias && alias->adwWOW[0] == initial.adwWOW[0]);
    {
        wow_user_borrow_scope callback_outer = {0}, callback_inner = {0};
        ULONG backing = mvdm_softpc_wow_page_domain_borrow_window(full);
        BORROW_CHECK(backing != 0);
        BORROW_CHECK(wow_user_borrow_enter(&callback_outer));
        SetLastError(0x7654);
        BORROW_CHECK(wow_user_borrow_set_callback(window));
        BORROW_CHECK(GetLastError() == 0x7654 && verify_callback_borrow(window, backing));
        BORROW_CHECK(!wow_user_borrow_set_callback(window) &&
            GetLastError() == ERROR_ALREADY_EXISTS);
        BORROW_CHECK(wow_user_borrow_enter(&callback_inner));
        BORROW_CHECK(verify_callback_borrow(window, backing));
        BORROW_CHECK(wow_user_borrow_set_callback(window));
        BORROW_CHECK(wow_user_borrow_leave(&callback_inner));
        BORROW_CHECK(verify_callback_borrow(window, backing));
        BORROW_CHECK(wow_user_borrow_leave(&callback_outer));
        BORROW_CHECK(verify_callback_borrow(window, 0));
        if (backing) BORROW_CHECK(mvdm_softpc_wow_page_domain_release_window(backing));
    }
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
    BORROW_CHECK(wow_user_runtime_unbind(&runtime_binding));
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

static void trace_class_lifetime_view(const char *phase)
{
    ULONG count = 0, i;
    mvdm_softpc_wow_window_observation *rows;
    if (!mvdm_softpc_wow_page_domain_copy_window_view(0, NULL, &count)) return;
    rows = HeapAlloc(GetProcessHeap(), 0, count * sizeof(*rows));
    if (!rows) return;
    if (mvdm_softpc_wow_page_domain_copy_window_view(count, rows, &count)) {
        for (i = 0; i < count; ++i)
            fprintf(stderr, "WOW_CLASS_LIFETIME_VIEW phase=%s hwnd=%08lx live=%d class=%s next=%08lx parent=%08lx child=%08lx owner=%08lx\n",
                phase, rows[i].links.window,
                IsWindow((HWND)(ULONG_PTR)rows[i].links.window),
                rows[i].descriptor.name, rows[i].links.next,
                rows[i].links.parent, rows[i].links.child, rows[i].links.owner);
    }
    HeapFree(GetProcessHeap(), 0, rows);
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
    trace_class_lifetime_view("before-native-classes");
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
        {
            WORD matched = 0;
            WNDPROC canonical = NULL;
            WNDPROC selected_a = wow_control_client_procedure(entry->fnid, FALSE);
            WNDPROC selected_w = wow_control_client_procedure(entry->fnid, TRUE);
            BOOL list_alias = (entry->fnid == FNID_LISTBOX ||
                entry->fnid == FNID_COMBOLISTBOX) &&
                wow_control_client_procedure(FNID_LISTBOX, FALSE) ==
                wow_control_client_procedure(FNID_COMBOLISTBOX, FALSE);
            WNDCLASSEXW wide = {sizeof(wide)};
            if (entry->fnid == FNID_DIALOG) {
                selected_a = wow_dialog_window_procA;
                selected_w = wow_dialog_window_procW;
            }
            BORROW_CHECK(GetClassInfoExW(NULL, MAKEINTRESOURCEW(entry->atomClassName), &wide));
            if (!selected_a) selected_a = native_class.native.lpfnWndProc;
            if (!selected_w) selected_w = wide.lpfnWndProc;
            SetLastError(0x447a3001);
            /* /OPT:ICF may fold the two original identical list procedures
             * and their wrappers. An address alone then has no unique FNID;
             * rejection must leave both outputs and last-error unchanged. */
            if (list_alias) {
                BORROW_CHECK(wow_control_client_procedure(FNID_LISTBOX, TRUE) ==
                    wow_control_client_procedure(FNID_COMBOLISTBOX, TRUE));
                BORROW_CHECK(!wow_class_words_match_native(&classes,
                    selected_a, FALSE, &matched, &canonical));
                BORROW_CHECK(!wow_class_words_match_native(&classes,
                    selected_w, TRUE, &matched, &canonical));
                BORROW_CHECK(!matched && !canonical && GetLastError() == 0x447a3001);
            } else {
            BORROW_CHECK(wow_class_words_match_native(&classes,
                selected_a, FALSE, &matched, &canonical));
            BORROW_CHECK(GetLastError() == 0x447a3001 && matched == entry->fnid &&
                canonical == selected_a);
            BORROW_CHECK(wow_class_words_match_native(&classes,
                selected_w, TRUE, &matched, &canonical));
            BORROW_CHECK(matched == entry->fnid && canonical == selected_a);
            }
            /* Recovering a control must not silently classify its unrelated
             * modern implementation as the original canonical owner. */
            if (selected_a != native_class.native.lpfnWndProc)
                BORROW_CHECK(!wow_class_words_match_native(&classes,
                    native_class.native.lpfnWndProc, FALSE, &matched, &canonical));
            matched = 0x1234;
            canonical = (WNDPROC)(ULONG_PTR)1;
            BORROW_CHECK(!wow_class_words_match_native(&classes,
                (WNDPROC)(ULONG_PTR)1, FALSE, &matched, &canonical));
            BORROW_CHECK(matched == 0x1234 && canonical == (WNDPROC)(ULONG_PTR)1);
        }
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
    verify_native_class_creation(&classes, teb);
    BORROW_CHECK(wow_class_words_forget_native(&classes));
    BORROW_CHECK(classes.pclsPublicList == NULL);
    BORROW_CHECK(VdmQueryFreeVirtualMemory(&free_after, &largest) >= 0 && free_after == free_before);
    fprintf(stderr, "WOW_CLASS_ALLOCATION before=%lu after=%lu\n", free_before, free_after);
    if (free_before != free_after) trace_class_lifetime_view("after-native-classes");
    verify_unowned_window_projection(teb);
    verify_observed_window_promotion(teb);
    verify_native_class_window_ownership();
    verify_native_window_first_subclass();
    verify_released_class_borrow(teb);
    verify_class_collection_reference_set(teb);
    verify_observed_window_retirement();
    verify_dead_graph_retirement(teb);
    verify_graph_preparation_rollback();
#ifdef WOW_RECLAIM_FAILURE_FIXTURE
    verify_failed_window_reclamation();
    verify_failed_menu_reclamation(c_sas_dw_at(teb + 0x60u));
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
    cls.lpfnWndProc = wow_window_class_creation_proc;
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
    if (callback_failure_kind) {
        HANDLE signal = OpenEventA(EVENT_MODIFY_STATE, FALSE, native_failure_signal);
        if (!signal) goto cleanup;
        if (!strcmp(callback_failure_kind, "--native-entry-failure")) {
            wow_user_native_call call;
            if (!wow_user_runtime_enter(&binding)) { CloseHandle(signal); goto cleanup; }
            runtime.owner_thread_id ^= 1u;
            SetEvent(signal); CloseHandle(signal);
            (void)wow_user_native_call_begin(&call, NULL);
        } else if (!strcmp(callback_failure_kind, "--callback-owner-failure")) {
            wow_task_callback_scope scope = {0};
            /* Inject a lock already held at the actual return boundary.
             * Production leave must not report a normal callback result.
             * This tests restoration rejection, not entry/scheduler policy. */
            scope.binding = &binding;
            if (!wow_user_runtime_enter(&binding)) { CloseHandle(signal); goto cleanup; }
            SetEvent(signal); CloseHandle(signal);
            (void)wow_task_callback_leave(&scope);
        } else {
            wow_user_borrow_scope outer, inner;
            if (!wow_user_borrow_enter(&outer) || !wow_user_borrow_enter(&inner)) {
                CloseHandle(signal); goto cleanup;
            }
            SetEvent(signal); CloseHandle(signal);
            wow_user_borrow_leave_required(&outer);
        }
        /* Reaching this point is failure even if ordinary cleanup succeeds. */
        return FALSE;
    }
    {
        wow_user_native_call call;
        MSG message;
        step = "native-removed-message";
        PeekMessageA(&message, NULL, WM_APP + 93, WM_APP + 93, PM_NOREMOVE);
        if (!PostThreadMessageA(GetCurrentThreadId(), WM_APP + 93, 173u, 97u) ||
                !wow_user_native_call_begin(&call, NULL)) goto cleanup;
        if (thread.execution != &call.execution || wow_task_execution_ready(&thread) ||
                call.execution_held) goto cleanup;
        if (!PeekMessageA(&message, NULL, WM_APP + 93, WM_APP + 93, PM_REMOVE) ||
                message.wParam != 173u || message.lParam != 97u) goto cleanup;
        if (PeekMessageA(&message, NULL, WM_APP + 93, WM_APP + 93, PM_NOREMOVE)) goto cleanup;
        if (native_failure_signal) {
            HANDLE signal = OpenEventA(EVENT_MODIFY_STATE, FALSE, native_failure_signal);
            if (!signal) goto cleanup;
            SetEvent(signal); CloseHandle(signal);
            thread.ppi = NULL; /* Deliberate required-publication fault after consumption. */
        }
        if (!wow_user_native_call_end(&call) || native_failure_signal) goto cleanup;
        if (thread.execution || !wow_task_execution_ready(&thread)) goto cleanup;
        if (PeekMessageA(&message, NULL, WM_APP + 93, WM_APP + 93, PM_NOREMOVE)) goto cleanup;
        fprintf(stderr, "WOW_REMOVED_MESSAGE_CCPU consumed_once=1 restored=1 no_replay=1\n");
        fprintf(stderr, "WOW_RESOURCE_EXECUTION_SCOPE ready=1 restored=1 no_scheduler=1\n");
    }
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

static BOOL verify_callback_borrow(HWND window, ULONG expected)
{
    ULONG backing = 0xdeadbeefu;
    BOOL matched;
    SetLastError(0x7654);
    if (!mvdm_softpc_wow_page_domain_borrow_callback_window(
            (ULONG)(ULONG_PTR)window, &backing)) return FALSE;
    matched = backing == expected && GetLastError() == 0x7654;
    if (backing && !mvdm_softpc_wow_page_domain_release_window(backing)) return FALSE;
    return matched;
}

static BOOL verify_client_context_projection(ULONG teb, HWND window, ULONG backing)
{
    ULONG a[2] = {0}, b[2] = {0}, alias[2], rejected[2] = {0xdeadbeefu, 0xcafebabeu};
    ULONG pair[2] = {(ULONG)(ULONG_PTR)window, backing};
    ULONG bad[2] = {(ULONG)(ULONG_PTR)window ^ 0x10000u, backing};
    ULONG version = c_sas_dw_at(teb + 0x50u), cr3 = c_getCR3();
    ULONG self = c_sas_dw_at(teb + 0x18u), desktop = c_sas_dw_at(teb + 0x5cu);
    ULONG delta = c_sas_dw_at(teb + 0x60u);
    /* Actual CCPU/SAS view, including the currently retired but pinned WND.
     * The context snapshot never acquires or substitutes for its owner. */
    SetLastError(0x7654);
    if (!mvdm_softpc_wow_page_domain_exchange_client_context(a, 0x030au, NULL) ||
            GetLastError() != 0x7654 || a[0] != pair[0] || a[1] != pair[1] ||
            c_sas_dw_at(teb + 0x50u) != 0x030au ||
            c_sas_dw_at(teb + 0x6cu) || c_sas_dw_at(teb + 0x70u)) return FALSE;
    if (!mvdm_softpc_wow_page_domain_exchange_client_context(b, 0x0400u, a) ||
            b[0] || b[1] || c_sas_dw_at(teb + 0x50u) != 0x0400u ||
            !verify_callback_borrow(window, backing)) return FALSE;
    if (mvdm_softpc_wow_page_domain_exchange_client_context(rejected, 0x9999u, bad) ||
            GetLastError() != ERROR_INVALID_STATE || rejected[0] != 0xdeadbeefu ||
            rejected[1] != 0xcafebabeu || c_sas_dw_at(teb + 0x50u) != 0x0400u ||
            !verify_callback_borrow(window, backing)) return FALSE;
    if (!mvdm_softpc_wow_page_domain_exchange_client_context(a, 0x030au, b) ||
            a[0] != pair[0] || a[1] != pair[1]) return FALSE;
    CopyMemory(alias, a, sizeof(alias));
    if (!mvdm_softpc_wow_page_domain_exchange_client_context(alias, 0x0400u, alias) ||
            alias[0] || alias[1] || !verify_callback_borrow(window, backing)) return FALSE;
    c_setCR3(cr3 ^ 0x1000u);
    {
        BOOL invalid = mvdm_softpc_wow_page_domain_exchange_client_context(
            rejected, 0x9999u, NULL);
        c_setCR3(cr3);
        if (invalid || rejected[0] != 0xdeadbeefu || rejected[1] != 0xcafebabeu ||
                c_sas_dw_at(teb + 0x50u) != 0x0400u ||
                !verify_callback_borrow(window, backing)) return FALSE;
    }
    if (!mvdm_softpc_wow_page_domain_exchange_client_context(NULL, version, pair) ||
            c_sas_dw_at(teb + 0x18u) != self ||
            c_sas_dw_at(teb + 0x5cu) != desktop ||
            c_sas_dw_at(teb + 0x60u) != delta ||
            !verify_callback_borrow(window, backing)) return FALSE;
    fprintf(stderr, "WOW_CLIENT_CONTEXT_CCPU versions=2 suspended=1 retired=1 alias=1 rejected=2 shared=unchanged errors=0\n");
    return TRUE;
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
#ifdef WOW_WINDOW_BORROW_FIXTURE
    unsigned shutdown_disposals = 0;
    BOOL shutdown_storage_armed = FALSE;
    HWND shutdown_window = NULL;
#endif

    /* A worker domain is never a process-global fallback: without a bound
     * session or protected mode, admission must leave no published view. */
    stage = "unbound-rejection";
    {
        ULONG count = 0x4479u;
        if (mvdm_softpc_wow_page_domain_copy_window_menu_view(0, NULL, &count) ||
                GetLastError() != ERROR_INVALID_STATE || count != 0x4479u) goto done;
    }
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
    stage = "cold-window-copy";
    {
        mvdm_softpc_wow_window_observation cold;
        mvdm_softpc_wow_window_menu_observation menu;
        ULONG count = 0;
        SetLastError(0x55bbu);
        if (!mvdm_softpc_wow_page_domain_copy_window_view(1, &cold, &count) ||
                count != 1 || GetLastError() != 0x55bbu ||
                cold.links.window != (ULONG)(ULONG_PTR)GetDesktopWindow() ||
                cold.descriptor.atom != 0) goto done;
        SetLastError(0x55bbu);
        if (!mvdm_softpc_wow_page_domain_copy_window_menu_view(1, &menu, &count) ||
                count != 1 || GetLastError() != 0x55bbu ||
                menu.window != cold.links.window || menu.style != cold.style ||
                menu.process_id != cold.process_id) goto done;
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
    {
        mvdm_softpc_wow_menu_observation rows[2] = {
            {0x98760004u, 0u, 0u, 0u, 1u},
            {0x98760003u, 0u, 0u, 1u, 0u}
        };
        mvdm_softpc_wow_menu_query_item item = {0u, 0u, 4u, 0x98760004u};
        ULONG parent, child, items, delta = c_sas_dw_at(teb + 0x60u);
        stage = "guest-menu-external-edge-withdrawal";
        if (!mvdm_softpc_wow_page_domain_publish_menus(2, rows, 1, &item)) goto done;
        parent = c_sas_dw_at(handles + 3u * 16u) - delta;
        child = c_sas_dw_at(handles + 4u * 16u);
        items = c_sas_dw_at(parent + 56u) - delta;
        if (c_sas_dw_at(items + 12u) != child ||
                !mvdm_softpc_wow_page_domain_retire_menu(0x98760004u) ||
                c_sas_dw_at(items + 12u) != 0u ||
                !mvdm_softpc_wow_page_domain_retire_menu(0x98760003u)) goto done;
        fprintf(stderr, "WOW_MENU_EXTERNAL_EDGE_CCPU external_unlink=1 local_guard=1\n");
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
        mvdm_softpc_wow_window_observation retained_view;
        ULONG relation_backing;
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
        relation_backing = 0x12345678u;
        if (!mvdm_softpc_wow_page_domain_borrow_window_relation(child, 0, &relation_backing) ||
                relation_backing != sibling ||
                !mvdm_softpc_wow_page_domain_release_window(relation_backing) ||
                !mvdm_softpc_wow_page_domain_borrow_window_relation(child, 1, &relation_backing) ||
                relation_backing != parent ||
                !mvdm_softpc_wow_page_domain_release_window(relation_backing) ||
                !mvdm_softpc_wow_page_domain_borrow_window_relation(child, 2, &relation_backing) ||
                relation_backing) goto done;
        relation_backing = 0x12345678u;
        if (mvdm_softpc_wow_page_domain_borrow_window_relation(child, 4, &relation_backing) ||
                relation_backing != 0x12345678u) goto done;
        if (!mvdm_softpc_wow_page_domain_read_window_view(child, &retained_view) ||
                retained_view.links.window != 0x34560005u ||
                retained_view.links.parent != 0x23450002u ||
                retained_view.links.next != 0x34560006u) goto done;
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
        /* A retained old generation must not resolve the newly reused slot.
         * Its terminal links come from the existing withdrawal owner. */
        if (!mvdm_softpc_wow_page_domain_read_window_view(child, &retained_view) ||
                retained_view.links.window != 0x34560005u ||
                retained_view.links.parent != native_desktop ||
                retained_view.links.next || retained_view.links.child ||
                retained_view.links.owner) goto done;
        if (c_sas_dw_at(handles + 5u * 16u) - delta == child ||
            c_sas_dw_at(child) != 0x34560005u ||
            !mvdm_softpc_wow_page_domain_release_window(child) ||
            c_sas_dw_at(child) != 0x34560005u ||
            !mvdm_softpc_wow_page_domain_release_window(child) ||
            mvdm_softpc_wow_page_domain_release_window(child)) goto done;
        retained_view.links.window = 0x12345678u;
        if (mvdm_softpc_wow_page_domain_read_window_view(child, &retained_view) ||
                retained_view.links.window != 0x12345678u) goto done;
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
        {
            ULONG original_style = c_sas_dw_at(window_client + 168u);
            /* Synthetic carrier WND, not a live guest window: preserve the
             * original TestwndChild distinction for a nonempty menu edge. */
            c_sas_storedw(window_client + 168u,
                original_style | WS_CHILD | WS_POPUP);
            stage = "guest-menu-mixed-style-reference-retention";
            if (!mvdm_softpc_wow_page_domain_window_menu(0x23450002u, 0x45670003u) ||
                    mvdm_softpc_wow_page_domain_retire_menu(0x45670003u) ||
                    GetLastError() != ERROR_BUSY ||
                    c_sas_dw_at(window_client + 104u) != root_menu ||
                    !mvdm_softpc_wow_page_domain_window_menu(0x23450002u, 0u) ||
                    c_sas_dw_at(window_client + 104u) != 0u ||
                    !mvdm_softpc_wow_page_domain_window_menu(0x23450002u, 0x45670003u))
                goto done;
            c_sas_storedw(window_client + 168u, original_style);
        }
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
        {
            mvdm_softpc_wow_menu_observation trees[2] = {
                {0x45670004u, 0x4321u, 0u, 1u}, {0x45670003u, 0x4321u, 1u, 1u}
            };
            mvdm_softpc_wow_menu_query_item batch[2] = {
                {0u, MF_DISABLED, 17u, 0u}, {0u, 0u, 4u, 0x98760004u}
            };
            ULONG child_items = c_sas_dw_at(child_menu - delta + 56u) - delta;
            stage = "guest-menu-whole-tree-rollback";
            if (mvdm_softpc_wow_page_domain_publish_menus(2, trees, 2, batch) ||
                    c_sas_dw_at(child_items + 8u) != 7u ||
                    c_sas_dw_at(root_client + 56u) != item_array + delta) goto done;
            batch[1].submenu = 0x45670004u;
            stage = "guest-menu-whole-tree-commit";
            if (!mvdm_softpc_wow_page_domain_publish_menus(2, trees, 2, batch)) goto done;
            child_items = c_sas_dw_at(child_menu - delta + 56u) - delta;
            item_array = c_sas_dw_at(root_client + 56u) - delta;
            if (c_sas_dw_at(child_items + 8u) != 17u ||
                    c_sas_dw_at(item_array + 12u) != child_menu) goto done;
        }
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
#ifdef WOW_WINDOW_BORROW_FIXTURE
        /* Original cleanup caches its process-wide restore table on first
         * entry, including menu-only cleanup. Supply the production table
         * before that entry; a zeroed fixture table would poison later
         * original-control module-unload tests in this same process. */
        if (!wow_standard_classes_bind_cleanup(&cleanup)) goto done;
#endif
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
        !verify_callback_borrow(geometry_window, retained_backing) ||
        !verify_callback_borrow(NULL, 0) ||
        c_sas_dw_at(teb + 0x6cu) != (ULONG)(ULONG_PTR)geometry_window ||
        c_sas_dw_at(teb + 0x70u) != retained_backing ||
        !wow_user_window_publish(&object_table, geometry_window,
            &object_storage, (struct wow_task_order_thread *)&thread_storage) ||
        c_sas_dw_at(entry) - c_sas_dw_at(teb + 0x60u) == retained_backing ||
        !mvdm_softpc_wow_page_domain_callback_window(
            (ULONG)(ULONG_PTR)geometry_window, inner_callback) ||
        c_sas_dw_at(teb + 0x70u) == retained_backing ||
        !verify_callback_borrow(geometry_window, c_sas_dw_at(teb + 0x70u)) ||
        !mvdm_softpc_wow_page_domain_restore_callback(inner_callback) ||
        c_sas_dw_at(teb + 0x70u) != retained_backing ||
        !verify_client_context_projection(teb, geometry_window, retained_backing) ||
        !verify_callback_borrow(geometry_window, retained_backing) ||
        !wow_user_window_retire(&object_table, geometry_window,
            &object_storage, NULL) ||
        c_sas_dw_at(retained_backing) != (ULONG)(ULONG_PTR)geometry_window ||
        !mvdm_softpc_wow_page_domain_restore_callback(outer_callback) ||
        c_sas_dw_at(teb + 0x6cu) != 0u || c_sas_dw_at(teb + 0x70u) != 0u ||
        !verify_callback_borrow(geometry_window, 0) ||
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
#ifdef WOW_ORIGINAL_MODAL_FIXTURE
    /* One process-level registered owner per test process. Do not insert a
     * fresh process lifetime inside the unrelated class-allocation matrix. */
    stage="registered-original-modal";
    verify_original_modal_task();
    if(borrow_errors)goto done;
    fprintf(stderr,"WOW_ORIGINAL_MODAL_SCOPE real_ccpu=1 registered_owner=1 normal_cleanup=1\n");
    passed=1;goto done;
#endif
    stage = "production-class-registration";
    if (!verify_class_registration(teb)) goto done;
    stage = "native-class-projection";
    if (!verify_native_class_projection(teb)) goto done;
    stage = "production-window-borrow";
    if (!verify_callback_scope(teb)) goto done;
#ifdef WOW_GRAPH_SNAPSHOT_FIXTURE
    stage = "whole-graph-snapshot";
    verify_graph_snapshot_commit(teb);
    verify_local_observation_commit();
    verify_native_slot_reuse();
    verify_external_observation_churn(FALSE);
    verify_external_observation_churn(TRUE);
    verify_terminal_failure("--post-native-failure", "WOW_POST_NATIVE_FAILURE_CCPU");
    verify_terminal_failure("--native-entry-failure", "WOW_NATIVE_ENTRY_FAILURE_CCPU");
    verify_terminal_failure("--callback-owner-failure", "WOW_CALLBACK_OWNER_FAILURE_CCPU");
    verify_terminal_failure("--callback-borrow-failure", "WOW_CALLBACK_BORROW_FAILURE_CCPU");
    verify_terminal_failure("--window-cache-failure", "WOW_WINDOW_CACHE_FAILURE_CCPU");
    verify_terminal_failure("--dialog-cache-failure", "WOW_DIALOG_CACHE_FAILURE_CCPU");
    verify_class_retirement_failure();
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
#ifdef WOW_STANDARD_CLASS_FIXTURE
    {
        extern int __cdecl wow_standard_class_fixture(void);
        stage = "standard-class-owner";
        if (wow_standard_class_fixture()) goto done;
    }
#endif
#ifdef WOW_CLASS_PROCEDURE_FIXTURE
    {
        extern int __cdecl wow_class_procedure_fixture(void);
        stage = "class-procedure-owner";
        if (wow_class_procedure_fixture()) goto done;
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
#ifdef WOW_WINDOW_BORROW_FIXTURE
    {
        ULONG backing;
        stage = "host-storage-domain-exit";
        /* Original control graph traversal now owns the desktop's host WND.
         * Do not overwrite that owner with a disposal witness. A standalone
         * observed HWND supplies unused storage for the same domain-exit
         * assertion, remaining live until after session disposal. */
        shutdown_window = CreateWindowExA(0, "STATIC", "Domain exit witness",
            WS_POPUP, 0, 0, 24, 24, NULL, NULL, GetModuleHandle(NULL), NULL);
        if (!shutdown_window) goto done;
        stage = "host-storage-domain-exit-project";
        fprintf(stderr, "WOW_DOMAIN_EXIT_WITNESS hwnd=%08lx prior=%08lx\n",
            (ULONG)shutdown_window,
            mvdm_softpc_wow_page_domain_window_handle(LOWORD(shutdown_window)));
        /* Native-only test windows may have retired since the last callback
         * boundary and USER may reuse their slot. Use the production graph
         * transaction to reconcile generations, not the low-level observation
         * helper whose precondition is an empty or same-generation slot. */
        if (!wow_user_window_project_graph(&shutdown_window, 1) ||
                mvdm_softpc_wow_page_domain_window_handle(LOWORD(shutdown_window)) !=
                    (ULONG)shutdown_window) goto done;
        stage = "host-storage-domain-exit-borrow";
        backing = mvdm_softpc_wow_page_domain_borrow_window((ULONG)shutdown_window);
        if (!backing) goto done;
        stage = "host-storage-domain-exit-bind";
        fprintf(stderr, "WOW_DOMAIN_EXIT_STORAGE backing=%08lx existing=%p\n",
            backing, mvdm_softpc_wow_page_domain_window_host_storage(backing));
        if (!mvdm_softpc_wow_page_domain_bind_window_host_storage(backing,
                &host_storage_cookie, dispose_host_storage_witness)) {
            (void)mvdm_softpc_wow_page_domain_release_window(backing);
            goto done;
        }
        shutdown_disposals = host_storage_disposals;
        shutdown_storage_armed = TRUE;
        stage = "host-storage-domain-exit-release";
        if (!mvdm_softpc_wow_page_domain_release_window(backing) ||
                host_storage_disposals != shutdown_disposals) goto done;
    }
#endif
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
#ifdef WOW_WINDOW_BORROW_FIXTURE
    if (shutdown_storage_armed && host_storage_disposals != shutdown_disposals + 1)
        passed = 0;
    if (shutdown_window && !DestroyWindow(shutdown_window)) passed = 0;
#endif
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

int main(int argc, char **argv)
{
    if (argc == 5 && !strcmp(argv[1], "--terminal-log")) {
        FILE *output;
        /* Test-only log protocol: retain CreateProcess(FALSE), original
         * fault event, injection and exit assertions without inheriting any
         * unrelated parent handles. Capture startup failures as well. */
        if (freopen_s(&output, argv[2], "w", stderr)) return 9;
        argv[1] = argv[3]; argv[2] = argv[4]; argc = 3;
    }
#ifdef WOW_GRAPH_SNAPSHOT_FIXTURE
    {
        DWORD result = 0x447a2001, error;
        BOOL accepted;
        /* Exercise the actual exported original's pre-frame rejection.
         * Invalid size must be refused before reading even a NULL argument
         * buffer, changing the output, or requiring any guest context. */
        SetLastError(ERROR_SUCCESS);
        accepted = WOWCallback16Ex(0, 0, MAXDWORD, NULL, &result);
        error = GetLastError();
        if (accepted || error != ERROR_INVALID_PARAMETER || result != 0x447a2001)
            return 12;
        fprintf(stderr, "WOW_EXPORTED_CALLBACK_REJECT errors=0 output_untouched=1 no_guest_entry=1\n");
    }
    if (argc == 4 && (strcmp(argv[1], "--external-window-churn") == 0 ||
            strcmp(argv[1], "--external-window-hold") == 0))
        return external_window_churn(argv[2], argv[3], strcmp(argv[1], "--external-window-hold") == 0);
    if (argc == 3 && strcmp(argv[1], "--post-native-failure") == 0) {
        native_failure_signal = argv[2];
        SetErrorMode(SEM_NOGPFAULTERRORBOX | SEM_FAILCRITICALERRORS);
    }
    if (argc == 3 && !strcmp(argv[1], "--class-retirement-failure")) {
        class_failure_signal = argv[2];
        SetErrorMode(SEM_NOGPFAULTERRORBOX | SEM_FAILCRITICALERRORS);
    }
    if (argc == 3 && (!strcmp(argv[1], "--native-entry-failure") ||
            !strcmp(argv[1], "--callback-owner-failure") ||
            !strcmp(argv[1], "--callback-borrow-failure"))) {
        native_failure_signal = argv[2];
        callback_failure_kind = argv[1];
        SetErrorMode(SEM_NOGPFAULTERRORBOX | SEM_FAILCRITICALERRORS);
    }
    if (argc == 3 && (!strcmp(argv[1], "--window-cache-failure") ||
            !strcmp(argv[1], "--dialog-cache-failure"))) {
        callback_cache_signal = argv[2];
        dialog_cache_failure = !strcmp(argv[1], "--dialog-cache-failure");
        SetErrorMode(SEM_NOGPFAULTERRORBOX | SEM_FAILCRITICALERRORS);
    }
#endif
#ifdef WOW_ORIGINAL_MODAL_FIXTURE
    {
        WCHAR name[80];PROCESS_INFORMATION child={0};
        HDESK desktop;DWORD result=3;
        if(argc==2 && !strcmp(argv[1],"--modal-worker"))return run_fixture();
        wsprintfW(name,L"T447Modal-%lu",GetCurrentProcessId());
        desktop=CreateDesktopW(name,NULL,NULL,0,GENERIC_ALL,NULL);
        if(!desktop)return 2;
        /* The parent never joins the test desktop. A complete child-process
         * lifetime also bounds USER/UxTheme's auxiliary GUI threads. */
        {
            STARTUPINFOW wide={sizeof(wide)};WCHAR executable[MAX_PATH],command[MAX_PATH+40];
            if(!GetModuleFileNameW(NULL,executable,MAX_PATH)){CloseDesktop(desktop);return 3;}
            swprintf_s(command,ARRAYSIZE(command),L"\"%ls\" --modal-worker",executable);
            wide.lpDesktop=name;wide.dwFlags=STARTF_USESTDHANDLES|STARTF_USESHOWWINDOW;
            wide.wShowWindow=SW_HIDE;wide.hStdInput=GetStdHandle(STD_INPUT_HANDLE);
            wide.hStdOutput=GetStdHandle(STD_OUTPUT_HANDLE);wide.hStdError=GetStdHandle(STD_ERROR_HANDLE);
            if(CreateProcessW(executable,command,NULL,NULL,TRUE,CREATE_NO_WINDOW,NULL,NULL,&wide,&child)){
                if(WaitForSingleObject(child.hProcess,55000)!=WAIT_OBJECT_0){
                    TerminateProcess(child.hProcess,ERROR_TIMEOUT);WaitForSingleObject(child.hProcess,5000);
                }
                GetExitCodeProcess(child.hProcess,&result);CloseHandle(child.hThread);CloseHandle(child.hProcess);
            }
        }
        if(!CloseDesktop(desktop))result=4;
        return (int)result;
    }
#elif defined(WOW_WINDOW_BORROW_FIXTURE)
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
