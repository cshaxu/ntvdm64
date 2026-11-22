#ifndef WOW_CLASS_CLIENT_FIXTURE_PROVIDER_H
#define WOW_CLASS_CLIENT_FIXTURE_PROVIDER_H
#include "compat/include/wow_class_client_bindings.h"
extern ATOM WINAPI wow_original_register_class(WNDCLASSA *, DWORD *);
extern BOOL WINAPI wow_original_unregister_class(LPCSTR, HINSTANCE);
extern BOOL WINAPI wow_original_get_class_info(HINSTANCE, LPCSTR, WNDCLASSA *);
static const wow_class_client_provider wow_fixture_class_client_provider = {
    wow_original_register_class, wow_original_unregister_class, wow_original_get_class_info, _RegisterClassEx, GetClassPtr, _InnerGetClassPtr, _UnregisterClass
};
#endif
