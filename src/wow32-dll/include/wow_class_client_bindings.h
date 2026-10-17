#ifndef WOW_CLASS_CLIENT_BINDINGS_H
#define WOW_CLASS_CLIENT_BINDINGS_H
#include "wow_class_register_bindings.h"

/* ADAPTER-WOW-038: caller supplies InitTask's task version and the distinct
 * original module-version callback. Do not derive one from the other: original
 * ntcftxt.h uses task version for USER hInstance validation, module version
 * for style/brush/return compatibility. The caller serializes its class context, including
 * nested callbacks; this is not a scheduler or a default version policy. */
typedef struct wow_class_client_context {
    wow_class_lookup_context *classes;
    DWORD dwExpWinVer;
    DWORD (WINAPI *get_version)(HANDLE);
    wow_window_callback callback;
    wow_task_order_thread *thread;
    /* Optional scoped provider override. NULL uses the native class owner. */
    wow_class_callproc *(WINAPI *resolve_callproc)(HANDLE);
} wow_class_client_context;
ATOM WINAPI wow_class_client_register(wow_class_client_context *, WNDCLASSA *, DWORD *);
/* WU32RegisterClass's callback supplies an already encoded guest procedure,
 * not a native CPD candidate. Keep that origin through original registration. */
ATOM WINAPI wow_class_client_register_guest(wow_class_client_context *, WNDCLASSA *, DWORD *);
BOOL WINAPI wow_class_client_unregister(wow_class_client_context *, LPCSTR, HINSTANCE);
BOOL WINAPI wow_class_client_query(wow_class_client_context *, HINSTANCE,
    LPCSTR, WNDCLASSA *);

#ifdef WOW_ORIGINAL_CLASS_CLIENT
#include <tchar.h>
#include "wow_user_capture_bindings.h"
#include "opennt-host/windows/core/ntuser/inc/ntsend.h"
wow_class_client_context *WINAPI wow_class_client_current(void);
ATOM WINAPI wow_class_client_publish(WNDCLASSEXA *, PUNICODE_STRING,
    PCLSMENUNAME, PROC, WORD, DWORD, DWORD *);
BOOL WINAPI wow_class_client_remove(PUNICODE_STRING, HINSTANCE, PCLSMENUNAME);
BOOL WINAPI wow_class_client_query_native(HINSTANCE, PUNICODE_STRING,
    WNDCLASSEXW *, LPWSTR *, BOOL);
int MBToWCSEx(WORD, LPCSTR, int, LPWSTR *, int, BOOL);
#define TEXT_FN(x) x##A
#define GetClientInfo() wow_class_client_current()
#define GETEXPWINVER(module) (wow_class_client_current()->get_version(module))
#define hmodUser GetModuleHandleW(L"user32.dll")
#define VER30 0x0300
#define VER31 0x030a
#define VER40 0x0400
#define CS_VALID40 0x0801feeb
#define FNID_DDE_BIT 0x2000
#define COLOR_MAX (COLOR_INFOBK + 1)
#define RIPMSG1(level,text,arg) ((void)0)
#define RIPERR0(code,level,text) SetLastError(code)
#define GdiValidateHandle(handle) (GetObjectType(handle) != 0)
#define MBToWCS(text,count,out,cap,alloc) MBToWCSEx(0,text,count,out,cap,alloc)
#define NtUserRegisterClassExWOW wow_class_client_publish
#define NtUserUnregisterClass wow_class_client_remove
#define NtUserGetClassInfo wow_class_client_query_native
#define IS_ANSI TRUE
#undef GetClassInfo
#define GetClassInfo wow_original_get_class_info
#undef UnregisterClass
#define UnregisterClass wow_original_unregister_class
#define RegisterClassWOWA wow_original_register_class
#endif
#endif
