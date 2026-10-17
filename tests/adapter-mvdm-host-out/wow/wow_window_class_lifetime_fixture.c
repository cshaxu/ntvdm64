#include "wow_window_words_binding.h"
#include "wow_user_borrow_scope.h"
#include "wow_user_private_access.h"
#include "wow_user_client_view_layout.h"
#include "wow_window_client_state_bindings.h"
#include "opennt-host/windows/core/ntuser/rtl/userrtl.h"
#include <stdio.h>

static unsigned errors, callbacks;
static wow_class_lookup_context context;
static ATOM atom;
static HWND sibling;
static HINSTANCE instance;
static DWORD *shared_words;
/* Retained original WND capacity with zero FNID: storage alone must not
 * select EDIT cleanup. Use application data for lifetime markers; head.h
 * must remain the actual HWND used by identity and relationship consumers. */
static DWORD *edit_payload;
#define CHECK(x) do { if (!(x)) { ++errors; fprintf(stderr,"FAIL line=%d error=%lu\n", __LINE__, GetLastError()); } } while (0)
static LRESULT CALLBACK native_proc(HWND window, UINT message, WPARAM wp, LPARAM lp)
{
    if (message == WM_NCDESTROY) wow_window_words_detach(window);
    return DefWindowProcA(window, message, wp, lp);
}
static DWORD WINAPI callback(HWND window, UINT message, UINT wp, LONG lp,
    DWORD procedure, PVOID words)
{
    wow_window_words_binding *borrow = wow_window_words_acquire(window);
    (void)wp; (void)lp;
    ++callbacks;
    CHECK(message == WM_USER && procedure == 0x92340012);
    CHECK(borrow != NULL);
    CHECK(words == wow_window_words_value(borrow)->adwWOW);
    CHECK(wow_window_class_words_value(borrow) == shared_words);
    CHECK(wow_user_borrow_window(window) == wow_window_words_value(borrow));
    CHECK(wow_user_borrow_window(window) == wow_window_words_value(borrow));
    CHECK(wow_user_borrow_window_class(window) == shared_words);
    CHECK(wow_user_borrow_class(&context, atom, instance) == shared_words);
    CHECK(DestroyWindow(sibling));
    CHECK(DestroyWindow(window));
    CHECK(context.pclsPrivateList->cWndReferenceCount == 0);
    CHECK(wow_class_words_unregister(&context, atom, instance, NULL));
    CHECK(context.pclsPrivateList == NULL);
    CHECK(wow_window_words_client_state_value(borrow) ==
        (struct tagWND *)edit_payload);
    CHECK(((PWND)edit_payload)->dwUserData == 0x45444954);
    CHECK(((PWND)edit_payload)->head.h == window);
    CHECK(((PWND)edit_payload)->pcls != NULL);
    if (((PWND)edit_payload)->pcls)
        CHECK(((PWND)edit_payload)->pcls->atomClassName == atom);
    CHECK(REBASEALWAYS((PWND)edit_payload, pcls) == ((PWND)edit_payload)->pcls);
    CHECK(REBASEPWND((PWND)edit_payload, spwndOwner) == NULL);
    CHECK(REBASE((PWND)edit_payload, spwndParent) == NULL);
    CHECK(REBASEPTR((PWND)edit_payload, shared_words) == shared_words);
    /* Native HWNDs and registration are gone, but original dispatch still
     * holds this window, which in turn retains the shared class words. */
    CHECK(shared_words[0] == 0x12345678 && shared_words[1] == 0x33445566);
    shared_words[1] = 0x77889900;
    wow_window_words_release(borrow);
    return 0x2468;
}
int __cdecl main(void)
{
    static const char name[] = "S40_WINDOW_CLASS_LIFETIME";
    WNDCLASSEXA wc = {sizeof(wc)};
    UNICODE_STRING capture;
    DWORD class_words[2] = {0x12345678, 0x87654321};
    WW words = {0};
    HWND window;
    wow_class_words_binding *class_borrow;
    wow_window_words_binding *first, *second, *previous;
    LONG result = 0;
    wow_user_borrow_scope outer, inner;
    DWORD *scoped_class;
    unsigned i;
    wow_class_lookup_entry *retained_class;
    CLSMENUNAME failed_menu={(LPSTR)1,(LPWSTR)2,(PUNICODE_STRING)3};
    DWORD *old_edit, *rejected_edit;
    wow_window_words_binding *unpublished;
    old_edit = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(WND));
    rejected_edit = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(WND));
    edit_payload = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(WND));
    CHECK(old_edit && rejected_edit && edit_payload);
    if (!old_edit || !rejected_edit || !edit_payload) return 3;
    ((PWND)old_edit)->dwUserData = 0x4f4c4445;
    ((PWND)rejected_edit)->dwUserData = 0x52454a45;
    ((PWND)edit_payload)->dwUserData = 0x45444954;
    CHECK(!wow_window_words_attach_client_state(NULL, (struct tagWND *)rejected_edit));
    CHECK(GetLastError() == ERROR_INVALID_PARAMETER);
    unpublished = wow_window_words_allocate();
    CHECK(unpublished != NULL);
    if (!unpublished) return 3;
    CHECK(!wow_window_words_attach_client_state(unpublished, (struct tagWND *)rejected_edit));
    CHECK(GetLastError() == ERROR_INVALID_STATE);
    CHECK(wow_window_words_client_state_value(unpublished) == NULL);
    CHECK(!wow_window_words_owns_client_state(unpublished, (PWND)rejected_edit));
    CHECK(!wow_window_words_prepare_resource_state(unpublished, GetDesktopWindow()));
    CHECK(GetLastError() == ERROR_INVALID_STATE);
    CHECK(wow_window_words_client_state_value(unpublished) == NULL);
    wow_window_words_release(unpublished);
    CHECK(((PWND)rejected_edit)->dwUserData == 0x52454a45);
    CHECK(wow_user_borrow_window(NULL) == NULL);
    CHECK(GetLastError() == ERROR_INVALID_STATE);
    instance = (HINSTANCE)0x23451234;
    wc.lpfnWndProc = native_proc; wc.hInstance = instance; wc.lpszClassName = name;
    RtlInitUnicodeString(&capture, L"S40_WINDOW_CLASS_LIFETIME");
    atom = wow_class_words_register(&context, &wc, &capture, class_words, NULL);
    CHECK(atom != 0); if (!atom) return 3;
    window = CreateWindowExA(0, name, "", 0, 0, 0, 32, 32, NULL, NULL, instance, NULL);
    sibling = CreateWindowExA(0, name, "", 0, 0, 0, 32, 32, NULL, NULL, instance, NULL);
    CHECK(window && sibling); if (!window || !sibling) return 3;
    ((PWND)old_edit)->head.h = window;
    ((PWND)rejected_edit)->head.h = window;
    ((PWND)edit_payload)->head.h = window;
    /* Native windows exist before enrollment: the final native rejection
     * must not leak server menu outputs or unlink class metadata. */
    CHECK(context.pclsPrivateList->cWndReferenceCount==0);
    CHECK(!wow_class_words_unregister(&context,atom,instance,&failed_menu));
    CHECK(GetLastError()==ERROR_CLASS_HAS_WINDOWS);
    CHECK(failed_menu.pszClientAnsiMenuName==(LPSTR)1 &&
        failed_menu.pwszClientUnicodeMenuName==(LPWSTR)2 && failed_menu.pusMenuName==(PUNICODE_STRING)3);
    retained_class = context.pclsPrivateList;
    /* Cleanup's original DestroyClass loop assumes successful unlinking.
     * Prove the native counterexample without actually running an infinite
     * cleanup loop: repeated failures leave this exact node and its data live.
     * These windows deliberately have no private enrollment yet. */
    for (i = 0; i != 32; ++i) {
        CHECK(!wow_class_words_unregister(&context,atom,instance,&failed_menu));
        CHECK(GetLastError()==ERROR_CLASS_HAS_WINDOWS);
        CHECK(context.pclsPrivateList==retained_class);
        CHECK(retained_class->cWndReferenceCount==0 && retained_class->next==NULL);
        CHECK(retained_class->adwWOW[0]==class_words[0] &&
            retained_class->adwWOW[1]==class_words[1]);
        CHECK(failed_menu.pszClientAnsiMenuName==(LPSTR)1 &&
            failed_menu.pwszClientUnicodeMenuName==(LPWSTR)2 &&
            failed_menu.pusMenuName==(PUNICODE_STRING)3);
    }
    class_borrow = wow_class_words_acquire(&context, atom, instance);
    CHECK(wow_window_words_attach_class(window, &words, class_borrow));
    class_borrow = wow_class_words_acquire(&context, atom, instance);
    CHECK(wow_window_words_attach_class(sibling, &words, class_borrow));
    CHECK(context.pclsPrivateList->cWndReferenceCount == 2);
    CHECK(!wow_class_words_unregister(&context,atom,instance,&failed_menu));
    CHECK(GetLastError()==ERROR_CLASS_HAS_WINDOWS && failed_menu.pszClientAnsiMenuName==(LPSTR)1);
    /* Rejected publication must not consume the caller's class borrow. */
    class_borrow = wow_class_words_acquire(&context, atom, instance);
    CHECK(!wow_window_words_attach_class(window, &words, class_borrow));
    CHECK(GetLastError() == ERROR_ALREADY_EXISTS);
    CHECK(context.pclsPrivateList->cWndReferenceCount == 2);
    CHECK(wow_class_words_value(class_borrow)[0] == class_words[0]);
    wow_class_words_release(class_borrow);
    first = wow_window_words_acquire(window);
    CHECK(wow_window_words_client_state_value(first) == NULL);
    /* A valid but different HWND must not grant this binding ownership of
     * the candidate allocation. The caller still owns it after rejection. */
    ((PWND)rejected_edit)->head.h = sibling;
    CHECK(!wow_window_words_attach_client_state(first, (PWND)rejected_edit));
    CHECK(GetLastError() == ERROR_INVALID_WINDOW_HANDLE);
    CHECK(wow_window_words_client_state_value(first) == NULL);
    CHECK(((PWND)rejected_edit)->dwUserData == 0x52454a45);
    ((PWND)rejected_edit)->head.h = window;
    SetLastError(0x1234);
    CHECK(wow_window_words_attach_client_state(first, (struct tagWND *)old_edit));
    CHECK(GetLastError() == 0x1234);
    CHECK(wow_window_words_owns_client_state(first, (PWND)old_edit));
    CHECK(!wow_window_words_owns_client_state(first, (PWND)rejected_edit));
    CHECK(SetWindowTextA(window, "old binding name"));
    CHECK(wow_window_words_refresh(window, first));
    {
    wow_user_borrow_scope identity_scope;
    CHECK(wow_user_borrow_enter(&identity_scope));
    CHECK(wow_user_borrow_client_state(window, (PWND)old_edit) == first);
    CHECK(!wow_user_borrow_client_state(sibling, (PWND)old_edit));
    CHECK(GetLastError() == ERROR_INVALID_WINDOW_HANDLE);
    CHECK(wow_user_borrow_client_state(window, (PWND)old_edit) == first);
    CHECK(wow_window_words_thread_id(first) == GetCurrentThreadId());
    previous = first;
    CHECK(wow_window_words_detach_if(window, previous));
    CHECK(wow_window_words_client_state_value(previous) == (struct tagWND *)old_edit);
    CHECK(((PWND)old_edit)->dwUserData == 0x4f4c4445);
    CHECK(((PWND)old_edit)->head.h == window);
    CHECK(!wow_window_words_attach_client_state(previous, (struct tagWND *)rejected_edit));
    CHECK(GetLastError() == ERROR_INVALID_STATE);
    CHECK(context.pclsPrivateList->cWndReferenceCount == 1);
    class_borrow = wow_class_words_acquire(&context, atom, instance);
    CHECK(wow_window_words_attach_class(window, &words, class_borrow));
    first = wow_window_words_acquire(window);
    CHECK(first != previous);
    CHECK(wow_window_words_owns_client_state(previous, (PWND)old_edit));
    CHECK(!wow_window_words_owns_client_state(first, (PWND)old_edit));
    CHECK(wow_user_borrow_client_state(window, (PWND)old_edit) == previous);
    CHECK(wow_window_client_owns_state((PWND)old_edit));
    {
        WND copied = *(PWND)old_edit;
        copied.head.h = window;
        SetLastError(0x5678);
        CHECK(!wow_window_client_owns_state(&copied));
        CHECK(GetLastError() == 0x5678);
    }
    CHECK(wow_window_words_thread_id(previous) == GetCurrentThreadId());
    CHECK(wow_window_words_thread_id(first) == GetCurrentThreadId());
    CHECK(wow_user_borrow_leave(&identity_scope));
    }
    CHECK(wow_window_words_client_state_value(first) == NULL);
    CHECK(!wow_window_words_attach_client_state(first, NULL));
    CHECK(GetLastError() == ERROR_INVALID_PARAMETER);
    CHECK(wow_window_words_attach_client_state(first, (struct tagWND *)edit_payload));
    CHECK(((PWND)edit_payload)->pcls == ((PWND)old_edit)->pcls);
    CHECK(((PWND)edit_payload)->pcls != NULL);
    if (((PWND)edit_payload)->pcls)
        CHECK(((PWND)edit_payload)->pcls->atomClassName == atom);
    CHECK(SetWindowTextA(window, "replacement binding name"));
    CHECK(wow_window_words_refresh(window, first));
    /* The same live HWND now has a different binding generation. A stale
     * refresh must not overwrite the retained old name or the replacement. */
    CHECK(!wow_window_words_refresh(window, previous));
    CHECK(GetLastError() == ERROR_INVALID_WINDOW_HANDLE);
    {
        wow_window_client_graph_row graph[2] = {0};
        PWND old_state = (PWND)old_edit, new_state = (PWND)edit_payload;
        ULONG row, edge;
        graph[0].binding = previous; graph[0].state = old_state;
        graph[1].binding = first; graph[1].state = new_state;
        for (row = 0; row < 2; ++row) {
            graph[row].window = graph[row].state->head.h;
            graph[row].info.cbSize = sizeof(WINDOWINFO);
            graph[row].info.rcWindow = graph[row].state->rcWindow;
            graph[row].info.rcClient = graph[row].state->rcClient;
            graph[row].info.dwStyle = graph[row].state->style;
            graph[row].info.dwExStyle = graph[row].state->ExStyle;
            graph[row].module = (DWORD)(ULONG_PTR)graph[row].state->hModule;
            for (edge = 0; edge < 4; ++edge) graph[row].target[edge] = MAXDWORD;
        }
        graph[0].target[2] = 1; graph[1].target[1] = 0;
        SetLastError(0x5678);
        CHECK(wow_window_words_publish_client_graph(2, graph));
        CHECK(GetLastError() == 0x5678);
        CHECK(old_state->spwndChild == new_state && new_state->spwndParent == old_state);
        /* A later invalid row must reject the whole transaction, including
         * an otherwise valid scalar/link change in its first row. */
        graph[0].target[2] = MAXDWORD;
        graph[0].info.rcClient.left += 99;
        graph[1].target[1] = 2;
        CHECK(!wow_window_words_publish_client_graph(2, graph));
        CHECK(GetLastError() == ERROR_INVALID_STATE);
        CHECK(old_state->spwndChild == new_state && new_state->spwndParent == old_state);
        CHECK(old_state->rcClient.left == graph[0].info.rcClient.left - 99);
        graph[0].info.rcClient.left -= 99;
        graph[1].target[1] = MAXDWORD;
        CHECK(wow_window_words_publish_client_graph(2, graph));
        CHECK(!old_state->spwndChild && !new_state->spwndParent);
    }
    {
        PWND old_name = wow_window_words_lock_client_name(previous);
        CHECK(old_name == (PWND)old_edit);
        if (old_name) {
            CHECK(lstrcmpW(old_name->strName.Buffer, L"old binding name") == 0);
            wow_window_words_unlock_client_name(previous);
        }
        {
            PWND new_name = wow_window_words_lock_client_name(first);
            CHECK(new_name == (PWND)edit_payload);
            if (new_name) {
                CHECK(lstrcmpW(new_name->strName.Buffer,
                    L"replacement binding name") == 0);
                wow_window_words_unlock_client_name(first);
            }
        }
    }
    CHECK(!wow_window_words_attach_client_state(first, (struct tagWND *)rejected_edit));
    CHECK(GetLastError() == ERROR_ALREADY_EXISTS);
    CHECK(((PWND)rejected_edit)->dwUserData == 0x52454a45);
    CHECK(HeapFree(GetProcessHeap(), 0, rejected_edit));
    rejected_edit = NULL;
    CHECK(context.pclsPrivateList->cWndReferenceCount == 2);
    /* Simulate stale creation rollback after replacement: old token cannot
     * remove the new association. The old token remains borrowed, so its
     * address cannot be recycled into the replacement (ABA). */
    CHECK(!wow_window_words_detach_if(window, previous));
    CHECK(context.pclsPrivateList->cWndReferenceCount == 2);
    second = wow_window_words_acquire(window);
    CHECK(second == first);
    CHECK(wow_window_words_client_state_value(second) == (struct tagWND *)edit_payload);
    wow_window_words_release(second);
    CHECK(wow_user_borrow_enter(&outer));
    scoped_class = wow_user_borrow_class(&context, atom, instance);
    CHECK(scoped_class == wow_window_class_words_value(first));
    CHECK((PWW)wow_user_get_window_longA(window, -1) == wow_window_words_value(first));
    CHECK(wow_user_borrow_client_state(window, (PWND)edit_payload) == first);
    for (i = 0; i != 3; ++i) {
        CHECK(wow_user_set_window_longA(window, -30 - (int)i, 0x12340000 + i) == 0);
        CHECK((DWORD)wow_user_get_window_longA(window, -30 - (int)i) == 0x12340000 + i);
    }
    CHECK((DWORD *)wow_user_get_class_longA(window, -27) == scoped_class);
    CHECK(wow_user_set_class_longA(window, -28, 0x11223344) == 0x12345678);
    CHECK(((DWORD *)wow_user_get_class_longA(sibling, -27))[0] == 0x11223344);
    CHECK(wow_user_set_class_longA(sibling, -28, 0x12345678) == 0x11223344);
    CHECK(wow_user_set_window_longA(window, GWL_USERDATA, 77) == 0);
    CHECK(GetWindowLongA(window, GWL_USERDATA) == 77);
    CHECK(wow_user_get_window_longA(window, GWL_USERDATA) == 77);
    CHECK(wow_user_get_class_longA(window, GCW_ATOM) == atom);
    CHECK(!wow_user_borrow_enter(&outer) && GetLastError() == ERROR_ALREADY_EXISTS);
    CHECK(wow_user_borrow_enter(&inner));
    CHECK(!wow_user_borrow_leave(&outer) && GetLastError() == ERROR_INVALID_PARAMETER);
    CHECK(wow_user_borrow_leave(&inner));
    CHECK(wow_window_class_words_value(previous) == wow_window_class_words_value(first));
    wow_window_words_release(previous);
    second = wow_window_words_acquire(sibling);
    shared_words = wow_window_class_words_value(first);
    CHECK(shared_words == wow_window_class_words_value(second));
    shared_words[1] = 0x33445566;
    CHECK(wow_window_class_words_value(second)[1] == 0x33445566);
    wow_window_words_release(second);
    CHECK(wow_window_dispatch_bound(window, WM_USER, 0, 0,
        0x92340012, callback, &result));
    CHECK(result == 0x2468 && callbacks == 1);
    CHECK(!IsWindow(window) && !IsWindow(sibling));
    CHECK(wow_user_borrow_enter(&inner));
    SetLastError(0x5678);
    CHECK(wow_user_borrow_client_state(window, (PWND)edit_payload) == first);
    CHECK(GetLastError() == 0x5678);
    CHECK(wow_window_client_owns_state((PWND)edit_payload));
    CHECK(GetLastError() == 0x5678);
    CHECK(wow_window_words_thread_id(first) == GetCurrentThreadId());
    CHECK(wow_user_borrow_leave(&inner));
    CHECK(wow_window_class_words_value(first)[1] == 0x77889900);
    CHECK(wow_window_words_client_state_value(first) == (struct tagWND *)edit_payload);
    CHECK(((PWND)edit_payload)->dwUserData == 0x45444954);
    CHECK(((PWND)edit_payload)->head.h == window);
    {
        PWND retained = wow_window_words_lock_client_name(first);
        CHECK(retained == (PWND)edit_payload);
        if (retained) {
            CHECK(lstrcmpW(retained->strName.Buffer,
                L"replacement binding name") == 0);
            wow_window_words_unlock_client_name(first);
        }
    }
    wow_window_words_release(first);
    CHECK(scoped_class[1] == 0x77889900);
    CHECK(wow_user_borrow_leave(&outer));
    CHECK(wow_user_borrow_window(NULL) == NULL);
    CHECK(GetLastError() == ERROR_INVALID_STATE);
    fprintf(stderr,"WOW_WINDOW_CLASS_SHARED_LIFETIME errors=%u callbacks=%u native_failure_retains_node=32\n", errors, callbacks);
    return errors ? 3 : 0;
}
