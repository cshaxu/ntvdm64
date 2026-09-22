#ifndef WOW_CLASS_REGISTER_BINDINGS_H
#define WOW_CLASS_REGISTER_BINDINGS_H
#include "wow_class_words_binding.h"
#include "wow_task_order_bindings.h"

/* ADAPTER-WOW-044: original registration state, not a second class registry.
 * Both callbacks are required; a CPD is resolved by its actual owner or fails.
 * Native registration supplies an existing owned entry in the class lists. */
typedef struct wow_class_callproc { ULONG_PTR pfnClientPrevious; } wow_class_callproc;
typedef struct wow_class_register_context {
    wow_task_order_thread *thread;
    wow_class_lookup_entry *(WINAPI *register_class)(struct wow_class_register_context *, WNDCLASSEXA *, WORD, DWORD);
    wow_class_callproc *(WINAPI *resolve_callproc)(struct wow_class_register_context *, HANDLE);
    PVOID binding;
} wow_class_register_context;
ATOM WINAPI _RegisterClassEx(WNDCLASSEXA *, PROC, PCLSMENUNAME, WORD, DWORD,
    DWORD *, wow_class_register_context *);
#ifdef WOW_ORIGINAL_CLASS_REGISTRATION
typedef wow_class_lookup_entry *PCLS;
typedef WNDCLASSEXA *LPWNDCLASSEX;
typedef wow_task_order_thread *PTHREADINFO;
typedef wow_class_callproc *PCALLPROCDATA;
#define PtiCurrent() (context->thread)
#define ISCPDTAG(proc) (HIWORD(proc) == 0xffff)
#define HMValidateHandleNoRip(handle, type) context->resolve_callproc(context,handle)
#define InternalRegisterClassEx(wc, fnid, flags) context->register_class(context,wc,fnid,flags)
#define TIF_16BIT WOW_TASK_TIF_16BIT
#endif
#endif
