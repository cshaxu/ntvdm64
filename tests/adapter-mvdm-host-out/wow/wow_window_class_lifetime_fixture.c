#include "wow_window_words_binding.h"
#include "wow_user_borrow_scope.h"
#include "wow_user_private_access.h"
#include <stdio.h>
typedef VOID (APIENTRY *PFNW32ET)(VOID);
#pragma warning(push)
#pragma warning(disable:4201)
#include "opennt-abi/source/public/internal/windows/inc/wowuserp.h"
#pragma warning(pop)

static unsigned errors, callbacks;
static wow_class_lookup_context context;
static ATOM atom;
static HWND sibling;
static HINSTANCE instance;
static DWORD *shared_words;
#define CHECK(x) do { if (!(x)) { ++errors; printf("FAIL line=%d error=%lu\n", __LINE__, GetLastError()); } } while (0)
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
    previous = first;
    CHECK(wow_window_words_detach_if(window, previous));
    CHECK(context.pclsPrivateList->cWndReferenceCount == 1);
    class_borrow = wow_class_words_acquire(&context, atom, instance);
    CHECK(wow_window_words_attach_class(window, &words, class_borrow));
    first = wow_window_words_acquire(window);
    CHECK(first != previous);
    CHECK(context.pclsPrivateList->cWndReferenceCount == 2);
    /* Simulate stale creation rollback after replacement: old token cannot
     * remove the new association. The old token remains borrowed, so its
     * address cannot be recycled into the replacement (ABA). */
    CHECK(!wow_window_words_detach_if(window, previous));
    CHECK(context.pclsPrivateList->cWndReferenceCount == 2);
    second = wow_window_words_acquire(window);
    CHECK(second == first);
    wow_window_words_release(second);
    CHECK(wow_user_borrow_enter(&outer));
    scoped_class = wow_user_borrow_class(&context, atom, instance);
    CHECK(scoped_class == wow_window_class_words_value(first));
    CHECK((PWW)wow_user_get_window_longA(window, -1) == wow_window_words_value(first));
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
    CHECK(wow_window_class_words_value(first)[1] == 0x77889900);
    wow_window_words_release(first);
    CHECK(scoped_class[1] == 0x77889900);
    CHECK(wow_user_borrow_leave(&outer));
    CHECK(wow_user_borrow_window(NULL) == NULL);
    CHECK(GetLastError() == ERROR_INVALID_STATE);
    printf("WOW_WINDOW_CLASS_SHARED_LIFETIME errors=%u callbacks=%u native_failure_retains_node=32\n", errors, callbacks);
    return errors ? 3 : 0;
}
