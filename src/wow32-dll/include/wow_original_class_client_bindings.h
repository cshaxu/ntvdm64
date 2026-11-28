#ifndef WOW_ORIGINAL_CLASS_CLIENT_BINDINGS_H
#define WOW_ORIGINAL_CLASS_CLIENT_BINDINGS_H
#include "compat/include/wow_class_client_bindings.h"
#include "nt.h"
#include "wow_edit_lookaside_bindings.h"
#include "wow_user_capture_bindings.h"
wow_class_client_context *WINAPI wow_class_client_current(void);
ATOM WINAPI wow_class_client_publish(WNDCLASSEXA *, PUNICODE_STRING,
    PUNICODE_STRING, PCLSMENUNAME, WORD, DWORD, DWORD *);
BOOL WINAPI wow_class_client_remove(PUNICODE_STRING, HINSTANCE, PCLSMENUNAME);
BOOL WINAPI wow_class_client_query_native(HINSTANCE, PUNICODE_STRING,
    WNDCLASSEXW *, LPWSTR *, BOOL);
#include "compat/include/wow_multibyte.h"
#include "compat/include/wow_task_init_bindings.h"
#include "sxstypes.h"
UINT WINAPI wow_class_client_atom_name(ATOM, PUNICODE_STRING);
#define NtUserGetAtomName wow_class_client_atom_name
#define MAX_ATOM_LEN 256
#define UserAssert(value) ((void)0)
#define FUNCLOG2(...)
#define FUNCLOG3(...)
#define CW_FLAGS_VERSIONCLASS 0x40000000
#define TIF_16BIT WOW_TASK_TIF_16BIT
#define VERMAX 0xffff
DWORD GetAppCompatFlags2(WORD);
#define TEXT_FN(x) x##A
#define GetClientInfo() (wow_class_client_current()->thread->pClientInfo)
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
#undef GetClassInfoEx
#define GetClassInfoEx wow_original_get_class_info_ex
#undef UnregisterClass
#define UnregisterClass wow_original_unregister_class
#define RegisterClassWOWA wow_original_register_class
#endif
