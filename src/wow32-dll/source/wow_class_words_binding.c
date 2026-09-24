#include "wow_class_words_binding.h"
#include "wow_class_remove_bindings.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_wow_page_domain.h"

#include <stdio.h>

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
    /* Storage lifetime: registration plus borrowers (including window aliases).
     * Not original CLS.cWndReferenceCount; cleanup must not infer live windows
     * or task ownership from this number. */
    LONG references;
    UNICODE_STRING name;
    BOOL has_client_menu;
    wow_window_callback callback;
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

ATOM WINAPI wow_class_words_register(wow_class_lookup_context *context,
    const WNDCLASSEXA *native_class, const UNICODE_STRING *name,
    const DWORD words[2], const CLSMENUNAME *menu)
{
    return wow_class_words_register_target(context, native_class, name,
        words, menu, NULL);
}

BOOL WINAPI wow_class_words_target(wow_class_words_binding *binding,
    wow_window_dispatch_target *target)
{
    DWORD procedure;
    if (!binding || !target) return FALSE;
    procedure = (DWORD)InterlockedCompareExchange(
        (LONG *)&binding->entry.lpfnWndProc, 0, 0);
    if (!procedure) return FALSE;
    target->procedure = procedure;
    target->callback = binding->callback;
    return TRUE;
}

BOOL WINAPI wow_class_words_exchange_procedure(wow_class_words_binding *binding,
    DWORD procedure, DWORD *previous)
{
    if (!binding || !previous || !binding->entry.lpfnWndProc || !procedure ||
            HIWORD(procedure) == 0xffff ||
            ((procedure & 0x80000000) && !binding->callback)) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    *previous = (DWORD)InterlockedExchange(
        (LONG *)&binding->entry.lpfnWndProc, (LONG)procedure);
    return TRUE;
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
    /* Original WNDPROC_HANDLE (FFFF) is a CPD, not a callable WOW target.
     * Original WNDPROC_WOW requires the registered WOW callback. */
    if (target && (!target->procedure ||
            target->procedure == (DWORD)native_class->lpfnWndProc ||
            HIWORD(target->procedure) == 0xffff ||
            ((target->procedure & 0x80000000) && !target->callback))) {
        SetLastError(ERROR_INVALID_PARAMETER); return 0;
    }
    binding = prepare(name, words);
    if (!binding) return 0;
    if (target) {
        binding->entry.lpfnWndProc = (WNDPROC)target->procedure;
        binding->callback = target->callback;
    }
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
            !mvdm_softpc_wow_page_domain_retire_class(binding->entry.guest_server)) {
        SetLastError(ERROR_INVALID_STATE);
        return FALSE;
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
