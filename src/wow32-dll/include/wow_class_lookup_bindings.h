#ifndef WOW_CLASS_LOOKUP_BINDINGS_H
#define WOW_CLASS_LOOKUP_BINDINGS_H
#include <windows.h>

/* ADAPTER-WOW-028: finite native metadata view for original class.c lookup.
 * Not a kernel CLS/PROCESSINFO layout and never published to guest memory.
 * Registration owns list membership; mutable public class style is not a key.
 * The caller owns synchronization and lifetime of every entry and list link. */
typedef struct wow_class_lookup_entry {
    struct wow_class_lookup_entry *next; /* Original ppcls = (PPCLS)*ppcls. */
    ATOM atomClassName;
    WORD fnid;
    HANDLE hModule;
    DWORD flags;
    LONG cWndReferenceCount; /* Enrolled WND associations, not alias borrows. */
    DWORD hTaskWow;
    PROC lpfnWorker;
    WNDPROC lpfnWndProc; /* Enrolled logical target; native gateway stays native. */
    LPSTR lpszClientAnsiMenuName;
    LPWSTR lpszClientUnicodeMenuName;
    DWORD adwWOW[2];
    /* Server-form address of the worker's fixed-width original CLS
     * projection. This is a guest numeric value, never a native CLS alias. */
    ULONG guest_server;
} wow_class_lookup_entry;

typedef struct wow_class_lookup_context {
    wow_class_lookup_entry *pclsPrivateList;
    wow_class_lookup_entry *pclsPublicList;
    wow_class_lookup_entry *system_classes;
    HANDLE client_module;
} wow_class_lookup_context;

#define CSF_WOWDEFERDESTROY 0x00000004 /* Original user.h. */
wow_class_lookup_entry **WINAPI _InnerGetClassPtr(ATOM,
    wow_class_lookup_entry **, HANDLE);
wow_class_lookup_entry **WINAPI GetClassPtr(ATOM,
    wow_class_lookup_context *, HANDLE);

#ifdef WOW_ORIGINAL_CLASS_LOOKUP
typedef wow_class_lookup_entry **PPCLS;
typedef wow_class_lookup_context *PPROCESSINFO;
/* Bind original per-process/system roots to this worker's supplied context;
 * do not create global USER process records or a second native resolver. */
#define gpclsList (ppi->system_classes)
#define hModClient (ppi->client_module)
#endif
#endif
