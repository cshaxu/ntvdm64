#include "wow_window_creation_binding.h"
#include "wow_user_runtime.h"
typedef VOID (APIENTRY *PFNW32ET)(VOID);
#pragma warning(push)
#pragma warning(disable:4201)
#include "opennt-abi/source/public/internal/windows/inc/wowuserp.h"
#pragma warning(pop)

/* ADAPTER-WOW-031: original createw.c binds class and WW before CBT/WNDPROC.
 * Public USER owns creation, message order and failure; a thread-local CBT
 * scope supplies only unavailable metadata publication. Application creation
 * parameters are never changed. No guest conversion or USER policy lives here. */
typedef struct creation_scope {
    struct creation_scope *previous;
    const CREATESTRUCTA *request;
    const WW *words;
    const wow_window_dispatch_target *target;
    wow_window_dispatch_target target_snapshot;
    ATOM atom;
    HHOOK hook;
    HWND window;
    BOOL claimed;
    DWORD failure;
    wow_class_words_binding *class_words;
    wow_window_words_binding *borrow;
    wow_task_order_thread *thread;
} creation_scope;
static __declspec(thread) creation_scope *current_scope;

static LPVOID application_parameter(const creation_scope *scope,
    const CREATESTRUCTA *creation)
{
    /* Original _CreateWindowEx wraps this parameter for WS_EX_MDICHILD.
     * Public USER performs that policy; match the original opaque value,
     * without replacing or retaining its temporary MDICREATESTRUCT. */
    if ((scope->request->dwExStyle & WS_EX_MDICHILD) &&
            (creation->dwExStyle & WS_EX_MDICHILD) && creation->lpCreateParams)
        return (LPVOID)((const MDICREATESTRUCTA *)creation->lpCreateParams)->lParam;
    return creation->lpCreateParams;
}

static LRESULT CALLBACK creation_hook(int code, WPARAM wp, LPARAM lp)
{
    creation_scope *scope = current_scope;
    if (code == HCBT_CREATEWND && scope && !scope->claimed) {
        CBT_CREATEWNDA *creation = (CBT_CREATEWNDA *)lp;
        CREATESTRUCTA *cs = creation->lpcs;
        if (cs->hInstance == scope->request->hInstance &&
                (ATOM)GetClassLongA((HWND)wp, GCW_ATOM) == scope->atom &&
                application_parameter(scope, cs) == scope->request->lpCreateParams) {
            WW words = *scope->words;
            scope->claimed = TRUE;
            words.dwStyle = cs->style;
            words.dwExStyle = cs->dwExStyle;
            words.hInstance = cs->hInstance;
            if (!wow_window_words_attach_target((HWND)wp, &words,
                    scope->class_words, scope->target)) {
                scope->failure = GetLastError();
                return 1;
            }
            scope->class_words = NULL; /* Ownership moved to the window. */
            scope->window = (HWND)wp;
            scope->borrow = wow_window_words_acquire(scope->window);
            if (!scope->borrow) {
                scope->failure = GetLastError();
                wow_window_words_detach(scope->window);
                return 1;
            }
            if (!wow_window_words_publish_owner(scope->window,scope->borrow,scope->thread)) {
                scope->failure=GetLastError();
                return 1;
            }
        }
    }
    return CallNextHookEx(NULL, code, wp, lp);
}

static HWND create_bound(wow_task_order_thread *thread,
    ATOM atom, const CREATESTRUCTA *request, const WW *words,
    const wow_window_dispatch_target *target, BOOL unicode)
{
    creation_scope scope = {0};
    HWND result = NULL;
    DWORD error = ERROR_SUCCESS;
    wow_class_lookup_context *context;
    if (!thread || !thread->ppi || !thread->ppi->classes || !thread->ppi->objects ||
            !atom || !request || !words) {
        SetLastError(ERROR_INVALID_PARAMETER); return NULL;
    }
    context = thread->ppi->classes;
    scope.thread = thread;
    scope.class_words = wow_class_words_acquire(context, atom, request->hInstance);
    if (!scope.class_words) return NULL;
    scope.request = request; scope.words = words; scope.atom = atom;
    /* Original createw.c initializes the window's procedure from its class.
     * Explicit targets remain available for already resolved creation paths. */
    if (target) scope.target_snapshot = *target;
    else (void)wow_class_words_target(scope.class_words, &scope.target_snapshot);
    scope.target = (target || scope.target_snapshot.procedure) ? &scope.target_snapshot : NULL;
    scope.previous = current_scope;
    scope.hook = SetWindowsHookExA(WH_CBT, creation_hook, NULL, GetCurrentThreadId());
    if (!scope.hook) {
        error = GetLastError();
        wow_class_words_release(scope.class_words);
        SetLastError(error); return NULL;
    }
    current_scope = &scope;
    __try {
        if (unicode)
            result = CreateWindowExW(request->dwExStyle, (LPCWSTR)request->lpszClass,
                (LPCWSTR)request->lpszName, request->style, request->x, request->y,
                request->cx, request->cy, request->hwndParent, request->hMenu,
                request->hInstance, request->lpCreateParams);
        else result = CreateWindowExA(request->dwExStyle, request->lpszClass,
            request->lpszName, request->style, request->x, request->y,
            request->cx, request->cy, request->hwndParent, request->hMenu,
            request->hInstance, request->lpCreateParams);
        error = scope.failure ? scope.failure : GetLastError();
        if (result && (!scope.borrow || scope.window != result)) {
            DestroyWindow(result);
            result = NULL; error = ERROR_INVALID_DATA;
        }
    } __finally {
        if (!result && scope.borrow)
            wow_window_words_detach_if(scope.window, scope.borrow);
        current_scope = scope.previous;
        UnhookWindowsHookEx(scope.hook);
        wow_window_words_release(scope.borrow);
        wow_class_words_release(scope.class_words);
    }
    SetLastError(error);
    return result;
}

HWND WINAPI wow_window_create_boundA(wow_task_order_thread *context,
    ATOM atom, const CREATESTRUCTA *request, const WW *words,
    const wow_window_dispatch_target *target)
{
    return create_bound(context, atom, request, words, target, FALSE);
}

HWND WINAPI wow_window_create_boundW(wow_task_order_thread *context,
    ATOM atom, const CREATESTRUCTW *request, const WW *words,
    const wow_window_dispatch_target *target)
{
    CREATESTRUCTA carrier;
    C_ASSERT(sizeof(CREATESTRUCTA) == sizeof(CREATESTRUCTW));
    if (!request) { SetLastError(ERROR_INVALID_PARAMETER); return NULL; }
    /* Public A/W structures differ only in string pointer types. Copy their
     * representation, never convert or dereference strings in the shared scope. */
    CopyMemory(&carrier, request, sizeof(carrier));
    return create_bound(context, atom, &carrier, words, target, TRUE);
}

HWND WINAPI wow_user_create_window_ex(DWORD ex_style, LPCTSTR class_name,
    LPCTSTR window_name, DWORD style, int x, int y, int width, int height,
    HWND parent, HMENU menu, HANDLE instance, LPVOID parameter, DWORD flags,
    LPDWORD input_words)
{
    wow_user_runtime_thread *binding = wow_user_runtime_current();
    UNICODE_STRING name;
    WCHAR *converted = NULL;
    int characters;
    ATOM atom;
    CREATESTRUCTA request = {0};
    WW words = {0};
    HWND result;

    /* `wuwind.c` is the sole selected caller and passes CW_FLAGS_ANSI.  Other
     * historical create flags were private NtUser policy; reject them rather
     * than silently changing their meaning at the public USER32 boundary. */
    if (!binding || !binding->thread || !binding->thread->ppi ||
            !binding->thread->ppi->classes || !class_name || !input_words ||
            flags != CW_FLAGS_ANSI) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return NULL;
    }
    if (!HIWORD((ULONG_PTR)class_name)) {
        name.Buffer = (PWSTR)class_name;
        name.Length = name.MaximumLength = 0;
    } else {
        characters = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)class_name, -1,
            NULL, 0);
        if (!characters) return NULL;
        converted = HeapAlloc(GetProcessHeap(), 0,
            (SIZE_T)characters * sizeof(*converted));
        if (!converted) {
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            return NULL;
        }
        if (!MultiByteToWideChar(CP_ACP, 0, (LPCSTR)class_name, -1,
                converted, characters)) {
            DWORD error = GetLastError();
            HeapFree(GetProcessHeap(), 0, converted);
            SetLastError(error);
            return NULL;
        }
        name.Buffer = converted;
        name.Length = name.MaximumLength =
            (USHORT)((characters - 1) * sizeof(*converted));
    }
    atom = wow_class_words_find_atom(binding->thread->ppi->classes, &name);
    if (converted) HeapFree(GetProcessHeap(), 0, converted);
    if (!atom) return NULL;

    /* OpenNT's _CreateWindowEx sends precisely WND_CNT_WOWDWORDS (three
     * DWORDs) to the private server.  Do not read a whole WW from the caller;
     * the remaining native view is initialized by the creation hook. */
    CopyMemory(words.adwWOW, input_words, sizeof(words.adwWOW));
    request.dwExStyle = ex_style;
    request.lpszClass = (LPCSTR)class_name;
    request.lpszName = (LPCSTR)window_name;
    request.style = style;
    request.x = x;
    request.y = y;
    request.cx = width;
    request.cy = height;
    request.hwndParent = parent;
    request.hMenu = menu;
    request.hInstance = (HINSTANCE)instance;
    request.lpCreateParams = parameter;
    result = wow_window_create_boundA(binding->thread, atom, &request, &words,
        NULL);
    return result;
}
