#include "wow_class_client_bindings.h"
#include "wow_window_words_binding.h"

ATOM WINAPI wow_original_register_class(WNDCLASSA *, DWORD *);
BOOL WINAPI wow_original_unregister_class(LPCSTR, HINSTANCE);
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
    (void)fnid; (void)flags; /* Validated before entering original wrapper. */
    target.procedure = (DWORD)wc->lpfnWndProc;
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
    return (native_class.style & CS_GLOBALCLASS) ?
        classes->pclsPublicList : classes->pclsPrivateList;
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
