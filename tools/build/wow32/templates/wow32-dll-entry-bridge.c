/* Generated build-only CRT-to-original DLLENTRY bridge. */
#include "wow_user_client_view_layout.h"
#include "wow_common_dialog_callback.h"
#include "wow_hook_callback.h"
#include "wow_idle_hook_binding.h"
#include "wow_standard_class_provider.h"
#include "wow_task_lifecycle_provider.h"
static const wow_task_lifecycle_provider task_lifecycle_provider = {
    xxxSleepTask, xxxDirectedYield, xxxInitTask, xxxRegisterUserHungAppHandlers,
    xxxUserYield, DestroyTask };
#include "wow_window_access_provider.h"
HWND WINAPI wow_original_window_handle(struct tagWND *);
VOID WINAPI wow_original_restore_window_words(struct _WW *, const struct mvdm_softpc_wow_window_words_view *);
DWORD WINAPI xxxSetWindowLong(struct tagWND *, int, DWORD, BOOL);
DWORD WINAPI _GetWindowLong(struct tagWND *, int, BOOL);
LONG WINAPI _SetWindowLong(HWND, int, LONG, BOOL);
HMENU WINAPI wow_user_load_menuW(HINSTANCE, LPCWSTR);
const wow_window_layout *WINAPI wow_original_window_layout(void);
VOID WINAPI wow_original_capture_window_words(const struct _WW *, struct mvdm_softpc_wow_window_words_view *);
VOID WINAPI wow_original_initialize_window_state(struct tagWND *, const wow_window_state_initialization *);
SIZE_T WINAPI wow_original_window_state_extent(const struct tagWND *);
WCHAR *WINAPI wow_original_promote_window_state(struct tagWND *, struct tagWND *, struct tagCLS *);
VOID WINAPI wow_original_initialize_resource_window(struct tagWND *, struct tagCLS *, const struct mvdm_softpc_wow_window_observation *, HWND);
WCHAR *WINAPI wow_original_refresh_window_state(struct tagWND *, struct _WW *, const WINDOWINFO *, DWORD, DWORD, WCHAR *, ULONG, ULONG);
VOID WINAPI wow_original_publish_window_links(struct tagWND *, struct tagCLS *, ATOM, struct tagWND *, struct tagWND *, struct tagWND *, struct tagWND *);
WCHAR *WINAPI wow_original_window_name(struct tagWND *);
WORD WINAPI wow_original_window_fnid(struct tagWND *);
WORD WINAPI wow_original_mark_window_destroyed(struct tagWND *);
BOOL WINAPI wow_original_creation_class_name(const struct _LARGE_STRING *, struct _UNICODE_STRING *);
C_ASSERT(sizeof(WW) % __alignof(CLS) == 0);
static const wow_window_access_provider window_access_provider = {
    xxxSetWindowLong, _GetWindowLong, _SetWindowLong, wow_user_load_menuW, wow_original_window_handle, FNID_DEFWINDOWPROC, wow_original_restore_window_words, wow_original_window_layout, wow_original_capture_window_words, wow_original_initialize_window_state, wow_original_window_state_extent, wow_original_promote_window_state, wow_original_initialize_resource_window, wow_original_refresh_window_state, wow_original_publish_window_links, wow_original_window_name, wow_original_window_fnid, wow_original_mark_window_destroyed, wow_original_creation_class_name };
#include "wow_registration_provider.h"
VOID WINAPI FreeDDEData(HANDLE, BOOL, BOOL);
LONG WINAPI GetClassWOWWords(HINSTANCE, LPCTSTR);
static const wow_registration_provider registration_provider = { FreeDDEData, GetClassWOWWords, wow_original_registration_exchange, wow_original_registered_callbacks, wow_original_set_registered_callbacks, wow_original_edit_callbacks_ready, wow_original_registration_lifecycle, wow_original_registration_window_callback };
#include "wow_task_cleanup_entry.h"
#include "wow_timer_callback.h"
#include "wow_native_extension_provider.h"
#include "wow_thunk_provider.h"
INT WINAPI wow_original_dispatch_exception(DWORD, PEXCEPTION_POINTERS, struct _TD *, DWORD, BOOL);
VOID WINAPI wow_original_thunk_capture(wow_thunk_mode *);
VOID WINAPI wow_original_thunk_restore(const wow_thunk_mode *);
VOID WINAPI wow_original_thunk_frame(struct _VDMFRAME *, wow_thunk_frame *);
static const wow_thunk_provider thunk_provider = {
    wow_original_dispatch_exception, wow_original_thunk_capture,
    wow_original_thunk_restore, wow_original_thunk_frame };
BOOL WINAPI wow_original_kernel_resource_return(INT);
BOOL WINAPI wow_original_callback_guarded(INT, union _PARM16 *, DWORD, DWORD *);
DWORD WINAPI wow_original_WK32ICallProc32(struct _VDMFRAME *);
ULONG WINAPI wow_original_WMM32CallProc32(struct _VDMFRAME *);
VOID WINAPI wow_original_WOWYield16(void);
VOID WINAPI wow_original_WOWDirectedYield16(WORD);
static const wow_native_extension_provider extension_provider = {
    wow_original_kernel_resource_return, wow_original_callback_guarded,
    wow_original_WK32ICallProc32, wow_original_WMM32CallProc32,
    wow_original_WOWYield16, wow_original_WOWDirectedYield16 };
VOID WINAPI WowOriginalTimerFunc(UINT, HWND, UINT, DWORD);
BOOL WINAPI wow_original_task_cleanup_uninitialized(DWORD);
VOID WINAPI wow_original_task_retire_async(void);
VOID WINAPI WowOriginalFreeTask(PVOID);
static const wow_task_cleanup_provider cleanup_provider = {
    wow_original_task_cleanup_uninitialized, wow_original_task_retire_async, WowOriginalFreeTask };
#include "compat/include/wow_class_client_bindings.h"
ATOM WINAPI wow_original_register_class(WNDCLASSA *, DWORD *);
BOOL WINAPI wow_original_unregister_class(LPCSTR, HINSTANCE);
BOOL WINAPI wow_original_get_class_info(HINSTANCE, LPCSTR, WNDCLASSA *);
static const wow_class_client_provider class_client_provider = {
    wow_original_register_class, wow_original_unregister_class, wow_original_get_class_info, xxxRegisterClassEx, GetClassPtr, _InnerGetClassPtr, wow_original_class_unregister_committed, sizeof(CLS), FNID_DIALOG, wow_original_class_initialize_identity, wow_original_class_capture_menu };
#include "compat/include/wow_button_lifetime.h"
PVOID WINAPI wow_original_button_data(struct tagWND *);
const wow_button_resource_provider *WINAPI wow_original_button_resources(void);
VOID WINAPI FreeLookasideEntry(PLOOKASIDE, PVOID);
#include "compat/include/wow_task_profile_bindings.h"
UINT WINAPI wow_original_task_profile(UINT, LPCWSTR, LPCWSTR, LPWSTR, DWORD);
#include "compat/include/wow_native_text_descriptor.h"
#include "compat/include/wow_bitmap_bindings.h"
#include "compat/include/wow_cleanup_bindings.h"
#include "compat/include/wow_default_window_provider.h"
LONG WINAPI wow_original_default_window_proc(struct tagWND *, UINT, WPARAM, LPARAM, DWORD);
extern const wow_cleanup_provider wow_original_cleanup_provider;
HBITMAP WINAPI WOWLoadBitmapA(HINSTANCE, LPCSTR, LPBYTE, DWORD);
void WINAPI wow_original_text_capture(wow_native_text_descriptor *, const struct _LARGE_STRING *);
INT WINAPI GetStdClassNumber(LPSTR);
LPCSTR WINAPI wow_original_class_name(UINT);
ATOM WINAPI wow_original_class_atom(UINT);
BOOL WINAPI wow_original_class_capture(LPCSTR, PUNICODE_STRING);
static const wow_standard_class_provider class_provider = {
    GetStdClassNumber, wow_original_class_name, wow_original_class_atom, wow_original_class_capture, { 0, 0, FNID_BUTTON, FNID_COMBOBOX, FNID_EDIT, FNID_LISTBOX, FNID_MDICLIENT, FNID_SCROLLBAR, FNID_STATIC, FNID_DESKTOP, FNID_DIALOG, FNID_ICONTITLE, FNID_MENU, FNID_SWITCH, FNID_COMBOLISTBOX } };
BOOL WINAPI wow_original_idle_registered(DWORD);
BOOL WINAPI wow_original_separate_launch(void);
LRESULT CALLBACK WowOriginalForegroundIdleHook(int, WPARAM, LPARAM);
static const wow_task_state_callbacks task_callbacks = {
    wow_original_idle_registered, wow_original_separate_launch, WowOriginalForegroundIdleHook };
void WINAPI wow_hook_capture_context(wow_hook_context *);
void WINAPI wow_hook_restore_context(const wow_hook_context *);
BOOL WINAPI wow_hook_query_type(INT, INT *);
LONG WINAPI WowOriginalStdHookProc(INT, LONG, LONG, INT);
static const wow_hook_callbacks hook_callbacks = {
    wow_hook_capture_context, wow_hook_restore_context, wow_hook_query_type, WowOriginalStdHookProc };
UINT WINAPI WowOriginalDialogProc(HWND, UINT, WPARAM, LPARAM);
UINT WINAPI WowOriginalPrintSetupDialogProc(HWND, UINT, WPARAM, LPARAM);
UINT WINAPI WowOriginalFindReplaceDialogProc(HWND, UINT, WPARAM, LPARAM);
static const wow_common_dialog_callbacks dialog_callbacks = {
    WowOriginalDialogProc, WowOriginalPrintSetupDialogProc, WowOriginalFindReplaceDialogProc };
extern unsigned char __stdcall W32DllInitialize(void *, unsigned long, void *);
extern void __stdcall wow_bitmap_bindings_shutdown(void);
extern void __stdcall wow_registry_binding_shutdown(void);
#include "compat/include/wow_control_brush_bindings.h"
HBRUSH WINAPI xxxGetControlBrush(struct tagWND *, HDC, UINT);
#include "compat/include/wow_menu_parser_bindings.h"
#include "compat/include/wow_ole_descriptor_provider.h"
HGLOBAL WINAPI ConvertObjDescriptor(HANDLE, UINT);
#include "compat/include/wow_edit_lookaside_provider.h"
extern CRITICAL_SECTION gcsLookaside;
#include "compat/include/wow_edit_entry_bindings.h"
LONG WINAPI ComboBoxWndProcA(HWND, UINT, WPARAM, LPARAM);
LONG WINAPI ComboBoxWndProcW(HWND, UINT, WPARAM, LPARAM);
LONG WINAPI ListBoxWndProcA(HWND, UINT, WPARAM, LPARAM);
LONG WINAPI ListBoxWndProcW(HWND, UINT, WPARAM, LPARAM);
LONG WINAPI ComboListBoxWndProcA(HWND, UINT, WPARAM, LPARAM);
LONG WINAPI ComboListBoxWndProcW(HWND, UINT, WPARAM, LPARAM);
LONG WINAPI ButtonWndProcA(HWND, UINT, WPARAM, LPARAM);
LONG WINAPI ButtonWndProcW(HWND, UINT, WPARAM, LPARAM);
VOID WINAPI xxxLBoxCaretBlinker(HWND, UINT, UINT_PTR, DWORD);
#include "compat/include/wow_edit_conversion_bindings.h"
LONG WINAPI EditWndProcA(HWND, UINT, WPARAM, LONG);
LONG WINAPI EditWndProcW(HWND, UINT, WPARAM, LONG);
static const wow_edit_conversion_provider edit_conversion_provider = { EditWndProcA, EditWndProcW, FNID_CALLWINDOWPROC, CPD_WND | CPD_ANSI_TO_UNICODE, CPD_WND | CPD_UNICODE_TO_ANSI };
BOOL WINAPI wow_original_list_caret_ready(struct tagWND *);
BOOL WINAPI wow_original_combo_info(HWND, struct tagWND *, PCOMBOBOXINFO, BOOL *);
BOOL WINAPI wow_original_finalize_control(HWND);
static const wow_control_entry_provider control_entry_provider = { EditWndProcA, EditWndProcW, ComboBoxWndProcA, ComboBoxWndProcW, ListBoxWndProcA, ListBoxWndProcW, ComboListBoxWndProcA, ComboListBoxWndProcW, ButtonWndProcA, ButtonWndProcW, xxxLBoxCaretBlinker, FNID_BUTTON, FNID_EDIT, FNID_COMBOBOX, FNID_LISTBOX, FNID_COMBOLISTBOX, ICLS_EDIT, ICLS_COMBOLISTBOX, wow_original_list_caret_ready, wow_original_combo_info, wow_original_finalize_control };
#include "compat/include/wow_dialog_keyboard_provider.h"
BOOL WINAPI wow_original_IsDialogMessageA(HWND, LPMSG);
BOOL WINAPI wow_original_IsDialogMessageW(HWND, LPMSG);
static const wow_dialog_keyboard_provider dialog_keyboard_provider = { wow_original_IsDialogMessageA, wow_original_IsDialogMessageW };
#include "compat/include/wow_dialog_finalization_provider.h"
#include "compat/include/wow_dialog_api_bindings.h"
LONG WINAPI DefDlgProcWorker(struct tagWND *, UINT, WPARAM, LPARAM, DWORD);
BOOL WINAPI wow_original_EndDialog(HWND, int);
BOOL WINAPI wow_original_dialog_owned(struct tagWND *);
BOOL WINAPI wow_original_dialog_finalize(struct tagWND *, LONG (WINAPI *)(struct tagWND *, UINT, WPARAM, LPARAM, DWORD));
BOOL WINAPI wow_original_dialog_set_pointer(struct tagWND *, struct _DLG *);
static const wow_dialog_finalization_provider dialog_finalization_provider = { DefDlgProcWorker, wow_original_EndDialog, wow_original_MapDialogRect, wow_original_DlgDirListA, wow_original_DlgDirListComboBoxA, wow_original_DlgDirSelectExA, wow_original_DlgDirSelectComboBoxExA, wow_original_dialog_owned, wow_original_dialog_finalize, wow_original_dialog_set_pointer };
#include "compat/include/wow_dialog_creation_binding.h"
HWND WINAPI InternalCreateDialog(HANDLE, LPDLGTEMPLATE, DWORD, HWND, DLGPROC, LONG, UINT);
int WINAPI wow_original_DialogBoxIndirectParamAorW(HINSTANCE, LPCDLGTEMPLATEW, HWND, DLGPROC, LPARAM, UINT);
static const wow_dialog_creation_provider dialog_creation_provider = { InternalCreateDialog, wow_original_DialogBoxIndirectParamAorW };
#include "compat/include/wow_default_message_bindings.h"
static const wow_default_message_provider default_message_provider = { InitMessageTables, wow_default_message_copy, wow_default_message_reset };
#include "compat/include/wow_control_state_provider.h"
void WINAPI SetWindowState(struct tagWND *, UINT);
void WINAPI ClearWindowState(struct tagWND *, UINT);
void WINAPI wow_original_change_compat_state(struct tagWND *, BOOL);
static const wow_control_state_providers control_state_provider = { SetWindowState, ClearWindowState, WFWIN40COMPAT, wow_original_change_compat_state };
#include "compat/include/wow_control_fnid_provider.h"
BOOL WINAPI wow_edit_set_window_fnid(HWND, WORD);
#include "compat/include/wow_control_extent_bindings.h"
#include "compat/include/wow_control_draw_state.h"
#pragma comment(linker, "/alternatename:_wow_original_DrawStateW@40=_DrawStateW@40")
BOOL WINAPI wow_original_DrawStateW(HDC, HBRUSH, DRAWSTATEPROC, LPARAM, WPARAM, int, int, int, int, UINT);
int __stdcall DllMain(void *instance, unsigned long reason, void *reserved)
{
    int result;
    if (reason == 1 && !wow_menu_parser_bind(CreateMenuFromResource)) return 0;
    if (reason == 1 && !wow_ole_descriptor_bind(ConvertObjDescriptor)) return 0;
    if (reason == 1 && !wow_edit_lookaside_bind(&gcsLookaside)) return 0;
    if (reason == 1 && !wow_control_entry_bind(&control_entry_provider)) return 0;
    if (reason == 1 && !wow_edit_conversion_bind(&edit_conversion_provider)) return 0;
    if (reason == 1 && !wow_dialog_keyboard_bind(&dialog_keyboard_provider)) return 0;
    if (reason == 1 && !wow_dialog_finalization_bind(&dialog_finalization_provider)) return 0;
    if (reason == 1 && !wow_dialog_creation_bind(&dialog_creation_provider)) return 0;
    if (reason == 1 && !wow_default_messages_bind(&default_message_provider)) return 0;
    if (reason == 1 && !wow_control_state_bind(&control_state_provider)) return 0;
    if (reason == 1 && !wow_control_fnid_bind(wow_edit_set_window_fnid)) return 0;
    if (reason == 1 && !wow_control_extent_bind(wow_control_registered_extent)) return 0;
    if (reason == 1 && !wow_control_draw_bind(wow_original_DrawStateW)) return 0;
    if (reason == 1 && !wow_control_brush_bind(xxxGetControlBrush)) return 0;
    if (reason == 1 && !wow_common_dialog_bind(&dialog_callbacks)) return 0;
    if (reason == 1 && !wow_hook_bind(&hook_callbacks)) return 0;
    if (reason == 1 && !wow_task_state_bind(&task_callbacks)) return 0;
    if (reason == 1 && !wow_standard_class_bind(&class_provider)) return 0;
    if (reason == 1 && !wow_registration_bind(&registration_provider)) return 0;
    if (reason == 1 && !wow_window_access_bind(&window_access_provider)) return 0;
    if (reason == 1 && !wow_task_lifecycle_bind(&task_lifecycle_provider)) return 0;
    if (reason == 1 && !wow_class_client_bind(&class_client_provider)) return 0;
    if (reason == 1 && !wow_button_resources_bind(wow_original_button_resources())) return 0;
    if (reason == 1 && !wow_button_bind(wow_original_button_data, FreeLookasideEntry)) return 0;
    if (reason == 1 && !wow_native_text_bind(wow_original_text_capture)) return 0;
    if (reason == 1 && !wow_profile_bind(wow_original_task_profile, FastGetProfileIntW)) return 0;
    if (reason == 1 && !wow_bitmap_bind_provider(WOWLoadBitmapA)) return 0;
    if (reason == 1 && !wow_cleanup_bind(&wow_original_cleanup_provider)) return 0;
    if (reason == 1 && !wow_default_window_bind(wow_original_default_window_proc)) return 0;
    if (reason == 1 && !wow_task_cleanup_bind(&cleanup_provider)) return 0;
    if (reason == 1 && !wow_timer_bind(WowOriginalTimerFunc)) return 0;
    if (reason == 1 && !wow_native_extension_bind(&extension_provider)) return 0;
    if (reason == 1 && !wow_thunk_bind(&thunk_provider)) return 0;
    result = W32DllInitialize(instance, reason, reserved);
    if (reason == 0 && reserved == 0) { wow_registry_binding_shutdown(); wow_bitmap_bindings_shutdown(); }
    return result;
}
