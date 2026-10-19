#include <windows.h>
#include <commdlg.h>
#include <stdio.h>

/* Exact production wrappers with lower services controlled. This proves the
 * seven typed scope boundaries, not native USER or real guest scheduling. */
typedef struct wow_user_native_call {
    HWND target_window, returned_window;
    unsigned prior;
} wow_user_native_call;
static unsigned errors, depth, starts, ends, leaves, mode, rejected, end_failed;
static HWND expected_owner = (HWND)0x1234, expected_return = (HWND)0x5678;
static HWND published;
static void check(BOOL condition, const char *name)
{ if (!condition) { ++errors; printf("FAIL %s\n", name); } }
static BOOL WINAPI wow_user_native_call_begin(wow_user_native_call *call, HWND owner)
{
    ++starts;
    if (rejected) { SetLastError(9001); return FALSE; }
    call->target_window = owner; call->returned_window = NULL;
    call->prior = depth++;
    check(owner == expected_owner, "exact owner");
    SetLastError(9002);
    return TRUE;
}
static BOOL WINAPI wow_user_native_call_end(wow_user_native_call *call)
{
    ++ends; check(depth == call->prior + 1, "nested LIFO");
    depth = call->prior; published = call->returned_window;
    SetLastError(9003); return !end_failed;
}
__declspec(noreturn) static void WINAPI wow_user_native_call_failed(const char *where, DWORD error)
{
    check(lstrcmpA(where, "common-dialog-return") == 0 && error == 9003,
        "terminal end failure");
    RaiseException(0xE0449702, 0, 0, NULL);
    ExitProcess(99);
}
HWND WINAPI wow_native_replace_text(LPFINDREPLACEA);
static BOOL native_leaf(void *request)
{
    ++leaves;
    check(depth != 0 && GetLastError() == 7001, "native input error/execution");
    if (mode == 2) { SetLastError(7002); RaiseException(0xE0449701, 0, 0, NULL); }
    if (mode == 3) {
        FINDREPLACEA nested = {0};
        unsigned saved_mode = mode;
        nested.hwndOwner = expected_owner; mode = 1;
        check(wow_native_replace_text(&nested) == expected_return, "nested native result");
        check(depth == 1 && GetLastError() == 7002, "nested restore");
        mode = saved_mode;
    }
    /* Structures are forwarded without copying or substituting output fields. */
    if (request) ((CHOOSECOLORA *)request)->lStructSize = 0x7654;
    SetLastError(7002); return mode != 0;
}
static BOOL WINAPI leaf_color(LPCHOOSECOLORA p) { return native_leaf(p); }
static BOOL WINAPI leaf_font(LPCHOOSEFONTA p) { return native_leaf(p); }
static BOOL WINAPI leaf_open(LPOPENFILENAMEA p) { return native_leaf(p); }
static BOOL WINAPI leaf_save(LPOPENFILENAMEA p) { return native_leaf(p); }
static BOOL WINAPI leaf_print(LPPRINTDLGA p) { return native_leaf(p); }
static HWND WINAPI leaf_find(LPFINDREPLACEA p) { return native_leaf(p) ? expected_return : NULL; }
static HWND WINAPI leaf_replace(LPFINDREPLACEA p) { return native_leaf(p) ? expected_return : NULL; }
#define ChooseColorA leaf_color
#define ChooseFontA leaf_font
#define GetOpenFileNameA leaf_open
#define GetSaveFileNameA leaf_save
#define PrintDlgA leaf_print
#define FindTextA leaf_find
#define ReplaceTextA leaf_replace
#include "common_dialog_exact_body.h"

#define VERIFY(name, request_type, modeless) do { \
    unsigned scenario; \
    for (scenario = 0; scenario != 6; ++scenario) { \
        request_type request; \
        ULONG_PTR result = 0; \
        DWORD exception = 0; \
        ZeroMemory(&request, sizeof(request)); request.hwndOwner = expected_owner; \
        starts = ends = leaves = depth = 0; published = NULL; \
        mode = scenario == 5 ? 1 : scenario; \
        rejected = scenario == 4; end_failed = scenario == 5; \
        SetLastError(7001); \
        __try { result = (ULONG_PTR)name(&request); } \
        __except(EXCEPTION_EXECUTE_HANDLER) { exception = GetExceptionCode(); } \
        check(depth == 0 && starts == (scenario == 3 ? 2U : 1U), "balanced entry"); \
        check(ends == (rejected ? 0U : starts), "exactly once finalization"); \
        check(leaves == (rejected ? 0U : starts), "native exactly once/no replay"); \
        check(exception == (scenario == 2 ? 0xE0449701U : scenario == 5 ? 0xE0449702U : 0U), "SEH propagation"); \
        if (scenario != 2 && scenario != 5) { \
            check(GetLastError() == (rejected ? 9001U : 7002U), "native/rejection LastError"); \
            check(result == ((scenario == 0 || rejected) ? 0 : modeless ? (ULONG_PTR)expected_return : 1), "typed native result"); \
        } \
        if (!rejected && scenario != 2) check(request.lStructSize == 0x7654, "native output forwarded"); \
        if (!rejected && scenario != 2 && scenario != 5) \
            check(published == ((modeless && scenario != 0) ? expected_return : NULL), "returned window publication"); \
    } \
    printf("COMMON_DIALOG_LEAF %s scenarios=6\n", #name); \
} while (0)
int main(void)
{
    VERIFY(wow_native_choose_color, CHOOSECOLORA, 0);
    VERIFY(wow_native_choose_font, CHOOSEFONTA, 0);
    VERIFY(wow_native_open_file, OPENFILENAMEA, 0);
    VERIFY(wow_native_save_file, OPENFILENAMEA, 0);
    VERIFY(wow_native_find_text, FINDREPLACEA, 1);
    VERIFY(wow_native_replace_text, FINDREPLACEA, 1);
    VERIFY(wow_native_print_dialog, PRINTDLGA, 0);
    printf("COMMON_DIALOG_BOUNDARIES errors=%u leaves=7 scenarios=42 guest_proof=0\n", errors);
    return errors ? 1 : 0;
}
