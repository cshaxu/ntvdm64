#include "precomp.h"
#include "wow_window_dispatch_bindings.h"
#include "wow_window_creation_binding.h"
#include "wow_creation_owner_fixture.h"
#include <stdio.h>

/* Hash-pinned, exact W32Win16WndProcEx body. Task TLS and final guest callback
 * are controlled; selected messages bypass message thunks. Not Win16 execution. */
static OPENNT_SUPPORT_TEB teb;
static TD task;
static unsigned errors, calls, blocks, unblocks;
static DWORD expected_proc;
static BOOL callback_success = TRUE;
static HWND test_window;
static wow_class_lookup_context class_context;
static ATOM class_atom;
static const char class_name[] = "S40_ORIGINAL_WNDPROC_CHAIN";
M32 aw32Msg[1024];
BOOL gfIgnoreInputAssertGiven;
#define CHECK(x) do { if (!(x)) { ++errors; printf("FAIL line=%d\n", __LINE__); } } while (0)
#include "original-wndproc.inc"
BOOL FASTCALL WM32NoThunking(LPWM32MSGPARAMEX frame)
{
    (void)frame; ++errors; return FALSE; /* Address marker, must not execute. */
}
POPENNT_SUPPORT_TEB NTAPI opennt_support_current_teb(void) { return &teb; }
PWC FindPWC(HWND window) { (void)window; ++errors; return NULL; }
WNDPROC GetStdClassWndProc(DWORD cls) { (void)cls; ++errors; return NULL; }
VOID __cdecl BlockWOWIdle(BOOL blocking)
{
    if (blocking) ++blocks;
    else ++unblocks;
}
BOOL CallBack16(INT id, PPARM16 parms, VPPROC procedure, PVPVOID result)
{
    ++calls;
    CHECK(id == RET_WNDPROC && procedure == expected_proc);
    CHECK(parms->WndProc.hwnd == GETHWND16(test_window));
    CHECK(parms->WndProc.wMsg == WM_USER + 7);
    CHECK(parms->WndProc.wParam == 0x4567 && parms->WndProc.lParam == -17);
    CHECK(parms->WndProc.hInst == 0x5678);
    CHECK(unblocks == blocks + 1);
    *result = 0x76543210;
    return callback_success;
}
static DWORD WINAPI selected_callback(HWND window, UINT message, UINT wp,
    LONG lp, DWORD procedure, PVOID words)
{
    /* Creation messages are native-only in this bounded fixture: the actual
     * message-thunk table and guest callbacks are not supplied here. */
    if (message != WM_USER + 7) return DefWindowProcA(window, message, wp, lp);
    return W32Win16WndProcEx(window, message, wp, lp, procedure, words);
}
static HWND create_case(DWORD procedure, const WW *words)
{
    CREATESTRUCTA request = {0};
    wow_window_dispatch_target target = {procedure, selected_callback};
    request.hInstance = (HINSTANCE)words->hInstance;
    request.lpszClass = class_name; request.lpszName = "";
    request.cx = request.cy = 32;
    return wow_window_create_boundA(wow_fixture_creation_owner(&class_context), class_atom, &request, words, &target);
}
int __cdecl main(void)
{
    WW words = {0};
    WNDCLASSEXA cls = {sizeof(cls)};
    WCHAR wide_name[] = L"S40_ORIGINAL_WNDPROC_CHAIN";
    UNICODE_STRING capture = {sizeof(wide_name) - sizeof(WCHAR), sizeof(wide_name), wide_name};
    DWORD class_words[2] = {0x12340056, 0};
    unsigned before;
    teb.WOW32Reserved = &task;
    words.iClass = WOWCLASS_WIN16; words.flState = WWSTATE_ICLASSISSET;
    words.hInstance = (HANDLE)0x23455678;
    cls.hInstance = (HINSTANCE)words.hInstance; cls.lpszClassName = class_name;
    cls.lpfnWndProc = wow_window_native_proc;
    class_atom = wow_class_words_register(&class_context, &cls, &capture, class_words, NULL);
    CHECK(class_atom != 0); if (!class_atom) return 3;
    test_window = create_case(0x92340056, &words);
    CHECK(test_window != NULL); if (!test_window) return 3;
    /* WU32RegisterClass supplies the encoded tag. Original callback decodes
     * it; do not strip the selector high bit again in a native gateway. */
    expected_proc = 0x12340056;
    CHECK(SendMessageA(test_window, WM_USER + 7, 0x12344567, -17) == 0x76543210);
    CHECK(DestroyWindow(test_window));
    test_window = create_case(0x92300056, &words);
    CHECK(test_window != NULL); if (!test_window) return 3;
    expected_proc = 0x92340056;
    CHECK(SendMessageA(test_window, WM_USER + 7, 0x12344567, -17) == 0x76543210);
    CHECK(calls == 2 && blocks == 2 && unblocks == 2);
    callback_success = FALSE;
    CHECK(SendMessageA(test_window, WM_USER + 7, 0x4567, -17) ==
        DefWindowProcA(test_window, WM_USER + 7, 0x4567, -17));
    CHECK(calls == 3 && blocks == 3 && unblocks == 3);
    before = calls; task.dwFlags |= TDF_IGNOREINPUT;
    CHECK(SendMessageA(test_window, WM_USER + 7, 0x4567, -17) ==
        DefWindowProcA(test_window, WM_USER + 7, 0x4567, -17));
    CHECK(calls == before && blocks == 3 && unblocks == 3);
    CHECK(DestroyWindow(test_window));
    CHECK(wow_class_words_unregister(&class_context, class_atom, cls.hInstance, NULL));
    CHECK(class_context.pclsPrivateList == NULL);
    printf("WOW_ORIGINAL_WNDPROC errors=%u callbacks=%u idle_pairs=%u\n", errors, calls, blocks);
    return errors ? 3 : 0;
}
