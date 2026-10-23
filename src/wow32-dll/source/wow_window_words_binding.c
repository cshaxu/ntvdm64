#include "wow_window_words_binding.h"
#include "wow_window_creation_binding.h"
#include "wow_window_procedure_entry.h"
#include "wow_window_client_state_bindings.h"
#include "wow_dialog_state_bindings.h"
#include "wow_dialog_procedure_bindings.h"
#include "wow_edit_entry_bindings.h"
#include "wow_user_client_view_layout.h"
#include "wow_user_borrow_scope.h"
#include "wow_user_task_lifecycle.h"
#include "wow_user_runtime.h"
#include "wow_class_client_bindings.h"
#include "wow_user_object_bindings.h"
#include "wow_task_order_bindings.h"
#include "wow_menu_native_bindings.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_wow_page_domain.h"

#include <stdio.h>
#include <limits.h>

/* DIVERGENCE(ADAPTER-WOW-027): original kernel WND/thread locks retain WW
 * across callbacks, even if a callback destroys the window. Public HWND does
 * not expose original WW storage. Keep exactly that storage in a native
 * property, with borrowed references; no window/class/guest policy lives here.
 * One worker per process; the lock covers only property/ref publication. */
struct wow_window_words_binding {
    LONG references;
    BOOL enrolled;
    BOOL terminal_pending;
    BOOL creation_encoding_known;
    BOOL creation_ansi;
    BOOL creation_facts_known;
    wow_window_resource_destroy resource_destroy;
    BOOL resource_destroy_consumed;
    DWORD native_thread_id;
    wow_window_creation_facts creation_facts;
    wow_window_creation_menu_state creation_menu;
    WW words;
    wow_class_words_binding *class_words;
    wow_cleanup_window cleanup;
    wow_user_object_table *objects;
    ULONG retired_guest_backing;
    ULONG generation_backing; /* Weak identity; the generation owns us. */
    /* ADAPTER-WOW-063: one retained original WND/class-extra allocation.
     * Original canonical owners release private data before its retirement. */
    struct tagWND *client_state;
    /* Original allocation provenance, not current HWND membership.
     * Resource-only projections must never gain this capability. */
    BOOL client_state_owned;
    SIZE_T projected_state_bytes; /* Capacity of resource-only WND before promotion. */
    CLS projected_class; /* Controlled atom-only projection, never native CLS. */
    SRWLOCK name_lock;
};
static SRWLOCK publication_lock = SRWLOCK_INIT;
static const WCHAR property_name[] = L"NTVDM.WOW.WindowWords";
static VOID window_trace(HWND, UINT, LONG, const char *, DWORD);

/* ADAPTER-WOW-075: registration fact only. Original deferred resource
 * owners retain all their data, release algorithms and guest conversions. */
BOOL WINAPI wow_window_words_bind_resource_destroy(HWND window,
    wow_window_resource_destroy callback)
{
    DWORD error=GetLastError();BOOL result=FALSE;
    wow_user_runtime_thread *runtime=wow_user_runtime_current();
    wow_window_words_binding *binding;
    if(!callback){SetLastError(ERROR_INVALID_PARAMETER);return FALSE;}
    binding=wow_window_words_acquire(window);
    if(!binding)return FALSE;
    AcquireSRWLockExclusive(&publication_lock);
    if(binding->native_thread_id!=GetCurrentThreadId())error=ERROR_INVALID_THREAD_ID;
    else if(!runtime || !runtime->thread || binding->cleanup.thread!=runtime->thread ||
        (runtime->thread->TIF_flags&WOW_TASK_TIF_INCLEANUP) ||
        !binding->enrolled || binding->terminal_pending || binding->resource_destroy_consumed ||
        GetPropW(window,property_name)!=binding ||
        (binding->resource_destroy && binding->resource_destroy!=callback))error=ERROR_INVALID_STATE;
    else {binding->resource_destroy=callback;result=TRUE;}
    ReleaseSRWLockExclusive(&publication_lock);
    wow_window_words_release(binding);SetLastError(error);return result;
}

BOOL WINAPI wow_window_words_consume_resource_destroy(HWND window,
    wow_window_resource_destroy expected,BOOL invoke)
{
    DWORD error=GetLastError();BOOL result=FALSE;
    wow_user_runtime_thread *runtime=wow_user_runtime_current();
    wow_window_resource_destroy callback=NULL;
    if(invoke ? expected!=NULL : expected==NULL){
        SetLastError(ERROR_INVALID_PARAMETER);return FALSE;
    }
    wow_window_words_binding *binding=wow_window_words_acquire(window);
    if(!binding)return FALSE;
    AcquireSRWLockExclusive(&publication_lock);
    if(binding->native_thread_id!=GetCurrentThreadId())error=ERROR_INVALID_THREAD_ID;
    else if(!runtime || !runtime->thread || binding->cleanup.thread!=runtime->thread ||
        !binding->enrolled || GetPropW(window,property_name)!=binding ||
        (expected && binding->resource_destroy && binding->resource_destroy!=expected) ||
        (invoke && binding->resource_destroy &&
            (!runtime->resource_cleanup_live || !(runtime->thread->TIF_flags&WOW_TASK_TIF_INCLEANUP))))
        error=ERROR_INVALID_STATE;
    else {
        callback=binding->resource_destroy;
        binding->resource_destroy=NULL;
        if(callback || expected)binding->resource_destroy_consumed=TRUE;
        result=TRUE;
    }
    ReleaseSRWLockExclusive(&publication_lock);
    SetLastError(error);
    __try {if(result && invoke && callback)(void)callback(window,WM_DESTROY,0,0);}
    __finally {
        error=GetLastError();wow_window_words_release(binding);SetLastError(error);
    }
    return result;
}

BOOL WINAPI wow_window_words_set_creation_facts(wow_window_words_binding *binding,
    const wow_window_creation_facts *facts)
{
    BOOL result = FALSE;
    DWORD error = GetLastError();
    if (!binding || !facts || facts->class_extra_bytes < 0) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    AcquireSRWLockExclusive(&publication_lock);
    if (!binding->enrolled || binding->terminal_pending)
        error = ERROR_INVALID_STATE;
    else if (binding->creation_facts_known &&
            (binding->creation_facts.module_version != facts->module_version ||
             binding->creation_facts.compatibility_flags != facts->compatibility_flags ||
             binding->creation_facts.class_extra_bytes != facts->class_extra_bytes))
        error = ERROR_INVALID_STATE;
    else {
        binding->creation_facts = *facts;
        binding->creation_facts_known = TRUE;
        result = TRUE;
    }
    ReleaseSRWLockExclusive(&publication_lock);
    SetLastError(error);
    return result;
}

BOOL WINAPI wow_window_words_creation_facts(wow_window_words_binding *binding,
    wow_window_creation_facts *facts)
{
    BOOL result;
    DWORD error = GetLastError();
    if (!binding || !facts) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    AcquireSRWLockShared(&publication_lock);
    result = binding->creation_facts_known;
    if (result) *facts = binding->creation_facts;
    ReleaseSRWLockShared(&publication_lock);
    SetLastError(result ? error : ERROR_NOT_FOUND);
    return result;
}

BOOL WINAPI wow_window_words_set_creation_ansi(wow_window_words_binding *binding,
    BOOL ansi)
{
    DWORD error = GetLastError();
    BOOL result = FALSE;
    if (!binding) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    ansi = !!ansi;
    AcquireSRWLockExclusive(&publication_lock);
    if (!binding->enrolled || binding->terminal_pending ||
            (binding->creation_encoding_known && binding->creation_ansi != ansi))
        error = ERROR_INVALID_STATE;
    else {
        binding->creation_ansi = ansi;
        binding->creation_encoding_known = TRUE;
        result = TRUE;
    }
    ReleaseSRWLockExclusive(&publication_lock);
    SetLastError(error);
    return result;
}

BOOL WINAPI wow_window_words_creation_ansi(wow_window_words_binding *binding,
    BOOL *ansi)
{
    DWORD error = GetLastError();
    BOOL known;
    if (!binding || !ansi) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    AcquireSRWLockShared(&publication_lock);
    known = binding->creation_encoding_known;
    if (known) *ansi = binding->creation_ansi;
    ReleaseSRWLockShared(&publication_lock);
    SetLastError(known ? error : ERROR_NOT_FOUND);
    return known;
}

static BOOL own_window(HWND window)
{
    /* Original GetWindowData(GWL_WNDPROC) enforces TestWindowProcess.
     * Native USER keeps that check and accepts original short HWNDs, unlike
     * GetWindowThreadProcessId. Validate before reading the private property. */
    return GetWindowLongA(window, GWL_WNDPROC) != 0;
}

BOOL WINAPI wow_window_words_attach(HWND window, const WW *words)
{
    return wow_window_words_attach_class(window, words, NULL);
}

wow_window_words_binding *WINAPI wow_window_words_allocate(void)
{
    wow_window_words_binding *binding = HeapAlloc(GetProcessHeap(),
        HEAP_ZERO_MEMORY, sizeof(*binding));
    if (!binding) SetLastError(ERROR_NOT_ENOUGH_MEMORY);
    else binding->references = 1;
    return binding;
}

static void __cdecl release_generation_storage(void *storage)
{
    wow_window_words_binding *binding = storage;
    /* Quiesced domain destruction can run after USER/process teardown.
     * Never call into the retired guest domain from host storage release. */
    binding->generation_backing = 0;
    binding->retired_guest_backing = 0;
    wow_window_words_release(binding);
}

wow_window_words_binding *WINAPI wow_window_words_acquire_generation(ULONG backing)
{
    wow_window_words_binding *binding;
    DWORD error = GetLastError();
    binding = mvdm_softpc_wow_page_domain_window_host_storage(backing);
    if (!binding) {
        binding = wow_window_words_allocate();
        if (!binding) return NULL;
        if (!mvdm_softpc_wow_page_domain_bind_window_host_storage(backing,
                binding, release_generation_storage)) {
            wow_window_words_release(binding);
            SetLastError(ERROR_INVALID_STATE);
            return NULL;
        }
        binding->generation_backing = backing;
        /* Initial allocation reference now belongs to the generation. */
    } else if (binding->generation_backing != backing) {
        SetLastError(ERROR_INVALID_STATE);
        return NULL;
    }
    InterlockedIncrement(&binding->references);
    SetLastError(error);
    return binding;
}

static void release_reciprocal_generation_owner(wow_window_words_binding *binding)
{
    ULONG backing;
    DWORD error = GetLastError();
    if (!binding || !binding->generation_backing || !binding->retired_guest_backing)
        return;
    /* Called outside publication_lock, while property/table/caller references
     * still retain binding. A generation owning us cannot also be retained
     * until our final release: that would form a permanent reference cycle. */
    backing = binding->retired_guest_backing;
    binding->retired_guest_backing = 0;
    if (!mvdm_softpc_wow_page_domain_release_retained_window(backing))
        wow_user_native_call_failed("release-generation-owner", ERROR_INVALID_STATE);
    SetLastError(error);
}

BOOL WINAPI wow_window_words_pin_generation(wow_window_words_binding *binding, ULONG *backing)
{
    DWORD saved = GetLastError();
    BOOL result;
    ULONG generation;
    if (!binding || !backing) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    AcquireSRWLockShared(&publication_lock);
    generation = binding->generation_backing;
    result = !generation || mvdm_softpc_wow_page_domain_retain_host_window(generation, binding);
    ReleaseSRWLockShared(&publication_lock);
    if (!result) { SetLastError(ERROR_INVALID_STATE); return FALSE; }
    *backing = generation;
    SetLastError(saved);
    return TRUE;
}

BOOL WINAPI wow_window_words_attach_client_state(wow_window_words_binding *binding,
    struct tagWND *state)
{
    BOOL attached = FALSE;
    DWORD error = GetLastError();
    ULONG backing = 0;
    void *storage;
    PWND consumed = NULL;
    WCHAR *old_name = NULL;
    if (!binding || !state) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    AcquireSRWLockExclusive(&publication_lock);
    if (!binding->enrolled || binding->terminal_pending)
        error = ERROR_INVALID_STATE;
    else if (binding->client_state_owned)
        error = ERROR_ALREADY_EXISTS;
    else if (binding->client_state == state)
        error = ERROR_INVALID_PARAMETER; /* Never consume the published view itself. */
    else if (GetPropW(state->head.h, property_name) != binding)
        error = ERROR_INVALID_WINDOW_HANDLE;
    else if (binding->client_state && (state->cbwndExtra < 0 ||
            sizeof(WND) + (SIZE_T)state->cbwndExtra > binding->projected_state_bytes))
        error = ERROR_INVALID_DATA;
    else {
        /* ADAPTER-WOW-069: managed windows publish their guest identity
         * before constructing client WND. Transfer a separate host reference
         * to that exact generation before exposing the retained PWND.
         * Property-only native bindings have no graph publication to retain. */
        if (binding->objects) {
            backing = mvdm_softpc_wow_page_domain_borrow_window(
                (ULONG)state->head.h);
            if (!backing) error = ERROR_INVALID_STATE;
            else {
                storage = mvdm_softpc_wow_page_domain_window_host_storage(backing);
                if (storage == binding && binding->generation_backing == backing)
                    attached = TRUE;
                else if (storage || binding->generation_backing)
                    error = ERROR_INVALID_STATE;
                else {
                    InterlockedIncrement(&binding->references);
                    if (mvdm_softpc_wow_page_domain_bind_window_host_storage(
                            backing, binding, release_generation_storage)) {
                        binding->generation_backing = backing;
                        attached = TRUE;
                    } else {
                        InterlockedDecrement(&binding->references);
                        error = ERROR_INVALID_STATE;
                    }
                }
            }
        } else attached = TRUE;
        if (attached) {
            /* Registration's immutable identity view shares the class ref
             * already held by this binding, not native USER's CLS layout. */
            state->pcls = wow_class_words_client_identity(binding->class_words);
            if (binding->client_state) {
                PWND destination = binding->client_state;
                PWND next = destination->spwndNext, parent = destination->spwndParent;
                PWND child = destination->spwndChild, owner = destination->spwndOwner;
                /* Promotion must keep every previously borrowed PWND valid.
                 * Preserve graph edges until their next atomic publication;
                 * original creation facts replace only the resource view. */
                AcquireSRWLockExclusive(&binding->name_lock);
                old_name = destination->strName.Buffer;
                CopyMemory(destination, state, sizeof(WND) + (SIZE_T)state->cbwndExtra);
                destination->head.pSelf = (PBYTE)destination;
                destination->spwndNext = next; destination->spwndParent = parent;
                destination->spwndChild = child; destination->spwndOwner = owner;
                ReleaseSRWLockExclusive(&binding->name_lock);
                consumed = state; /* Name ownership transferred by the copy. */
            } else binding->client_state = state;
            binding->client_state_owned = TRUE;
        }
    }
    ReleaseSRWLockExclusive(&publication_lock);
    if (old_name) HeapFree(GetProcessHeap(), 0, old_name);
    if (consumed) HeapFree(GetProcessHeap(), 0, consumed);
    /* Do not reclaim generation-owned storage under publication_lock.
     * Failure here consumes the borrow and leaves domain reclamation pending. */
    if (backing && !mvdm_softpc_wow_page_domain_release_window(backing))
        wow_user_native_call_failed("release-client-state-generation", ERROR_INVALID_STATE);
    SetLastError(error);
    return attached;
}

BOOL WINAPI wow_window_words_prepare_resource_state(wow_window_words_binding *binding,
    HWND desktop)
{
    DWORD saved = GetLastError(), error;
    ULONG backing = 0;
    mvdm_softpc_wow_window_observation view;
    PWND candidate = NULL;
    SIZE_T bytes;
    BOOL result = FALSE;
    if (!binding || !desktop) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    if (!wow_window_words_pin_generation(binding, &backing)) return FALSE;
    if (!backing) { SetLastError(ERROR_INVALID_STATE); return FALSE; }
    __try {
        if (!mvdm_softpc_wow_page_domain_read_window_view(backing, &view)) __leave;
        if (wow_window_words_client_state_value(binding)) {
            result = TRUE; __leave; /* Construction only, not scalar refresh. */
        }
        if (view.descriptor.window_extra > (ULONG)(INT_MAX - sizeof(WND))) {
            SetLastError(ERROR_INVALID_DATA); __leave;
        }
        bytes = sizeof(WND) + max(view.descriptor.window_extra, sizeof(PVOID));
        candidate = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, bytes);
        if (!candidate) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); __leave; }
        candidate->head.h = (HWND)(ULONG_PTR)view.links.window;
        candidate->head.pSelf = (PBYTE)candidate;
        CopyMemory(&candidate->rcWindow, view.window_rect, sizeof(RECT));
        CopyMemory(&candidate->rcClient, view.client_rect, sizeof(RECT));
        candidate->style = view.style; candidate->ExStyle = view.exstyle;
        candidate->hModule = (HANDLE)(ULONG_PTR)view.module;
        if (TestwndChild(candidate)) candidate->spmenu = (PMENU)(ULONG_PTR)view.control_id;
        if (candidate->head.h == desktop) candidate->fnid = FNID_DESKTOP;
        /* No guessed creator encoding/version, dialog flag, procedure or
         * control-private extent. This view permits relationship/resource
         * reads only; it never enables original private getters or setters. */
        AcquireSRWLockExclusive(&publication_lock);
        if (!binding->client_state) {
            binding->projected_class.atomClassName = view.descriptor.atom;
            candidate->pcls = &binding->projected_class;
            binding->projected_state_bytes = bytes;
            binding->client_state = candidate;
            candidate = NULL;
        }
        ReleaseSRWLockExclusive(&publication_lock);
        result = TRUE;
    } __finally {
        error = result ? saved : GetLastError();
        if (candidate) HeapFree(GetProcessHeap(), 0, candidate);
        if (!mvdm_softpc_wow_page_domain_release_window(backing))
            wow_user_native_call_failed("release-resource-state-generation", ERROR_INVALID_STATE);
        SetLastError(error);
    }
    return result;
}

struct tagWND *WINAPI wow_window_words_client_state_value(wow_window_words_binding *binding)
{
    struct tagWND *state;
    if (!binding) return NULL;
    AcquireSRWLockShared(&publication_lock);
    state = binding->client_state;
    ReleaseSRWLockShared(&publication_lock);
    return state;
}

struct tagWND *WINAPI wow_window_words_lock_client_name(wow_window_words_binding *binding)
{
    struct tagWND *state = wow_window_words_client_state_value(binding);
    if (state) AcquireSRWLockShared(&binding->name_lock);
    return state;
}

BOOL WINAPI wow_window_words_owns_client_state(wow_window_words_binding *binding,
    struct tagWND *state)
{
    BOOL result;
    if (!binding || !state) return FALSE;
    AcquireSRWLockShared(&publication_lock);
    result = binding->client_state_owned && binding->client_state == state;
    ReleaseSRWLockShared(&publication_lock);
    return result;
}

void WINAPI wow_window_words_unlock_client_name(wow_window_words_binding *binding)
{
    ReleaseSRWLockShared(&binding->name_lock);
}

BOOL WINAPI wow_window_words_attach_class(HWND window, const WW *words,
    wow_class_words_binding *class_words)
{
    return wow_window_words_attach_target(window, words, class_words, NULL);
}

BOOL WINAPI wow_window_words_attach_target(HWND window, const WW *words,
    wow_class_words_binding *class_words, const wow_window_dispatch_target *target)
{
    wow_window_words_binding *binding;
    wow_class_words_binding *previous_class = NULL;
    BOOL attached = FALSE;
    BOOL observed;
    DWORD error, process_id = 0, thread_id;
    if (!words) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    if (target && (!target->procedure ||
            (target->wow_encoded && (!target->callback || !(target->procedure & WNDPROC_WOW))) ||
            target->procedure == (DWORD)wow_window_class_creation_proc)) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    if (!own_window(window)) return FALSE;
    thread_id = GetWindowThreadProcessId(window, &process_id);
    if (!thread_id) return FALSE;
    if (process_id != GetCurrentProcessId()) {
        SetLastError(ERROR_ACCESS_DENIED); return FALSE;
    }
    binding = wow_user_borrow_observed_binding(window);
    observed = binding != NULL;
    if (!binding) binding = wow_window_words_allocate();
    if (!binding) return FALSE;
    AcquireSRWLockExclusive(&publication_lock);
    if (binding->native_thread_id && binding->native_thread_id != thread_id)
        SetLastError(ERROR_INVALID_STATE);
    else if (binding->enrolled || GetPropW(window, property_name))
        SetLastError(ERROR_ALREADY_EXISTS);
    else attached = SetPropW(window, property_name, binding);
    error = GetLastError();
    if (attached) {
        if (observed) InterlockedIncrement(&binding->references); /* Property. */
        binding->enrolled = TRUE;
        binding->native_thread_id = thread_id;
        binding->words = *words;
        ZeroMemory(&binding->creation_menu, sizeof(binding->creation_menu));
        binding->creation_menu.checked = TRUE; /* Not necessarily a new HWND. */
        previous_class = binding->class_words;
        binding->class_words = class_words;
        ZeroMemory(&binding->cleanup, sizeof(binding->cleanup));
        binding->cleanup.storage = binding;
        binding->cleanup.native_window = window;
        binding->cleanup.procedure_is_guest = target && target->wow_encoded;
        binding->cleanup.pcls = wow_class_words_entry(class_words);
        binding->cleanup.adwWOW = binding->words.adwWOW;
        binding->cleanup.lpfnWndProc = target ? (WNDPROC)target->procedure : NULL;
        binding->objects = NULL;
        binding->retired_guest_backing = 0u;
        wow_class_words_window_association(class_words, TRUE);
    }
    ReleaseSRWLockExclusive(&publication_lock);
    if (attached) wow_class_words_release(previous_class);
    if (!attached && !observed) wow_window_words_release(binding);
    SetLastError(error);
    return attached;
}

wow_window_words_binding *WINAPI wow_window_words_acquire(HWND window)
{
    wow_window_words_binding *binding;
    if (!own_window(window)) return NULL;
    AcquireSRWLockShared(&publication_lock);
    binding = (wow_window_words_binding *)GetPropW(window, property_name);
    if (binding) InterlockedIncrement(&binding->references);
    ReleaseSRWLockShared(&publication_lock);
    if (!binding) SetLastError(ERROR_NOT_FOUND);
    return binding;
}

WW *WINAPI wow_window_words_value(wow_window_words_binding *binding)
{
    return binding ? &binding->words : NULL;
}

DWORD WINAPI wow_window_words_thread_id(wow_window_words_binding *binding)
{
    /* Captured before publication, immutable through retirement. A retained
     * HWND owner must not change identity when its native property disappears. */
    return binding ? binding->native_thread_id : 0;
}

BOOL WINAPI wow_window_words_set_observed_thread_id(wow_window_words_binding *binding,
    DWORD thread_id)
{
    DWORD error = GetLastError();
    BOOL result = FALSE;
    if (!binding || !thread_id) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    AcquireSRWLockExclusive(&publication_lock);
    if (binding->enrolled || binding->terminal_pending ||
            (binding->native_thread_id && binding->native_thread_id != thread_id))
        error = ERROR_INVALID_STATE;
    else {
        binding->native_thread_id = thread_id;
        result = TRUE;
    }
    ReleaseSRWLockExclusive(&publication_lock);
    SetLastError(error);
    return result;
}

void WINAPI wow_window_words_begin_creation(wow_window_words_binding *binding,
    HMENU menu)
{
    ZeroMemory(&binding->creation_menu, sizeof(binding->creation_menu));
    binding->creation_menu.input_menu = menu;
}

static BOOL prepare_creation_menu(HWND window, UINT message, LPARAM lp,
    wow_window_words_binding *binding)
{
    if (message == WM_GETMINMAXINFO && !binding->creation_menu.checked) {
        CREATESTRUCTA creation = {0};
        /* Original createw.c associates the menu before sizing callbacks.
         * Retain the caller's explicit menu even before USER exposes it. */
        creation.style = binding->words.dwStyle;
        creation.hMenu = binding->creation_menu.input_menu;
        return wow_window_creation_menu(window, &creation, binding->cleanup.pcls,
            &binding->creation_menu);
    }
    if (message == WM_NCCREATE || message == WM_CREATE)
        return wow_window_creation_menu(window, (CREATESTRUCTA *)lp,
            binding->cleanup.pcls, &binding->creation_menu);
    return TRUE;
}

BOOL WINAPI wow_window_words_refresh(HWND window, wow_window_words_binding *binding)
{
    return wow_window_words_refresh_info(window, binding, NULL, NULL);
}

/* Requires publication_lock. All fallible capture/identity work precedes
 * this operation; no native query or callback belongs in publication. */
static WCHAR *commit_window_info(wow_window_words_binding *binding,
    const WINDOWINFO *info, DWORD control_id, DWORD instance,
    WCHAR *name, ULONG name_length, ULONG name_capacity)
{
    WCHAR *old_name = NULL;
    BOOL child = (info->dwStyle & (WS_CHILD | WS_POPUP)) == WS_CHILD;
    binding->words.dwStyle = info->dwStyle;
    binding->words.dwExStyle = info->dwExStyle;
    binding->words.hInstance = (HANDLE)(ULONG_PTR)instance;
    if (binding->client_state) {
        if (name) {
            AcquireSRWLockExclusive(&binding->name_lock);
            old_name = binding->client_state->strName.Buffer;
            binding->client_state->strName.Buffer = name;
            binding->client_state->strName.Length = name_length;
            binding->client_state->strName.MaximumLength = name_capacity;
            binding->client_state->strName.bAnsi = FALSE;
            ReleaseSRWLockExclusive(&binding->name_lock);
        }
        if (child || TestwndChild(binding->client_state))
            binding->client_state->spmenu = (PMENU)(ULONG_PTR)control_id;
        binding->client_state->rcWindow = info->rcWindow;
        binding->client_state->rcClient = info->rcClient;
        binding->client_state->style = info->dwStyle;
        binding->client_state->ExStyle = info->dwExStyle;
        binding->client_state->hModule = binding->words.hInstance;
    }
    return old_name;
}

BOOL WINAPI wow_window_words_publish_client_graph(ULONG count,
    const wow_window_client_graph_row *rows)
{
    ULONG i, edge;
    DWORD saved = GetLastError();
    BOOL result = FALSE;
    if (!count || !rows) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    AcquireSRWLockExclusive(&publication_lock);
    for (i = 0; i < count; ++i) {
        if (!rows[i].binding || !rows[i].state ||
                rows[i].binding->client_state != rows[i].state ||
                rows[i].state->head.h != rows[i].window ||
                rows[i].info.cbSize != sizeof(WINDOWINFO)) goto done;
        for (edge = 0; edge < 4; ++edge)
            if (rows[i].target[edge] != MAXDWORD &&
                    rows[i].target[edge] >= count) goto done;
    }
    /* Nothing below can fail or call application code. Execution ownership
     * excludes original readers until the complete graph is installed. */
    for (i = 0; i < count; ++i) {
        PWND state = rows[i].state;
        (void)commit_window_info(rows[i].binding, &rows[i].info,
            rows[i].control_id, rows[i].module, NULL, 0, 0);
        if (!rows[i].binding->client_state_owned)
            rows[i].binding->projected_class.atomClassName = rows[i].class_atom;
        state->spwndNext = rows[i].target[0] == MAXDWORD ? NULL : rows[rows[i].target[0]].state;
        state->spwndParent = rows[i].target[1] == MAXDWORD ? NULL : rows[rows[i].target[1]].state;
        state->spwndChild = rows[i].target[2] == MAXDWORD ? NULL : rows[rows[i].target[2]].state;
        state->spwndOwner = rows[i].target[3] == MAXDWORD ? NULL : rows[rows[i].target[3]].state;
    }
    result = TRUE;
done:
    ReleaseSRWLockExclusive(&publication_lock);
    SetLastError(result ? saved : ERROR_INVALID_STATE);
    return result;
}

static BOOL apply_window_info(HWND window,
    wow_window_words_binding *binding, const WINDOWINFO *info,
    const DWORD *observed_id, const DWORD *observed_module, DWORD *published_id)
{
    LONG instance, control_id = 0;
    BOOL child = (info->dwStyle & (WS_CHILD | WS_POPUP)) == WS_CHILD;
    DWORD saved = GetLastError();
    WCHAR *name = NULL, *old_name = NULL;
    ULONG name_length = 0, name_capacity = 0;
    if (!binding) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    if (observed_module) instance = (LONG)*observed_module;
    else {
        SetLastError(ERROR_SUCCESS);
        instance = GetWindowLongA(window, GWL_HINSTANCE);
        if (!instance && GetLastError() != ERROR_SUCCESS) return FALSE;
    }
    if (child) {
        if (observed_id) control_id = (LONG)*observed_id;
        else {
            SetLastError(ERROR_SUCCESS);
            control_id = GetWindowLongA(window, GWL_ID);
            if (!control_id && GetLastError() != ERROR_SUCCESS) return FALSE;
        }
    }
    if (wow_window_words_client_state_value(binding) &&
            !wow_window_client_capture_name(window, &name, &name_length, &name_capacity))
        return FALSE;
    AcquireSRWLockExclusive(&publication_lock);
    if (GetPropW(window, property_name) != binding) {
        ReleaseSRWLockExclusive(&publication_lock);
        if (name) HeapFree(GetProcessHeap(), 0, name);
        SetLastError(ERROR_INVALID_WINDOW_HANDLE); return FALSE;
    }
    /* Preserve private flags/FNID/control data and guest procedures. */
    old_name = commit_window_info(binding, info, (DWORD)control_id,
        (DWORD)instance, name, name_length, name_capacity);
    ReleaseSRWLockExclusive(&publication_lock);
    if (old_name) HeapFree(GetProcessHeap(), 0, old_name);
    if (published_id) *published_id = (DWORD)control_id;
    SetLastError(saved);
    return TRUE;
}

BOOL WINAPI wow_window_words_refresh_info(HWND window,
    wow_window_words_binding *binding, WINDOWINFO *snapshot, DWORD *snapshot_id)
{
    WINDOWINFO info = {sizeof(info)};
    DWORD error = GetLastError();
    if (!binding) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    if (!GetWindowInfo(window, &info) ||
            !apply_window_info(window, binding, &info, NULL, NULL, snapshot_id)) return FALSE;
    if (snapshot) *snapshot = info;
    SetLastError(error);
    return TRUE;
}

BOOL WINAPI wow_window_words_refresh_observed(HWND window, const WINDOWINFO *info,
    DWORD control_id, DWORD module)
{
    DWORD error = GetLastError();
    wow_window_words_binding *binding;
    BOOL result;
    if (!info || info->cbSize != sizeof(*info)) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    binding = wow_window_words_acquire(window);
    if (!binding) {
        /* An observed native window need not be a recovered control. Never
         * enroll it or allocate private storage merely to refresh a view. */
        if (GetLastError() != ERROR_NOT_FOUND) return FALSE;
        SetLastError(error); return TRUE;
    }
    result = apply_window_info(window, binding, info, &control_id, &module, NULL);
    if (!result) error = GetLastError();
    wow_window_words_release(binding);
    SetLastError(error);
    return result;
}

BOOL WINAPI wow_window_words_project_observed(ULONG count,
    const mvdm_softpc_wow_window_observation *rows)
{
    typedef struct local_update {
        wow_window_words_binding *binding;
        PWND state;
        WCHAR *name, *old_name;
        ULONG length, capacity;
    } local_update;
    local_update *updates;
    DWORD pid = GetCurrentProcessId(), saved = GetLastError();
    ULONG i;
    BOOL result = FALSE, locked = FALSE;
    if (!rows || !count || count >= 0x10000u) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    updates = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
        count * sizeof(*updates));
    if (!updates) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return FALSE; }
    __try {
        for (i = 0; i < count; ++i) {
            HWND window = (HWND)(ULONG_PTR)rows[i].links.window;
            if (rows[i].process_id != pid) continue;
            updates[i].binding = wow_window_words_acquire(window);
            if (!updates[i].binding) {
                if (GetLastError() != ERROR_NOT_FOUND) goto done;
                continue; /* Native observation, not a new enrollment. */
            }
            updates[i].state = wow_window_words_client_state_value(updates[i].binding);
            if (updates[i].state && !wow_window_client_capture_name(window,
                    &updates[i].name, &updates[i].length, &updates[i].capacity))
                goto done;
        }
        /* Existing execution ownership excludes original callbacks. Hold
         * property publication stable through the page-domain commit, then
         * perform only prepared local swaps. The page-domain path has no
         * guest/native callout; release scratch/references outside this lock. */
        AcquireSRWLockExclusive(&publication_lock);
        locked = TRUE;
        for (i = 0; i < count; ++i) {
            if (rows[i].process_id != pid) continue;
            if (GetPropW((HWND)(ULONG_PTR)rows[i].links.window, property_name) !=
                    updates[i].binding ||
                    (updates[i].binding &&
                     (updates[i].binding->terminal_pending ||
                      updates[i].binding->client_state != updates[i].state))) {
                SetLastError(ERROR_INVALID_STATE); goto done;
            }
        }
        if (!mvdm_softpc_wow_page_domain_project_window_graph(count, rows)) goto done;
        for (i = 0; i < count; ++i) if (updates[i].binding) {
            WINDOWINFO info = {sizeof(info)};
            CopyMemory(&info.rcWindow, rows[i].window_rect, sizeof(RECT));
            CopyMemory(&info.rcClient, rows[i].client_rect, sizeof(RECT));
            info.dwStyle = rows[i].style;
            info.dwExStyle = rows[i].exstyle;
            updates[i].old_name = commit_window_info(updates[i].binding,
                &info, (info.dwStyle & (WS_CHILD | WS_POPUP)) == WS_CHILD ?
                    rows[i].control_id : 0, rows[i].module,
                updates[i].name, updates[i].length, updates[i].capacity);
            updates[i].name = NULL; /* Single transfer to retained WND. */
        }
        result = TRUE;
done:
        if (result) SetLastError(saved);
    } __finally {
        DWORD error = GetLastError();
        if (locked) ReleaseSRWLockExclusive(&publication_lock);
        for (i = 0; i < count; ++i) {
            if (updates[i].name) HeapFree(GetProcessHeap(), 0, updates[i].name);
            if (updates[i].old_name) HeapFree(GetProcessHeap(), 0, updates[i].old_name);
            wow_window_words_release(updates[i].binding);
        }
        HeapFree(GetProcessHeap(), 0, updates);
        SetLastError(error);
    }
    return result;
}

wow_cleanup_window *WINAPI wow_window_words_cleanup_value(wow_window_words_binding *binding)
{
    return binding ? &binding->cleanup : NULL;
}

BOOL WINAPI wow_window_words_exchange(HWND window, unsigned index,
    DWORD value, DWORD *previous)
{
    wow_window_words_binding *binding;
    ULONG backing = 0, ignored;
    BOOL result = FALSE;
    if (index >= WND_CNT_WOWDWORDS || !previous) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    binding = wow_window_words_acquire(window);
    if (!binding) return FALSE;
    AcquireSRWLockExclusive(&publication_lock);
    if (GetPropW(window, property_name) != binding) goto done;
    if (binding->objects) {
        ULONG full = (ULONG)(ULONG_PTR)binding->objects->windows[LOWORD(window)];
        backing = mvdm_softpc_wow_page_domain_borrow_window(full);
        if (!backing || !mvdm_softpc_wow_page_domain_exchange_window_word(
                backing, index, value, &ignored)) goto done;
    }
    *previous = (DWORD)InterlockedExchange((LONG *)&binding->words.adwWOW[index], value);
    result = TRUE;
done:
    /* A live published WND cannot be freed by this temporary release. */
    if (backing) (void)mvdm_softpc_wow_page_domain_release_window(backing);
    ReleaseSRWLockExclusive(&publication_lock);
    wow_window_words_release(binding);
    if (!result) SetLastError(ERROR_INVALID_STATE);
    return result;
}

BOOL WINAPI wow_window_words_publish_owner(HWND window, wow_window_words_binding *binding,
    wow_task_order_thread *thread)
{
    BOOL result=FALSE;
    if (!binding || !thread || !thread->ppi || !thread->ppi->objects) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    AcquireSRWLockExclusive(&publication_lock);
    if (GetPropW(window,property_name)!=binding || binding->objects) {
        SetLastError(ERROR_INVALID_STATE);
    } else if (wow_user_window_publish(thread->ppi->objects,window,&binding->cleanup,thread)) {
        binding->cleanup.thread=thread;
        binding->objects=thread->ppi->objects;
        InterlockedIncrement(&binding->references); /* Published table owner. */
        result=TRUE;
    }
    ReleaseSRWLockExclusive(&publication_lock);
    return result;
}

void WINAPI wow_window_words_release(wow_window_words_binding *binding)
{
    if (binding && InterlockedDecrement(&binding->references) == 0) {
        /* Like HMMarkObjectDestroy/ThreadUnlock, the final existing native
         * borrow owns physical WND release. A terminated worker has already
         * discarded its entire guest address space. */
        if (binding->retired_guest_backing &&
                mvdm_softpc_wow_page_domain_active())
            (void)mvdm_softpc_wow_page_domain_release_retained_window(
                binding->retired_guest_backing);
        wow_class_words_release(binding->class_words);
        /* Host carrier only; no guest-capable work belongs in final release. */
        if (binding->client_state) {
            if (binding->client_state->strName.Buffer)
                HeapFree(GetProcessHeap(), 0, binding->client_state->strName.Buffer);
            HeapFree(GetProcessHeap(), 0, binding->client_state);
        }
        HeapFree(GetProcessHeap(), 0, binding);
    }
}

DWORD *WINAPI wow_window_class_words_value(wow_window_words_binding *binding)
{
    return binding ? wow_class_words_value(binding->class_words) : NULL;
}

BOOL WINAPI wow_window_class_target_read(HWND window, wow_window_dispatch_target *target)
{
    wow_window_words_binding *binding;
    BOOL found;
    if (!target) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    binding = wow_window_words_acquire(window);
    if (!binding) return FALSE;
    found = wow_class_words_target(binding->class_words, target);
    wow_window_words_release(binding);
    if (!found) SetLastError(ERROR_NOT_FOUND);
    return found;
}

BOOL WINAPI wow_window_class_procedure_exchange(HWND window, DWORD procedure,
    BOOL guest, DWORD *previous, BOOL *previous_guest)
{
    wow_window_words_binding *binding;
    BOOL result;
    if (procedure == (DWORD)wow_window_class_creation_proc) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    binding = wow_window_words_acquire(window);
    if (!binding) return FALSE;
    result = wow_class_words_exchange_procedure(binding->class_words,
        procedure, guest, previous, previous_guest);
    wow_window_words_release(binding);
    return result;
}

static BOOL detach_matching(HWND window, wow_window_words_binding *expected)
{
    wow_window_words_binding *binding;
    BOOL table_reference;
    if (!own_window(window)) return FALSE;
    AcquireSRWLockExclusive(&publication_lock);
    binding = (wow_window_words_binding *)GetPropW(window, property_name);
    if (!binding || (expected && binding != expected)) {
        ReleaseSRWLockExclusive(&publication_lock);
        return FALSE;
    }
    table_reference = binding->objects != NULL;
    if (binding->objects && !wow_user_window_retire(binding->objects, window,
            &binding->cleanup, &binding->retired_guest_backing)) {
        ReleaseSRWLockExclusive(&publication_lock);
        return FALSE;
    }
    binding = (wow_window_words_binding *)RemovePropW(window, property_name);
    if (binding) {
        binding->enrolled = FALSE;
        if (binding->objects) {
            binding->objects=NULL;
        }
        wow_class_words_window_association(binding->class_words, FALSE);
        binding->cleanup.thread = NULL; /* Retired borrows no longer own a thread. */
    }
    ReleaseSRWLockExclusive(&publication_lock);
    release_reciprocal_generation_owner(binding);
    if (table_reference) wow_window_words_release(binding);
    wow_window_words_release(binding);
    return binding != NULL;
}

void WINAPI wow_window_words_detach(HWND window)
{
    detach_matching(window, NULL);
}

BOOL WINAPI wow_window_words_detach_if(HWND window, wow_window_words_binding *expected)
{
    if (!expected) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    return detach_matching(window, expected);
}

BOOL WINAPI wow_window_words_terminal(HWND window, wow_window_words_binding *binding)
{
    BOOL retired = TRUE, property_reference = FALSE, table_reference = FALSE;
    WORD finalize_fnid;
    if (!binding) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    AcquireSRWLockExclusive(&publication_lock);
    /* ADAPTER-WOW-063: createw.c marks WFDESTROYED before xxxFreeWindow's
     * canonical final-control release. This is the existing terminal owner,
     * not a generic message-based inference or a write to guest/native WND.
     * Retained original getset.c uses this bit to allow private-word cleanup. */
    if (binding->enrolled && binding->client_state)
        SetWF(binding->client_state, WFDESTROYED);
    finalize_fnid = binding->enrolled && binding->client_state ?
        GETFNID(binding->client_state) : 0;
    ReleaseSRWLockExclusive(&publication_lock);
    /* Original xxxFreeWindow canonical-control fallback precedes identity
     * withdrawal. No publication lock may span original LOCAL callbacks or
     * combo parent notification. Nested destruction may retire this owner;
     * recheck enrollment below after the scoped finalizer returns. */
    if ((finalize_fnid == FNID_EDIT || finalize_fnid == FNID_COMBOBOX ||
            finalize_fnid == FNID_LISTBOX || finalize_fnid == FNID_BUTTON) &&
            !wow_control_finalize_bound(window))
        wow_user_native_call_failed("finalize-original-control", GetLastError());
    if (finalize_fnid == FNID_DIALOG && !wow_dialog_finalize_bound(window))
        wow_user_native_call_failed("finalize-original-dialog", GetLastError());
    /* Original finalization can reenter destruction and retire this binding.
     * Never clean a replacement HWND generation's appearance state. No
     * publication lock spans the DLL's resource release. */
    AcquireSRWLockExclusive(&publication_lock);
    if (!binding->enrolled && !binding->terminal_pending) {
        ReleaseSRWLockExclusive(&publication_lock);
        return TRUE; /* Nested destruction already consumed this owner. */
    }
    table_reference = binding->objects != NULL;
    if (table_reference)
        retired = wow_user_window_retire(binding->objects, window,
            &binding->cleanup, &binding->retired_guest_backing);
    if (binding->enrolled) {
        if (GetPropW(window, property_name) == binding)
            RemovePropW(window, property_name);
        binding->enrolled = FALSE;
        property_reference = TRUE;
    }
    binding->terminal_pending = !retired;
    if (retired) {
        binding->objects = NULL;
        binding->cleanup.thread = NULL;
        wow_class_words_window_association(binding->class_words, FALSE);
    }
    ReleaseSRWLockExclusive(&publication_lock);
    if (retired) release_reciprocal_generation_owner(binding);
    if (retired && table_reference) wow_window_words_release(binding);
    if (property_reference) wow_window_words_release(binding);
    return retired;
}

LRESULT CALLBACK wow_window_terminal_observer(int code, WPARAM wp, LPARAM lp)
{
    wow_window_words_binding *binding = NULL;
    const CWPRETSTRUCT *message = (const CWPRETSTRUCT *)lp;
    HWND window = NULL;
    DWORD error = GetLastError();
    LRESULT result;
    /* Retain the exact generation before forwarding the hook chain. A later
     * hook can synchronously destroy/reuse the HWND; it must not make this
     * terminal edge act on the replacement generation. */
    if (code >= 0 && message && message->message == WM_NCDESTROY) {
        window = message->hwnd;
        binding = wow_window_words_acquire(window);
    }
    SetLastError(error);
    __try {
        result = CallNextHookEx(NULL, code, wp, lp);
    } __finally {
        error = GetLastError();
        if (binding) {
            BOOL live;
            AcquireSRWLockShared(&publication_lock);
            live = binding->enrolled && GetPropW(window, property_name) == binding;
            ReleaseSRWLockShared(&publication_lock);
            /* A downstream hook may have completed nested window/task
             * retirement. Its retained old storage is not permission to
             * reenter the already retired task or clean up a successor. */
            if (!live) {
                wow_window_words_release(binding);
                binding = NULL;
            }
        }
        if (binding) {
            if (binding->native_thread_id != GetCurrentThreadId())
                wow_user_native_call_failed("terminal-observer-owner", ERROR_INVALID_STATE);
            __try {
                /* Canonical original control/dialog finalizers already own
                 * their required task/resource/borrow scopes. Property-only
                 * native retirement must not acquire guest execution merely
                 * to remove metadata, or depend on guest entry succeeding. */
                (void)wow_window_words_terminal(window, binding);
            } __finally {
                wow_window_words_release(binding);
            }
        }
        SetLastError(error);
    }
    /* A synthetic NCDESTROY remains a message, not evidence that USER freed
     * the real HWND. This preserves the old gateway's terminal-message
     * contract; actual destruction/reuse has separate acceptance assertions. */
    return result;
}

BOOL WINAPI wow_window_words_collect_retired(wow_user_object_table *objects)
{
    ULONG i;
    BOOL complete = TRUE;
    if (!objects) return TRUE;
    for (i = 1u; i <= objects->last_handle; ++i) {
        wow_window_words_binding *binding = NULL;
        HWND window = NULL;
        AcquireSRWLockShared(&publication_lock);
        if (objects->entries[i].bType == TYPE_WINDOW) {
            wow_cleanup_window *view = objects->entries[i].phead;
            if (view && view->storage && view->storage->terminal_pending) {
                binding = view->storage;
                window = objects->windows[i];
                InterlockedIncrement(&binding->references);
            }
        }
        ReleaseSRWLockShared(&publication_lock);
        if (binding) {
            if (!wow_window_words_terminal(window, binding)) complete = FALSE;
            wow_window_words_release(binding);
        }
    }
    return complete;
}

void WINAPI wow_window_note_native_procedure(HWND window, WNDPROC procedure,
    BOOL unicode)
{
    DWORD error = GetLastError();
    wow_user_runtime_thread *runtime = wow_user_runtime_current();
    wow_window_words_binding *binding;
    BOOL held;
    BOOL matched = FALSE;
    WORD fnid;
    WNDPROC canonical;
    if (!runtime || !runtime->thread) return;
    binding = wow_window_words_acquire(window);
    if (!binding) { SetLastError(error); return; }
    held = runtime->exclusive_held;
    if (!held && !wow_user_runtime_enter(runtime))
        wow_user_native_call_failed("record-native-cleanup-owner", GetLastError());
    if (runtime->thread->ppi)
        matched = wow_class_words_match_native(runtime->thread->ppi->classes,
            procedure, unicode, &fnid, &canonical);
    /* GetClassInfo's process lookup need not identify the native class
     * attached to this HWND. For an imported native registration, compare
     * the procedure actually being called with this window's class target,
     * not with a name or the current process-wide lookup winner. Logical
     * application classes are excluded; cross-class standard calls above
     * still select the called standard target, as PatchThreadWindows needs. */
    if (!matched && wow_class_words_native_registration(binding->class_words)) {
        WNDPROC actual = (WNDPROC)(unicode ? GetClassLongW(window, GCL_WNDPROC) :
            GetClassLongA(window, GCL_WNDPROC));
        if (actual && actual == procedure) {
            canonical = (WNDPROC)GetClassLongA(window, GCL_WNDPROC);
            fnid = wow_class_words_entry(binding->class_words)->fnid;
            matched = canonical != NULL && fnid != 0;
        }
    }
    if (matched) {
        binding->cleanup.fnid = fnid;
        binding->cleanup.native_cleanup = canonical;
    }
    if (!held && !wow_user_runtime_leave(runtime))
        wow_user_native_call_failed("release-native-cleanup-owner", GetLastError());
    wow_window_words_release(binding);
    SetLastError(error);
}

BOOL WINAPI wow_window_words_prepare_exit(HWND window,
    wow_window_words_binding *binding)
{
    wow_user_runtime_thread *runtime = wow_user_runtime_current();
    WORD fnid;
    WNDPROC terminal;
    /* Preserve the original cleanup boundary's invalid-HWND diagnosis.
     * A disappeared native object is not a lock/owner-state failure. */
    if (!IsWindow(window)) {
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return FALSE;
    }
    if (!runtime || !runtime->exclusive_held || !runtime->thread || !binding ||
            binding->cleanup.thread != runtime->thread ||
            !(runtime->thread->TIF_flags & WOW_TASK_TIF_INCLEANUP) ||
            GetPropW(window, property_name) != binding) {
        SetLastError(ERROR_INVALID_STATE);
        return FALSE;
    }
    /* createw.c PatchThreadWindows chooses the exercised standard target,
     * not an application subclass. FNID_DIALOG has no server procedure:
     * patch it to the no-callback terminal procedure, which retires native
     * dialog storage at WM_NCDESTROY. The terminal boundary releases
     * original dialog resources after native child destruction, while the
     * early task-resource scope is still available. Recovered controls retain their existing
     * resource-aware entry. Keep prepare-all before any DestroyWindow. */
    fnid = binding->client_state ? GETFNID(binding->client_state) : 0;
    /* A native COMMDLG window may have no original client FNID. Its
     * explicitly registered resource owner is sufficient authority for the
     * resource-only terminal edge; never manufacture an original dialog. */
    terminal = binding->resource_destroy ? wow_dialog_host_terminal_proc :
        wow_control_client_procedure(fnid, FALSE);
    if (!terminal)
        terminal = fnid == FNID_DIALOG ? wow_dialog_host_terminal_proc :
            binding->cleanup.native_cleanup ? binding->cleanup.native_cleanup : DefWindowProcA;
    /* PatchThreadWindows changes the real native WND selection too. An old
     * immutable guest entry must not remain selected after its task is dead.
     * This index neither sends style messages nor invokes application code. */
    if (!SetWindowLongA(window, GWL_WNDPROC, (LONG)terminal))
        return FALSE;
    binding->cleanup.lpfnWndProc = terminal;
    binding->cleanup.procedure_is_guest = FALSE;
    binding->cleanup.serverSideProc = TRUE;
    return TRUE;
}

static BOOL dispatch_bound(HWND window, UINT message, WPARAM wp,
    LPARAM lp, DWORD procedure, wow_window_callback callback, LONG *result,
    BOOL immutable_guest)
{
    wow_window_words_binding *binding;
    wow_window_dispatch_view view;
    wow_user_borrow_scope scope;
    wow_task_callback_scope task_scope;
    ULONG saved_callback[2];
    BOOL callback_published = FALSE;
    if (!result || !procedure ||
            ((procedure & WNDPROC_WOW) && !callback)) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    binding = wow_window_words_acquire(window);
    if (!binding) return FALSE;
    /* The untyped original dispatch helper cannot classify FFFF by guessing
     * current HWND state. Only an explicitly typed immutable guest entry may
     * carry that encoded selector; native CPDs use native USER APIs. */
    if (!immutable_guest && HIWORD(procedure) == WNDPROC_HANDLE) {
        wow_window_words_release(binding);
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    view.window = window;
    view.adwWOW = binding->words.adwWOW;
    view.callback = callback;
    /* Original client.c dispatches native procedures without entering WOW.
     * Keep the window alive, but do not make a native callback wait for the
     * guest CPU or publish a guest callback window it does not consume. */
    if (!(procedure & WNDPROC_WOW)) {
        __try {
            if (!prepare_creation_menu(window, message, lp, binding)) return FALSE;
            wow_window_note_native_procedure(window, (WNDPROC)procedure, FALSE);
            *result = DispatchClientMessage(&view, message, wp, lp, procedure);
        } __finally {
            DWORD error = GetLastError();
            wow_window_words_release(binding);
            SetLastError(error);
        }
        return TRUE;
    }
    if (!wow_task_callback_enter(&task_scope)) {
        DWORD error = GetLastError();
        wow_window_words_release(binding); SetLastError(error); return FALSE;
    }
    if (!wow_user_borrow_enter(&scope)) {
        DWORD error = GetLastError();
        (void)wow_task_callback_leave(&task_scope);
        wow_window_words_release(binding); SetLastError(error); return FALSE;
    }
    __try {
        /* ADAPTER-WOW-076: ordinary guest windows need not have an original
         * host WND. Their existing published owner still has the exact full
         * native identity. Normalize only this retained guest entry; native
         * procedure pass-through above keeps its original input argument. */
        if (binding->objects) {
            HWND full = wow_user_window_full_handle(binding->objects, LOWORD(window));
            if (!full || GetPropW(full, property_name) != binding) {
                SetLastError(ERROR_INVALID_WINDOW_HANDLE); return FALSE;
            }
            window = full;
        } else if (binding->client_state) window = binding->client_state->head.h;
        view.window = window;
        /* Resource conversion may enter the guest. Establish the same task
         * and alias scope as the callback before loading the class menu. */
        if (!prepare_creation_menu(window, message, lp, binding)) return FALSE;
        /* Original wowuserp.h WW aliases WND's live style/ExStyle/hModule;
         * rtl/getset.c exposes those same fields through public indices.
         * Refresh before dispatch, preserving all WOW-owned words and flags.
         * This is not a replacement for direct-access/callback scope binding. */
        if (!wow_window_words_refresh(window, binding)) return FALSE;
        if (binding->objects) {
            if (!wow_user_window_refresh_message(window, message, wp, lp)) return FALSE;
            if (!mvdm_softpc_wow_page_domain_callback_window(
                    (unsigned long)(ULONG_PTR)window, saved_callback)) return FALSE;
            callback_published = TRUE;
            /* Native creation/SetMenu installs the menu before calling the
             * client (original createw.c/mnapi.c ordering). USER16 GetMenu
             * reads WND/MENU directly inside that callback, so publishing
             * only after the native API returns is too late. A failed view
             * refresh must not dispatch this callback with stale fields. */
            if (!wow_menu_native_refresh_window(window)) return FALSE;
            if (!wow_menu_native_refresh_message(message, wp, lp)) return FALSE;
        }
        *result = DispatchClientMessage(&view, message, wp, lp, procedure);
    } __finally {
        DWORD error = GetLastError();
        if (callback_published &&
                !mvdm_softpc_wow_page_domain_restore_callback(saved_callback))
            wow_user_native_call_failed("restore-window-callback-cache", ERROR_INVALID_STATE);
        /* Release this WND owner while the alias/task scope is still live.
         * Its final retained backing can then be reclaimed by scope leave,
         * including the sampled CLS it kept alive during nested destruction. */
        wow_window_words_release(binding);
        wow_user_borrow_leave_required(&scope);
        (void)wow_task_callback_leave(&task_scope);
        SetLastError(error);
    }
    return TRUE;
}

BOOL WINAPI wow_window_dispatch_bound(HWND window, UINT message, WPARAM wp,
    LPARAM lp, DWORD procedure, wow_window_callback callback, LONG *result)
{
    return dispatch_bound(window, message, wp, lp, procedure, callback, result, FALSE);
}

BOOL WINAPI wow_window_dispatch_immutable(HWND window, UINT message, WPARAM wp,
    LPARAM lp, const wow_window_dispatch_target *target, LONG *result)
{
    BOOL dispatched = FALSE;
    DWORD error = GetLastError();
    if (!result || !target || !target->wow_encoded ||
            !(target->procedure & WOW_WINDOW_PROC_TAG) || !target->callback) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    /* This target is an immutable native entry's original guest identity,
     * not a value inferred from the HWND's newest procedure. A saved previous
     * entry must remain callable after native/guest subclass replacement.
     * Shared original dispatch, task, borrow, graph and error scopes stay
     * at the existing owner. Terminal observation belongs to the thread's
     * separate native-message owner in the replacement cohort. */
    window_trace(window, message, 0, "WindowEntry", error);
    SetLastError(error);
    __try {
        dispatched = dispatch_bound(window,message,wp,lp,target->procedure,
            target->callback,result,TRUE);
    } __finally {
        error = GetLastError();
        window_trace(window, message, *result,
            dispatched ? "WindowDispatch" : "WindowRejected", error);
        SetLastError(error);
    }
    return dispatched;
}

BOOL WINAPI wow_window_procedure_read(HWND window, DWORD *procedure)
{
    wow_window_words_binding *binding;
    wow_window_dispatch_target target;
    DWORD native, error;
    if (!procedure) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    binding = wow_window_words_acquire(window);
    if (!binding) return FALSE;
    native = (DWORD)GetWindowLongA(window, GWL_WNDPROC);
    error = GetLastError();
    if (native) *procedure = wow_window_entry_guest_target((WNDPROC)native, &target) ?
        target.procedure : native;
    wow_window_words_release(binding);
    SetLastError(error);
    return native != 0;
}

static void cancel_native_adoption(HWND window, wow_window_words_binding *binding)
{
    BOOL table_reference;
    AcquireSRWLockExclusive(&publication_lock);
    if (GetPropW(window, property_name) != binding || !binding->enrolled ||
            binding->class_words || binding->terminal_pending)
        wow_user_native_call_failed("cancel-native-subclass-identity", ERROR_INVALID_STATE);
    table_reference = binding->objects != NULL;
    if (table_reference && !wow_user_window_cancel_native_adoption(binding->objects,
            window, &binding->cleanup, binding->generation_backing))
        wow_user_native_call_failed("cancel-native-subclass-publication", GetLastError());
    if (RemovePropW(window, property_name) != binding)
        wow_user_native_call_failed("cancel-native-subclass-property", ERROR_INVALID_STATE);
    binding->enrolled = FALSE;
    binding->objects = NULL;
    ZeroMemory(&binding->cleanup, sizeof(binding->cleanup));
    ReleaseSRWLockExclusive(&publication_lock);
    /* Keep the observed generation, its WW aliases and resource-only view.
     * Neither terminal() nor retire_handle() belongs to this failure path. */
    if (table_reference) wow_window_words_release(binding);
    wow_window_words_release(binding); /* Property. */
}

BOOL WINAPI wow_window_procedure_adopt_native(HWND input, DWORD procedure, DWORD *previous)
{
    wow_user_runtime_thread *runtime = wow_user_runtime_current();
    wow_task_order_thread *owner;
    wow_window_words_binding *binding = NULL;
    wow_window_dispatch_target target = {0};
    WW *borrowed, words;
    HWND window;
    DWORD full, pid, tid, saved = GetLastError(), error;
    LONG old;
    WNDPROC entrance;
    BOOL held, result = FALSE;
    if (!previous || !(procedure & WNDPROC_WOW) || !runtime || !runtime->thread ||
            !runtime->classes || !runtime->classes->callback) {
        SetLastError(ERROR_INVALID_STATE); return FALSE;
    }
    held = runtime->exclusive_held;
    if (!held && !wow_user_runtime_enter(runtime)) return FALSE;
    __try {
        /* The original caller already holds a PWW borrow. Resolve the same
         * full generation, including original signed-short HWND inputs. */
        full = mvdm_softpc_wow_page_domain_window_handle(LOWORD(input));
        if (!full || ((ULONG)(ULONG_PTR)input != full &&
                (ULONG)(ULONG_PTR)input != LOWORD(input) &&
                (LONG)(LONG_PTR)input != (LONG)(SHORT)LOWORD(input))) {
            SetLastError(ERROR_INVALID_WINDOW_HANDLE); __leave;
        }
        window = (HWND)(ULONG_PTR)full;
        tid = GetWindowThreadProcessId(window, &pid);
        if (!tid || pid != GetCurrentProcessId()) {
            SetLastError(ERROR_ACCESS_DENIED); __leave;
        }
        owner = tid == GetCurrentThreadId() ? runtime->thread :
            (runtime->thread->host && runtime->thread->host->find_thread ?
             runtime->thread->host->find_thread(runtime->thread, tid) : NULL);
        if (!owner || owner->ppi != runtime->thread->ppi ||
                (owner->TIF_flags & WOW_TASK_TIF_INCLEANUP)) {
            SetLastError(ERROR_INVALID_STATE); __leave;
        }
        borrowed = wow_user_borrow_window(window);
        if (!borrowed) __leave;
        words = *borrowed;
        target.procedure = procedure;
        target.callback = runtime->classes->callback;
        target.wow_encoded = TRUE;
        entrance = wow_window_entry_for_guest(&target);
        if (!entrance) __leave;
        if (!wow_window_words_attach_target(window, &words, NULL, &target)) __leave;
        binding = wow_window_words_acquire(window);
        if (!binding)
            wow_user_native_call_failed("retain-native-subclass", GetLastError());
        if (!wow_window_words_publish_owner(window, binding, owner)) __leave;
        /* One native WNDPROC exchange, after all allocating publication work.
         * This index has no style notification/callout. The returned native
         * value, including CPD, is the actual predecessor, not an earlier
         * sampled target. Original WU32SetWindowLong owns thunk/WW updates. */
        SetLastError(ERROR_SUCCESS);
        old = SetWindowLongA(window, GWL_WNDPROC, (LONG)entrance);
        if (!old) {
            if (!GetLastError()) SetLastError(ERROR_INVALID_STATE);
            __leave;
        }
        *previous = (DWORD)old;
        result = TRUE;
    } __finally {
        error = result ? saved : GetLastError();
        if (binding) {
            if (!result) cancel_native_adoption(window, binding);
            wow_window_words_release(binding);
        }
        if (!held && !wow_user_runtime_leave(runtime))
            wow_user_native_call_failed("leave-native-subclass", GetLastError());
        SetLastError(error);
    }
    return result;
}

/* The product witness is deliberately off by default.  It observes the
 * native entrance and the already-completed original dispatch separately so
 * an integration test can distinguish USER delivery from WOW callback work.
 * It neither retains the HWND nor affects message ownership. */
static VOID window_trace(HWND window, UINT message, LONG result,
    const char *stage, DWORD error)
{
    char path[MAX_PATH], line[176];
    DWORD bytes, written;
    HANDLE file;
    LONG native_procedure;

    if (!GetEnvironmentVariableA("MVDM_WOW_WINDOW_TRACE_PATH", path,
            sizeof(path))) return;
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ |
        FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return;
    native_procedure = GetWindowLongA(window, GWL_WNDPROC);
    bytes = (DWORD)sprintf_s(line, sizeof(line),
        "%lu %s hwnd=%08lX message=%04X native=%08lX result=%08lX tid=%lu error=%lu\r\n",
        (unsigned long)GetCurrentProcessId(), stage,
        (unsigned long)(ULONG_PTR)window, (unsigned)message,
        (unsigned long)native_procedure, (unsigned long)result,
        (unsigned long)GetCurrentThreadId(), (unsigned long)error);
    if (bytes) (void)WriteFile(file, line, bytes, &written, NULL);
    CloseHandle(file);
}
