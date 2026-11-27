#define WOW_ORIGINAL_EDIT_CLIENT
#define WOW_ORIGINAL_BUTTON_CLIENT
#include "control-client/precomp.h"
#include <stdio.h>
#undef ReleaseDC
#undef SendMessage
#undef SetCapture
#undef GetCapture
#undef ReleaseCapture
#undef SetFocus
#undef SendMessageW
#undef InvalidateRect
#undef DestroyWindow
#undef SetWindowTextW

HDC WINAPI xxxBNGetDC(PBUTN,HBRUSH *);
VOID WINAPI BNReleaseDC(PBUTN,HDC);
LONG WINAPI ButtonWndProcWorker(PWND,UINT,WPARAM,LPARAM,DWORD);
VOID WINAPI xxxBNDrawText(PBUTN,HDC,BOOL,BOOL);
VOID WINAPI xxxBNPaint(PBUTN,HDC);
VOID WINAPI xxxBNReleaseCapture(PBUTN,BOOL);
BOOL WINAPI xxxBNSetCapture(PBUTN,UINT);
static BUTNWND control;
static BUTN data;
static unsigned failures, acquired, released, logical_frees, colors;
static BOOL destroy_in_color;
static WND parent_view;
static HWND other;
static unsigned fired, notifications, after_death;
static int mode;
static HWND expected, parent;
static HBRUSH (WINAPI *native_color)(HWND,HWND,HDC,UINT);
#define CHECK(x) do { if (!(x)) { ++failures; printf("failure=%d error=%lu\n",__LINE__,GetLastError()); } } while (0)

static void retire(void)
{
    ++fired;
    CHECK(DestroyWindow(expected));
    CHECK(!data.spwnd && logical_frees==1);
    SetCapture(other);
    CHECK(GetCapture()==other);
}
static LRESULT CALLBACK proc(HWND w,UINT m,WPARAM p,LPARAM l)
{
    if(!fired && ((mode==1 && w==other && m==WM_CAPTURECHANGED) ||
                  (mode==2 && w==expected && m==WM_SETFOCUS) ||
                  (mode==7 && w==expected && m==WM_CAPTURECHANGED))) retire();
    if(w==parent && m==WM_COMMAND) {
        ++notifications;
        if(!IsWindow((HWND)l))++after_death;
        if(mode==8 && !fired)retire();
        return 0;
    }
    if (m == WM_CTLCOLORBTN) {
        ++colors;
        if (destroy_in_color) CHECK(DestroyWindow((HWND)l));
        return (LRESULT)GetSysColorBrush(COLOR_BTNFACE);
    }
    if(w==expected && data.spwnd) {
        if(!fired && ((mode==3 && m==BM_SETSTATE) ||
                      (mode==4 && m==BM_SETCHECK) ||
                      (mode==5 && m==WM_GETDLGCODE) ||
                      (mode==6 && m==BM_SETCHECK))) {
            retire();return m==WM_GETDLGCODE?DLGC_RADIOBUTTON:0;
        }
        if(m==WM_NCDESTROY || m==WM_SETFOCUS || m==WM_KILLFOCUS ||
           m==BM_SETSTATE || m==BM_SETCHECK || m==WM_GETDLGCODE ||
           m==WM_LBUTTONDOWN || m==WM_LBUTTONUP || m==WM_CAPTURECHANGED)
            return ButtonWndProcWorker(&control.wnd,m,p,l,FALSE);
    }
    return DefWindowProcW(w,m,p,l);
}
HDC WINAPI fixture_button_get_dc(HWND w)
{
    CHECK(w == expected); ++acquired; return GetDC(w);
}
int WINAPI fixture_button_release_dc(HWND w,HDC d)
{
    CHECK((w == expected || w == NULL) && d); ++released;
    return ReleaseDC(w,d);
}
HBRUSH WINAPI GetControlBrush(HWND w,HDC d,UINT m)
{
    CHECK(w == expected && d && m == WM_CTLCOLORBTN);
    return native_color(parent,w,d,m);
}
BOOL WINAPI IsVisible(PWND w)
{
    /* Match original IsVisible(NULL), not a substitute rejection. */
    return !w || !!(w->style & WS_VISIBLE);
}
BOOL WINAPI wow_control_set_fnid(HWND w,WORD f)
{
    CHECK(w == expected); control.wnd.fnid |= f; return TRUE;
}
VOID WINAPI wow_button_free_state(PLOOKASIDE p,PVOID v)
{
    (void)p; CHECK(v == &data && !data.spwnd); ++logical_frees;
    /* Boundary double only: the existing production invocation scope pins
     * BUTN until return. This witness exercises original logical cleanup. */
}
BOOL WINAPI _DefSetText(HWND w,LPCWSTR t,BOOL a)
{
    (void)t;(void)a;
    CHECK(!IsWindow(w));
    return SetWindowTextW(w,L"retired");
}
static void setup(void)
{
    ZeroMemory(&control,sizeof(control)); ZeroMemory(&data,sizeof(data));
    expected = CreateWindowW(L"T483OriginalButtonDc",L"",WS_CHILD,0,0,16,16,
        parent,NULL,GetModuleHandleW(NULL),NULL);
    CHECK(expected);
    control.wnd.head.h=expected; control.wnd.style=WS_VISIBLE|BS_PUSHBUTTON;
    control.wnd.fnid=FNID_BUTTON;
    control.wnd.rcClient.right=control.wnd.rcClient.bottom=16;
    control.pbutn=&data; data.spwnd=&control.wnd;
}
LRESULT WINAPI wow_native_DefWindowProcW(HWND w,UINT m,WPARAM p,LPARAM l)
{return DefWindowProcW(w,m,p,l);}
HWND WINAPI wow_native_SetCapture(HWND w){return SetCapture(w);}
HWND WINAPI wow_native_SetFocus(HWND w)
{
    /* This case owns the reentry point, not desktop focus policy. Hidden
     * windows need not receive native focus; dispatch the selected callback
     * through the real WndProc, which destroys the window and changes capture. */
    if(mode==2){SendMessageW(w,WM_SETFOCUS,0,0);return NULL;}
    return SetFocus(w);
}
BOOL WINAPI wow_native_ReleaseCapture(void){return ReleaseCapture();}
BOOL WINAPI wow_native_InvalidateRect(HWND w,const RECT*r,BOOL e){return InvalidateRect(w,r,e);}
LRESULT WINAPI wow_native_SendMessageW(HWND w,UINT m,WPARAM p,LPARAM l)
{return SendMessageW(w,m,p,l);}
LONG WINAPI wow_window_client_send_message(PWND w,UINT m,WPARAM p,LPARAM l,BOOL a)
{(void)a;return w?(LONG)SendMessageW(w->head.h,m,p,l):0;}
BOOL WINAPI _GetClientRect(PWND w,LPRECT r)
{*r=w->rcClient;OffsetRect(r,-r->left,-r->top);return TRUE;}
LONG WINAPI wow_default_window_proc(PWND w,UINT m,WPARAM p,LPARAM l,DWORD a)
{(void)a;if(mode==11 && m==WM_UPDATEUISTATE)retire();
 return (LONG)DefWindowProcW(w->head.h,m,p,l);}
static void run(int selected)
{
    DWORD exception=0;
    mode=0;fired=logical_frees=notifications=after_death=0;
    ZeroMemory(&control,sizeof(control));ZeroMemory(&data,sizeof(data));
    expected=CreateWindowW(L"T483OriginalButtonDc",L"",WS_CHILD,0,0,16,16,parent,NULL,GetModuleHandleW(NULL),NULL);
    CHECK(expected);
    control.wnd.head.h=expected;control.wnd.fnid=FNID_BUTTON;
    control.wnd.style=WS_CHILD | ((selected==5||selected==6)?BS_AUTORADIOBUTTON:(selected==3?BS_PUSHBUTTON:BS_AUTOCHECKBOX));
    control.wnd.spwndParent=&parent_view;control.wnd.rcClient.right=control.wnd.rcClient.bottom=16;
    control.pbutn=&data;data.spwnd=&control.wnd;
    if(selected==1)SetCapture(other);
    else if(selected>=3 && selected!=10){SetCapture(expected);data.buttonState=BST_CAPTURED|BST_PUSHED;}
    mode=selected;
    if(selected>=9)retire();
    if(selected==9)data.buttonState=BST_CAPTURED;
    __try {
        if(mode<=2)ButtonWndProcWorker(&control.wnd,WM_LBUTTONDOWN,0,0,FALSE);
        else if(mode==8)ButtonWndProcWorker(&control.wnd,WM_KILLFOCUS,0,0,FALSE);
        else if(mode==10)xxxBNSetCapture(&data,0);
        else xxxBNReleaseCapture(&data,TRUE);
    } __except(EXCEPTION_EXECUTE_HANDLER){exception=GetExceptionCode();}
    printf("case=%d fired=%u exception=%08lx foreign_capture=%u notifications=%u after_death=%u\n",mode,fired,exception,GetCapture()==other,notifications,after_death);
    CHECK(fired==1 && !exception && !data.spwnd && GetCapture()==other && !after_death);
    mode=0;ReleaseCapture();expected=NULL;
}
int __cdecl main(void)
{
    WNDCLASSW cls={0}; HDC dc; HBRUSH brush=NULL; DWORD exception=0; int i;
    cls.lpfnWndProc=proc; cls.hInstance=GetModuleHandleW(NULL);
    cls.lpszClassName=L"T483OriginalButtonDc";
    CHECK(RegisterClassW(&cls));
    parent=CreateWindowW(cls.lpszClassName,L"",WS_POPUP,0,0,32,32,NULL,NULL,cls.hInstance,NULL);
    native_color=(HBRUSH (WINAPI *)(HWND,HWND,HDC,UINT))GetProcAddress(GetModuleHandleW(L"win32u.dll"),"NtUserGetControlColor");
    CHECK(parent && native_color); if(!parent || !native_color)return 1;
    setup(); dc=xxxBNGetDC(&data,&brush);
    CHECK(dc && brush==GetSysColorBrush(COLOR_BTNFACE));
    CHECK(DestroyWindow(expected) && !data.spwnd && logical_frees==1);
    __try { BNReleaseDC(&data,dc); }
    __except(EXCEPTION_EXECUTE_HANDLER){exception=GetExceptionCode();}
    CHECK(!exception && acquired==1 && released==1 && colors==1);
    expected=NULL; setup(); destroy_in_color=TRUE; exception=0;
    brush=(HBRUSH)(ULONG_PTR)1;
    __try { dc=xxxBNGetDC(&data,&brush); }
    __except(EXCEPTION_EXECUTE_HANDLER){exception=GetExceptionCode();}
    CHECK(!exception && !dc && acquired==2 && released==2);
    CHECK(brush==(HBRUSH)(ULONG_PTR)1);
    CHECK(!data.spwnd && logical_frees==2 && colors==2);
    exception=0;
    __try { dc=xxxBNGetDC(&data,NULL); }
    __except(EXCEPTION_EXECUTE_HANDLER){exception=GetExceptionCode();}
    CHECK(!exception && !dc && acquired==2);
    printf("dead_dc_entry_exception=%08lx\n",exception);
    exception=0;
    __try { xxxBNDrawText(&data,NULL,0,FALSE); }
    __except(EXCEPTION_EXECUTE_HANDLER){exception=GetExceptionCode();}
    CHECK(!exception);
    printf("dead_text_entry_exception=%08lx\n",exception);
    expected=NULL;setup();dc=GetDC(expected);exception=0;
    __try { xxxBNPaint(&data,dc); }
    __except(EXCEPTION_EXECUTE_HANDLER){exception=GetExceptionCode();}
    CHECK(!exception);
    CHECK(!data.spwnd);printf("paint_color_cleanup_exception=%08lx\n",exception);
    expected=NULL;setup();control.wnd.style=WS_VISIBLE|BS_GROUPBOX;exception=0;
    __try { ButtonWndProcWorker(&control.wnd,WM_SETTEXT,0,(LPARAM)L"retired",FALSE); }
    __except(EXCEPTION_EXECUTE_HANDLER){exception=GetExceptionCode();}
    CHECK(!exception);
    CHECK(!data.spwnd);printf("settext_color_cleanup_exception=%08lx\n",exception);
    expected=NULL; destroy_in_color=FALSE;
    other=CreateWindowW(cls.lpszClassName,L"",WS_CHILD,0,0,16,16,parent,NULL,cls.hInstance,NULL);
    CHECK(other); parent_view.head.h=parent; parent_view.style=WS_POPUP;
    for(i=1;i<=10;++i)run(i);
    /* Native UI-state is authoritative; retained WND private bits cannot
     * substitute for it. Exercise both transitions and the XP flag binding. */
    fired=logical_frees=0;
    setup();
    DefWindowProcW(expected,WM_UPDATEUISTATE,
        MAKEWPARAM(UIS_SET,UISF_HIDEFOCUS|UISF_HIDEACCEL),0);
    CHECK(TestWF(&control.wnd,WEFPUIFOCUSHIDDEN));
    CHECK(TestWF(&control.wnd,WEFPUIACCELHIDDEN));
    DefWindowProcW(expected,WM_UPDATEUISTATE,
        MAKEWPARAM(UIS_CLEAR,UISF_HIDEFOCUS|UISF_HIDEACCEL),0);
    CHECK(!TestWF(&control.wnd,WEFPUIFOCUSHIDDEN));
    CHECK(!TestWF(&control.wnd,WEFPUIACCELHIDDEN));
    mode=11;fired=0;exception=0;
    __try { ButtonWndProcWorker(&control.wnd,WM_UPDATEUISTATE,0,0,FALSE); }
    __except(EXCEPTION_EXECUTE_HANDLER){exception=GetExceptionCode();}
    CHECK(fired==1 && !exception && !data.spwnd && !data.fPaintKbdCuesOnly);
    printf("ui_state_cleanup_exception=%08lx\n",exception);
    mode=0;ReleaseCapture();
    expected=NULL; CHECK(DestroyWindow(parent));
    CHECK(UnregisterClassW(cls.lpszClassName,cls.hInstance));
    printf("button_dc_failures=%u colors=%u cleanup=%u acquired=%u released=%u\n",failures,colors,logical_frees,acquired,released);
    return failures?1:0;
}
