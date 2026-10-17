#include <windows.h>
#include <stdio.h>
#include <string.h>

/* Research of the public native state owner, not an original dialog test.
 * Hidden test-owned windows only; no desktop switch or input injection.
 * Default mode delivers WM_SETFOCUS directly. --actual-focus instead uses
 * SetFocus/GetFocus. Neither mode proves original dialog navigation or guest
 * acceptance; these radio controls expose the native suppression service. */
static unsigned failures, clicks, intercepted;
static BOOL actual_focus;
static WNDPROC original;
#define CHECK(c) do { if (!(c)) { ++failures; printf("FAIL line=%d error=%lu\n", __LINE__, GetLastError()); } } while (0)
static LRESULT CALLBACK parent_proc(HWND w, UINT m, WPARAM a, LPARAM b)
{
    if (m == WM_COMMAND && HIWORD(a) == BN_CLICKED) {
        ++clicks;
        printf("click id=%u source=%p count=%u\n", LOWORD(a), (void *)b, clicks);
    }
    return DefWindowProcW(w, m, a, b);
}
static LRESULT CALLBACK subclass_proc(HWND w, UINT m, WPARAM a, LPARAM b)
{
    if (m == BM_SETDONTCLICK) { ++intercepted; return 0; }
    return CallWindowProcW(original, w, m, a, b);
}
static void focus_message(HWND button)
{
    printf("focus before window=%p state=%lx\n", (void *)button,
        (unsigned long)SendMessageW(button,BM_GETSTATE,0,0));
    if (actual_focus) {
        SetFocus(button);
        CHECK(GetFocus() == button);
        printf("focus acquired state=%lx clicks=%u\n",
            (unsigned long)SendMessageW(button,BM_GETSTATE,0,0),clicks);
        SetFocus(NULL);
        CHECK(GetFocus() == NULL);
    } else {
        SendMessageW(button, WM_SETFOCUS, 0, 0);
        SendMessageW(button, WM_KILLFOCUS, 0, 0);
    }
}
static void probe(HWND parent, BOOL ansi, DWORD style)
{
    WNDCLASSEXW cls = {sizeof(cls)};
    HWND button = ansi ? CreateWindowExA(0, "Button", "radio", WS_CHILD | style,
        0,0,80,25,parent,(HMENU)1,GetModuleHandleW(NULL),NULL) :
        CreateWindowExW(0,L"Button",L"radio",WS_CHILD | style,
        0,0,80,25,parent,(HMENU)1,GetModuleHandleW(NULL),NULL);
    HWND other = CreateWindowExW(0,L"Button",L"other",WS_CHILD | BS_RADIOBUTTON,
        0,30,80,25,parent,(HMENU)2,GetModuleHandleW(NULL),NULL);
    CHECK(button && other); if (!button || !other) goto cleanup;
    CHECK(GetClassInfoExW(NULL,L"Button",&cls));
    if (!cls.lpfnWndProc) goto cleanup;
    original = (WNDPROC)SetWindowLongPtrW(button,GWLP_WNDPROC,(LONG_PTR)subclass_proc);
    CHECK(original != NULL); if (!original) goto cleanup;
    clicks = intercepted = 0;
    focus_message(button);
    CHECK(clicks == 1); /* Negative control: native focus notifies. */
    SendMessageW(button,BM_SETDONTCLICK,TRUE,0);
    printf("subclass after swallowed set state=%lx\n",
        (unsigned long)SendMessageW(button,BM_GETSTATE,0,0));
    CHECK(intercepted == 1);
    focus_message(button);
    CHECK(clicks == 2); /* A public send can be swallowed by a subclass. */
    CallWindowProcW(cls.lpfnWndProc,button,BM_SETDONTCLICK,TRUE,0);
    CHECK(intercepted == 1);
    focus_message(button);
    CHECK(clicks == 2);
    focus_message(other);
    CHECK(clicks == 3); /* State belongs to this button, not the thread. */
    CallWindowProcW(cls.lpfnWndProc,button,BM_SETDONTCLICK,FALSE,0);
    focus_message(button);
    CHECK(clicks == 4 && intercepted == 1);
    /* Original bit operations are set/clear, not reference counting. */
    CallWindowProcW(cls.lpfnWndProc,button,BM_SETDONTCLICK,TRUE,0);
    CallWindowProcW(cls.lpfnWndProc,button,BM_SETDONTCLICK,TRUE,0);
    CallWindowProcW(cls.lpfnWndProc,button,BM_SETDONTCLICK,FALSE,0);
    focus_message(button);
    CHECK(clicks == 5);
    CallWindowProcW(cls.lpfnWndProc,button,BM_SETDONTCLICK,TRUE,0);
    CHECK(DestroyWindow(button)); button = NULL;
    focus_message(other);
    CHECK(clicks == 6);
    printf("button_focus_service ansi=%u style=%lu clicks=%u intercepted=%u\n",
        ansi,style,clicks,intercepted);
cleanup:
    if (button) CHECK(DestroyWindow(button));
    if (other) CHECK(DestroyWindow(other));
}
static void public_service_check(HWND parent)
{
    HWND button = CreateWindowExW(0,L"Button",L"public",WS_CHILD | BS_RADIOBUTTON,
        0,0,80,25,parent,(HMENU)3,GetModuleHandleW(NULL),NULL);
    CHECK(button != NULL); if (!button) return;
    clicks = 0;
    SendMessageW(button,BM_SETDONTCLICK,TRUE,0);
    printf("public after set state=%lx\n",
        (unsigned long)SendMessageW(button,BM_GETSTATE,0,0));
    focus_message(button);
    printf("public_send_suppressed_clicks=%u\n",clicks);
    CHECK(clicks == 0);
    SendMessageW(button,BM_SETDONTCLICK,FALSE,0);
    focus_message(button);
    CHECK(clicks == 1);
    CHECK(DestroyWindow(button));
}
int __cdecl main(int argc, char **argv)
{
    WNDCLASSEXW cls = {sizeof(cls)};
    HWND parent;
    ATOM atom;
    WCHAR module_path[MAX_PATH];
    WNDCLASSEXW native_button = {sizeof(native_button)};
    HMODULE user = GetModuleHandleW(L"user32.dll");
    CHECK(GetModuleFileNameW(user,module_path,ARRAYSIZE(module_path)) != 0);
    wprintf(L"loaded_user32=%ls base=%p\n",module_path,(void *)user);
    CHECK(GetClassInfoExW(NULL,L"Button",&native_button));
    printf("native_button_proc=%p\n",(void *)native_button.lpfnWndProc);
    {
        HMODULE owner = NULL;
        if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                (LPCWSTR)native_button.lpfnWndProc,&owner)) {
            CHECK(GetModuleFileNameW(owner,module_path,ARRAYSIZE(module_path)) != 0);
            wprintf(L"button_owner=%ls base=%p rva=%lx\n",module_path,(void *)owner,
                (unsigned long)((ULONG_PTR)native_button.lpfnWndProc-(ULONG_PTR)owner));
        } else CHECK(FALSE);
    }
    actual_focus = argc == 2 && !strcmp(argv[1], "--actual-focus");
    if (argc > 1 && !actual_focus) return 2;
    printf("actual_focus=%u\n", actual_focus);
    cls.hInstance = GetModuleHandleW(NULL);
    cls.lpfnWndProc = parent_proc;
    cls.lpszClassName = L"T447ButtonFocusService";
    atom = RegisterClassExW(&cls);
    CHECK(atom != 0); if (!atom) return 1;
    parent = CreateWindowExW(0,MAKEINTATOM(atom),L"",WS_OVERLAPPED,
        0,0,120,100,NULL,NULL,cls.hInstance,NULL);
    CHECK(parent != NULL);
    if (parent) {
        public_service_check(parent);
        probe(parent,FALSE,BS_RADIOBUTTON);
        probe(parent,TRUE,BS_RADIOBUTTON);
        probe(parent,FALSE,BS_AUTORADIOBUTTON);
        probe(parent,TRUE,BS_AUTORADIOBUTTON);
        CHECK(DestroyWindow(parent));
    }
    CHECK(UnregisterClassW(MAKEINTATOM(atom),cls.hInstance));
    printf("button_focus_service failures=%u\n",failures);
    return failures ? 1 : 0;
}
