#include <windows.h>
#include <stdio.h>

/* Research-only public USER lifecycle evidence. The DLL is required by the
 * documented in-context WinEvent ABI; it is not a product provider. */
#ifdef WOW_DESTROY_EVENT_DLL
static HINSTANCE module;
static WINEVENTPROC observer;
BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
    (void)reserved;
    if (reason == DLL_PROCESS_ATTACH) module = instance;
    return TRUE;
}
static void CALLBACK event_entry(HWINEVENTHOOK hook, DWORD event, HWND window,
    LONG object, LONG child, DWORD thread, DWORD time)
{
    if (observer) observer(hook, event, window, object, child, thread, time);
}
__declspec(dllexport) HWINEVENTHOOK WINAPI ObserveDestroy(WINEVENTPROC callback)
{
    observer = callback;
    return SetWinEventHook(EVENT_OBJECT_DESTROY, EVENT_OBJECT_DESTROY,
        module, event_entry, GetCurrentProcessId(), GetCurrentThreadId(),
        WINEVENT_INCONTEXT);
}
#else
static unsigned failures, mode, sequence;
static DWORD owner_thread;
static BOOL cancel_destroy;
static HWND subjects[4];
static unsigned used, events[4], terminal[4];
#define CHECK(x) do { if (!(x)) { ++failures; printf("FAIL line=%d error=%lu\n", __LINE__, GetLastError()); } } while (0)
static int subject_index(HWND window)
{
    unsigned i;
    for (i = 0; i < used; ++i) if (subjects[i] == window) return (int)i;
    return -1;
}
static void CALLBACK observe(HWINEVENTHOOK hook, DWORD event, HWND window,
    LONG object, LONG child, DWORD thread, DWORD time)
{
    int index = subject_index(window);
    (void)hook; (void)time;
    if (event != EVENT_OBJECT_DESTROY || object != OBJID_WINDOW ||
            child != CHILDID_SELF || index < 0) return;
    CHECK(GetCurrentThreadId() == owner_thread && thread == owner_thread);
    ++events[index];
    printf("DESTROY_EVENT mode=%u index=%d order=%u terminal=%u live=%u\n",
        mode, index, ++sequence, terminal[index], IsWindow(window));
}
static LRESULT CALLBACK window_proc(HWND window, UINT message, WPARAM wp, LPARAM lp)
{
    int index;
    if (message == WM_NCCREATE) {
        CHECK(used < ARRAYSIZE(subjects));
        if (used < ARRAYSIZE(subjects)) subjects[used++] = window;
        if (mode == 2) return FALSE;
    }
    if (message == WM_CREATE && mode == 1) return -1;
    index = subject_index(window);
    if (message == WM_NCDESTROY && index >= 0) {
        ++terminal[index];
        printf("DESTROY_BODY mode=%u index=%d order=%u events=%u live=%u\n",
            mode, index, ++sequence, events[index], IsWindow(window));
        return 73; /* Deliberately swallow canonical cleanup. */
    }
    return DefWindowProcW(window, message, wp, lp);
}
static LRESULT CALLBACK cbt(int code, WPARAM wp, LPARAM lp)
{
    if (code == HCBT_DESTROYWND && cancel_destroy && subject_index((HWND)wp) >= 0)
        return 1;
    return CallNextHookEx(NULL, code, wp, lp);
}
int __cdecl main(void)
{
    typedef HWINEVENTHOOK (WINAPI *install_fn)(WINEVENTPROC);
    HMODULE dll = LoadLibraryW(L"wow_window_destroy_event_probe.dll");
    install_fn install;
    HWINEVENTHOOK hook;
    HHOOK veto;
    WNDCLASSEXW cls = {sizeof(cls)};
    ATOM atom;
    CHECK(dll != NULL); if (!dll) return 1;
    install = (install_fn)GetProcAddress(dll, "_ObserveDestroy@4");
    CHECK(install != NULL); if (!install) { FreeLibrary(dll); return 1; }
    owner_thread = GetCurrentThreadId();
    hook = install(observe);
    CHECK(hook != NULL); if (!hook) { FreeLibrary(dll); return 1; }
    veto = SetWindowsHookExW(WH_CBT, cbt, NULL, owner_thread);
    CHECK(veto != NULL);
    cls.hInstance = GetModuleHandleW(NULL);
    cls.lpfnWndProc = window_proc;
    cls.lpszClassName = L"WOWDestroyEventResearch";
    atom = RegisterClassExW(&cls); CHECK(atom != 0);
    if (atom) for (mode = 0; mode < 6; ++mode) {
        HWND root, child = NULL;
        unsigned i;
        used = sequence = 0; cancel_destroy = FALSE;
        ZeroMemory(events, sizeof(events)); ZeroMemory(terminal, sizeof(terminal));
        root = CreateWindowExW(0, MAKEINTATOM(atom), L"", WS_POPUP,
            0, 0, 16, 16, NULL, NULL, cls.hInstance, NULL);
        if (mode == 1 || mode == 2) CHECK(!root);
        else {
            CHECK(root != NULL);
            if (root && mode == 3) {
                CHECK(SendMessageW(root, WM_NCDESTROY, 0, 0) == 73);
                CHECK(IsWindow(root));
                printf("DESTROY_SYNTHETIC events=%u terminal=%u\n", events[0], terminal[0]);
            }
            if (root && mode == 4) {
                child = CreateWindowExW(0, MAKEINTATOM(atom), L"", WS_CHILD,
                    0, 0, 8, 8, root, NULL, cls.hInstance, NULL);
                CHECK(child != NULL);
            }
            if (root && mode == 5) {
                cancel_destroy = TRUE;
                CHECK(!DestroyWindow(root) && IsWindow(root));
                CHECK(!events[0] && !terminal[0]);
                cancel_destroy = FALSE;
            }
            if (root) CHECK(DestroyWindow(root));
            if (child) CHECK(!IsWindow(child));
        }
        for (i = 0; i < used; ++i)
            printf("DESTROY_RESULT mode=%u index=%u events=%u terminal=%u live=%u\n",
                mode, i, events[i], terminal[i], IsWindow(subjects[i]));
    }
    if (atom) CHECK(UnregisterClassW(MAKEINTATOM(atom), cls.hInstance));
    if (veto) CHECK(UnhookWindowsHookEx(veto));
    CHECK(UnhookWinEvent(hook));
    CHECK(FreeLibrary(dll));
    printf("DESTROY_EVENT_RESEARCH failures=%u semantic_equivalence=unproven\n", failures);
    return failures ? 1 : 0;
}
#endif
