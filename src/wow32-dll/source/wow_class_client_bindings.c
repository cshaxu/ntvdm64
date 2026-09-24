#include "wow_class_client_bindings.h"
#include "wow_window_words_binding.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_wow_page_domain.h"
#include <stdio.h>

ATOM WINAPI wow_original_register_class(WNDCLASSA *, DWORD *);
BOOL WINAPI wow_original_unregister_class(LPCSTR, HINSTANCE);
extern unsigned short __cdecl getCS(void);
extern unsigned short __cdecl getIP(void);
static __declspec(thread) wow_class_client_context *current;

wow_class_client_context *WINAPI wow_class_client_current(void) { return current; }

static BOOL valid(wow_class_client_context *context)
{
    if (context && context->classes && context->get_version && context->callback && context->thread)
        return TRUE;
    SetLastError(ERROR_INVALID_STATE);
    return FALSE;
}

ATOM WINAPI wow_class_client_register(wow_class_client_context *context,
    WNDCLASSA *wc, DWORD *words)
{
    wow_class_client_context *previous;
    ATOM result;
    if (!valid(context) || !wc || !words) return 0;
    previous = current; current = context;
    __try { result = wow_original_register_class(wc, words); }
    __finally { current = previous; }
    return result;
}

BOOL WINAPI wow_class_client_unregister(wow_class_client_context *context,
    LPCSTR name, HINSTANCE module)
{
    wow_class_client_context *previous;
    BOOL result;
    if (!valid(context)) return FALSE;
    previous = current; current = context;
    __try { result = wow_original_unregister_class(name, module); }
    __finally { current = previous; }
    return result;
}

typedef struct class_publication {
    wow_class_client_context *client;
    PUNICODE_STRING name;
    PCLSMENUNAME menu;
} class_publication;

/* A default-off witness for the original WU32RegisterClass hand-off.  It
 * records the already-encoded procedure and the exact value passed
 * to the native class publication; it is not part of class selection or
 * lifetime. */
static void trace_publication(LPCSTR name, DWORD procedure, DWORD tagged,
    HINSTANCE instance)
{
    char path[MAX_PATH], line[256];
    HANDLE file;
    DWORD written, bytes;

    if (!GetEnvironmentVariableA("MVDM_WOW_CLASS_PUBLICATION_TRACE_PATH", path,
            sizeof(path))) return;
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ |
        FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return;
    bytes = (DWORD)sprintf_s(line, sizeof(line),
        "%lu ClassPublication name=%s procedure=%08lX tagged=%08lX instance=%p cpu=%04X:%04X\\r\\n",
        (unsigned long)GetCurrentProcessId(), name ? name : "<null>",
        (unsigned long)procedure, (unsigned long)tagged,
        instance,
        (unsigned)getCS(), (unsigned)getIP());
    if (bytes) (void)WriteFile(file, line, bytes, &written, NULL);
    CloseHandle(file);
}

static wow_class_callproc *WINAPI resolve_callproc(wow_class_register_context *context,
    HANDLE handle)
{
    class_publication *publication = context->binding;
    return publication->client->resolve_callproc(handle);
}

static wow_class_lookup_entry *WINAPI publish_native(wow_class_register_context *context,
    WNDCLASSEXA *wc, WORD fnid, DWORD flags)
{
    class_publication *publication = context->binding;
    wow_class_lookup_context *classes = publication->client->classes;
    WNDCLASSEXA native_class = *wc;
    wow_window_dispatch_target target;
    DWORD initial[2] = {0,0};
    ATOM atom;
    wow_class_lookup_entry *entry;
    /* Preserve wuclass.c's exact 16-bit procedure encoding.  The original
     * USER client dispatches WNDPROC_WOW through pfnWowWndProcEx; without the
     * tag a valid guest far address is mistaken for a native host function. */
    /* WU32RegisterClass has already encoded both the WOW tag and the
     * virtual high selector bit. Encoding again corrupts low selectors. */
    target.procedure = (DWORD)wc->lpfnWndProc;
    trace_publication(wc->lpszClassName, (DWORD)wc->lpfnWndProc,
        target.procedure, wc->hInstance);
    target.callback = publication->client->callback;
    native_class.lpfnWndProc = wow_window_native_proc;
    /* Native USER cannot load resources from a packed WOW module. Original
     * client names remain owned by the class; creation must load through
     * the recovered resource client, not native automatic menu loading. */
    native_class.lpszMenuName = NULL;
    atom = wow_class_words_register_target(classes, &native_class,
        publication->name, initial, publication->menu, &target);
    if (!atom) return NULL;
    /* Existing native publication prepends to the original selected list. */
    entry = (native_class.style & CS_GLOBALCLASS) ?
        classes->pclsPublicList : classes->pclsPrivateList;
    if (!entry) {
        SetLastError(ERROR_INVALID_STATE);
        return NULL;
    }
    entry->fnid = fnid;
    entry->flags = flags;
    entry->hTaskWow = publication->client->thread->ptdb ?
        publication->client->thread->ptdb->hTaskWow : 0u;
    entry->guest_server = mvdm_softpc_wow_page_domain_publish_class(atom, fnid,
        entry->flags, entry->adwWOW[0], entry->adwWOW[1], entry->hTaskWow,
        native_class.style, (DWORD)(ULONG_PTR)entry->lpfnWndProc,
        (DWORD)native_class.cbWndExtra, 0u);
    if (!entry->guest_server) {
        wow_class_lookup_entry **link = (native_class.style & CS_GLOBALCLASS) ?
            &classes->pclsPublicList : &classes->pclsPrivateList;

        (void)wow_class_words_destroy_native(link, native_class.hInstance,
            TRUE);
        SetLastError(ERROR_INVALID_STATE);
        return NULL;
    }
    return entry;
}

ATOM WINAPI wow_class_client_publish(WNDCLASSEXA *wc, PUNICODE_STRING name,
    PCLSMENUNAME menu, PROC worker, WORD fnid, DWORD flags, DWORD *words)
{
    class_publication publication;
    wow_class_register_context context;
    /* Preserve the selected ANSI entry profile. Do not fabricate a CPD owner. */
    if (!current || !current->thread || worker || fnid || flags != 2 ||
            (HIWORD(wc->lpfnWndProc) == 0xffff && !current->resolve_callproc)) {
        SetLastError(ERROR_INVALID_PARAMETER); return 0;
    }
    publication.client=current; publication.name=name; publication.menu=menu;
    context.thread=current->thread; context.register_class=publish_native;
    context.resolve_callproc=resolve_callproc; context.binding=&publication;
    return _RegisterClassEx(wc,worker,menu,fnid,flags,words,&context);
}

BOOL WINAPI wow_class_client_remove(PUNICODE_STRING name, HINSTANCE module,
    PCLSMENUNAME menu)
{
    ATOM atom = wow_class_words_find_atom(current->classes, name);
    return wow_class_words_unregister(current->classes, atom, module, menu);
}
