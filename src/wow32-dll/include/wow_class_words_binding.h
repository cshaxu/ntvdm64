#ifndef WOW_CLASS_WORDS_BINDING_H
#define WOW_CLASS_WORDS_BINDING_H
#include "wow_class_lookup_bindings.h"
#include <winternl.h>
#include "wow_window_dispatch_bindings.h"

typedef struct wow_class_words_binding wow_class_words_binding;
/* Same borrowed lifetime as class words; no new lookup or copied entry. */
wow_class_lookup_entry *WINAPI wow_class_words_entry(wow_class_words_binding *);
/* Bottom native release for an already selected, locked class link. Cleanup
 * must not repeat ordinary GetClassPtr, which excludes deferred classes.
 * TRUE in the last argument means the caller has accounted for client menu
 * names before release; failure preserves the link and all metadata. */
BOOL WINAPI wow_class_words_destroy_native(wow_class_lookup_entry **,
    HINSTANCE, BOOL);
/* Original windows/core/ntuser/inc/user.h declaration. The captured Unicode
 * descriptor is call-local; only the two client pointers survive register. */
typedef struct tagCLSMENUNAME
{
    LPSTR               pszClientAnsiMenuName;
    LPWSTR              pwszClientUnicodeMenuName;
    PUNICODE_STRING     pusMenuName;
} CLSMENUNAME, *PCLSMENUNAME;
/* Caller holds its class-publication lock across all list operations.
 * Original server registration can publish hTaskWow. cWndReferenceCount tracks enrolled window
 * associations; acquire/release independently counts storage borrowers.
 * Register receives an already validated native WNDCLASSEXA, not a tagged
 * guest procedure. Original client policy/version/capture runs before this
 * boundary. All context entries must originate from register; do not mix raw
 * lookup views with owned bindings. No raw words survive release. */
ATOM WINAPI wow_class_words_register(wow_class_lookup_context *,
    const WNDCLASSEXA *, const UNICODE_STRING *, const DWORD words[2],
    const CLSMENUNAME *);
/* Gateway class variant: native_class already names the native entrance;
 * target preserves original CLS.lpfnWndProc and its registered WOW callback.
 * No CPD resolution or guest selector conversion takes place here. */
ATOM WINAPI wow_class_words_register_target(wow_class_lookup_context *,
    const WNDCLASSEXA *, const UNICODE_STRING *, const DWORD words[2],
    const CLSMENUNAME *, const wow_window_dispatch_target *);
BOOL WINAPI wow_class_words_target(wow_class_words_binding *,
    wow_window_dispatch_target *);
/* Snapshot/replace only an enrolled class's raw target. The HWND gateway
 * boundary excludes recursion; CPD conversion stays in public USER policy.
 * Callback identity is immutable. Neither operation changes existing WNDs. */
BOOL WINAPI wow_class_words_exchange_procedure(wow_class_words_binding *,
    DWORD, DWORD *);
/* Name is the original client's captured Unicode class name. This maps
 * retained names to native class atoms, never allocates another atom ID. */
ATOM WINAPI wow_class_words_find_atom(wow_class_lookup_context *,
    const UNICODE_STRING *);
BOOL WINAPI wow_class_words_unregister(wow_class_lookup_context *, ATOM,
    HINSTANCE, PCLSMENUNAME);
/* Menu pointers transfer on successful register only. Successful unregister
 * returns them for original client cleanup, never a captured stack descriptor.
 * Failure leaves ownership and the output unchanged. Registrations without
 * a menu carrier may use NULL output; otherwise output is required. */
wow_class_words_binding *WINAPI wow_class_words_acquire(
    wow_class_lookup_context *, ATOM, HANDLE);
DWORD *WINAPI wow_class_words_value(wow_class_words_binding *);
void WINAPI wow_class_words_release(wow_class_words_binding *);
/* Called only when publishing/removing a window's association, while that
 * window's property-publication lock is held. NULL means no enrolled class. */
void WINAPI wow_class_words_window_association(wow_class_words_binding *, BOOL);
#endif
