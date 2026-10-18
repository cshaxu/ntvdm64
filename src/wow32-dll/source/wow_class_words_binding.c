#include "wow_class_words_binding.h"
#include "wow_class_remove_bindings.h"
#include "wow_window_words_binding.h"
#include "wow_window_creation_binding.h"
#include "wow_standard_class_bindings.h"
#include "wow_edit_entry_bindings.h"
#include "wow_dialog_state_bindings.h"
#include "wow_user_client_view_layout.h"
#include "opennt-host/windows/core/ntuser/inc/user.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_wow_page_domain.h"

#include <stdio.h>

BOOL WINAPI wow_native_class_resolve_procedure(DWORD procedure, DWORD *resolved)
{
    WNDCLASSEXA request = {sizeof(request)}, observed = {sizeof(observed)};
    char name[96];
    ATOM atom;
    DWORD saved = GetLastError(), error;
    BOOL result = FALSE;
    if (!resolved) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    if (HIWORD(procedure) != 0xffff) { *resolved = procedure; return TRUE; }
    /* ADAPTER-WOW-071: kernel/class.c needs only pfnClientPrevious. Let
     * native USER validate its own token through a scoped class registration.
     * Register/query cannot dispatch a window procedure; no HWND is created.
     * Thread plus live stack identity separates concurrent/nested operations.
     * A namespace collision fails without touching the existing class. */
    sprintf_s(name, sizeof(name), "NTVDM.ClassAddress.%08lx.%p",
        GetCurrentThreadId(), (void *)&request);
    request.hInstance = GetModuleHandleW(NULL);
    request.lpszClassName = name;
    request.lpfnWndProc = (WNDPROC)procedure;
    atom = RegisterClassExA(&request);
    if (!atom) return FALSE;
    __try {
        result = GetClassInfoExA(request.hInstance, MAKEINTATOM(atom), &observed);
    } __finally {
        error = GetLastError();
        if (!UnregisterClassA(MAKEINTATOM(atom), request.hInstance)) {
            result = FALSE;
            error = GetLastError();
        }
        SetLastError(error);
    }
    if (!result) return FALSE;
    *resolved = (DWORD)observed.lpfnWndProc;
    SetLastError(saved);
    return TRUE;
}

#ifndef OPENNT_HOST_NTRTL_SUBSET_H
NTSYSAPI BOOLEAN NTAPI RtlEqualUnicodeString(const UNICODE_STRING *,
    const UNICODE_STRING *, BOOLEAN);
#endif

/* ADAPTER-WOW-030: native ownership for original CLS.adwWOW's two DWORDs.
 * Original class.c owns lookup/list selection. Public USER has no CLS WOW
 * storage; retain one shared record per registration, not per HWND. Native
 * registration and unregister remain public USER operations. Retained names
 * map only to returned native atoms; no second atom namespace, guest object
 * graph, USER server or task policy is introduced. */
struct wow_class_words_binding {
    wow_class_lookup_entry entry; /* Must remain first. */
    CLS client_identity; /* Immutable atom-only original dialog input. */
    /* Storage lifetime: registration plus borrowers (including window aliases).
     * Not original CLS.cWndReferenceCount; cleanup must not infer live windows
     * or task ownership from this number. */
    LONG references;
    UNICODE_STRING name;
    BOOL has_client_menu;
    wow_window_callback callback;
    BOOL procedure_is_guest;
    SRWLOCK procedure_lock;
    WNDPROC original_native_procedure;
    BOOL native_registration_borrowed;
};

static wow_class_words_binding *prepare(const UNICODE_STRING *name,
    const DWORD words[2])
{
    wow_class_words_binding *binding;
    if (!words || !name || !HIWORD(name->Buffer) ||
            (name->Length & 1) || name->Length > name->MaximumLength) {
        SetLastError(ERROR_INVALID_PARAMETER); return NULL;
    }
    binding = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
        sizeof(*binding) + name->Length);
    if (!binding) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return NULL; }
    binding->references = 1;
    binding->entry.adwWOW[0] = words[0];
    binding->entry.adwWOW[1] = words[1];
    binding->name.Buffer = (WCHAR *)(binding + 1);
    binding->name.Length = binding->name.MaximumLength = name->Length;
    CopyMemory(binding->name.Buffer, name->Buffer, name->Length);
    return binding;
}

static void publish(wow_class_lookup_entry **list,
    wow_class_words_binding *binding, ATOM atom, HANDLE module)
{
    binding->entry.atomClassName = atom;
    binding->client_identity.atomClassName = atom;
    binding->entry.hModule = module;
    /* Original class.c registration's head insertion; list selection belongs
     * to the registration owner, not the mutable CS_GLOBALCLASS style. */
    binding->entry.next = *list;
    *list = &binding->entry;
}

wow_class_words_binding *WINAPI wow_class_words_acquire(
    wow_class_lookup_context *context, ATOM atom, HANDLE module)
{
    wow_class_lookup_entry **link = GetClassPtr(atom, context, module);
    wow_class_words_binding *binding;
    if (!link) { SetLastError(ERROR_CLASS_DOES_NOT_EXIST); return NULL; }
    binding = (wow_class_words_binding *)*link;
    InterlockedIncrement(&binding->references);
    return binding;
}

DWORD *WINAPI wow_class_words_value(wow_class_words_binding *binding)
{
    return binding ? binding->entry.adwWOW : NULL;
}

wow_class_lookup_entry *WINAPI wow_class_words_entry(wow_class_words_binding *binding)
{
    return binding ? &binding->entry : NULL;
}

struct tagCLS *WINAPI wow_class_words_client_identity(wow_class_words_binding *binding)
{
    return binding ? &binding->client_identity : NULL;
}

BOOL WINAPI wow_class_words_publish_guest(wow_class_lookup_entry *entry,
    const WNDCLASSEXA *native_class)
{
    wow_class_words_binding *binding = (wow_class_words_binding *)entry;
    ANSI_STRING name = {0};
    DWORD procedure;
    if (!entry || !native_class || !entry->atomClassName) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    if (entry->guest_server) {
        SetLastError(ERROR_ALREADY_EXISTS); return FALSE;
    }
    /* Preserve original InternalRegisterClassEx's captured-name conversion.
     * Native callback/CPD identities are not guest addresses. USER16's class
     * procedure queries use the retained host thunks, not this sparse field. */
    procedure = (DWORD)entry->lpfnWndProc;
    if (!binding->procedure_is_guest)
        procedure = 0;
    if (RtlUnicodeStringToAnsiString(&name, &binding->name, TRUE) < 0) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY); return FALSE;
    }
    entry->guest_server = mvdm_softpc_wow_page_domain_publish_named_class(
        entry->atomClassName, entry->fnid, entry->flags,
        entry->adwWOW[0], entry->adwWOW[1], entry->hTaskWow,
        native_class->style, procedure, (DWORD)native_class->cbWndExtra,
        0u, name.Buffer, (ULONG)name.Length + 1u);
    RtlFreeAnsiString(&name);
    if (!entry->guest_server) SetLastError(ERROR_INVALID_STATE);
    return entry->guest_server != 0;
}

BOOL WINAPI wow_class_words_native_registration(wow_class_words_binding *binding)
{
    return binding && binding->native_registration_borrowed;
}

BOOL WINAPI wow_class_words_match_native(wow_class_lookup_context *context,
    WNDPROC procedure, BOOL unicode, WORD *fnid, WNDPROC *canonical)
{
    wow_class_lookup_entry *entry;
    DWORD error = GetLastError();
    WORD found = 0;
    WNDPROC target = NULL;
    if (!context || !procedure || !fnid || !canonical) return FALSE;
    for (entry = context->pclsPublicList; entry; entry = entry->next) {
        wow_class_words_binding *binding = (wow_class_words_binding *)entry;
        WNDPROC candidate, recovered;
        WNDCLASSEXW native = {sizeof(native)};
        if (!binding->native_registration_borrowed || !entry->fnid) continue;
        candidate = binding->original_native_procedure;
        recovered = wow_control_client_procedure(entry->fnid, unicode);
        if (recovered) {
            candidate = recovered;
        } else if (entry->fnid == FNID_DIALOG) {
            candidate = unicode ? wow_dialog_window_procW : wow_dialog_window_procA;
        } else if (unicode) {
            if (!GetClassInfoExW(NULL, MAKEINTATOM(entry->atomClassName), &native)) continue;
            candidate = native.lpfnWndProc;
        }
        if (candidate != procedure) continue;
        if (found && found != entry->fnid) { found = 0; break; }
        found = entry->fnid;
        target = wow_control_client_procedure(entry->fnid, FALSE);
        if (!target) target = entry->fnid == FNID_DIALOG ? wow_dialog_window_procA :
            binding->original_native_procedure;
    }
    if (found) { *fnid = found; *canonical = target; }
    SetLastError(error);
    return found != 0;
}

void WINAPI wow_class_words_release(wow_class_words_binding *binding)
{
    if (binding && InterlockedDecrement(&binding->references) == 0)
        HeapFree(GetProcessHeap(), 0, binding);
}

void WINAPI wow_class_words_window_association(wow_class_words_binding *binding,
    BOOL attached)
{
    if (!binding) return;
    /* ADAPTER-WOW-030: original ReferenceClass/DereferenceClass's separate
     * association domain; native USER still owns its class clones/counts. */
    if (attached) InterlockedIncrement(&binding->entry.cWndReferenceCount);
    else InterlockedDecrement(&binding->entry.cWndReferenceCount);
}

static void detach(wow_class_lookup_entry **link)
{
    wow_class_words_binding *binding = (wow_class_words_binding *)*link;
    *link = binding->entry.next;
    binding->entry.next = NULL;
    wow_class_words_release(binding);
}

BOOL WINAPI wow_class_words_import_native(wow_class_lookup_context *context,
    const WNDCLASSEXA *native_class, const UNICODE_STRING *name,
    ATOM atom, WORD fnid, wow_window_callback callback)
{
    const DWORD initial[2] = {0, 0};
    wow_class_words_binding *binding;
    if (!context || !context->client_module || !native_class || !atom || !fnid ||
            !native_class->lpfnWndProc ||
            native_class->lpfnWndProc == wow_window_class_creation_proc ||
            native_class->lpfnWndProc == wow_class_call_original_native) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    if (_InnerGetClassPtr(atom, &context->pclsPublicList, NULL)) {
        SetLastError(ERROR_CLASS_ALREADY_EXISTS); return FALSE;
    }
    binding = prepare(name, initial);
    if (!binding) return FALSE;
    binding->entry.fnid = fnid;
    binding->entry.lpfnWndProc = native_class->lpfnWndProc;
    binding->callback = callback;
    binding->native_registration_borrowed = TRUE;
    binding->original_native_procedure = native_class->lpfnWndProc;
    if (!wow_native_class_resolve_procedure((DWORD)native_class->lpfnWndProc,
            (DWORD *)&binding->entry.lpfnWndProc)) {
        wow_class_words_release(binding); return FALSE;
    }
    /* Original clinit.c selects the client control implementations. The native
     * registration supplies the HWND/class identity, not the control body.
     * Existing logical class queries and WALIAS cache consume this target. */
    if (wow_control_client_procedure(fnid, FALSE)) {
        binding->entry.lpfnWndProc = wow_control_client_procedure(fnid, FALSE);
    }
    if (fnid == FNID_DIALOG) {
        binding->entry.lpfnWndProc = wow_dialog_window_procA;
    }
    /* Original clinit.c RW_RegisterControls registers CS_GLOBALCLASS;
     * init.c's per-process ScrollBar is also public via CSF_SERVERSIDEPROC.
     * Neither is a second system-class registration. Standard WC words and
     * hTaskWow start at zero, not a fabricated Win16 procedure/owner. */
    publish(&context->pclsPublicList, binding, atom,
        native_class->hInstance ? native_class->hInstance : context->client_module);
    return TRUE;
}

BOOL WINAPI wow_class_words_forget_native(wow_class_lookup_context *context)
{
    wow_class_lookup_entry **link, *entry;
    if (!context) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    /* Validate before removing any member. The existing window reference
     * domain, not storage borrower count, determines live associations. */
    for (entry = context->pclsPublicList; entry; entry = entry->next) {
        wow_class_words_binding *binding = (wow_class_words_binding *)entry;
        if (binding->native_registration_borrowed && entry->cWndReferenceCount) {
            SetLastError(ERROR_CLASS_HAS_WINDOWS); return FALSE;
        }
    }
    for (link = &context->pclsPublicList; (entry = *link) != NULL;) {
        wow_class_words_binding *binding = (wow_class_words_binding *)entry;
        if (!binding->native_registration_borrowed) {
            link = &entry->next;
            continue;
        }
        if (entry->guest_server && mvdm_softpc_wow_page_domain_active() &&
                !mvdm_softpc_wow_page_domain_retire_class(entry->guest_server))
            return FALSE;
        entry->guest_server = 0;
        /* The worker owns this metadata, not USER's standard registration.
         * Public UnregisterClass remains exclusively in explicit removal. */
        detach(link);
    }
    return TRUE;
}

ATOM WINAPI wow_class_words_register(wow_class_lookup_context *context,
    const WNDCLASSEXA *native_class, const UNICODE_STRING *name,
    const DWORD words[2], const CLSMENUNAME *menu)
{
    return wow_class_words_register_target(context, native_class, name,
        words, menu, NULL);
}

static DWORD logical_class_procedure(wow_class_words_binding *binding)
{
    DWORD procedure = (DWORD)binding->entry.lpfnWndProc;
    if (procedure == (DWORD)wow_class_call_original_native)
        return (DWORD)binding->original_native_procedure;
    return procedure;
}

LRESULT CALLBACK wow_class_call_original_native(HWND window, UINT message,
    WPARAM wp, LPARAM lp)
{
    wow_window_words_binding *window_words = wow_window_words_acquire(window);
    wow_cleanup_window *view = wow_window_words_cleanup_value(window_words);
    wow_class_words_binding *binding = view ?
        (wow_class_words_binding *)view->pcls : NULL;
    LRESULT result = 0;
    /* Original cleanup stores a callable native target. New WNDs normally
     * snapshot its resolved identity instead; preserve safe dispatch too. */
    __try {
        if (!binding || !binding->original_native_procedure) {
            SetLastError(ERROR_INVALID_STATE); __leave;
        }
        wow_window_note_native_procedure(window, binding->original_native_procedure, FALSE);
        result = CallWindowProcA(binding->original_native_procedure,
            window, message, wp, lp);
    } __finally {
        wow_window_words_release(window_words);
    }
    return result;
}

BOOL WINAPI wow_class_words_target(wow_class_words_binding *binding,
    wow_window_dispatch_target *target)
{
    DWORD procedure;
    if (!binding || !target) return FALSE;
    /* This is original CLS behavior, not a cached native GCL_WNDPROC.
     * Original module cleanup can restore it with zero HWNDs. The native
     * class is only its resource/creation carrier; CBT snapshots this target
     * into each WND's real immutable/native procedure before first dispatch. */
    AcquireSRWLockShared(&binding->procedure_lock);
    procedure = logical_class_procedure(binding);
    target->wow_encoded = binding->procedure_is_guest;
    ReleaseSRWLockShared(&binding->procedure_lock);
    if (!procedure) return FALSE;
    target->procedure = procedure;
    target->callback = binding->callback;
    return TRUE;
}

BOOL WINAPI wow_class_words_exchange_procedure(wow_class_words_binding *binding,
    DWORD procedure, BOOL guest, DWORD *previous, BOOL *previous_guest)
{
    DWORD old, resolved;
    if (!binding || !previous || !previous_guest || !procedure ||
            procedure == (DWORD)wow_class_call_original_native ||
            procedure == (DWORD)wow_window_class_creation_proc ||
            (guest && (!(procedure & WOW_WINDOW_PROC_TAG) || !binding->callback))) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    resolved = procedure;
    if (!guest && !wow_native_class_resolve_procedure(procedure, &resolved)) return FALSE;
    AcquireSRWLockExclusive(&binding->procedure_lock);
    old = logical_class_procedure(binding);
    if (!old) {
        ReleaseSRWLockExclusive(&binding->procedure_lock);
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    *previous = old;
    *previous_guest = binding->procedure_is_guest;
    /* Original class.c::_SetClassData consumes the resolved address only,
     * and records the setter's ANSI entry, unlike GWL_WNDPROC restoration.
     * Do not preserve a source-owned CPD past its native class lifetime. */
    binding->entry.lpfnWndProc = (WNDPROC)resolved;
    binding->procedure_is_guest = guest;
    binding->entry.flags |= CSF_ANSIPROC;
    ReleaseSRWLockExclusive(&binding->procedure_lock);
    return TRUE;
}

void WINAPI wow_class_words_restore_procedure(wow_class_words_binding *binding,
    WNDPROC procedure)
{
    AcquireSRWLockExclusive(&binding->procedure_lock);
    binding->entry.lpfnWndProc = procedure;
    binding->procedure_is_guest = FALSE;
    ReleaseSRWLockExclusive(&binding->procedure_lock);
}

ATOM WINAPI wow_class_words_register_target(wow_class_lookup_context *context,
    const WNDCLASSEXA *native_class, const UNICODE_STRING *name,
    const DWORD words[2], const CLSMENUNAME *menu,
    const wow_window_dispatch_target *target)
{
    wow_class_words_binding *binding;
    ATOM atom;
    DWORD error;
    if (!context || !native_class || !native_class->hInstance) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    /* The original register owner has already attempted CPD validation.
     * An unresolved encoded selector7fff is still a WOW procedure. */
    if (target && (!target->procedure ||
            target->procedure == (DWORD)wow_class_call_original_native ||
            (target->wow_encoded && target->procedure == (DWORD)native_class->lpfnWndProc) ||
            (target->wow_encoded && (!(target->procedure & WOW_WINDOW_PROC_TAG) ||
                !target->callback)))) {
        SetLastError(ERROR_INVALID_PARAMETER); return 0;
    }
    binding = prepare(name, words);
    if (!binding) return 0;
    {
        wow_standard_class_view standard;
        DWORD saved_error = GetLastError();
        /* Original cleanup matches the class atom, including a private
         * application class shadowing a standard name. Capture USER's
         * standard identity before native registration, not the app target.
         * Classification still belongs to original walias.c's one table. */
        if (wow_standard_class_describe(native_class->lpszClassName, &standard))
            binding->original_native_procedure = standard.native.lpfnWndProc;
        SetLastError(saved_error);
    }
    if (target) {
        binding->entry.lpfnWndProc = (WNDPROC)target->procedure;
        binding->callback = target->callback;
        binding->procedure_is_guest = target->wow_encoded;
    } else binding->entry.lpfnWndProc = native_class->lpfnWndProc;
    /* Original CLS retains the two client names, not pcmn->pusMenuName.
     * Failure frees metadata only: original RegisterClassExWOW owns cleanup. */
    if (menu) {
        binding->entry.lpszClientAnsiMenuName = menu->pszClientAnsiMenuName;
        binding->entry.lpszClientUnicodeMenuName = menu->pwszClientUnicodeMenuName;
        binding->has_client_menu = TRUE;
    }
    atom = RegisterClassExA(native_class);
    error = GetLastError();
    if (atom) publish((native_class->style & CS_GLOBALCLASS) ?
        &context->pclsPublicList : &context->pclsPrivateList,
        binding, atom, native_class->hInstance);
    else wow_class_words_release(binding);
    SetLastError(error);
    return atom;
}

ATOM WINAPI wow_class_words_find_atom(wow_class_lookup_context *context,
    const UNICODE_STRING *name)
{
    wow_class_lookup_entry *lists[3], *entry;
    unsigned i;
    if (!context || !name) { SetLastError(ERROR_INVALID_PARAMETER); return 0; }
    /* Original FindClassAtom accepts an integer identity without a string
     * dereference. String identity is case-insensitive in original RTL atoms;
     * use the native RTL Unicode comparison, not a CRT locale or ANSI fold. */
    if (!HIWORD(name->Buffer)) return LOWORD(name->Buffer);
    if ((name->Length & 1) || name->Length > name->MaximumLength) {
        SetLastError(ERROR_INVALID_PARAMETER); return 0;
    }
    lists[0] = context->pclsPrivateList;
    lists[1] = context->pclsPublicList;
    lists[2] = context->system_classes;
    for (i = 0; i < 3; ++i) {
        for (entry = lists[i]; entry; entry = entry->next) {
            wow_class_words_binding *binding = (wow_class_words_binding *)entry;
            char path[MAX_PATH];
            if (GetEnvironmentVariableA("MVDM_WOW_CLASS_TRACE_PATH", path,
                    sizeof(path))) {
                char line[192], query[64] = "", stored[64] = "";
                HANDLE file;
                DWORD bytes, written, saved = GetLastError();
                (void)WideCharToMultiByte(CP_ACP, 0, name->Buffer,
                    name->Length / sizeof(WCHAR), query, sizeof(query) - 1, NULL, NULL);
                (void)WideCharToMultiByte(CP_ACP, 0, binding->name.Buffer,
                    binding->name.Length / sizeof(WCHAR), stored, sizeof(stored) - 1, NULL, NULL);
                file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
                    NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
                if (file != INVALID_HANDLE_VALUE) {
                    bytes = (DWORD)sprintf_s(line, sizeof(line),
                        "%lu ClassCompare query=%s/%u stored=%s/%u atom=%04X equal=%u\r\n",
                        (unsigned long)GetCurrentProcessId(), query, (unsigned)name->Length,
                        stored, (unsigned)binding->name.Length, (unsigned)entry->atomClassName,
                        (unsigned)RtlEqualUnicodeString(&binding->name, name, TRUE));
                    if (bytes) (void)WriteFile(file, line, bytes, &written, NULL);
                    CloseHandle(file);
                }
                SetLastError(saved);
            }
            if (RtlEqualUnicodeString(&binding->name, name, TRUE))
                return entry->atomClassName;
        }
    }
    SetLastError(ERROR_CLASS_DOES_NOT_EXIST);
    return 0;
}

BOOL WINAPI wow_class_words_destroy_native(wow_class_lookup_entry **link,
    HINSTANCE module, BOOL menu_output_available)
{
    wow_class_words_binding *binding = (wow_class_words_binding *)*link;
    DWORD error;
    if (binding->has_client_menu && !menu_output_available) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    if (!UnregisterClassA(MAKEINTATOM(binding->entry.atomClassName), module)) return FALSE;
    error = GetLastError();
    if (binding->entry.guest_server &&
            !mvdm_softpc_wow_page_domain_release_class(binding->entry.guest_server)) {
        /* Default-off evidence at the failed release, not a retry or waiver. */
        char path[MAX_PATH], line[192];
        if (GetEnvironmentVariableA("MVDM_WOW_WINDOW_TRACE_PATH", path, sizeof(path))) {
            DWORD written;
            HANDLE file = CreateFileA(path, FILE_APPEND_DATA,
                FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, 0, NULL);
            if (file != INVALID_HANDLE_VALUE) {
                int bytes = sprintf_s(line, sizeof(line),
                    "%lu ClassRetirement atom=%04X server=%08lX live=%lu refs=%ld native-unregistered=1\r\n",
                    GetCurrentProcessId(), binding->entry.atomClassName,
                    binding->entry.guest_server, binding->entry.cWndReferenceCount,
                    binding->references);
                if (bytes > 0) (void)WriteFile(file, line, (DWORD)bytes, &written, NULL);
                CloseHandle(file);
            }
        }
        wow_user_native_call_failed("release-class-after-native-unregister",
            ERROR_INVALID_STATE);
    }
    binding->entry.guest_server = 0u;
    binding->entry.lpszClientAnsiMenuName = NULL;
    binding->entry.lpszClientUnicodeMenuName = NULL;
    detach(link);
    SetLastError(error);
    return TRUE;
}

static BOOL WINAPI destroy_native(wow_class_remove_context *context,
    wow_class_lookup_entry **link)
{
    return wow_class_words_destroy_native(link, context->module,
        context->menu_output_available);
}

BOOL WINAPI wow_class_words_unregister(wow_class_lookup_context *classes,
    ATOM atom, HINSTANCE module, PCLSMENUNAME menu)
{
    wow_class_remove_context context;
    CLSMENUNAME result;
    if (!classes) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    context.ppi=classes; context.module=module;
    context.name.Buffer=(LPWSTR)MAKEINTATOM(atom);
    context.name.Length=context.name.MaximumLength=0;
    context.menu_output_available=menu!=NULL; context.destroy=destroy_native;
    /* The server's output is not published to its client on native failure. */
    if (!_UnregisterClass(context.name.Buffer,module,&result,&context)) return FALSE;
    if (menu) *menu=result;
    return TRUE;
}
