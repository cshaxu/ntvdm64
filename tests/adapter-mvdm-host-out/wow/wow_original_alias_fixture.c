#include "precomp.h"
#include "wow_user_borrow_scope.h"
#include "wow_user_thunk_scope.h"
#include "original-alias.inc"
#include "original-getprocedure.inc"
#include "original-subclass.inc"
#include "original-class-subclass.inc"
#undef GetWindowLongA /* Diagnostics below query native USER, not the facade. */

static unsigned errors, calls;
static HWND window, standard;
static wow_class_lookup_context context;
static ATOM atom;
static HINSTANCE module;
static OPENNT_SUPPORT_TEB teb;
static TD task;
static DWORD thunk_input;
#define CHECK(x) do { if (!(x)) { ++errors; printf("FAIL line=%d error=%lu\n", __LINE__, GetLastError()); } } while (0)
static LRESULT CALLBACK procedure(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    if (msg == WM_USER) return 42;
    if (msg == WM_NCDESTROY) wow_window_words_detach(hwnd);
    return DefWindowProcA(hwnd, msg, wp, lp);
}
POPENNT_SUPPORT_TEB NTAPI opennt_support_current_teb(void) { return &teb; }
PVOID FASTCALL GetPModeVDMPointerAssert(DWORD address)
{
    (void)address;
    CHECK(FALSE); return NULL; /* Menu-string guest path is not selected. */
}
DWORD GetThunkWindowProc(DWORD proc, LPSTR cls, PWW value, HWND hwnd)
{
    CHECK(cls == NULL && value != NULL && hwnd != NULL);
    thunk_input = proc;
    return 0x24680010; /* Only final guest thunk allocation is simulated. */
}
DWORD IsThunkWindowProc(DWORD proc, PINT cls)
{
    if (cls) *cls = WOWCLASS_WIN16;
    if (proc == 0x24680012) return (DWORD)wow_window_native_proc;
    return proc == 0x24680010 ? (DWORD)procedure : 0;
}
LONG W32DialogFunc(HWND hwnd, UINT msg, DWORD wp, LONG lp)
{
    (void)hwnd; (void)msg; (void)wp; (void)lp;
    CHECK(FALSE); return 0; /* Dialog branch is not selected by this fixture. */
}
static DWORD WINAPI callback(HWND hwnd, UINT msg, UINT wp, LONG lp,
    DWORD proc, PVOID value)
{
    (void)value;
    if (msg == WM_USER) return proc;
    return DefWindowProcA(hwnd, msg, wp, lp);
}
static ULONG query(HWND hwnd)
{
    BYTE storage[sizeof(VDMFRAME) + sizeof(GETWINDOWLONG16)] = {0};
    PVDMFRAME frame = (PVDMFRAME)storage;
    PGETWINDOWLONG16 args = (PGETWINDOWLONG16)&frame->bArgs;
    args->f1 = GETHWND16(hwnd); args->f2 = GWL_WNDPROC;
    thunk_input = 0;
    return wow_user_invoke_thunk(WU32GetWindowLong, frame);
}
static ULONG update(HWND hwnd, DWORD proc)
{
    BYTE storage[sizeof(VDMFRAME) + sizeof(SETWINDOWLONG16)] = {0};
    PVDMFRAME frame = (PVDMFRAME)storage;
    PSETWINDOWLONG16 args = (PSETWINDOWLONG16)&frame->bArgs;
    args->f1 = GETHWND16(hwnd); args->f2 = GWL_WNDPROC; args->f3 = proc;
    return wow_user_invoke_thunk(WU32SetWindowLong, frame);
}
static ULONG update_class(HWND hwnd, SHORT index, DWORD proc)
{
    BYTE storage[sizeof(VDMFRAME) + sizeof(SETCLASSLONG16)] = {0};
    PVDMFRAME frame = (PVDMFRAME)storage;
    PSETCLASSLONG16 args = (PSETCLASSLONG16)&frame->bArgs;
    args->f1 = GETHWND16(hwnd); args->f2 = index; args->f3 = proc;
    return wow_user_invoke_thunk(WU32SetClassLong, frame);
}
static ULONG FASTCALL consumer(PVDMFRAME frame)
{
    PWW words, again, standard_words;
    PWC class_words;
    ++calls; CHECK(frame->wAX == 0x1357);
    CHECK(FindPWW(NULL, WOWCLASS_UNKNOWN) == NULL);
    words = FindPWW(window, WOWCLASS_UNKNOWN);
    CHECK(words && words->iClass == WOWCLASS_UNKNOWN);
    CHECK(words && (words->flState & WWSTATE_ICLASSISSET));
    again = FindPWW(window, WOWCLASS_EDIT);
    CHECK(again == words && again->iClass == WOWCLASS_UNKNOWN);
    SETWL(window, GWL_WOWiClassAndflState, 0);
    CHECK(FindPWW(window, WOWCLASS_WIN16) == words && words->iClass == WOWCLASS_WIN16);
    class_words = FindPWC(window);
    CHECK(class_words && class_words->vpfnWndProc == 0x12345678);
    CHECK(update_class(window, GCL_WNDPROC, 0x92340078) == 0x12345678);
    CHECK(class_words && class_words->vpfnWndProc == 0x92340078);
    CHECK(wow_user_get_class_longA(window, GCL_WNDPROC) == 0x92300078);
    CHECK(query(window) == 0x12340056);
    CHECK(update_class(window, GCL_WNDPROC, 0x24680010) == 0x92340078);
    CHECK(class_words && class_words->vpfnWndProc == 0);
    CHECK(wow_user_get_class_longA(window, GCL_WNDPROC) == (DWORD)procedure);
    CHECK(update_class(window, GCL_WNDPROC, 0x12345678) == 0x24680010);
    CHECK(thunk_input == (DWORD)procedure);
    CHECK(class_words && class_words->vpfnWndProc == 0x12345678);
    /* Original compatibility policy uses two reserved class DWORDs. The
     * recovered logical GCL_WNDPROC tag must select the WOW branch. */
    CHECK(update_class(window, GCL_CBCLSEXTRA, 5) == 8);
    CHECK(update_class(window, GCL_CBCLSEXTRA, 7) == 5);
    CHECK(GetClassLongA(window, GCL_CBCLSEXTRA) == 16);
    CHECK(GetClassLongA(window, 8) == 7 && GetClassLongA(window, 12) == 1);
    SETWC(window, GCL_WOWvpszMenu, 0x11223344);
    CHECK(class_words && class_words->vpszMenu == 0x11223344);
    standard_words = FindPWW(standard, WOWCLASS_UNKNOWN);
    CHECK(standard_words && standard_words->iClass == WOWCLASS_STATIC);
    CHECK(GetStdClassNumber("sTaTiC") == WOWCLASS_STATIC);
    CHECK(query(window) == 0x12340056 && !thunk_input);
    CHECK(update(window, 0x92340078) == 0x12340056);
    CHECK(words->vpfnWndProc == 0x92340078 && query(window) == 0x92340078);
    CHECK((DWORD)SendMessageA(window, WM_USER, 0, 0) == 0x92300078);
    {
        WW before = *words;
        CHECK(update(window, 0x24680012) == 0);
        CHECK(GetLastError() == ERROR_INVALID_PARAMETER);
        CHECK(memcmp(&before, words, sizeof(before)) == 0);
        CHECK(query(window) == 0x92340078);
    }
    CHECK(update(window, 0x24680010) == 0x92340078);
    CHECK(words->vpfnWndProc == 0 && query(window) == 0x24680010);
    CHECK(thunk_input == (DWORD)procedure);
    CHECK(SendMessageA(window, WM_USER, 0, 0) == 42);
    CHECK(GetWindowLongA(window, GWL_WNDPROC) == (LONG)wow_window_native_proc);
    {
        DWORD previous;
        CHECK(wow_window_procedure_exchange(window, 0x92340056, &previous));
        CHECK(wow_window_procedure_exchange(window, 0x92300056, &previous));
        CHECK(previous == 0x92340056);
        CHECK(query(window) == 0x92340056 && !thunk_input);
        CHECK(wow_window_procedure_exchange(window, (DWORD)procedure, &previous));
        CHECK(query(window) == 0x24680010 && thunk_input == (DWORD)procedure);
        CHECK(query(standard) == 0x24680010 && thunk_input != 0);
    }
    wow_window_words_detach(standard);
    CHECK(DestroyWindow(standard));
    CHECK(DestroyWindow(window));
    CHECK(wow_class_words_unregister(&context, atom, module, NULL));
    CHECK(words && words->iClass == WOWCLASS_WIN16);
    CHECK(class_words && class_words->vpszMenu == 0x11223344);
    CHECK(standard_words && standard_words->iClass == WOWCLASS_STATIC);
    return 0x76543210;
}
int __cdecl main(void)
{
    WNDCLASSEXA cls = {sizeof(cls)};
    UNICODE_STRING name;
    DWORD wc[2] = {0x12345678, 0};
    WW words = {0};
    VDMFRAME frame = {0};
    wow_class_words_binding *class_borrow;
    wow_window_dispatch_target target = {0x92340056, callback};
    wow_window_dispatch_target class_target = {0x92345678, callback};
    teb.WOW32Reserved = &task;
    module = (HINSTANCE)0x23451234;
    cls.hInstance = module; cls.lpfnWndProc = procedure;
    cls.cbClsExtra = 16; /* Eight app bytes and original WOW's two DWORDs. */
    cls.lpszClassName = "S40_ORIGINAL_ALIAS";
    RtlInitUnicodeString(&name, L"S40_ORIGINAL_ALIAS");
    atom = wow_class_words_register_target(&context, &cls, &name, wc, NULL, &class_target);
    CHECK(atom != 0); if (!atom) return 3;
    window = CreateWindowExA(0, cls.lpszClassName, "", 0, 0, 0, 16, 16,
        NULL, NULL, module, NULL);
    standard = CreateWindowExA(0, "STATIC", "", 0, 0, 0, 16, 16,
        NULL, NULL, GetModuleHandleA(NULL), NULL);
    CHECK(window && standard); if (!window || !standard) return 3;
    printf("WOW_NATIVE_HWND full=%08lx low=%04x full_valid=%d zero_valid=%d signed_valid=%d\n",
        (DWORD)window, LOWORD(window), IsWindow(window),
        IsWindow((HWND)(DWORD)LOWORD(window)), IsWindow(HWND32(GETHWND16(window))));
    class_borrow = wow_class_words_acquire(&context, atom, module);
    CHECK(wow_window_words_attach_target(window, &words, class_borrow, &target));
    CHECK(SetWindowLongA(window, GWL_WNDPROC, (LONG)wow_window_native_proc) != 0);
    CHECK(SetClassLongA(window, GCL_WNDPROC, (LONG)wow_window_native_proc) != 0);
    CHECK(wow_window_words_attach(standard, &words));
    {
        HWND short_window = HWND32(GETHWND16(window));
        DWORD process = 0;
        DWORD thread = GetWindowThreadProcessId(short_window, &process);
        printf("WOW_SHORT_HWND thread=%lu process=%lu property=%p query=%08lx\n",
            thread, process,
            GetPropW(short_window, L"NTVDM.WOW.WindowWords"),
            (DWORD)GetWindowLongA(short_window, GWL_WNDPROC));
    }
    frame.wAX = 0x1357;
    CHECK(wow_user_invoke_thunk(consumer, &frame) == 0x76543210);
    CHECK(wow_user_borrow_window(NULL) == NULL && GetLastError() == ERROR_INVALID_STATE);
    CHECK(!context.pclsPrivateList && !context.pclsPublicList);
    printf("WOW_ORIGINAL_ALIAS errors=%u calls=%u\n", errors, calls);
    return errors ? 3 : 0;
}
