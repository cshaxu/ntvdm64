#define UNICODE
#define _UNICODE
#include <windows.h>
#include <winternl.h>
#include <stdio.h>
#ifdef WOW_CREATE_NATIVE
#include "wow_window_creation_binding.h"
#include "wow_creation_owner_fixture.h"
typedef VOID (APIENTRY *PFNW32ET)(VOID);
#pragma warning(push)
#pragma warning(disable:4201)
#include "opennt-abi/source/public/internal/windows/inc/wowuserp.h"
#pragma warning(pop)
#endif

/* Complete original entry; finite test doubles below model only its external
 * dependencies. No claim of native MDI, kernel creation or guest execution. */
#include "original-large-types.inc"
typedef struct { LARGE_UNICODE_STRING strCapture; PLARGE_UNICODE_STRING pstr; BOOL fAllocated; }
    LARGE_IN_STRING, *PLARGE_IN_STRING;
typedef struct { int cy, cx, y, x; LONG style; HMENU hMenu; } SHORTCREATE;
typedef struct { WORD fnid; } TEST_WND, *PWND;
static TEST_WND parent = {0x123};
static unsigned errors, calls, captures, frees, prepared, completed, cases;
static DWORD version, seen_ex, seen_version, seen_style, seen_flags;
static LPDWORD seen_wow;
static PVOID seen_param;
static BOOL valid_parent, mdi_ok, backend_ok, capture_ok;
static UINT class_kind, title_kind;
static int seen_x;
static HMENU seen_menu;
#ifdef WOW_CREATE_NATIVE
static BOOL native_backend, native_reject;
static wow_class_lookup_context native_context;
static unsigned native_created, native_destroyed, native_messages;
static DWORD native_prefix[3] = {0x11223344, 0x12345678, 0};
#endif
#define CHECK(x) do { if (!(x)) { ++errors; printf("FAIL line=%d\n", __LINE__); } } while (0)
#define GETEXPWINVER(module) version
#define VER40 0x0400
/* Value verified against original winuserp.h before the fixture is accepted. */
#define WS_EX_VALID40 (WS_EX_DLGMODALFRAME | 0x00000002L | \
    WS_EX_NOPARENTNOTIFY | WS_EX_TOPMOST | WS_EX_ACCEPTFILES | \
    WS_EX_TRANSPARENT | WS_EX_MDICHILD | WS_EX_WINDOWEDGE | \
    WS_EX_CLIENTEDGE | WS_EX_CONTEXTHELP | WS_EX_TOOLWINDOW | \
    WS_EX_RIGHT | WS_EX_LEFT | WS_EX_RTLREADING | WS_EX_LEFTSCROLLBAR | \
    WS_EX_CONTROLPARENT | WS_EX_STATICEDGE | WS_EX_APPWINDOW)
#define WS_EX_ANSICREATOR 0x80000000L
#ifndef CW_FLAGS_ANSI
#define CW_FLAGS_ANSI 1
#endif
#define CW_FLAGS_DIFFHMOD 0x80000000
#define FNID_MDICLIENT 0x123
#define GETFNID(w) ((w)->fnid)
#define RIPMSG0(level,text) ((void)0)
#define RtlProcessHeap() GetProcessHeap()
static BOOL free_capture(HANDLE heap, DWORD flags, PVOID p)
{ ++frees; return HeapFree(heap, flags, p); }
#define RtlFreeHeap free_capture
#include "ntsend-fixture.h"
static PWND ValidateHwnd(HWND hwnd)
{ CHECK(hwnd == (HWND)0x3456); return valid_parent ? &parent : NULL; }
static BOOL CreateMDIChild(SHORTCREATE *sc, MDICREATESTRUCTW *md,
    DWORD ver, HMENU *menu, PWND wnd)
{
    ++prepared; CHECK(wnd == &parent && ver == version);
    CHECK(md->lParam == 0x7654 && md->x == 1 && sc->x == 1);
    sc->x = 17; sc->hMenu = (HMENU)0x5555; *menu = (HMENU)0x6666;
    return mdi_ok;
}
static BOOL MDICompleteChildCreation(HWND hwnd, HMENU menu, BOOL visible, BOOL disabled)
{
    ++completed; CHECK(hwnd == (HWND)0x7777 && menu == (HMENU)0x6666);
    CHECK(visible && disabled); return TRUE;
}
static PVOID allocate_capture(HANDLE heap, DWORD flags, SIZE_T bytes)
{ ++captures; return capture_ok ? HeapAlloc(heap, flags, bytes) : NULL; }
#define RtlAllocateHeap allocate_capture
#define STATIC_UNICODE_BUFFER_LENGTH 261
static struct { WCHAR StaticUnicodeBuffer[STATIC_UNICODE_BUFFER_LENGTH]; } capture_teb;
#undef NtCurrentTeb
#define NtCurrentTeb() (&capture_teb)
#define ARGUMENT_PRESENT(p) ((p) != NULL)
#ifndef UNICODE_NULL
#define UNICODE_NULL ((WCHAR)0)
#endif
NTSYSAPI NTSTATUS NTAPI RtlMultiByteToUnicodeN(PWCH, ULONG, PULONG, const CHAR *, ULONG);
#pragma warning(push)
/* Original capture accepts unused cchLimit and passes UINT* to PULONG.
 * Both integer types are 32-bit in this required x86 fixture. */
#pragma warning(disable:4100 4057)
#include "original-large-capture.inc"
#pragma warning(pop)
#ifdef WOW_CREATE_NATIVE
static DWORD WINAPI native_callback(HWND window, UINT message, UINT wp,
    LONG lp, DWORD procedure, PVOID raw_words)
{
    WW *words = raw_words;
    CHECK(procedure == 0x81234567);
    CHECK(!memcmp(words->adwWOW, native_prefix, sizeof(native_prefix)));
    CHECK(words->hInstance == (HANDLE)0x23451234);
    if (message == WM_NCCREATE) {
        CREATESTRUCTA *cs = (CREATESTRUCTA *)lp;
        ++native_created;
        CHECK(cs->lpCreateParams == (PVOID)0x7654);
        if (native_reject) return FALSE;
    }
    if (message == WM_NCDESTROY) ++native_destroyed;
    if (message == WM_APP + 7) { ++native_messages; return 0x76543210; }
    return DefWindowProcA(window, message, wp, lp);
}
static HWND native_create(DWORD ex, PLARGE_STRING cls, PLARGE_STRING title,
    DWORD style, int x, int y, int cx, int cy, HWND owner, HMENU menu,
    HANDLE module, PVOID param, LPDWORD prefix)
{
    UNICODE_STRING name;
    ATOM atom;
    WW words = {0};
    CREATESTRUCTA request = {0};
    wow_window_dispatch_target target = {0x81234567, native_callback};
    /* Test-only ordinary A/W composition, not an installed output slot.
     * MDI and ordinal titles are exercised separately above. */
    CHECK(!(ex & WS_EX_MDICHILD));
    if (!HIWORD(cls)) atom = LOWORD(cls);
    else {
        if (!cls->Buffer) return NULL;
        name.Buffer = cls->Buffer;
        name.Length = (USHORT)cls->Length;
        name.MaximumLength = (USHORT)cls->MaximumLength;
        atom = wow_class_words_find_atom(&native_context, &name);
    }
    CHECK(title && title->Length == (title->bAnsi ? 6u : 12u));
    /* The original owner copies only adwWOW; never read sizeof(WW) here. */
    CopyMemory(words.adwWOW, prefix, sizeof(words.adwWOW));
    request.dwExStyle = ex & ~WS_EX_ANSICREATOR;
    request.lpszClass = (LPCSTR)(ULONG_PTR)atom; request.lpszName = title->Buffer;
    request.style = style; request.x = x; request.y = y;
    request.cx = cx; request.cy = cy; request.hwndParent = owner;
    request.hMenu = menu; request.hInstance = module; request.lpCreateParams = param;
    if (ex & WS_EX_ANSICREATOR)
        return wow_window_create_boundA(wow_fixture_creation_owner(&native_context), atom, &request, &words, &target);
    else {
        CREATESTRUCTW wide;
        CopyMemory(&wide, &request, sizeof(wide));
        return wow_window_create_boundW(wow_fixture_creation_owner(&native_context), atom, &wide, &words, &target);
    }
}
#endif
static HWND NtUserCreateWindowEx(DWORD ex, PLARGE_STRING cls, PLARGE_STRING title,
    DWORD style, int x, int y, int cx, int cy, HWND owner, HMENU menu,
    HANDLE module, PVOID param, DWORD ver, LPDWORD wow)
{
#ifdef WOW_CREATE_NATIVE
    if (native_backend) return native_create(ex, cls, title, style, x, y, cx,
        cy, owner, menu, module, param, wow);
#endif
    UNREFERENCED_PARAMETER(y); UNREFERENCED_PARAMETER(cx); UNREFERENCED_PARAMETER(cy);
    CHECK(owner == (HWND)0x3456 && module == (HANDLE)0x23451234);
    ++calls; seen_ex = ex; seen_version = ver; seen_style = style;
    seen_wow = wow; seen_param = param; seen_x = x; seen_menu = menu;
    if (class_kind == 0) CHECK((ULONG_PTR)cls == 42);
    else {
        CHECK(cls != NULL && (cls->Buffer != NULL || !capture_ok));
        if (capture_ok && cls->Buffer) CHECK(!wcscmp(cls->Buffer, L"CLASS"));
        if (!capture_ok) CHECK(cls->Buffer == NULL);
    }
    if (title_kind == 0) CHECK(title == NULL);
    if (title_kind == 1) CHECK(title && title->bAnsi && title->Length == 3);
    if (title_kind == 2) CHECK(title && !title->bAnsi && title->Length == 4);
    if (ex & WS_EX_MDICHILD) CHECK(((MDICREATESTRUCTW *)param)->lParam == 0x7654);
    return backend_ok ? (HWND)0x7777 : NULL;
}
#pragma warning(push)
/* Preserve the original body. Its MDI out-parameter and conditional capture
 * cleanup trigger modern flow-analysis warnings; all selected paths tested. */
#pragma warning(disable:4701 4703)
#include "original-create-entry.inc"
#pragma warning(pop)
static HWND run(DWORD ex, LPCWSTR cls, LPCWSTR title, DWORD flags)
{
    static DWORD prefix[3] = {1,2,3};
    HWND result;
    ++cases; calls = captures = frees = prepared = completed = 0;
    seen_flags = flags;
    result = _CreateWindowEx(ex, cls, title, WS_VISIBLE | WS_DISABLED,
        1,2,3,4,(HWND)0x3456,(HMENU)0x4444,(HANDLE)0x23451234,
        (PVOID)0x7654,flags,prefix);
    if (calls) CHECK(seen_wow == prefix);
    CHECK(captures == frees || !capture_ok);
    return result;
}
#ifdef WOW_CREATE_NATIVE
static void verify_native_chain(void)
{
    WNDCLASSEXA cls = {sizeof(cls)};
    UNICODE_STRING name;
    DWORD wc[2] = {0x12345678, 0}, old;
    SYSTEM_INFO info;
    BYTE *pages;
    LPDWORD prefix;
    HWND window;
    ATOM atom;
    cls.hInstance = (HINSTANCE)0x23451234;
    cls.lpszClassName = "S40_ORIGINAL_CREATE_NATIVE";
    cls.lpfnWndProc = wow_window_native_proc;
    RtlInitUnicodeString(&name, L"S40_ORIGINAL_CREATE_NATIVE");
    atom = wow_class_words_register(&native_context, &cls, &name, wc, NULL);
    CHECK(atom != 0); if (!atom) return;
    GetSystemInfo(&info);
    pages = VirtualAlloc(NULL, info.dwPageSize * 2, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    CHECK(pages != NULL); if (!pages) return;
    CHECK(VirtualProtect(pages + info.dwPageSize, info.dwPageSize, PAGE_NOACCESS, &old));
    prefix = (LPDWORD)(pages + info.dwPageSize - sizeof(native_prefix));
    CopyMemory(prefix, native_prefix, sizeof(native_prefix));
    native_backend = capture_ok = TRUE;
    captures = frees = 0;
    window = _CreateWindowEx(0, (LPCWSTR)cls.lpszClassName, (LPCWSTR)"native",
        WS_OVERLAPPEDWINDOW, 1, 2, 32, 32, NULL, NULL, cls.hInstance,
        (PVOID)0x7654, CW_FLAGS_ANSI, prefix);
    CHECK(window != NULL);
    if (window) {
        CHECK(SendMessageA(window, WM_APP + 7, 0, 0) == 0x76543210);
        CHECK(DestroyWindow(window));
    }
    native_reject = TRUE;
    window = _CreateWindowEx(0, (LPCWSTR)cls.lpszClassName, (LPCWSTR)"native",
        WS_OVERLAPPEDWINDOW, 1, 2, 32, 32, NULL, NULL, cls.hInstance,
        (PVOID)0x7654, CW_FLAGS_ANSI, prefix);
    CHECK(window == NULL); if (window) DestroyWindow(window);
    native_reject = FALSE;
    window = _CreateWindowEx(0, L"S40_ORIGINAL_CREATE_NATIVE", L"native",
        WS_OVERLAPPEDWINDOW, 1, 2, 32, 32, NULL, NULL, cls.hInstance,
        (PVOID)0x7654, 0, prefix);
    CHECK(window != NULL);
    if (window) {
        WCHAR title[16];
        CHECK(GetWindowTextW(window, title, 16) == 6 && !wcscmp(title, L"native"));
        CHECK(SendMessageA(window, WM_APP + 7, 0, 0) == 0x76543210);
        CHECK(DestroyWindow(window));
    }
    native_reject = TRUE;
    window = _CreateWindowEx(0, (LPCWSTR)(ULONG_PTR)atom, L"native",
        WS_OVERLAPPEDWINDOW, 1, 2, 32, 32, NULL, NULL, cls.hInstance,
        (PVOID)0x7654, 0, prefix);
    CHECK(window == NULL); if (window) DestroyWindow(window);
    native_reject = FALSE;
    CHECK(captures == 2 && frees == 2);
    CHECK(native_created == 4 && native_destroyed == 4 && native_messages == 2);
    CHECK(VirtualFree(pages, 0, MEM_RELEASE));
    CHECK(wow_class_words_unregister(&native_context, atom, cls.hInstance, NULL));
    CHECK(!native_context.pclsPrivateList && !native_context.pclsPublicList);
    printf("WOW_ORIGINAL_CREATE_NATIVE created=%u destroyed=%u messages=%u captures=%u frees=%u\n",
        native_created, native_destroyed, native_messages, captures, frees);
    native_backend = FALSE;
}
#endif
int __cdecl main(void)
{
    unsigned char ordinal_a[3] = {0xff, 7, 0};
    WCHAR ordinal_w[2] = {0xffff, 7};
    valid_parent = mdi_ok = backend_ok = capture_ok = TRUE;
    version = 0x030a;
    CHECK(run(0x800, (LPCWSTR)42, NULL, CW_FLAGS_ANSI) != NULL);
    CHECK((seen_ex & WS_EX_TOOLWINDOW) && !(seen_ex & 0x800));
    CHECK(seen_ex & WS_EX_ANSICREATOR);
    CHECK(seen_param == (PVOID)0x7654 && seen_x == 1);
    version = 0x0400;
    CHECK(run(0x10000000, (LPCWSTR)42, NULL, 0) == NULL && !calls);
    version = 0x030a;
    CHECK(run(0x10000000, (LPCWSTR)42, NULL, CW_FLAGS_DIFFHMOD) != NULL);
    CHECK(seen_version == (version | CW_FLAGS_DIFFHMOD));
    class_kind = 1; title_kind = 1;
    CHECK(run(0, (LPCWSTR)"CLASS", (LPCWSTR)ordinal_a, CW_FLAGS_ANSI) != NULL);
    CHECK(captures == 1 && frees == 1);
    title_kind = 2;
    CHECK(run(0, L"CLASS", ordinal_w, 0) != NULL && captures == 0);
    title_kind = class_kind = 0;
    valid_parent = FALSE;
    CHECK(run(WS_EX_MDICHILD, (LPCWSTR)42, NULL, 0) == NULL && !prepared && !calls);
    valid_parent = TRUE; mdi_ok = FALSE;
    CHECK(run(WS_EX_MDICHILD, (LPCWSTR)42, NULL, 0) == NULL && prepared == 1 && !calls);
    mdi_ok = TRUE;
    CHECK(run(WS_EX_MDICHILD, (LPCWSTR)42, NULL, 0) != NULL && completed == 1);
    CHECK(seen_x == 17 && seen_menu == (HMENU)0x5555);
    backend_ok = FALSE; class_kind = 1;
    CHECK(run(WS_EX_MDICHILD, (LPCWSTR)"CLASS", NULL, CW_FLAGS_ANSI) == NULL);
    CHECK(captures == 1 && frees == 1 && completed == 0);
    capture_ok = FALSE;
    CHECK(run(0, (LPCWSTR)"CLASS", NULL, CW_FLAGS_ANSI) == NULL && calls == 1);
    printf("WOW_ORIGINAL_CREATE_ENTRY errors=%u cases=%u capture_failure_forwarded=%u\n", errors, cases, calls);
#ifdef WOW_CREATE_NATIVE
    verify_native_chain();
    printf("WOW_ORIGINAL_CREATE_NATIVE errors=%u\n", errors);
#endif
    return errors ? 3 : 0;
}
