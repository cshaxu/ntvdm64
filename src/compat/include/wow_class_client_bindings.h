#ifndef WOW_CLASS_CLIENT_BINDINGS_H
#define WOW_CLASS_CLIENT_BINDINGS_H
#include "wow_class_register_bindings.h"
#include "wow_class_remove_bindings.h"

/* ADAPTER-WOW-038: caller supplies InitTask's task version and the distinct
 * original module-version callback. Do not derive one from the other: original
 * ntcftxt.h uses task version for USER hInstance validation, module version
 * for style/brush/return compatibility. The caller serializes its class context, including
 * nested callbacks; this is not a scheduler or a default version policy. */
typedef struct wow_class_client_context {
    wow_class_lookup_context *classes;
    DWORD (WINAPI *get_version)(HANDLE);
    wow_window_callback callback;
    wow_task_order_thread *thread;
    /* Optional scoped provider override. NULL uses the native class owner. */
    wow_class_callproc *(WINAPI *resolve_callproc)(HANDLE);
} wow_class_client_context;
/* Original-layout storage uses the existing registration allocation and
 * borrow lifetime. No private CLS layout or menu descriptor enters B. */
VOID WINAPI wow_original_class_initialize_identity(struct tagCLS *, ATOM);
VOID WINAPI wow_original_class_capture_menu(const struct tagCLSMENUNAME *, wow_class_lookup_entry *);
BOOL WINAPI wow_original_class_unregister_committed(LPCWSTR, HANDLE,
    PCLSMENUNAME, wow_class_remove_context *);
typedef struct wow_class_client_provider {
    ATOM (WINAPI *register_class)(WNDCLASSA *, DWORD *);
    BOOL (WINAPI *unregister_class)(LPCSTR, HINSTANCE);
    BOOL (WINAPI *get_class_info)(HINSTANCE, LPCSTR, WNDCLASSA *);
    ATOM (WINAPI *register_class_ex)(WNDCLASSEXA *, PCLSMENUNAME, WORD, DWORD,
        DWORD *, wow_class_register_context *);
    wow_class_lookup_entry **(WINAPI *get_class_ptr)(ATOM, wow_class_lookup_context *, HANDLE);
    wow_class_lookup_entry **(WINAPI *inner_get_class_ptr)(ATOM, wow_class_lookup_entry **, HANDLE);
    BOOL (WINAPI *unregister_class_internal)(LPCWSTR, HANDLE, PCLSMENUNAME, wow_class_remove_context *);
    SIZE_T identity_bytes;
    WORD dialog_fnid;
    VOID (WINAPI *initialize_identity)(struct tagCLS *, ATOM);
    VOID (WINAPI *capture_menu)(const struct tagCLSMENUNAME *, wow_class_lookup_entry *);
} wow_class_client_provider;
BOOL WINAPI wow_class_client_bind(const wow_class_client_provider *);
const wow_class_client_provider *WINAPI wow_class_client_provider_current(void);
ATOM WINAPI wow_class_client_register(wow_class_client_context *, WNDCLASSA *, DWORD *);
/* WU32RegisterClass's callback supplies an already encoded guest procedure,
 * not a native CPD candidate. Keep that origin through original registration. */
ATOM WINAPI wow_class_client_register_guest(wow_class_client_context *, WNDCLASSA *, DWORD *);
BOOL WINAPI wow_class_client_unregister(wow_class_client_context *, LPCSTR, HINSTANCE);
BOOL WINAPI wow_class_client_query(wow_class_client_context *, HINSTANCE,
    LPCSTR, WNDCLASSA *);

#endif
