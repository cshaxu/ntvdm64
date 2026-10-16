#include "wow_window_creation_binding.h"
#include "wow_dialog_creation_binding.h"
#include "wow_user_runtime.h"
#include "wow_user_task_lifecycle.h"
#include "wow_user_private_access.h"
#include "wow_user_borrow_scope.h"
#include "wow_menu_resource_bindings.h"
#include <stdio.h>

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
    WNDPROC previous_native;
} creation_scope;
static __declspec(thread) creation_scope *current_scope;
static BOOL claim_explicit(HWND, const CREATESTRUCTA *, wow_task_order_thread *,
    wow_window_creation_enrollment *, BOOL *);

static void rollback_enrollment(HWND window, wow_window_words_binding *borrow,
    WNDPROC previous)
{
    DWORD error = GetLastError();
    wow_window_words_binding *current = wow_window_words_acquire(window);
    if (current == borrow && previous &&
            GetWindowLongA(window, GWL_WNDPROC) == (LONG)wow_window_native_proc)
        SetWindowLongA(window, GWL_WNDPROC, (LONG)previous);
    wow_window_words_release(current);
    (void)wow_window_words_terminal(window, borrow);
    SetLastError(error);
}

/* One WW/class/object ownership boundary for explicit and internal creation.
 * The native class remains registered with USER. Its existing per-window
 * procedure is retained below our entrance, including opaque CallProc values. */
static BOOL enroll_created_window(HWND window, const CREATESTRUCTA *request,
    const WW *input, wow_class_words_binding **class_words,
    const wow_window_dispatch_target *selected, wow_task_order_thread *thread,
    wow_window_words_binding **borrow, WNDPROC *previous)
{
    WW words = *input;
    wow_window_dispatch_target target = {0};
    BOOL native = wow_class_words_native_registration(*class_words);
    DWORD error;
    *borrow = NULL;
    *previous = NULL;
    if (selected) target = *selected;
    if (native && (HIWORD(target.procedure) == WNDPROC_HANDLE ||
            !(target.procedure & WNDPROC_WOW))) {
        target.procedure = (DWORD)GetWindowLongA(window, GWL_WNDPROC);
        if (!target.procedure) {
            SetLastError(ERROR_INVALID_WINDOW_HANDLE); return FALSE;
        }
    }
    words.dwStyle = request->style;
    words.dwExStyle = request->dwExStyle;
    words.hInstance = request->hInstance;
    if (!wow_window_words_attach_target(window, &words, *class_words,
            target.procedure ? &target : NULL)) return FALSE;
    *class_words = NULL; /* Attachment owns this acquired reference. */
    *borrow = wow_window_words_acquire(window);
    if (!*borrow) {
        error = GetLastError();
        wow_window_words_detach(window);
        SetLastError(error); return FALSE;
    }
    wow_window_words_begin_creation(*borrow, request->hMenu);
    if (!wow_window_words_publish_owner(window, *borrow, thread)) goto failed;
    if (native) {
        SetLastError(ERROR_SUCCESS);
        *previous = (WNDPROC)SetWindowLongA(window, GWL_WNDPROC,
            (LONG)wow_window_native_proc);
        if (!*previous) {
            if (!GetLastError()) SetLastError(ERROR_INVALID_STATE);
            goto failed;
        }
    }
    return TRUE;
failed:
    error = GetLastError();
    rollback_enrollment(window, *borrow, *previous);
    wow_window_words_release(*borrow);
    *borrow = NULL;
    SetLastError(error);
    return FALSE;
}

BOOL WINAPI wow_window_creation_enroll_internal(wow_task_order_thread *thread,
    HWND window, const CREATESTRUCTA *request,
    wow_window_creation_enrollment *enrollment)
{
    wow_window_words_binding *existing = NULL, *parent = NULL;
    wow_class_words_binding *class_words = NULL;
    wow_window_dispatch_target target = {0};
    WW words = {0};
    DWORD error = GetLastError();
    BOOL claimed = FALSE;

    ZeroMemory(enrollment, sizeof(*enrollment));
    if (!request || !thread || !thread->ppi)
        return TRUE;
    if (thread->TIF_flags & WOW_TASK_TIF_INCLEANUP) {
        SetLastError(ERROR_INVALID_STATE);
        return FALSE;
    }
    /* Nested explicit scopes may already own this HWND. Never steal their
     * metadata or retire it when a later hook rejects creation. */
    existing = wow_window_words_acquire(window);
    if (existing) {
        wow_window_words_release(existing);
        SetLastError(error);
        return TRUE;
    }
    if (!claim_explicit(window, request, thread, enrollment, &claimed)) return FALSE;
    if (!claimed && !wow_dialog_creation_claim(window, request, thread,
            enrollment, &claimed)) return FALSE;
    if (claimed) { SetLastError(error); return TRUE; }
    /* Dialog roots carry their own WW/property and need no class lookup.
     * Only the generic implicit-class path has this prerequisite. */
    if (!thread->ppi->classes) { SetLastError(error); return TRUE; }
    if (request->hwndParent)
        parent = wow_window_words_acquire(request->hwndParent);
    class_words = wow_class_words_acquire(thread->ppi->classes,
        (ATOM)GetClassLongA(window, GCW_ATOM), request->hInstance);
    /* Application classes already belong to this WOW process. Native
     * standard classes still require an enrolled parent/owner; do not turn
     * unrelated host top-level windows into WOW objects. */
    if (class_words && wow_class_words_native_registration(class_words) && !parent) {
        wow_class_words_release(class_words);
        class_words = NULL;
    }
    wow_window_words_release(parent);
    if (!class_words) {
        SetLastError(error);
        return TRUE;
    }
    (void)wow_class_words_target(class_words, &target);
    if (!enroll_created_window(window, request, &words, &class_words,
            &target, thread, &enrollment->borrow, &enrollment->previous_native)) {
        error = GetLastError();
        wow_class_words_release(class_words);
        SetLastError(error);
        return FALSE;
    }
    enrollment->window = window;
    SetLastError(error);
    return TRUE;
}

void WINAPI wow_window_creation_finish(wow_window_creation_enrollment *enrollment,
    BOOL accepted)
{
    DWORD error = GetLastError();
    if (enrollment->borrow) {
        if (!accepted && enrollment->dialog_root)
            wow_dialog_creation_cancel(enrollment->window, enrollment->borrow);
        if (!accepted) rollback_enrollment(enrollment->window,
            enrollment->borrow, enrollment->previous_native);
        wow_window_words_release(enrollment->borrow);
        enrollment->borrow = NULL;
    }
    SetLastError(error);
}

LRESULT WINAPI wow_window_creation_forward_internal(wow_task_order_thread *thread,
    int code, WPARAM wp, LPARAM lp)
{
    wow_window_creation_enrollment enrollment = {0};
    BOOL accepted = FALSE;
    LRESULT result;
    if (code == HCBT_CREATEWND && lp &&
            !wow_window_creation_enroll_internal(thread, (HWND)wp,
                ((CBT_CREATEWNDA *)lp)->lpcs, &enrollment)) return 1;
    __try {
        result = CallNextHookEx(NULL, code, wp, lp);
        accepted = result == 0;
    } __finally {
        wow_window_creation_finish(&enrollment, accepted);
    }
    return result;
}

LRESULT CALLBACK wow_window_creation_observer(int code, WPARAM wp, LPARAM lp)
{
    wow_user_runtime_thread *binding = wow_user_runtime_current();
    wow_user_borrow_scope borrow;
    BOOL borrowed = FALSE;
    LRESULT result = code == HCBT_CREATEWND ? 1 : 0;
    if (code != HCBT_CREATEWND || !binding || !binding->thread)
        return CallNextHookEx(NULL, code, wp, lp);
    /* This observer supplies metadata, not a guest callback. Original guest
     * hooks and tagged WNDPROCs enter their own existing task scopes; native
     * controls must not acquire WOW execution merely for being created. */
    __try {
        if (!wow_user_borrow_enter(&borrow)) __leave;
        borrowed = TRUE;
        result = wow_window_creation_forward_internal(binding->thread, code, wp, lp);
    } __finally {
        if (borrowed) wow_user_borrow_leave_required(&borrow);
    }
    return result;
}

BOOL WINAPI wow_window_creation_menu(HWND window, CREATESTRUCTA *creation,
    wow_class_lookup_entry *entry, wow_window_creation_menu_state *state)
{
    HMENU menu;
    DWORD error;
    if (state->failure) { SetLastError(state->failure); return FALSE; }
    if (state->checked) {
        /* Native USER supplies a fresh CREATESTRUCT for WM_CREATE; the
         * earlier callback's temporary structure is not retained. */
        if (state->attached && creation)
            creation->hMenu = GetMenu(window);
        return TRUE;
    }
    state->checked = TRUE; /* Resource loading can reenter creation. */
    if (!creation || creation->hMenu || (creation->style & WS_CHILD))
        return TRUE;
    if (!entry || !entry->lpszClientUnicodeMenuName) return TRUE;
    /* Original createw.c loads the class module's menu after CBT, before
     * WM_NCCREATE. Native registration intentionally has no menu name.
     * Reuse the original resource client/converter/parser, not USER32's
     * PE loader. The enrolled window retains the class across this callback. */
    menu = wow_user_load_menuW(entry->hModule,
        entry->lpszClientUnicodeMenuName);
    if (!menu) return TRUE; /* Original creation tolerates load failure. */
    /* Original getset.c's non-child GWL_ID branch performs the same menu
     * association as createw.c, without xxxSetMenu's frame recalculation.
     * SetMenu here would send size/move messages before guest WM_CREATE. */
    SetLastError(ERROR_SUCCESS);
    SetWindowLongPtrA(window, GWLP_ID, (LONG_PTR)menu);
    if (GetMenu(window) != menu) {
        error = GetLastError();
        if (!error) error = ERROR_INVALID_WINDOW_HANDLE;
        state->failure = error;
        DestroyMenu(menu);
        SetLastError(error);
        return FALSE;
    }
    /* USER now owns destruction of the attached menu, including rejected
     * creation. Both the original guest message and GetMenu see this handle. */
    creation->hMenu = menu;
    state->attached = TRUE;
    return TRUE;
}

/* Default-off creation-boundary witness. Do not replace USER's failure or
 * change the callback sequence merely to make creation succeed. */
static void trace_creation(const creation_scope *scope, HWND result, DWORD error)
{
    char path[MAX_PATH], line[256];
    HANDLE file;
    DWORD size, written;
    if (!GetEnvironmentVariableA("MVDM_WOW_WINDOW_TRACE_PATH", path,
            sizeof(path))) return;
    file=CreateFileA(path,FILE_APPEND_DATA,FILE_SHARE_READ|FILE_SHARE_WRITE,
        NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
    if (file==INVALID_HANDLE_VALUE) return;
    size=(DWORD)sprintf_s(line,sizeof(line),
        "%lu WindowCreate atom=%04X result=%p error=%lu bindingError=%lu claimed=%d menu=%p instance=%p style=%08lX tid=%lu actualMenu=%p\r\n",
        GetCurrentProcessId(),scope->atom,result,error,scope->failure,
        scope->claimed,scope->request->hMenu,scope->request->hInstance,
        scope->request->style,GetCurrentThreadId(),result ? GetMenu(result) : NULL);
    if (size) (void)WriteFile(file,line,size,&written,NULL);
    CloseHandle(file);
}

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

static BOOL claim_explicit(HWND window, const CREATESTRUCTA *cs,
    wow_task_order_thread *thread, wow_window_creation_enrollment *enrollment,
    BOOL *claimed)
{
    creation_scope *scope = current_scope;
    *claimed = FALSE;
    if (scope && scope->thread == thread && !scope->claimed) {
        if (cs->hInstance == scope->request->hInstance &&
                (ATOM)GetClassLongA(window, GCW_ATOM) == scope->atom &&
                application_parameter(scope, cs) == scope->request->lpCreateParams) {
            scope->claimed = TRUE;
            if (!enroll_created_window(window, cs, scope->words,
                    &scope->class_words, scope->target, scope->thread,
                    &scope->borrow, &scope->previous_native)) {
                scope->failure = GetLastError();
                return FALSE;
            }
            scope->window = window;
            enrollment->window = window;
            enrollment->borrow = wow_window_words_acquire(window);
            enrollment->previous_native = scope->previous_native;
            if (!enrollment->borrow) {
                scope->failure = GetLastError();
                rollback_enrollment(window, scope->borrow, scope->previous_native);
                return FALSE;
            }
            *claimed = TRUE;
        }
    }
    return TRUE;
}

static LRESULT CALLBACK creation_hook(int code, WPARAM wp, LPARAM lp)
{
    return wow_window_creation_forward_internal(
        current_scope ? current_scope->thread : NULL, code, wp, lp);
}

static HWND create_bound(wow_task_order_thread *thread,
    ATOM atom, const CREATESTRUCTA *request, const WW *words,
    const wow_window_dispatch_target *target, BOOL unicode)
{
    creation_scope scope = {0};
    wow_user_native_call call;
    BOOL call_started = FALSE;
    HWND result = NULL;
    DWORD error = ERROR_SUCCESS;
    wow_class_lookup_context *context;
    if (!thread || !thread->ppi || !thread->ppi->classes || !thread->ppi->objects ||
            !atom || !request || !words) {
        SetLastError(ERROR_INVALID_PARAMETER); return NULL;
    }
    context = thread->ppi->classes;
    if (thread->TIF_flags & WOW_TASK_TIF_INCLEANUP) {
        SetLastError(ERROR_INVALID_STATE); return NULL;
    }
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
        /* Creation can destroy a partially built window internally. Cover
         * the complete native operation, not only explicit rollback calls. */
        if (!wow_user_native_call_begin(&call, NULL)) {
            error = GetLastError();
            __leave;
        }
        call_started = TRUE;
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
            (void)wow_window_words_terminal(scope.window, scope.borrow);
        current_scope = scope.previous;
        UnhookWindowsHookEx(scope.hook);
        wow_window_words_release(scope.borrow);
        wow_class_words_release(scope.class_words);
        /* Retire vetoed/failed enrollment before publishing the final native
         * graph. Native USER has now finished any implicit destruction. */
        if (call_started && !wow_user_native_call_end(&call)) {
            error = GetLastError();
            if (result) (void)wow_native_DestroyWindow(result);
            result = NULL;
        }
    }
    trace_creation(&scope, result, error);
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
