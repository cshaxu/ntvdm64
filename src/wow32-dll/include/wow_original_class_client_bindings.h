#ifndef WOW_ORIGINAL_CLASS_CLIENT_BINDINGS_H
#define WOW_ORIGINAL_CLASS_CLIENT_BINDINGS_H
#include "compat/include/wow_class_client_bindings.h"
#include "wow_user_capture_bindings.h"
wow_class_client_context *WINAPI wow_class_client_current(void);
ATOM WINAPI wow_class_client_publish(WNDCLASSEXA *, PUNICODE_STRING,
    PCLSMENUNAME, PROC, WORD, DWORD, DWORD *);
BOOL WINAPI wow_class_client_remove(PUNICODE_STRING, HINSTANCE, PCLSMENUNAME);
BOOL WINAPI wow_class_client_query_native(HINSTANCE, PUNICODE_STRING,
    WNDCLASSEXW *, LPWSTR *, BOOL);
#include "compat/include/wow_multibyte.h"
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
#define NtUserRegisterClassExWOW wow_class_client_publish
#define NtUserUnregisterClass wow_class_client_remove
#define NtUserGetClassInfo wow_class_client_query_native
#undef GetClassInfo
#define GetClassInfo wow_original_get_class_info
#undef UnregisterClass
#define UnregisterClass wow_original_unregister_class
#define RegisterClassWOWA wow_original_register_class
#endif
