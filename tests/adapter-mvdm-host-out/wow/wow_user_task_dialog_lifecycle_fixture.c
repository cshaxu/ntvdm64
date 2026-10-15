#include "wow_dialog_creation_binding.h"
#include "wow_user_private_access.h"
#include "wow_user_runtime.h"
#include "wow_user_task_lifecycle.h"
#include "wow_window_words_binding.h"
#include "wow_window_creation_binding.h"
#include "ntvdm-exe/session/session.h"
#include "opennt-abi/host-compat/include/wow32_provider_private.h"

#include <stdio.h>
#include <string.h>

static unsigned errors;
static unsigned init_messages;
static unsigned terminal_messages;
static unsigned short published_index;
static BOOL fail_publication, veto_creation;
static wow_window_words_binding *failed_borrow;
static BOOL failed_reference;
static unsigned rollback_cases;

#define CHECK(value) do { if (!(value)) { ++errors; \
    fprintf(stderr, "FAIL line=%d error=%lu\n", __LINE__, GetLastError()); } } while (0)

/* This native lifecycle fixture has no CCPU page domain. Keep that boundary
 * explicit: real publication is covered by page-domain and guest tests.
 * Check paired publication/retirement rather than pretending to map memory. */
int __cdecl mvdm_softpc_wow_page_domain_publish_handle(unsigned short index,
    unsigned short uniqueness, unsigned char type, unsigned char flags,
    unsigned long window, unsigned long class_server)
{
    (void)uniqueness; (void)type; (void)flags; (void)class_server;
    CHECK(index && !published_index && index == LOWORD(window));
    if (fail_publication) {
        /* publish_handle runs under the production publication lock. Do not
         * reenter acquire here. Observe only the allocation identity; never
         * dereference it after the failed creation returns. */
        failed_borrow = (wow_window_words_binding *)GetPropW((HWND)window,
            L"NTVDM.WOW.WindowWords");
        failed_reference = FALSE;
        CHECK(failed_borrow != NULL);
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return 0;
    }
    published_index = index;
    return 1;
}

static BOOL allocation_live(PVOID address)
{
    PROCESS_HEAP_ENTRY entry = {0};
    HANDLE heap = GetProcessHeap();
    BOOL found = FALSE;
    CHECK(HeapLock(heap));
    while (HeapWalk(heap, &entry)) {
        if (entry.lpData == address && (entry.wFlags & PROCESS_HEAP_ENTRY_BUSY)) {
            found = TRUE;
            break;
        }
    }
    CHECK(HeapUnlock(heap));
    return found;
}

static void verify_failed_borrow(void)
{
    PVOID allocation = failed_borrow;
    CHECK(allocation != NULL);
    if (!allocation) return;
    if (failed_reference) {
        CHECK(allocation_live(allocation));
        wow_window_words_release(failed_borrow);
    }
    failed_borrow = NULL;
    /* This verifies physical WW release, not merely disappearance of HWND.
     * HeapWalk never dereferences the potentially freed allocation. */
    CHECK(!allocation_live(allocation));
    ++rollback_cases;
}

static LRESULT CALLBACK veto_hook(int code, WPARAM wp, LPARAM lp)
{
    if (code == HCBT_CREATEWND && veto_creation) {
        char name[16];
        if (GetClassNameA((HWND)wp, name, ARRAYSIZE(name)) &&
                (!lstrcmpiA(name, "#32770") || !lstrcmpiA(name, "S2Rollback"))) {
            failed_borrow = wow_window_words_acquire((HWND)wp);
            failed_reference = TRUE;
            CHECK(failed_borrow != NULL);
            CHECK(published_index == LOWORD(wp));
            return 1;
        }
    }
    return CallNextHookEx(NULL, code, wp, lp);
}

static void window_rollback_contract(wow_user_runtime_thread *binding)
{
    static const WCHAR text[] = L"S2Rollback";
    UNICODE_STRING name = {sizeof(text) - sizeof(WCHAR), sizeof(text), (PWSTR)text};
    wow_class_lookup_context *classes = binding->thread->ppi->classes;
    WNDCLASSEXA cls = {sizeof(cls)};
    wow_window_dispatch_target target = {(DWORD)DefWindowProcA, NULL};
    CREATESTRUCTA request = {0};
    WW words = {0};
    DWORD class_words[2] = {0};
    ATOM atom;
    HWND window;
    HHOOK veto;
    cls.hInstance = GetModuleHandleA(NULL);
    cls.lpfnWndProc = wow_window_native_proc;
    cls.lpszClassName = "S2Rollback";
    atom = wow_class_words_register_target(classes, &cls, &name,
        class_words, NULL, &target);
    CHECK(atom != 0);
    if (!atom) return;
    request.lpszClass = cls.lpszClassName;
    request.lpszName = "";
    request.hInstance = cls.hInstance;
    request.style = WS_POPUP;
    request.cx = request.cy = 16;
    window = wow_window_create_boundA(binding->thread, atom, &request, &words, NULL);
    CHECK(window != NULL && published_index == LOWORD(window));
    CHECK(DestroyWindow(window) && !published_index);
    fail_publication = TRUE;
    window = wow_window_create_boundA(binding->thread, atom, &request, &words, NULL);
    CHECK(!window && !published_index);
    verify_failed_borrow();
    fail_publication = FALSE;
    veto = SetWindowsHookExA(WH_CBT, veto_hook, NULL, GetCurrentThreadId());
    CHECK(veto != NULL);
    veto_creation = TRUE;
    window = wow_window_create_boundA(binding->thread, atom, &request, &words, NULL);
    CHECK(!window && !published_index);
    verify_failed_borrow();
    veto_creation = FALSE;
    CHECK(UnhookWindowsHookEx(veto));
    CHECK(wow_class_words_unregister(classes, atom, cls.hInstance, NULL));
}
int __cdecl mvdm_softpc_wow_page_domain_retire_handle(unsigned short index,
    unsigned long *retained_backing)
{
    CHECK(index && index == published_index);
    published_index = 0;
    if (retained_backing) *retained_backing = 0;
    return 1;
}

/* Link-time poison for unselected functions in the unchanged wcall32.c
 * translation unit.  /Gy does not prevent MSVC from requiring their symbols,
 * but no test is permitted to turn one into a success stub. */
static DWORD WINAPI unexpected1(DWORD value)
{
    UNREFERENCED_PARAMETER(value);
    ExitProcess(90);
}
static DWORD WINAPI unexpected2(DWORD first, DWORD second)
{
    UNREFERENCED_PARAMETER(first);
    UNREFERENCED_PARAMETER(second);
    ExitProcess(90);
}
static DWORD WINAPI unexpected3(DWORD first, DWORD second, DWORD third)
{
    UNREFERENCED_PARAMETER(first);
    UNREFERENCED_PARAMETER(second);
    UNREFERENCED_PARAMETER(third);
    ExitProcess(90);
}
static DWORD WINAPI unexpected4(DWORD first, DWORD second, DWORD third,
    DWORD fourth)
{
    UNREFERENCED_PARAMETER(first);
    UNREFERENCED_PARAMETER(second);
    UNREFERENCED_PARAMETER(third);
    UNREFERENCED_PARAMETER(fourth);
    ExitProcess(90);
}
DWORD WINAPI GetPModeVDMPointerAssert(DWORD value) { return unexpected1(value); }
DWORD WINAPI LocalAlloc16(DWORD a, DWORD b, DWORD c) { return unexpected3(a,b,c); }
DWORD WINAPI LocalReAlloc16(DWORD a, DWORD b, DWORD c) { return unexpected3(a,b,c); }
DWORD WINAPI LocalLock16(DWORD value) { return unexpected1(value); }
DWORD WINAPI LocalUnlock16(DWORD value) { return unexpected1(value); }
DWORD WINAPI LocalSize16(DWORD value) { return unexpected1(value); }
DWORD WINAPI LocalFree16(DWORD value) { return unexpected1(value); }
DWORD WINAPI CallBack16(DWORD a, DWORD b, DWORD c, DWORD d) { return unexpected4(a,b,c,d); }
DWORD WINAPI malloc16(DWORD value) { return unexpected1(value); }
DWORD WINAPI free16(DWORD value) { return unexpected1(value); }
DWORD WINAPI ParamMapUpdateNode(DWORD a, DWORD b, DWORD c) { return unexpected3(a,b,c); }
DWORD WINAPI WOWGlobalAllocLock16(DWORD a, DWORD b, DWORD c) { return unexpected3(a,b,c); }
DWORD WINAPI WOWGlobalUnlockFree16(DWORD value) { return unexpected1(value); }

static VOID WINAPI end_task(void)
{
    ++errors;
}

static INT_PTR CALLBACK dialog_proc(HWND window, UINT message, WPARAM wp,
    LPARAM lp)
{
    wow_user_runtime_thread *binding = wow_user_runtime_current();
    UNREFERENCED_PARAMETER(wp);
    CHECK(binding && binding->thread && !binding->exclusive_held);
    CHECK(binding->thread->ppi->pwpi->CSOwningThread == binding->thread);
    if (message == WM_INITDIALOG) {
        ++init_messages;
        CHECK(lp == 0x12345678);
        CHECK(wow_user_get_window_longA(window, GWL_WOWDWORD1) ==
            (LONG)MAKELONG(10, 1));
        CHECK(wow_user_get_window_longA(window, GWL_WOWDWORD3) ==
            (LONG)0x43210076);
        CHECK(GetDlgItem(window, 100) != NULL);
    }
    if (message == WM_NCDESTROY) {
        wow_window_words_binding *borrow;
        ++terminal_messages;
        borrow = wow_window_words_acquire(window);
        CHECK(borrow != NULL);
        wow_window_words_release(borrow);
    }
    if (message == WM_APP + 2) CHECK(DestroyWindow(window));
    return FALSE;
}

int __cdecl main(void)
{
    session owner;
    wow_user_runtime runtime = WOW_USER_RUNTIME_INITIALIZER;
    wow_user_runtime_thread binding = {0};
    wow_user_task_lifecycle lifecycle = {0};
    wow_user_task_lifecycle_callbacks callbacks = {0};
    HANDLE wowexec = CreateEventW(NULL, FALSE, FALSE, NULL);
    DWORD template_storage[128] = {0};
    DLGTEMPLATE *dialog = (DLGTEMPLATE *)template_storage;
    DLGITEMTEMPLATE *item;
    WORD *next;
    DWORD dialog_data[2] = { 0x43210076u, 0x12345678u };
    HWND window;
    HHOOK veto;

    session_initialize(&owner, 42041u);
    CHECK(session_activate(&owner));
    CHECK(session_thread_bind_owned(&owner,
        SESSION_THREAD_BINDING_ORIGINAL_WORKER));
    CHECK(wow_user_runtime_initialize(&runtime));
    CHECK(wow_user_runtime_bind(&binding, &runtime, NULL, NULL));
    CHECK(wow_user_task_lifecycle_initialize(&lifecycle, &runtime, &callbacks));
    CHECK(wow_user_task_lifecycle_register(&lifecycle, end_task, wowexec));
    CHECK(wow_user_task_lifecycle_init(&lifecycle, 0x0400, "DIALOG.EXE",
        0x1235, 0, 0, (DWORD)CW_USEDEFAULT, (DWORD)CW_USEDEFAULT,
        (DWORD)CW_USEDEFAULT, (DWORD)CW_USEDEFAULT, SW_SHOW));

    dialog->style = WS_POPUP | DS_SETFONT;
    dialog->cdit = 1;
    dialog->cx = 100;
    dialog->cy = 40;
    next = (WORD *)(dialog + 1);
    *next++ = 0;
    *next++ = 0;
    *next++ = 0;
    *next++ = 8;
    memcpy(next, L"MS Shell Dlg", sizeof(L"MS Shell Dlg"));
    next += ARRAYSIZE(L"MS Shell Dlg");
    item = (DLGITEMTEMPLATE *)(((UINT_PTR)next + 3) & ~(UINT_PTR)3);
    item->style = WS_CHILD | WS_VISIBLE | ES_LEFT;
    item->x = 2;
    item->y = 2;
    item->cx = 80;
    item->cy = 12;
    item->id = 100;
    next = (WORD *)(item + 1);
    *next++ = 0xffff;
    *next++ = 0x0081;
    *next++ = 0;
    *next++ = 0;

    CHECK(!wow_user_server_create_dialog(GetModuleHandleA(NULL), NULL, 0,
        NULL, dialog_proc, (LONG)dialog_data, SCDLG_CLIENT | SCDLG_ANSI));
    CHECK(GetLastError() == ERROR_INVALID_PARAMETER);
    window = wow_user_server_create_dialog(GetModuleHandleA(NULL), dialog,
        (DWORD)((BYTE *)next - (BYTE *)dialog), NULL, dialog_proc,
        (LONG)dialog_data, SCDLG_CLIENT | SCDLG_ANSI | SCDLG_NOREVALIDATE);
    CHECK(window != NULL);
    CHECK(init_messages == 1);
    CHECK(DestroyWindow(window));
    CHECK(terminal_messages == 1);
    CHECK(wow_window_words_acquire(window) == NULL);
    CHECK(!published_index);
    window = wow_user_server_create_dialog(GetModuleHandleA(NULL), dialog,
        (DWORD)((BYTE *)next - (BYTE *)dialog), NULL, dialog_proc,
        (LONG)dialog_data, SCDLG_CLIENT | SCDLG_ANSI | SCDLG_NOREVALIDATE);
    CHECK(window && init_messages == 2);
    SendMessageA(window, WM_APP + 2, 0, 0);
    CHECK(!IsWindow(window) && terminal_messages == 2);
    CHECK(!published_index);
    CHECK(wow_window_words_acquire(window) == NULL);
    fail_publication = TRUE;
    window = wow_user_server_create_dialog(GetModuleHandleA(NULL), dialog,
        (DWORD)((BYTE *)next - (BYTE *)dialog), NULL, dialog_proc,
        (LONG)dialog_data, SCDLG_CLIENT | SCDLG_ANSI | SCDLG_NOREVALIDATE);
    CHECK(!window && !published_index);
    CHECK(init_messages == 2 && terminal_messages == 2);
    verify_failed_borrow();
    fail_publication = FALSE;
    veto = SetWindowsHookExA(WH_CBT, veto_hook, NULL, GetCurrentThreadId());
    CHECK(veto != NULL);
    veto_creation = TRUE;
    window = wow_user_server_create_dialog(GetModuleHandleA(NULL), dialog,
        (DWORD)((BYTE *)next - (BYTE *)dialog), NULL, dialog_proc,
        (LONG)dialog_data, SCDLG_CLIENT | SCDLG_ANSI | SCDLG_NOREVALIDATE);
    CHECK(!window && !published_index);
    CHECK(init_messages == 2 && terminal_messages == 2);
    verify_failed_borrow();
    veto_creation = FALSE;
    CHECK(UnhookWindowsHookEx(veto));
    window_rollback_contract(&binding);
    fprintf(stderr, "WOW_CREATION_ROLLBACK cases=%u errors=%u\n", rollback_cases, errors);
    CHECK(wow_user_task_lifecycle_exit(&lifecycle,
        (HANDLE)(ULONG_PTR)0x1235, 0x1235, NULL, 0));
    CHECK(wow_user_runtime_unbind(&binding));
    wow_user_task_lifecycle_dispose(&lifecycle);
    CHECK(session_thread_unbind(&owner));
    CHECK(session_dispose(&owner));
    if (wowexec) CloseHandle(wowexec);
    fprintf(stderr, "WOW_USER_TASK_DIALOG_LIFECYCLE errors=%u init=%u terminal=%u\n",
        errors, init_messages, terminal_messages);
    return errors ? 1 : 0;
}
