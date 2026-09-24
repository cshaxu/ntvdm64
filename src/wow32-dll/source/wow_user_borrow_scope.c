#include "wow_user_borrow_scope.h"
#include "wow_user_runtime.h"
#include "wow_class_client_bindings.h"

#include <stdio.h>

/* Original private getters return aliases into retained WND/CLS objects.
 * Native storage references provide that lifetime, without reproducing USER
 * thread locks, window IDs or policy. Repeated reads share one scope borrow. */
typedef struct wow_user_borrow {
    struct wow_user_borrow *next;
    PVOID binding;
    BOOL is_class;
} wow_user_borrow;
static __declspec(thread) wow_user_borrow_scope *current_scope;

/* Opt-in evidence for the original GetClassWOWWords boundary.  This neither
 * selects a class nor changes its error: it only records the already-made
 * scoped lookup result while S2 establishes the real guest window path. */
static void trace_named_class(HINSTANCE module, const UNICODE_STRING *name,
    ATOM atom, const void *result)
{
    char path[MAX_PATH], line[320], class_name[96] = "<invalid>";
    DWORD written, bytes, saved = GetLastError();
    HANDLE file;
    if (name && name->Buffer && name->Length && HIWORD(name->Buffer)) {
        int copied = WideCharToMultiByte(CP_ACP, 0, name->Buffer,
            name->Length / sizeof(WCHAR), class_name,
            (int)sizeof(class_name) - 1, NULL, NULL);
        if (copied > 0) class_name[copied] = '\0';
    }
    if (!GetEnvironmentVariableA("MVDM_WOW_CLASS_TRACE_PATH", path,
            sizeof(path))) { SetLastError(saved); return; }
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ |
        FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) { SetLastError(saved); return; }
    bytes = (DWORD)sprintf_s(line, sizeof(line),
        "%lu GetClassWOWWords module=%08lX class=%s chars=%u atom=%04X result=%08lX error=%lu scope=%08lX runtime=%08lX runtimeClasses=%08lX\r\n",
        (unsigned long)GetCurrentProcessId(), (unsigned long)(ULONG_PTR)module,
        class_name, name ? (unsigned)(name->Length / sizeof(WCHAR)) : 0u, (unsigned)atom,
        (unsigned long)(ULONG_PTR)result, (unsigned long)saved,
        (unsigned long)(ULONG_PTR)(current_scope ? current_scope->classes : NULL),
        (unsigned long)(ULONG_PTR)wow_user_runtime_current(),
        (unsigned long)(ULONG_PTR)(wow_user_runtime_current() &&
            wow_user_runtime_current()->classes ?
            wow_user_runtime_current()->classes->classes : NULL));
    if (bytes) (void)WriteFile(file, line, bytes, &written, NULL);
    CloseHandle(file);
    SetLastError(saved);
}

BOOL WINAPI wow_user_borrow_enter(wow_user_borrow_scope *scope)
{
    wow_user_borrow_scope *p;
    wow_user_runtime_thread *runtime;
    if (!scope) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    for (p = current_scope; p; p = p->previous) {
        if (p == scope) { SetLastError(ERROR_ALREADY_EXISTS); return FALSE; }
    }
    scope->previous = current_scope;
    scope->borrows = NULL;
    /* The top-level W32Dispatch scope has no parent borrow, but it still
     * executes inside the worker's already-bound original WOW task.  Carry
     * that task's existing class view into the scoped original client getter;
     * do not allocate a parallel registry or infer a class from host USER. */
    runtime = wow_user_runtime_current();
    scope->classes = current_scope && current_scope->classes ? current_scope->classes :
        (runtime && runtime->classes ? runtime->classes->classes : NULL);
    current_scope = scope;
    return TRUE;
}

static void release_binding(PVOID binding, BOOL is_class)
{
    if (is_class) wow_class_words_release(binding);
    else wow_window_words_release(binding);
}

BOOL WINAPI wow_user_borrow_leave(wow_user_borrow_scope *scope)
{
    wow_user_borrow *p;
    DWORD error = GetLastError();
    if (!scope || current_scope != scope) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    current_scope = scope->previous;
    while ((p = scope->borrows) != NULL) {
        scope->borrows = p->next;
        release_binding(p->binding, p->is_class);
        HeapFree(GetProcessHeap(), 0, p);
    }
    scope->previous = NULL;
    scope->classes = NULL;
    SetLastError(error);
    return TRUE;
}

/* Consumes the incoming reference on every path. */
static BOOL retain(PVOID binding, BOOL is_class)
{
    wow_user_borrow *p;
    if (!binding) return FALSE;
    for (p = current_scope->borrows; p; p = p->next) {
        if (p->binding == binding && p->is_class == is_class) {
            release_binding(binding, is_class);
            return TRUE;
        }
    }
    p = HeapAlloc(GetProcessHeap(), 0, sizeof(*p));
    if (!p) {
        release_binding(binding, is_class);
        SetLastError(ERROR_NOT_ENOUGH_MEMORY); return FALSE;
    }
    p->binding = binding; p->is_class = is_class;
    p->next = current_scope->borrows; current_scope->borrows = p;
    return TRUE;
}

static wow_window_words_binding *borrow_window(HWND window)
{
    wow_window_words_binding *binding;
    if (!current_scope) { SetLastError(ERROR_INVALID_STATE); return NULL; }
    binding = wow_window_words_acquire(window);
    return retain(binding, FALSE) ? binding : NULL;
}

struct _WW *WINAPI wow_user_borrow_window(HWND window)
{
    return wow_window_words_value(borrow_window(window));
}

DWORD *WINAPI wow_user_borrow_window_class(HWND window)
{
    return wow_window_class_words_value(borrow_window(window));
}

DWORD *WINAPI wow_user_borrow_class(wow_class_lookup_context *context,
    ATOM atom, HANDLE module)
{
    wow_class_words_binding *binding;
    if (!current_scope) { SetLastError(ERROR_INVALID_STATE); return NULL; }
    binding = wow_class_words_acquire(context, atom, module);
    return retain(binding, TRUE) ? wow_class_words_value(binding) : NULL;
}

/* Same storage and original GetClassPtr policy as window-based WC queries.
 * The caller retains the class context and serializes list access. No USER
 * shared-heap pointer, desktop clone or new class namespace is fabricated. */
DWORD *WINAPI wow_user_borrow_named_class(HINSTANCE module,
    const UNICODE_STRING *name)
{
    ATOM atom;
    DWORD *result;
    if (!current_scope || !current_scope->classes) {
        trace_named_class(module, name, 0, NULL);
        SetLastError(ERROR_INVALID_STATE); return NULL;
    }
    if (!name || !name->Length || !HIWORD(name->Buffer)) {
        trace_named_class(module, name, 0, NULL);
        SetLastError(ERROR_INVALID_PARAMETER); return NULL;
    }
    atom = wow_class_words_find_atom(current_scope->classes, name);
    if (!atom) {
        trace_named_class(module, name, atom, NULL); return NULL;
    }
    result = wow_user_borrow_class(current_scope->classes, atom, module);
    trace_named_class(module, name, atom, result);
    return result;
}
