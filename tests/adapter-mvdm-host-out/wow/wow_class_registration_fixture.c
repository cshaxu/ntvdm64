#include <windows.h>
#include <winternl.h>
#include <tchar.h>
#include <stdio.h>
#include <string.h>
#include "wow_class_words_binding.h"

/* Original client algorithm, extracted with pinned hashes; the kernel and
 * NT4 thread/version environment below are test doubles, NOT product code. */
typedef struct _IN_STRING {
    UNICODE_STRING strCapture;
    PUNICODE_STRING pstr;
    BOOL fAllocated;
} IN_STRING, *PIN_STRING;
static struct { DWORD dwExpWinVer; } client_info;
static DWORD version;
static HINSTANCE hmodUser;
static unsigned errors, calls, cases, capture_count, fail_capture;
static unsigned live_capture, menu_allocations, menu_frees;
static BOOL valid_brush;
static ATOM kernel_result;
static WNDCLASSEXA received;
static DWORD received_flags, *received_wow;
static WORD received_fnid;
static CLSMENUNAME owned_menu;
static HLOCAL last_ansi_menu, last_unicode_menu;
static unsigned unregister_calls;
static BOOL reject_unregister;
static BOOL native_backend;
static wow_class_lookup_context class_context;
#define CHECK(x) do { if (!(x)) { ++errors; printf("FAIL line=%d\n", __LINE__); } } while (0)
#define TEXT_FN(x) x##A
#define GetClientInfo() (&client_info)
#define GETEXPWINVER(module) version
#define VER30 0x0300
#define VER31 0x030a
#define VER40 0x0400
#define CS_VALID40 0x0801feeb
#define FNID_DDE_BIT 0x2000
#define CSF_ANSIPROC 2
#define COLOR_MAX (COLOR_INFOBK + 1)
#define RIPMSG0(level,text) ((void)0)
#define RIPMSG1(level,text,arg) ((void)0)
#define RIPERR0(code,level,text) SetLastError(code)
#define GdiValidateHandle(handle) valid_brush
static HLOCAL tracked_alloc(UINT flags, SIZE_T size)
{
    HLOCAL p = LocalAlloc(flags, size);
    if (p) { ++menu_allocations; last_ansi_menu = p; }
    return p;
}
static HLOCAL tracked_free(HLOCAL p)
{
    if (p) ++menu_frees;
    return LocalFree(p);
}
/* Existing original chartran.c, not a rewritten conversion. */
int MBToWCSEx(WORD, LPCSTR, int, LPWSTR *, int, BOOL);
static int convert_menu(LPCSTR text, int count, LPWSTR *out, int cap, BOOL alloc)
{
    int result = MBToWCSEx(0, text, count, out, cap, alloc);
    if (result && alloc) { ++menu_allocations; last_unicode_menu = *out; }
    return result;
}
#define MBToWCS convert_menu
static void *capture_alloc(HANDLE heap, DWORD flags, SIZE_T bytes)
{
    void *p;
    if (++capture_count == fail_capture) return NULL;
    p = HeapAlloc(heap, flags, bytes);
    if (p) ++live_capture;
    return p;
}
static BOOL capture_free(HANDLE heap, DWORD flags, void *p)
{
    --live_capture;
    return HeapFree(heap, flags, p);
}
#define RtlProcessHeap() GetProcessHeap()
#define RtlAllocateHeap capture_alloc
#define RtlFreeHeap capture_free
#define STATIC_UNICODE_BUFFER_LENGTH 261
#ifndef MAXUSHORT
#define MAXUSHORT 0xffff
#endif
/* Finite test-only TEB scratch; no real host TEB is read or modified. */
static struct {
    UNICODE_STRING StaticUnicodeString;
    WCHAR StaticUnicodeBuffer[STATIC_UNICODE_BUFFER_LENGTH];
} capture_teb;
#undef NtCurrentTeb
#define NtCurrentTeb() (&capture_teb)
NTSYSAPI NTSTATUS NTAPI RtlMultiByteToUnicodeN(PWCH, ULONG, PULONG, const CHAR *, ULONG);
#pragma warning(push)
/* Original cbDst is int, the NTDLL declaration uses PULONG. */
#pragma warning(disable:4057)
#include "class-capture.inc"
#pragma warning(pop)
#include "ntsend-fixture.h"
static ATOM NtUserRegisterClassExWOW(WNDCLASSEXA *wc, PUNICODE_STRING name,
    PCLSMENUNAME menu, PROC worker, WORD fnid, DWORD flags, LPDWORD wow)
{
    ++calls;
    received = *wc; received_flags = flags; received_fnid = fnid; received_wow = wow;
    CHECK(worker == NULL);
    CHECK(name->Buffer && !wcscmp(name->Buffer, L"S40_CLASS"));
    if (HIWORD(wc->lpszMenuName)) {
        CHECK(!strcmp(menu->pszClientAnsiMenuName, wc->lpszMenuName));
        CHECK(!wcscmp(menu->pwszClientUnicodeMenuName, L"MENU"));
    } else CHECK(menu->pszClientAnsiMenuName == wc->lpszMenuName);
    if (native_backend) {
        return wow_class_words_register(&class_context, wc, name, wow, menu);
    }
    if (kernel_result) owned_menu = *menu;
    return kernel_result;
}
static BOOL NtUserUnregisterClass(PUNICODE_STRING name, HINSTANCE module, PCLSMENUNAME menu)
{
    ++unregister_calls;
    CHECK(module == (HINSTANCE)(native_backend ? 0x23451234 : 0x1234));
    CHECK(name->Buffer && !wcscmp(name->Buffer, L"S40_CLASS"));
    if (native_backend) {
        ATOM atom = wow_class_words_find_atom(&class_context, name);
        if (!wow_class_words_unregister(&class_context, atom, module, menu))
            return FALSE;
        CHECK(menu->pusMenuName == NULL);
        return TRUE;
    } else if (reject_unregister) { SetLastError(ERROR_CLASS_HAS_WINDOWS); return FALSE; }
    *menu = owned_menu;
    ZeroMemory(&owned_menu, sizeof(owned_menu));
    return TRUE;
}
#define LocalAlloc tracked_alloc
#define LocalFree tracked_free
#undef UnregisterClass
#define UnregisterClass fixture_UnregisterClassA
#pragma warning(push)
/* Original ENDCALL(BOOL) returns through the ATOM signature. */
#pragma warning(disable:4244)
#ifdef WOW_CLASS_MIRROR
#include "ntcftxt.h"
#include "class-registration-wrapper.inc"
#else
#include "class-registration.inc"
#endif
#pragma warning(pop)
#undef LocalAlloc
#undef LocalFree
static WNDCLASSEXA fresh(void)
{
    WNDCLASSEXA wc;
    ZeroMemory(&wc, sizeof(wc));
    wc.cbSize = sizeof(wc); wc.hInstance = (HINSTANCE)0x1234;
    wc.lpszClassName = "S40_CLASS"; wc.lpszMenuName = MAKEINTRESOURCEA(12);
    version = VER31; client_info.dwExpWinVer = VER31;
    valid_brush = TRUE; kernel_result = 0xc123; calls = 0;
    fail_capture = capture_count = 0;
    ZeroMemory(&owned_menu, sizeof(owned_menu));
    ++cases;
    return wc;
}
static void release_menu(void)
{
    if (HIWORD(owned_menu.pszClientAnsiMenuName)) tracked_free(owned_menu.pszClientAnsiMenuName);
    if (HIWORD(owned_menu.pwszClientUnicodeMenuName)) tracked_free(owned_menu.pwszClientUnicodeMenuName);
    CHECK(live_capture == 0 && menu_allocations == menu_frees);
}
int __cdecl main(void)
{
    WNDCLASSEXA wc;
    WNDCLASSA basic;
    DWORD wow[2] = {7, 9};
    hmodUser = GetModuleHandleA("user32.dll");
    capture_teb.StaticUnicodeString.Buffer = capture_teb.StaticUnicodeBuffer;
    capture_teb.StaticUnicodeString.MaximumLength = sizeof(capture_teb.StaticUnicodeBuffer);
    wc = fresh(); wc.lpszMenuName = "MENU";
    CHECK(RegisterClassExWOWA(&wc, wow, NULL, 0) == 0xc123);
    CHECK(calls == 1 && received_flags == CSF_ANSIPROC && received_wow == wow);
    release_menu();
    wc = fresh(); version = VER30;
    CHECK(RegisterClassExWOWA(&wc, wow, NULL, 0) == 1); release_menu();
    wc = fresh(); wc.style = 0x80000000 | CS_HREDRAW;
    CHECK(RegisterClassExWOWA(&wc, wow, NULL, 0) && wc.style == CS_HREDRAW); release_menu();
    wc = fresh(); version = VER40; wc.style = 0x80000000;
    CHECK(!RegisterClassExWOWA(&wc, wow, NULL, 0) && !calls && GetLastError() == ERROR_INVALID_PARAMETER);
    wc = fresh(); wc.cbClsExtra = -1;
    CHECK(!RegisterClassExWOWA(&wc, wow, NULL, 0) && !calls);
    wc = fresh(); wc.cbWndExtra = -1;
    CHECK(!RegisterClassExWOWA(&wc, wow, NULL, 0) && !calls);
    wc = fresh(); wc.cbSize = 1;
    CHECK(RegisterClassExWOWA(&wc, wow, NULL, 0) && calls == 1); release_menu();
    wc = fresh(); wc.hInstance = NULL;
    CHECK(RegisterClassExWOWA(&wc, wow, NULL, 0) && wc.hInstance == GetModuleHandle(NULL)); release_menu();
    wc = fresh(); wc.hInstance = hmodUser; client_info.dwExpWinVer = VER40;
    CHECK(!RegisterClassExWOWA(&wc, wow, NULL, 0) && !calls);
    wc = fresh(); wc.hInstance = hmodUser; client_info.dwExpWinVer = VER40;
    CHECK(RegisterClassExWOWA(&wc, wow, NULL, FNID_DDE_BIT) && received_fnid == 0); release_menu();
    wc = fresh(); version = VER30; wc.hbrBackground = (HBRUSH)0x1234; valid_brush = FALSE;
    CHECK(RegisterClassExWOWA(&wc, wow, NULL, 0) && !wc.hbrBackground); release_menu();
    wc = fresh(); wc.hbrBackground = (HBRUSH)0x1234; valid_brush = FALSE;
    CHECK(!RegisterClassExWOWA(&wc, wow, NULL, 0) && !calls);
    wc = fresh(); wc.lpszMenuName = "MENU"; kernel_result = 0;
    CHECK(!RegisterClassExWOWA(&wc, wow, NULL, 0) && calls == 1); release_menu();
    wc = fresh(); wc.lpszMenuName = "MENU"; fail_capture = 2;
    CHECK(!RegisterClassExWOWA(&wc, wow, NULL, 0) && !calls); release_menu();
    wc = fresh(); memcpy(&basic, &wc.style, sizeof(basic));
    CHECK(RegisterClassWOWA(&basic, wow) == 0xc123);
    CHECK(received.cbSize == sizeof(received) && !received.hIconSm); release_menu();
    /* Original InitClsMenuName failure returns before the caller's cleanup.
     * Observe the real source defect; release ONLY from the test afterwards. */
    wc = fresh(); wc.lpszMenuName = "MENU"; fail_capture = 1;
    CHECK(!RegisterClassExWOWA(&wc, wow, NULL, 0) && !calls);
#ifdef WOW_CLASS_MIRROR
    CHECK(live_capture == 0 && menu_allocations == menu_frees);
    printf("WOW_CORRECTED_CLASS_MENU_CAPTURE_FAILURE retained=%u expected=0\n",
        menu_allocations - menu_frees);
#else
    CHECK(live_capture == 0 && menu_allocations == menu_frees + 2);
    printf("WOW_ORIGINAL_CLASS_MENU_CAPTURE_FAILURE retained=%u expected=2\n",
        menu_allocations - menu_frees);
    tracked_free(last_ansi_menu); tracked_free(last_unicode_menu);
#endif
    CHECK(menu_allocations == menu_frees);
    wc = fresh(); wc.lpszMenuName = "MENU";
    CHECK(RegisterClassExWOWA(&wc, wow, NULL, 0) == 0xc123);
    reject_unregister = TRUE;
    CHECK(!fixture_UnregisterClassA("S40_CLASS", wc.hInstance));
    CHECK(unregister_calls == 1 && GetLastError() == ERROR_CLASS_HAS_WINDOWS);
    CHECK(menu_allocations == menu_frees + 2 && live_capture == 0);
    ++cases; reject_unregister = FALSE;
    CHECK(fixture_UnregisterClassA("S40_CLASS", wc.hInstance));
    /* Original ANSI FIRSTCOPYLPTSTRID aliases COPYLPSTRIDW (forced heap),
     * not FIRSTCOPYLPSTRIDW. Both unregister attempts allocate then free. */
    CHECK(unregister_calls == 2 && capture_count == 4);
    CHECK(menu_allocations == menu_frees && live_capture == 0);
    /* Compose the retained original client with actual native registration,
     * duplicate rejection and live-window unregistration refusal. The WOW
     * thread/version providers remain controlled; this is not tagged guest
     * WNDPROC registration. WC uses the real product storage binding. */
    {
        HWND window;
        WNDCLASSEXA duplicate;
        unsigned retained;
        wow_class_words_binding *borrow, *again;
        ATOM registered_atom;
        UNICODE_STRING lookup_name;
        wc = fresh(); native_backend = TRUE;
        wc.hInstance = (HINSTANCE)0x23451234;
        wc.lpfnWndProc = DefWindowProcA;
        wc.lpszMenuName = "MENU";
        CHECK(RegisterClassExWOWA(&wc, wow, NULL, 0) != 0);
        RtlInitUnicodeString(&lookup_name, L"s40_class");
        registered_atom = wow_class_words_find_atom(&class_context, &lookup_name);
        CHECK(registered_atom != 0);
        borrow = wow_class_words_acquire(&class_context, registered_atom, wc.hInstance);
        CHECK(borrow != NULL);
        if (borrow) {
            CHECK(wow_class_words_value(borrow)[0] == wow[0]);
            CHECK(wow_class_words_value(borrow)[1] == wow[1]);
        }
        retained = menu_allocations - menu_frees;
        CHECK(retained == 2 && live_capture == 0);
        CHECK(!wow_class_words_unregister(&class_context, registered_atom,
            wc.hInstance, NULL));
        CHECK(GetLastError() == ERROR_INVALID_PARAMETER);
        duplicate = wc;
        SetLastError(0);
        CHECK(!RegisterClassExWOWA(&duplicate, wow, NULL, 0));
        CHECK(GetLastError() == ERROR_CLASS_ALREADY_EXISTS);
        CHECK(menu_allocations - menu_frees == retained && live_capture == 0);
        again = wow_class_words_acquire(&class_context, registered_atom, wc.hInstance);
        CHECK(again == borrow);
        wow_class_words_release(again);
        window = CreateWindowExA(0, "S40_CLASS", "", WS_CHILD,
            0, 0, 32, 32, HWND_MESSAGE, NULL, wc.hInstance, NULL);
        CHECK(window != NULL);
        if (window) {
            CLSMENUNAME unchanged, output;
            memset(&unchanged, 0x5a, sizeof(unchanged)); output = unchanged;
            CHECK(!wow_class_words_unregister(&class_context, registered_atom,
                wc.hInstance, &output));
            CHECK(GetLastError() == ERROR_CLASS_HAS_WINDOWS);
            CHECK(!memcmp(&output, &unchanged, sizeof(output)));
            CHECK(!fixture_UnregisterClassA("S40_CLASS", wc.hInstance));
            CHECK(GetLastError() == ERROR_CLASS_HAS_WINDOWS);
            CHECK(menu_allocations - menu_frees == retained && live_capture == 0);
            CHECK(DestroyWindow(window));
        }
        CHECK(fixture_UnregisterClassA("S40_CLASS", wc.hInstance));
        CHECK(menu_allocations == menu_frees && live_capture == 0);
        CHECK(class_context.pclsPrivateList == NULL && class_context.pclsPublicList == NULL);
        if (borrow) CHECK(wow_class_words_value(borrow)[0] == wow[0]);
        wow_class_words_release(borrow);
        if (!errors) puts("WOW_ORIGINAL_CLASS_CLIENT_SHARED_WC_OK");
        if (!errors) puts("WOW_ORIGINAL_CLASS_CLIENT_NATIVE_LIFECYCLE_OK");
    }
    printf("WOW_ORIGINAL_CLASS_REGISTRATION errors=%u cases=%u captures=%u menus=%u/%u\n",
        errors, cases, live_capture, menu_allocations, menu_frees);
    return errors ? 3 : 0;
}
