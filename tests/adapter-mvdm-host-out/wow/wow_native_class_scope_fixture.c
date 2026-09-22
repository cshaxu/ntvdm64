#include <windows.h>
#include <stdio.h>

/* Native capability probe only. Original kernel/class.c matches module
 * HIWORD, sharing a class across WOW instances of one executable module.
 * Determine whether modern USER can own that policy without a local map. */
static unsigned errors, seen_a, seen_b;
static const char class_name[] = "S40_WOW_MODULE_SCOPE";
#define CHECK(x) do { if (!(x)) { ++errors; printf("FAIL line=%d error=%lu\n", __LINE__, GetLastError()); } } while (0)
static LRESULT CALLBACK procedure_a(HWND w, UINT m, WPARAM wp, LPARAM lp)
{
    if (m == WM_NCCREATE) ++seen_a;
    return DefWindowProcA(w, m, wp, lp);
}
static LRESULT CALLBACK procedure_b(HWND w, UINT m, WPARAM wp, LPARAM lp)
{
    if (m == WM_NCCREATE) ++seen_b;
    return DefWindowProcA(w, m, wp, lp);
}
int __cdecl main(void)
{
    WNDCLASSEXA wc = {sizeof(wc)}, info = {sizeof(info)};
    HINSTANCE a = (HINSTANCE)(ULONG_PTR)MAKELONG(0x1234, 0x2345);
    HINSTANCE a2 = (HINSTANCE)(ULONG_PTR)MAKELONG(0x5678, 0x2345);
    HINSTANCE b = (HINSTANCE)(ULONG_PTR)MAKELONG(0x1234, 0x3456);
    HINSTANCE absent = (HINSTANCE)(ULONG_PTR)MAKELONG(0x1234, 0x4567);
    ATOM atom_a, atom_b, duplicate;
    ATOM local_atom, global_atom;
    HWND wa = NULL, wa2 = NULL, wb = NULL, wrong = NULL;
    DWORD live_error;
    BOOL live_result;
    wc.lpfnWndProc = procedure_a; wc.hInstance = a;
    wc.lpszClassName = class_name; wc.cbClsExtra = sizeof(DWORD);
    atom_a = RegisterClassExA(&wc);
    printf("register_a=%u error=%lu\n", atom_a, GetLastError());
    CHECK(atom_a != 0);
    if (!atom_a) return 3;
    printf("class_atom=%u global_atom=%u local_atom=%u info_result=%u\n",
        atom_a, GlobalFindAtomA(class_name), FindAtomA(class_name),
        (unsigned)GetClassInfoExA(a, class_name, &info));
    /* Original USER owns UserAtomTableHandle, not BaseLocalAtomTable or
     * the global atom table. Adding/deleting names in either public atom
     * namespace must not change native class lookup or class lifetime.
     * Do not assert numeric inequality: unrelated tables may reuse IDs. */
    CHECK(FindAtomA(class_name) == 0 && GlobalFindAtomA(class_name) == 0);
    local_atom = AddAtomA(class_name);
    global_atom = GlobalAddAtomA(class_name);
    CHECK(local_atom != 0 && global_atom != 0);
    CHECK(GetClassInfoExA(a, MAKEINTATOM(atom_a), &info));
    CHECK(info.lpfnWndProc == procedure_a);
    if (local_atom) CHECK(DeleteAtom(local_atom) == 0);
    if (global_atom) CHECK(GlobalDeleteAtom(global_atom) == 0);
    CHECK(FindAtomA(class_name) == 0 && GlobalFindAtomA(class_name) == 0);
    CHECK(GetClassInfoExA(a, class_name, &info));
    CHECK(info.lpfnWndProc == procedure_a);
    wc.hInstance = a2;
    SetLastError(0); duplicate = RegisterClassExA(&wc);
    printf("same_module_other_instance=%u error=%lu\n", duplicate, GetLastError());
    CHECK(!duplicate && GetLastError() == ERROR_CLASS_ALREADY_EXISTS);
    if (duplicate) UnregisterClassA(class_name, a2);
    wc.hInstance = b; wc.lpfnWndProc = procedure_b;
    atom_b = RegisterClassExA(&wc);
    CHECK(atom_b != 0 && atom_b == atom_a);
    if (!atom_b) { UnregisterClassA(class_name, a); return 3; }
    CHECK(GetClassInfoExA(a2, class_name, &info) && info.lpfnWndProc == procedure_a);
    printf("private_query_instance=%08lx returned_instance=%08lx registered_instance=%08lx\n",
        (DWORD)a2, (DWORD)info.hInstance, (DWORD)a);
    CHECK(info.hInstance == a2);
    CHECK(GetClassInfoExA(b, class_name, &info) && info.lpfnWndProc == procedure_b);
    wa = CreateWindowExA(0, class_name, "", WS_OVERLAPPED, 0, 0, 32, 32, NULL, NULL, a, NULL);
    wa2 = CreateWindowExA(0, MAKEINTATOM(atom_a), "", WS_OVERLAPPED, 0, 0, 32, 32, NULL, NULL, a2, NULL);
    wb = CreateWindowExA(0, class_name, "", WS_OVERLAPPED, 0, 0, 32, 32, NULL, NULL, b, NULL);
    CHECK(wa && wa2 && wb && seen_a == 2 && seen_b == 1);
    if (wa && wa2 && wb) {
        SetClassLongA(wa, 0, 0xfeed1234);
        CHECK(GetClassLongA(wa2, 0) == 0xfeed1234 && GetClassLongA(wb, 0) == 0);
        CHECK((HINSTANCE)GetWindowLongA(wa2, GWL_HINSTANCE) == a2);
        CHECK((HINSTANCE)GetClassLongA(wa2, GCL_HMODULE) == a);
    }
    SetLastError(0);
    wrong = CreateWindowExA(0, class_name, "", WS_OVERLAPPED, 0, 0, 32, 32, NULL, NULL, absent, NULL);
    CHECK(wrong == NULL && GetLastError() == ERROR_CANNOT_FIND_WND_CLASS);
    if (wrong) DestroyWindow(wrong);
    SetLastError(0); live_result = UnregisterClassA(class_name, a2); live_error = GetLastError();
    CHECK(!live_result && live_error == ERROR_CLASS_HAS_WINDOWS);
    if (wa) CHECK(DestroyWindow(wa));
    if (wa2) CHECK(DestroyWindow(wa2));
    if (wb) CHECK(DestroyWindow(wb));
    CHECK(UnregisterClassA(MAKEINTATOM(atom_a), a2));
    CHECK(UnregisterClassA(class_name, b));
    CHECK(!GetClassInfoExA(a, class_name, &info));
    /* Original GetClassPtr first chooses the module-private class, then
     * the process-public (CS_GLOBALCLASS) class regardless of module. */
    wc.style = CS_GLOBALCLASS; wc.hInstance = a; wc.lpfnWndProc = procedure_a;
    atom_a = RegisterClassExA(&wc); CHECK(atom_a != 0);
    wc.hInstance = b; SetLastError(0);
    duplicate = RegisterClassExA(&wc);
    CHECK(!duplicate && GetLastError() == ERROR_CLASS_ALREADY_EXISTS);
    if (duplicate) UnregisterClassA(class_name, b);
    wc.style = 0; wc.lpfnWndProc = procedure_b;
    atom_b = RegisterClassExA(&wc); CHECK(atom_b != 0);
    CHECK(GetClassInfoExA(b, class_name, &info) && info.lpfnWndProc == procedure_b);
    CHECK(GetClassInfoExA(absent, class_name, &info) && info.lpfnWndProc == procedure_a);
    printf("public_query_instance=%08lx returned_instance=%08lx registered_instance=%08lx\n",
        (DWORD)absent, (DWORD)info.hInstance, (DWORD)a);
    CHECK(info.hInstance == absent);
    wa = CreateWindowExA(0, class_name, "", WS_OVERLAPPED, 0, 0, 32, 32, NULL, NULL, absent, NULL);
    wb = CreateWindowExA(0, class_name, "", WS_OVERLAPPED, 0, 0, 32, 32, NULL, NULL, b, NULL);
    CHECK(wa && wb && seen_a == 3 && seen_b == 2);
    if (wa) CHECK((HINSTANCE)GetClassLongA(wa, GCL_HMODULE) == a);
    if (wa) CHECK(DestroyWindow(wa));
    if (wb) CHECK(DestroyWindow(wb));
    CHECK(UnregisterClassA(class_name, b));
    CHECK(GetClassInfoExA(b, class_name, &info) && info.lpfnWndProc == procedure_a);
    CHECK(UnregisterClassA(class_name, absent));
    CHECK(!GetClassInfoExA(a, class_name, &info));
    /* Original permits a private class to shadow an existing public class,
     * even for the same module. Atom plus module is not a unique CLS key. */
    wc.style = CS_GLOBALCLASS; wc.hInstance = a; wc.lpfnWndProc = procedure_a;
    atom_a = RegisterClassExA(&wc); CHECK(atom_a != 0);
    wc.style = 0; wc.lpfnWndProc = procedure_b;
    atom_b = RegisterClassExA(&wc); CHECK(atom_b && atom_b == atom_a);
    wa = CreateWindowExA(0, class_name, "", 0, 0, 0, 32, 32, NULL, NULL, absent, NULL);
    wb = CreateWindowExA(0, class_name, "", 0, 0, 0, 32, 32, NULL, NULL, a2, NULL);
    CHECK(wa && wb && seen_a == 4 && seen_b == 3);
    if (wa && wb) {
        CHECK(GetClassLongA(wa, GCW_ATOM) == GetClassLongA(wb, GCW_ATOM));
        CHECK((HINSTANCE)GetClassLongA(wa, GCL_HMODULE) == a);
        CHECK((HINSTANCE)GetClassLongA(wb, GCL_HMODULE) == a);
        SetClassLongA(wa, 0, 0x11223344);
        SetClassLongA(wb, 0, 0x55667788);
        CHECK(GetClassLongA(wa, 0) == 0x11223344 && GetClassLongA(wb, 0) == 0x55667788);
        SetClassLongA(wb, GCL_STYLE, GetClassLongA(wb, GCL_STYLE) | CS_GLOBALCLASS);
        CHECK(GetClassLongA(wb, GCL_STYLE) & CS_GLOBALCLASS);
        /* Changing the public style bit does not move the private class
         * into the public registration list. */
        CHECK(GetClassInfoExA(absent, class_name, &info) && info.lpfnWndProc == procedure_a);
        CHECK(GetClassInfoExA(a2, class_name, &info) && info.lpfnWndProc == procedure_b);
    }
    if (wa) CHECK(DestroyWindow(wa));
    if (wb) CHECK(DestroyWindow(wb));
    CHECK(UnregisterClassA(class_name, a2));
    CHECK(GetClassInfoExA(a2, class_name, &info) && info.lpfnWndProc == procedure_a);
    CHECK(UnregisterClassA(class_name, absent));
    CHECK(!GetClassInfoExA(a, class_name, &info));
    puts("WOW_NATIVE_CLASS_QUERY_INSTANCE_IS_NOT_OWNER_OK");
    if (!errors) puts("WOW_NATIVE_CLASS_SAME_MODULE_PUBLIC_PRIVATE_DISTINCT_OK");
    if (!errors) puts("WOW_NATIVE_CLASS_ATOM_NAMESPACE_ISOLATION_OK");
    printf("WOW_NATIVE_CLASS_SCOPE errors=%u create_a=%u create_b=%u\n", errors, seen_a, seen_b);
    return errors ? 3 : 0;
}
