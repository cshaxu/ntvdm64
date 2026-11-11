#include "wow_default_window_bindings.h"
#include "wow_user_client_view_layout.h"
#include "wow_user_borrow_scope.h"
#include "wow_user_task_lifecycle.h"
#include <stdio.h>

/* Production receive binding with transport doubles. Native callback and
 * guest return-state validation remain in the full grouped suite. */
static int failures, calls;
static BOOL expected_ansi;
static BOOL styled, deny_borrow, deny_entry, in_native, raise_hit;
static unsigned enters, leaves;
static PWND hit_state;
#define CHECK(x) do { if (!(x)) { ++failures; printf("FAIL %d: %s\n", __LINE__, #x); } } while (0)

wow_window_words_binding *WINAPI wow_user_borrow_client_state(HWND w,PWND state)
{
    CHECK(state==hit_state && w==state->head.h);
    if(deny_borrow){SetLastError(ERROR_INVALID_STATE);return NULL;}
    return (wow_window_words_binding *)(ULONG_PTR)1;
}
BOOL WINAPI wow_user_native_call_begin(wow_user_native_call *call,HWND w)
{
    (void)call;CHECK(w==hit_state->head.h && !in_native);
    if(deny_entry){SetLastError(ERROR_INVALID_STATE);return FALSE;}
    ++enters;in_native=TRUE;return TRUE;
}
BOOL WINAPI wow_user_native_call_end(wow_user_native_call *call)
{ (void)call;CHECK(in_native);++leaves;in_native=FALSE;return TRUE; }
void WINAPI wow_user_native_call_failed(const char *where,DWORD error)
{ (void)where;(void)error;CHECK(FALSE);ExitProcess(2); }
BOOL WINAPI wow_style_hit_test(HWND w,LPARAM point,LRESULT *result)
{
    (void)point;CHECK(in_native && w==hit_state->head.h);
    if(raise_hit)RaiseException(0xe0424473,0,0,NULL);
    if(!styled)return FALSE;
    *result=HTSYSMENU;SetLastError(0x4473);return TRUE;
}

static void verify_hit_boundary(void)
{
    WND state={0};RECT rect;LONG point;unsigned before;BOOL caught=FALSE;
    HWND w=CreateWindowExW(0,L"STATIC",L"hit fixture",WS_OVERLAPPEDWINDOW,
        20,20,160,120,NULL,NULL,GetModuleHandleW(NULL),NULL);
    CHECK(w!=NULL);if(!w)return;
    state.head.h=w;hit_state=&state;
    CHECK(GetWindowRect(w,&rect));point=MAKELONG(rect.left+1,rect.top+1);
    CHECK(wow_default_find_nc_hit(&state,point)==DefWindowProcW(w,WM_NCHITTEST,0,point));
    styled=TRUE;
    CHECK(wow_default_find_nc_hit(&state,point)==HTSYSMENU);
    CHECK(GetLastError()==0x4473 && !in_native && enters==leaves);
    before=enters;deny_borrow=TRUE;
    CHECK(wow_default_find_nc_hit(&state,point)==HTERROR && enters==before);
    deny_borrow=FALSE;deny_entry=TRUE;
    CHECK(wow_default_find_nc_hit(&state,point)==HTERROR && enters==before);
    deny_entry=FALSE;raise_hit=TRUE;
    __try { (void)wow_default_find_nc_hit(&state,point); }
    __except(GetExceptionCode()==0xe0424473?EXCEPTION_EXECUTE_HANDLER:EXCEPTION_CONTINUE_SEARCH)
        { caught=TRUE; }
    CHECK(caught && !in_native && enters==leaves);
    raise_hit=FALSE;
    CHECK(wow_default_find_nc_hit(NULL,point)==HTERROR);
    CHECK(GetLastError()==ERROR_INVALID_WINDOW_HANDLE);
    CHECK(DestroyWindow(w));hit_state=NULL;
}

static LRESULT dispatch(HWND window, UINT message, WPARAM wp, LPARAM lp, BOOL ansi)
{
    CHECK(window == (HWND)(ULONG_PTR)0x1234);
    CHECK(message == WM_SETTEXT && wp == 73 && lp == 91);
    CHECK(ansi == expected_ansi && GetLastError() == 456);
    ++calls;
    SetLastError(789);
    return -123;
}
LRESULT WINAPI wow_native_DefWindowProcA(HWND w, UINT m, WPARAM p, LPARAM l)
{ return dispatch(w, m, p, l, TRUE); }
LRESULT WINAPI wow_native_DefWindowProcW(HWND w, UINT m, WPARAM p, LPARAM l)
{ return dispatch(w, m, p, l, FALSE); }

int main(void)
{
    HWND window = (HWND)(ULONG_PTR)0x1234;
    unsigned i;
    for (i = 0; i != 2; ++i) {
        expected_ansi = i == 0;
        SetLastError(456);
        CHECK(wow_default_CsSendMessage(window, WM_SETTEXT, 73, 91,
            0, 0x29e, expected_ansi) == -123);
        CHECK(GetLastError() == 789);
    }
    CHECK(!wow_default_CsSendMessage(window, WM_SETTEXT, 73, 91,
        1, 0x29e, TRUE));
    CHECK(GetLastError() == ERROR_NOT_SUPPORTED);
    CHECK(!wow_default_CsSendMessage(window, WM_SETTEXT, 73, 91,
        0, 0x2b2, FALSE));
    CHECK(GetLastError() == ERROR_NOT_SUPPORTED && calls == 2);
    verify_hit_boundary();
    printf("default_native_failures=%d\n", failures);
    return failures != 0;
}
