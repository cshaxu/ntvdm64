#ifndef WOW_CLASS_CLIENT_FIXTURE_PROVIDER_H
#define WOW_CLASS_CLIENT_FIXTURE_PROVIDER_H
#include "compat/include/wow_user_client_view_layout.h"
#include "compat/include/wow_class_client_bindings.h"
#include "wow_window_access_fixture_provider.h"
extern ATOM WINAPI wow_original_register_class(WNDCLASSA *, DWORD *);
extern BOOL WINAPI wow_original_unregister_class(LPCSTR, HINSTANCE);
extern BOOL WINAPI wow_original_get_class_info(HINSTANCE, LPCSTR, WNDCLASSA *);
static const wow_class_client_provider wow_fixture_class_client_provider = {
    wow_original_register_class, wow_original_unregister_class, wow_original_get_class_info, xxxRegisterClassEx, GetClassPtr, _InnerGetClassPtr, wow_original_class_unregister_committed, sizeof(CLS), FNID_DIALOG, wow_original_class_initialize_identity, wow_original_class_capture_menu
};
#endif
