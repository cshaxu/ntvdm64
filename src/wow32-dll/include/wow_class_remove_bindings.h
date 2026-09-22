#ifndef WOW_CLASS_REMOVE_BINDINGS_H
#define WOW_CLASS_REMOVE_BINDINGS_H
#include "wow_class_words_binding.h"
/* ADAPTER-WOW-045: finite original process/list context and native destruction.
 * Native USER may reject unregistration for windows outside our enrollment;
 * its failure must propagate without transferring client menu ownership. */
typedef struct wow_class_remove_context {
    wow_class_lookup_context *ppi;
    HINSTANCE module;
    UNICODE_STRING name;
    BOOL menu_output_available;
    BOOL (WINAPI *destroy)(struct wow_class_remove_context *, wow_class_lookup_entry **);
} wow_class_remove_context;
BOOL WINAPI _UnregisterClass(LPWSTR,HANDLE,PCLSMENUNAME,wow_class_remove_context *);
#ifdef WOW_ORIGINAL_CLASS_REMOVAL
typedef wow_class_lookup_entry **PPCLS;
typedef wow_class_remove_context *PTHREADINFO;
#define PtiCurrent() context
#define CheckCritIn() ((void)0)
#define FindClassAtom(rawName) ((void)(rawName), wow_class_words_find_atom(context->ppi,&context->name))
#define DestroyClass(link) context->destroy(context,link)
#define RIPERR0(error, level, message) SetLastError(error)
#endif
#endif
