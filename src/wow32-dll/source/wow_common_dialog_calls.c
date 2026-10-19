/* ADAPTER-WOW-097: native GUI leaves, not another COMMDLG implementation. */
#include "precomp.h"
#include "wow_common_dialog_calls.h"
#include "wow_user_native_call.h"

/* The original owner has already converted each request into a host structure.
 * Preserve its native result/error and run the existing return publication on
 * success, cancellation, failure and exception. No native operation is replayed. */
#define COMMON_DIALOG_CALL(type, name, request_type, native, failed, publication) \
type WINAPI name(request_type request) \
{ \
    wow_user_native_call call; \
    type result = failed; \
    DWORD error = GetLastError(); \
    if (!wow_user_native_call_begin(&call, request ? request->hwndOwner : NULL)) \
        return failed; \
    __try { \
        SetLastError(error); \
        result = native(request); \
        publication; \
    } __finally { \
        error = GetLastError(); \
        if (!wow_user_native_call_end(&call)) \
            wow_user_native_call_failed("common-dialog-return", GetLastError()); \
        SetLastError(error); \
    } \
    return result; \
}
COMMON_DIALOG_CALL(BOOL, wow_native_choose_color, LPCHOOSECOLORA,
    ChooseColorA, FALSE, (void)0)
COMMON_DIALOG_CALL(BOOL, wow_native_choose_font, LPCHOOSEFONTA,
    ChooseFontA, FALSE, (void)0)
COMMON_DIALOG_CALL(BOOL, wow_native_open_file, LPOPENFILENAMEA,
    GetOpenFileNameA, FALSE, (void)0)
COMMON_DIALOG_CALL(BOOL, wow_native_save_file, LPOPENFILENAMEA,
    GetSaveFileNameA, FALSE, (void)0)
COMMON_DIALOG_CALL(HWND, wow_native_find_text, LPFINDREPLACEA,
    FindTextA, NULL, call.returned_window = result)
COMMON_DIALOG_CALL(HWND, wow_native_replace_text, LPFINDREPLACEA,
    ReplaceTextA, NULL, call.returned_window = result)
COMMON_DIALOG_CALL(BOOL, wow_native_print_dialog, LPPRINTDLGA,
    PrintDlgA, FALSE, (void)0)
#undef COMMON_DIALOG_CALL
