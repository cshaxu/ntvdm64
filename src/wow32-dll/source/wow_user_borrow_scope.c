#include "wow_user_borrow_scope.h"
#include "wow_user_runtime.h"
#include "wow_user_task_lifecycle.h"
#include "wow_class_client_bindings.h"
#include "wow_task_order_bindings.h"
#include "wow_user_client_view_layout.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_wow_page_domain.h"

#include <stdio.h>

/* Original private getters return aliases into retained WND/CLS objects.
 * Native storage references provide that lifetime, without reproducing USER
 * thread locks, window IDs or policy. Repeated reads share one scope borrow. */
typedef struct wow_user_borrow {
    struct wow_user_borrow *next;
    PVOID binding;
    BOOL is_class;
    ULONG generation; /* Transient pin, never owned by the host storage. */
} wow_user_borrow;
static __declspec(thread) wow_user_borrow_scope *current_scope;

/* Scope owns a reference to unpublished WW storage plus shared WND backing.
 * Enrollment reuses that same WW, so previously returned PWW aliases remain
 * live. Observation alone never attaches a property or cleanup ownership. */
typedef struct wow_sampled_words {
    HWND window;
    ULONG backing;
    wow_window_words_binding *binding;
} wow_sampled_words;
#define WOW_BORROW_SAMPLED 2

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
    scope->callback_published = FALSE;
    ZeroMemory(scope->saved_callback, sizeof(scope->saved_callback));
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
    if (is_class == WOW_BORROW_SAMPLED) {
        wow_sampled_words *sample = binding;
        (void)mvdm_softpc_wow_page_domain_release_window(sample->backing);
        wow_window_words_release(sample->binding);
        HeapFree(GetProcessHeap(), 0, sample);
    }
    else if (is_class) wow_class_words_release(binding);
    else wow_window_words_release(binding);
}

static BOOL pin_generation(wow_user_borrow *borrow)
{
    /* Sampled records already own their backing; class records have none.
     * An existing property-only borrow can acquire a generation later during
     * creation. Upgrade on the next exposure, without resolving HWND again. */
    return borrow->is_class || borrow->generation ||
        wow_window_words_pin_generation(borrow->binding, &borrow->generation);
}

BOOL WINAPI wow_user_borrow_leave(wow_user_borrow_scope *scope)
{
    wow_user_borrow *p;
    wow_user_runtime_thread *runtime;
    BOOL reclaimed = TRUE;
    DWORD error = GetLastError();
    if (!scope || current_scope != scope) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    if (scope->callback_published) {
        if (!mvdm_softpc_wow_page_domain_restore_callback(scope->saved_callback))
            wow_user_native_call_failed("restore-scoped-window-callback", ERROR_INVALID_STATE);
        scope->callback_published = FALSE;
    }
    current_scope = scope->previous;
    while ((p = scope->borrows) != NULL) {
        scope->borrows = p->next;
        if (p->generation && !mvdm_softpc_wow_page_domain_release_window(p->generation))
            wow_user_native_call_failed("release-scoped-window-generation", ERROR_INVALID_STATE);
        release_binding(p->binding, p->is_class);
        HeapFree(GetProcessHeap(), 0, p);
    }
    scope->previous = NULL;
    scope->classes = NULL;
    runtime = wow_user_runtime_current();
    if (runtime && runtime->thread && runtime->thread->ppi)
        reclaimed = wow_window_words_collect_retired(runtime->thread->ppi->objects);
    /* Do not rescan or mutate native window relationships here. Drain
     * retired WND allocations and their now-unreferenced sampled CLS after
     * releasing this scope's aliases; the existing collector owns both. */
    if (mvdm_softpc_wow_page_domain_active() &&
            !mvdm_softpc_wow_page_domain_collect_observed_classes())
        reclaimed = FALSE;
    if (!reclaimed) {
        SetLastError(ERROR_INVALID_STATE);
        return FALSE; /* Scope is consumed; domain still owns pending frees. */
    }
    SetLastError(error);
    return TRUE;
}

VOID WINAPI wow_user_borrow_leave_required(wow_user_borrow_scope *scope)
{
    DWORD error = GetLastError();
    /* The low-level diagnostic API must still reject an unmatched leave
     * without consuming either scope. A production finally cannot continue
     * after that rejection: current_scope would point into an unwound stack.
     * After this check FALSE means only deferred collector work, whose owner
     * and backing remain live; never pop the already consumed scope again. */
    if (!scope || current_scope != scope)
        wow_user_native_call_failed("restore-callback-borrow", ERROR_INVALID_STATE);
    (void)wow_user_borrow_leave(scope);
    SetLastError(error);
}

/* Consumes the incoming reference on every path. */
static BOOL retain(PVOID binding, BOOL is_class)
{
    wow_user_borrow *p;
    if (!binding) return FALSE;
    for (p = current_scope->borrows; p; p = p->next) {
        if (p->binding == binding && p->is_class == is_class) {
            BOOL pinned = pin_generation(p);
            DWORD error = GetLastError();
            release_binding(binding, is_class);
            SetLastError(error);
            return pinned;
        }
    }
    p = HeapAlloc(GetProcessHeap(), 0, sizeof(*p));
    if (!p) {
        release_binding(binding, is_class);
        SetLastError(ERROR_NOT_ENOUGH_MEMORY); return FALSE;
    }
    p->binding = binding; p->is_class = is_class; p->generation = 0;
    if (!pin_generation(p)) {
        DWORD error = GetLastError();
        release_binding(binding, is_class);
        HeapFree(GetProcessHeap(), 0, p);
        SetLastError(error); return FALSE;
    }
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

wow_window_words_binding *WINAPI wow_user_borrow_enrolled_window(HWND window)
{
    return borrow_window(window);
}

BOOL WINAPI wow_user_borrow_set_callback(HWND window)
{
    DWORD saved = GetLastError();
    if (!current_scope || !window) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    if (current_scope->callback_published) {
        SetLastError(ERROR_ALREADY_EXISTS); return FALSE;
    }
    /* Recovered control/dialog entries have already published their graph.
     * Keep the enrolled allocation/generation through callback restoration;
     * this is an actual entry, not a search of arbitrary outer borrows. */
    if (!borrow_window(window)) return FALSE;
    if (!mvdm_softpc_wow_page_domain_callback_window((ULONG)(ULONG_PTR)window,
            current_scope->saved_callback)) return FALSE;
    current_scope->callback_published = TRUE;
    SetLastError(saved);
    return TRUE;
}

wow_window_words_binding *WINAPI wow_user_borrow_client_state(HWND window,
    struct tagWND *state)
{
    DWORD error = GetLastError();
    wow_user_borrow_scope *scope;
    wow_user_borrow *borrow;
    wow_window_words_binding *binding;
    if (!current_scope || !state) {
        SetLastError(ERROR_INVALID_STATE); return NULL;
    }
    /* Retained state can outlive native destruction, so validate its stored
     * identity, not the current HWND property (which may denote a successor). */
    if (state->head.h != window) {
        SetLastError(ERROR_INVALID_WINDOW_HANDLE); return NULL;
    }
    for (scope = current_scope; scope; scope = scope->previous)
        for (borrow = scope->borrows; borrow; borrow = borrow->next) {
            if (borrow->is_class == TRUE) continue;
            binding = borrow->is_class == WOW_BORROW_SAMPLED ?
                ((wow_sampled_words *)borrow->binding)->binding : borrow->binding;
            if (wow_window_words_client_state_value(binding) == state) {
                if (!pin_generation(borrow)) return NULL;
                SetLastError(error); return binding;
            }
        }
    binding = wow_window_words_acquire(window);
    if (!binding) return NULL;
    if (wow_window_words_client_state_value(binding) != state) {
        wow_window_words_release(binding);
        SetLastError(ERROR_INVALID_STATE); return NULL;
    }
    if (!retain(binding, FALSE)) return NULL;
    SetLastError(error);
    return binding;
}

wow_window_words_binding *WINAPI wow_user_borrow_observed_binding(HWND window)
{
    wow_user_borrow_scope *scope;
    wow_user_borrow *borrow;
    WW *words;
    /* Resolve the current generation first. A matching HWND in an outer
     * scope can denote an already retired allocation after handle reuse. */
    if (!current_scope || !(words = wow_user_borrow_window(window))) return NULL;
    for (scope = current_scope; scope; scope = scope->previous)
        for (borrow = scope->borrows; borrow; borrow = borrow->next)
            if (borrow->is_class == WOW_BORROW_SAMPLED) {
                wow_sampled_words *sample = borrow->binding;
                if (wow_window_words_value(sample->binding) == words)
                    return sample->binding;
            }
    return NULL;
}

/* Consume one exact-generation pin; the existing scope owns it on success.
 * No HWND lookup or native refresh is allowed on this retained path. */
static wow_window_words_binding *retain_window_generation(HWND window, ULONG backing)
{
    wow_sampled_words *sample = NULL;
    wow_user_borrow_scope *scope;
    wow_user_borrow *borrow;
    ULONG process_id, thread_id;
    for (scope = current_scope; scope && !sample; scope = scope->previous)
        for (borrow = scope->borrows; borrow; borrow = borrow->next)
            if (borrow->is_class == WOW_BORROW_SAMPLED &&
                    ((wow_sampled_words *)borrow->binding)->window == window &&
                    ((wow_sampled_words *)borrow->binding)->backing == backing) {
                sample = borrow->binding;
                break;
            }
    if (sample) {
        DWORD error = GetLastError();
        if (!mvdm_softpc_wow_page_domain_release_window(backing))
            wow_user_native_call_failed("release-duplicate-observed-borrow", ERROR_INVALID_STATE);
        SetLastError(error);
    } else {
        sample = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(*sample));
        if (!sample) {
            if (!mvdm_softpc_wow_page_domain_release_window(backing))
                wow_user_native_call_failed("release-unallocated-observed-borrow", ERROR_INVALID_STATE);
            SetLastError(ERROR_NOT_ENOUGH_MEMORY); return NULL;
        }
        sample->window = window;
        sample->backing = backing;
        sample->binding = wow_window_words_acquire_generation(sample->backing);
        if (!sample->binding) {
            release_binding(sample, WOW_BORROW_SAMPLED); return NULL;
        }
        if (!mvdm_softpc_wow_page_domain_read_window_identity(sample->backing,
                &process_id, &thread_id) ||
                (wow_window_words_thread_id(sample->binding) ?
                    wow_window_words_thread_id(sample->binding) != thread_id :
                    !wow_window_words_set_observed_thread_id(sample->binding, thread_id))) {
            release_binding(sample, WOW_BORROW_SAMPLED);
            SetLastError(ERROR_INVALID_STATE); return NULL;
        }
        if (!retain(sample, WOW_BORROW_SAMPLED)) return NULL;
    }
    return sample->binding;
}

struct tagWND *WINAPI wow_user_borrow_window_state(HWND window)
{
    DWORD saved = GetLastError();
    ULONG full = (ULONG)(ULONG_PTR)window, backing = 0;
    wow_window_words_binding *binding;
    if (!current_scope) { SetLastError(ERROR_INVALID_STATE); return NULL; }
    /* client/wow.c ValidateHwnd's callback fast path precedes the shared
     * table check. It deliberately keeps a retired callback's exact WND. */
    if (!mvdm_softpc_wow_page_domain_borrow_callback_window(full, &backing)) return NULL;
    if (!backing) {
        ULONG current = mvdm_softpc_wow_page_domain_window_handle(LOWORD(window));
        /* rtl/wow.c accepts matching uniqueness or either original 16-bit
         * form (zero/all-ones high word). Never accept a stale full handle. */
        if (!full || !current || (full != current && HIWORD(window) != 0 &&
                HIWORD(window) != 0xffffu)) {
            SetLastError(ERROR_INVALID_WINDOW_HANDLE); return NULL;
        }
        full = current;
        backing = mvdm_softpc_wow_page_domain_borrow_window(full);
        if (!backing) { SetLastError(ERROR_INVALID_STATE); return NULL; }
    }
    binding = retain_window_generation((HWND)(ULONG_PTR)full, backing);
    if (!binding) return NULL; /* Incoming pin consumed even on failure. */
    if (!wow_user_borrow_client_graph(binding, GetDesktopWindow())) return NULL;
    SetLastError(saved);
    return wow_window_words_client_state_value(binding);
}

struct _WW *WINAPI wow_user_borrow_window(HWND window)
{
    wow_window_words_binding *binding = borrow_window(window);
    mvdm_softpc_wow_window_words_view view;
    ULONG full, backing;
    WW *words;
    if (binding) {
        if (!wow_window_words_refresh(window, binding)) return NULL;
        return wow_window_words_value(binding);
    }
    if (!current_scope || (GetLastError() != ERROR_NOT_FOUND &&
            GetLastError() != ERROR_ACCESS_DENIED)) return NULL;
    full = mvdm_softpc_wow_page_domain_window_handle(LOWORD(window));
    if (!full || ((ULONG)(ULONG_PTR)window != full &&
            (ULONG)(ULONG_PTR)window != (ULONG)LOWORD(window) &&
            (ULONG)(ULONG_PTR)window != (ULONG)(LONG)(SHORT)LOWORD(window))) return NULL;
    backing = mvdm_softpc_wow_page_domain_borrow_window(full);
    if (!backing) return NULL;
    binding = retain_window_generation((HWND)(ULONG_PTR)full, backing);
    if (!binding) return NULL;
    words = wow_window_words_value(binding);
    if (!mvdm_softpc_wow_page_domain_read_window_words_view(backing,
            &view)) return NULL;
    CopyMemory(words->adwWOW, view.words, sizeof(view.words));
    words->dwStyle = view.style;
    words->dwExStyle = view.exstyle;
    words->hInstance = (HANDLE)(ULONG_PTR)view.module;
    return words;
}

BOOL WINAPI wow_user_borrow_window_relation(wow_window_words_binding *source,
    ULONG relation, wow_window_words_binding **target)
{
    ULONG backing = 0, destination = 0;
    DWORD saved = GetLastError(), error;
    BOOL result;
    mvdm_softpc_wow_window_observation view;
    wow_window_words_binding *binding;
    if (!current_scope || !source || !target || relation >= 4u) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    if (!wow_window_words_pin_generation(source, &backing)) return FALSE;
    if (!backing) { SetLastError(ERROR_INVALID_STATE); return FALSE; }
    result = mvdm_softpc_wow_page_domain_borrow_window_relation(backing, relation,
        &destination);
    error = GetLastError();
    if (!mvdm_softpc_wow_page_domain_release_window(backing))
        wow_user_native_call_failed("release-relation-source", ERROR_INVALID_STATE);
    SetLastError(error);
    if (!result) return FALSE;
    if (!destination) { *target = NULL; SetLastError(saved); return TRUE; }
    if (!mvdm_softpc_wow_page_domain_read_window_view(destination, &view)) {
        error = GetLastError();
        if (!mvdm_softpc_wow_page_domain_release_window(destination))
            wow_user_native_call_failed("release-invalid-relation-target", ERROR_INVALID_STATE);
        SetLastError(error); return FALSE;
    }
    binding = retain_window_generation((HWND)(ULONG_PTR)view.links.window, destination);
    if (!binding) return FALSE; /* Pin consumed even on failure. */
    *target = binding;
    SetLastError(saved);
    return TRUE;
}

static BOOL graph_row_index(wow_window_client_graph_row **rows, ULONG *count,
    ULONG *capacity, wow_window_words_binding *binding, ULONG *index)
{
    ULONG i, next;
    wow_window_client_graph_row *grown;
    if (!binding) { *index = MAXDWORD; return TRUE; }
    for (i = 0; i < *count; ++i)
        if ((*rows)[i].binding == binding) { *index = i; return TRUE; }
    if (*count == *capacity) {
        if (*capacity > (MAXDWORD / sizeof(**rows)) / 2) {
            SetLastError(ERROR_ARITHMETIC_OVERFLOW); return FALSE;
        }
        next = *capacity ? *capacity * 2 : 16;
        grown = *rows ? HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
            *rows, (SIZE_T)next * sizeof(**rows)) :
            HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (SIZE_T)next * sizeof(**rows));
        if (!grown) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return FALSE; }
        *rows = grown; *capacity = next;
    }
    *index = *count;
    (*rows)[(*count)++].binding = binding;
    return TRUE;
}

BOOL WINAPI wow_user_borrow_client_graph(wow_window_words_binding *root, HWND desktop)
{
    wow_window_client_graph_row *rows = NULL;
    ULONG count = 0, capacity = 0, i, edge, index, backing = 0;
    DWORD saved = GetLastError(), error;
    BOOL result = FALSE, read;
    mvdm_softpc_wow_window_observation view;
    wow_window_words_binding *binding, *target;
    wow_user_borrow_scope *scope, *outer;
    wow_user_borrow **link, *borrow;
    if (!current_scope && !root) return TRUE; /* No suspended original graph. */
    if (!current_scope || !desktop) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    __try {
        if (root) {
            if (!graph_row_index(&rows, &count, &capacity, root, &index)) __leave;
        } else {
            /* Snapshot roots before traversal can add/move borrow records.
             * Include retired original roots, not only the live desktop tree.
             * One combined traversal avoids rebuilding it for each control. */
            for (scope = current_scope; scope; scope = scope->previous)
                for (borrow = scope->borrows; borrow; borrow = borrow->next) {
                    if (borrow->is_class == TRUE) continue;
                    binding = borrow->is_class == WOW_BORROW_SAMPLED ?
                        ((wow_sampled_words *)borrow->binding)->binding : borrow->binding;
                    if (!wow_window_words_owns_client_state(binding,
                            wow_window_words_client_state_value(binding))) continue;
                    if (!graph_row_index(&rows, &count, &capacity, binding, &index)) __leave;
                }
        }
        if (!count) { result = TRUE; __leave; }
        for (i = 0; i < count; ++i) {
            binding = rows[i].binding;
            if (!wow_window_words_pin_generation(binding, &backing)) __leave;
            if (!backing) { SetLastError(ERROR_INVALID_STATE); __leave; }
            read = mvdm_softpc_wow_page_domain_read_window_view(backing, &view);
            if (!read) {
                error = GetLastError();
                if (!mvdm_softpc_wow_page_domain_release_window(backing))
                    wow_user_native_call_failed("release-graph-node", ERROR_INVALID_STATE);
                SetLastError(error); __leave;
            }
            target = retain_window_generation((HWND)(ULONG_PTR)view.links.window, backing);
            if (!target) __leave; /* Incoming pin consumed on all paths. */
            if (target != binding) { SetLastError(ERROR_INVALID_STATE); __leave; }
            if (!wow_window_words_prepare_resource_state(binding, desktop)) __leave;
            rows[i].state = wow_window_words_client_state_value(binding);
            rows[i].window = (HWND)(ULONG_PTR)view.links.window;
            rows[i].info.cbSize = sizeof(WINDOWINFO);
            CopyMemory(&rows[i].info.rcWindow, view.window_rect, sizeof(RECT));
            CopyMemory(&rows[i].info.rcClient, view.client_rect, sizeof(RECT));
            rows[i].info.dwStyle = view.style;
            rows[i].info.dwExStyle = view.exstyle;
            rows[i].control_id = view.control_id; rows[i].module = view.module;
            rows[i].class_atom = view.descriptor.atom;
            for (edge = 0; edge < 4; ++edge) {
                if (!wow_user_borrow_window_relation(binding, edge, &target)) __leave;
                if (!graph_row_index(&rows, &count, &capacity, target, &index)) __leave;
                rows[i].target[edge] = index;
            }
        }
        /* A nested refresh replaces links in the shared stable WND. Its new
         * targets must survive the nested scope, since suspended outer code
         * resumes with those same pointers. Transfer these exact graph pins
         * to the outermost active borrow, never restore stale links on unwind. */
        for (outer = current_scope; outer->previous; outer = outer->previous) {}
        for (i = 0; i < count; ++i) {
            BOOL found = FALSE;
            for (scope = current_scope; scope && !found; scope = scope->previous) {
                for (link = &scope->borrows; (borrow = *link) != NULL; link = &borrow->next) {
                    if (borrow->is_class != WOW_BORROW_SAMPLED ||
                            ((wow_sampled_words *)borrow->binding)->binding != rows[i].binding)
                        continue;
                    if (scope != outer) {
                        *link = borrow->next;
                        borrow->next = outer->borrows;
                        outer->borrows = borrow;
                    }
                    found = TRUE; break;
                }
            }
            if (!found) { SetLastError(ERROR_INVALID_STATE); __leave; }
        }
        result = wow_window_words_publish_client_graph(count, rows);
    } __finally {
        error = result ? saved : GetLastError();
        if (rows) HeapFree(GetProcessHeap(), 0, rows);
        /* Exact bindings/generations belong to the existing scope on every
         * path. No new permanent edge references or reciprocal cycles. */
        SetLastError(error);
    }
    return result;
}

BOOL WINAPI wow_user_borrow_window_exchange(HWND window, unsigned index,
    DWORD value, DWORD *previous)
{
    WW *words;
    wow_user_borrow_scope *scope;
    wow_user_borrow *borrow;
    if (index >= WND_CNT_WOWDWORDS || !previous) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    if (!(words = wow_user_borrow_window(window))) return FALSE;
    for (scope = current_scope; scope; scope = scope->previous)
        for (borrow = scope->borrows; borrow; borrow = borrow->next)
            if (borrow->is_class == WOW_BORROW_SAMPLED) {
                wow_sampled_words *sample = borrow->binding;
                DWORD ignored;
                if (wow_window_words_value(sample->binding) != words) continue;
                if (!mvdm_softpc_wow_page_domain_exchange_window_word(
                        sample->backing, index, value, &ignored)) return FALSE;
                *previous = (DWORD)InterlockedExchange(
                    (LONG *)&words->adwWOW[index], value);
                return TRUE;
            }
    return wow_window_words_exchange(window, index, value, previous);
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
