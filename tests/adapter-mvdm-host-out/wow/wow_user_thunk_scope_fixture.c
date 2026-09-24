#include "precomp.h"
#include "wow_user_borrow_scope.h"
#include "wow_user_thunk_scope.h"

static unsigned errors, calls;
static HWND window;
static BOOL raising;
#define CHECK(x) do { if (!(x)) { ++errors; printf("FAIL line=%d\n", __LINE__); } } while (0)
static ULONG FASTCALL error_thunk(PVDMFRAME frame)
{
    CHECK(GetLastError() == 0x1234);
    SetLastError(0x5678);
    return frame->wAX;
}
static ULONG FASTCALL nested_thunk(PVDMFRAME frame)
{
    PWW words = wow_user_borrow_window(window);
    ++calls;
    CHECK(frame->wAX == 0x2468 && words != NULL);
    CHECK(words && words->adwWOW[0] == 0x12345678);
    wow_window_words_detach(window);
    CHECK(DestroyWindow(window));
    CHECK(words && words->adwWOW[0] == 0x12345678);
    if (raising) RaiseException(0xE0420041, 0, 0, NULL);
    return 0x76543210;
}
static ULONG FASTCALL outer_thunk(PVDMFRAME frame)
{
    PWW words = wow_user_borrow_window(window);
    ULONG result;
    ++calls;
    CHECK(words != NULL);
    result = wow_user_invoke_thunk(nested_thunk, frame);
    CHECK(words && words->adwWOW[0] == 0x12345678);
    return result;
}
static void create_window(void)
{
    WW words = {0};
    words.adwWOW[0] = 0x12345678;
    window = CreateWindowExA(0, "STATIC", "", 0, 0, 0, 16, 16,
        NULL, NULL, GetModuleHandleA(NULL), NULL);
    CHECK(window != NULL);
    CHECK(wow_window_words_attach(window, &words));
}
int __cdecl main(void)
{
    VDMFRAME frame = {0};
    BOOL caught = FALSE;
    frame.wAX = 0x2468;
    CHECK(SetEnvironmentVariableA("MVDM_WOW_DISPATCH_TRACE_PATH", NULL));
    SetLastError(0x1234);
    CHECK(wow_user_invoke_thunk(error_thunk, &frame) == 0x2468);
    CHECK(GetLastError() == 0x5678);
    CHECK(SetEnvironmentVariableA("MVDM_WOW_DISPATCH_TRACE_PATH", "NUL\\trace.log"));
    SetLastError(0x1234);
    CHECK(wow_user_invoke_thunk(error_thunk, &frame) == 0x2468);
    CHECK(GetLastError() == 0x5678);
    CHECK(SetEnvironmentVariableA("MVDM_WOW_DISPATCH_TRACE_PATH", NULL));
    create_window();
    CHECK(wow_user_invoke_thunk(outer_thunk, &frame) == 0x76543210);
    CHECK(frame.wAX == 0x2468);
    CHECK(wow_user_borrow_window(NULL) == NULL && GetLastError() == ERROR_INVALID_STATE);
    create_window(); raising = TRUE;
    __try { wow_user_invoke_thunk(outer_thunk, &frame); }
    __except (GetExceptionCode() == 0xE0420041 ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
        caught = TRUE;
    }
    CHECK(caught && !IsWindow(window));
    CHECK(wow_user_borrow_window(NULL) == NULL && GetLastError() == ERROR_INVALID_STATE);
    CHECK(calls == 4);
    fprintf(stderr, "WOW_USER_THUNK_SCOPE errors=%u calls=%u exception=%u\n", errors, calls, caught);
    return errors ? 3 : 0;
}
