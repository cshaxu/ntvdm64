/* Independently authored raw host prerequisite. No WOW provider, guest,
 * injected code, hidden carrier window or product ownership policy. The
 * private BASE export is resolved only in this disposable x86 process. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <dde.h>
#include <stdio.h>

typedef BOOL (WINAPI *free_hook_fn)(HGLOBAL);
typedef VOID (WINAPI *register_hook_fn)(free_hook_fn);
typedef HGLOBAL (WINAPI *global_free_fn)(HGLOBAL);

static HGLOBAL watched;
static unsigned notifications, failures;
static BOOL permit_free = TRUE;
static unsigned hook_mode, hook_calls;
static LPARAM hook_packet;
static BOOL hook_freed;

static BOOL WINAPI observe_free(HGLOBAL handle)
{
    if (handle != watched) return TRUE;
    ++notifications;
    return permit_free;
}

static void begin_observation(HGLOBAL handle, BOOL permit)
{
    watched = handle;
    notifications = 0;
    permit_free = permit;
}

static void check(const char *name, BOOL success)
{
    if (!success) { ++failures; printf("FAIL %s\n", name); }
}

static LRESULT CALLBACK filter(int code, WPARAM wp, LPARAM lp)
{
    MSG *message;
    if (code < 0) return CallNextHookEx(NULL, code, wp, lp);
    message = (MSG *)lp;
    if (!message || message->message != WM_DDE_DATA || message->lParam != hook_packet)
        return CallNextHookEx(NULL, code, wp, lp);
    ++hook_calls;
    if (hook_mode == 1) return FALSE;
    if (hook_mode == 2) {
        hook_freed = FreeDDElParam(message->message, message->lParam);
        message->lParam = 0;
    }
    return TRUE;
}

static void filter_observation(unsigned mode)
{
    MSG message = {0};
    HHOOK installed;
    BOOL result;
    hook_mode = mode;
    hook_calls = 0;
    hook_freed = FALSE;
    message.message = WM_DDE_DATA;
    message.lParam = PackDDElParam(WM_DDE_DATA, 0, 0x1234);
    check("filter-pack", message.lParam != 0);
    if (!message.lParam) return;
    hook_packet = message.lParam;
    begin_observation((HGLOBAL)hook_packet, TRUE);
    installed = SetWindowsHookExW(WH_MSGFILTER, filter, NULL, GetCurrentThreadId());
    check("filter-install", installed != NULL);
    if (!installed) {
        FreeDDElParam(WM_DDE_DATA, hook_packet);
        return;
    }
    result = CallMsgFilterW(&message, MSGF_DIALOGBOX);
    check("filter-called", hook_calls == 1);
    check("filter-result", result == (mode != 1));
    check("filter-free-result", mode != 2 || hook_freed);
    printf("FILTER mode=%u result=%u hook_calls=%u freed=%u input_survives=%u notifications=%u\n",
        mode, (unsigned)result, hook_calls, (unsigned)hook_freed,
        (unsigned)(message.lParam == hook_packet), notifications);
    check("filter-unhook", UnhookWindowsHookEx(installed));
    /* Never free twice or query a freed packet. Both TRUE cases intentionally
     * return the same API result, but only one hook consumed the allocation. */
    if (!hook_freed) FreeDDElParam(WM_DDE_DATA, hook_packet);
    watched = NULL;
}

int main(void)
{
    HMODULE kernel, api_set;
    register_hook_fn register_hook;
    global_free_fn api_free;
    HGLOBAL memory;
    LPARAM packet;
    unsigned calls, i;
    BOOL alive;
    MSG message;
    WCHAR module_path[MAX_PATH];
    static const UINT kinds[] = { WM_DDE_ACK, WM_DDE_ADVISE, WM_DDE_DATA, WM_DDE_POKE };
    kernel = GetModuleHandleW(L"kernel32.dll");
#pragma warning(push)
#pragma warning(disable:4191)
    register_hook = (register_hook_fn)GetProcAddress(kernel, "RegisterWowBaseHandlers");
#pragma warning(pop)
    check("x86-registration-present", register_hook != NULL && sizeof(void *) == 4);
    if (!register_hook || sizeof(void *) != 4) return 1;
    api_set = LoadLibraryW(L"api-ms-win-core-heap-l2-1-0.dll");
    check("api-set-load", api_set != NULL);
    if (!api_set) return 1;
#pragma warning(push)
#pragma warning(disable:4191)
    api_free = (global_free_fn)GetProcAddress(api_set, "GlobalFree");
#pragma warning(pop)
    check("api-set-global-free", api_free != NULL);
    if (!api_free) { FreeLibrary(api_set); return 1; }
    module_path[0] = 0;
    GetModuleFileNameW(api_set, module_path, MAX_PATH);
    printf("HOST width=32 api_set_owner=%ls\n", module_path);
    register_hook(observe_free);

    memory = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, 32);
    check("direct-allocation", memory != NULL);
    if (memory) {
        begin_observation(memory, FALSE);
        GlobalFree(memory);
        alive = GlobalFlags(memory) != GMEM_INVALID_HANDLE;
        check("direct-kernel32-veto", notifications == 1 && alive);
        printf("FREE route=KERNEL32 permit=0 notifications=%u alive=%u\n",
            notifications, (unsigned)alive);
        permit_free = TRUE;
        GlobalFree(memory);
        check("direct-kernel32-release", notifications == 2);
        watched = NULL;
    }

    memory = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, 32);
    check("api-allocation", memory != NULL);
    if (memory) {
        begin_observation(memory, FALSE);
        api_free(memory);
        calls = notifications;
        alive = GlobalFlags(memory) != GMEM_INVALID_HANDLE;
        printf("FREE route=APISET permit=0 notifications=%u alive=%u\n",
            calls, (unsigned)alive);
        /* Observation, not an assumption that every OS routes identically. */
        if (alive) { permit_free = TRUE; GlobalFree(memory); }
        watched = NULL;
    }

    for (i = 0; i != sizeof(kinds) / sizeof(kinds[0]); ++i) {
        packet = PackDDElParam(kinds[i], 0, 0x1234);
        check("dde-pack", packet != 0);
        if (!packet) continue;
        begin_observation((HGLOBAL)packet, TRUE);
        check("dde-free", FreeDDElParam(kinds[i], packet));
        printf("PACKET message=%04X notifications=%u\n", kinds[i], notifications);
        watched = NULL;
    }

    PeekMessageW(&message, NULL, 0, 0, PM_NOREMOVE);
    filter_observation(1); /* Not eaten: caller still owns the packet. */
    filter_observation(2); /* Eaten and freed: do not touch it again. */
    filter_observation(3); /* Eaten but not freed: BOOL alone proves nothing. */
    register_hook(NULL);
    check("api-set-unload", FreeLibrary(api_set));
    printf("DDE_NATIVE_CONTRACT errors=%u guest_proof=0 product_proof=0\n", failures);
    return failures ? 1 : 0;
}
