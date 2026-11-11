#define WOW_ORIGINAL_DEFAULT_WINDOW
#include "control-client/default-precomp.h"
#include <stdio.h>
#include <string.h>
#include <wchar.h>
LONG WINAPI wow_original_default_window_proc(PWND, UINT, WPARAM, LPARAM, DWORD);

/* Actual clmsg and rtl/wow bodies. Dispatch/graph inputs are explicit
 * boundary doubles; this is not native USER or guest acceptance. */
wow_default_message_state_type wow_default_message_state;
__declspec(thread) wow_default_context *wow_default_current;
HBRUSH wow_edit_system_brushes[WOW_EDIT_COLOR_COUNT];
HBRUSH wow_edit_gray_brush;
static WND child, parent, desktop;
static unsigned failures, sends, native_calls;
static UINT last_message;
static LPARAM last_parameter;
static BOOL destroy_on_move;
static BYTE ordinary[128], special[128];
#define CHECK(x) do { if (!(x)) { ++failures; printf("FAIL %d\n", __LINE__); } } while (0)
PWND WINAPI wow_window_client_desktop(void) { return &desktop; }
LONG WINAPI wow_window_client_send_message(PWND w, UINT m, WPARAM p, LPARAM l, BOOL a)
{ (void)p; (void)a; CHECK(w == &child || w == &parent); CHECK(!TestWF(w,WFDESTROYED)); ++sends; last_message=m; last_parameter=l;
  if (destroy_on_move && m==WM_MOVE) SetWF(w,WFDESTROYED);
  return 0; }
LRESULT WINAPI wow_native_SendMessageW(HWND w, UINT m, WPARAM p, LPARAM l)
{ CHECK(w == parent.head.h); return wow_window_client_send_message(&parent,m,p,l,FALSE); }
LONG WINAPI wow_default_CsSendMessage(HWND w, UINT m, WPARAM p, LPARAM l, DWORD extra, DWORD fn, BOOL a)
{ (void)m; (void)p; (void)l; (void)a; CHECK(w == child.head.h && !extra && fn == FNID_DEFWINDOWPROC); ++native_calls; return 71; }
int WINAPI wow_default_find_nc_hit(PWND w, LONG l)
{ CHECK(w == &child && l == 123); return HTCAPTION; }
BOOL WINAPI wow_default_ensure_shell_message(HWND w)
{ CHECK(!w); wow_default_current->server.uiShellMsg = WM_USER+1; return TRUE; }

int __cdecl main(void)
{
    wow_default_context context = {0};
    WCHAR wide[8]; char narrow[8];
    WINDOWPOS position = {0};
    HDC dc = CreateCompatibleDC(NULL);
    wow_default_current=&context;
    wow_default_message_state.DefWindowMsgs.maxMsgs=1023;
    wow_default_message_state.DefWindowMsgs.abMsgs=ordinary;
    wow_default_message_state.DefWindowSpecMsgs.maxMsgs=1023;
    wow_default_message_state.DefWindowSpecMsgs.abMsgs=special;
    memset(special,255,sizeof(special));
    child.head.h=(HWND)1; parent.head.h=(HWND)2;
    child.spwndParent=&parent; child.style=WS_CHILD;
    child.strName.Buffer=L"Hello"; child.strName.Length=10;
    child.rcClient=(RECT){20,30,120,80}; parent.rcClient=(RECT){10,10,200,200};
    CHECK(wow_original_default_window_proc(&child,WM_GETTEXT,4,(LPARAM)wide,FALSE)==3 && !wcscmp(wide,L"Hel"));
    CHECK(wow_original_default_window_proc(&child,WM_GETTEXT,4,(LPARAM)narrow,TRUE)==3 && !strcmp(narrow,"Hel"));
    CHECK(wow_original_default_window_proc(&child,WM_GETTEXTLENGTH,0,0,FALSE)==5);
    CHECK(wow_original_default_window_proc(&child,WM_GETTEXTLENGTH,0,0,TRUE)==5);
    CHECK(wow_original_default_window_proc(&child,WM_NCHITTEST,0,123,FALSE)==HTCAPTION);
    CHECK(wow_original_default_window_proc(&child,WM_QUERYENDSESSION,0,0,FALSE)==TRUE);
    CHECK(wow_original_default_window_proc(&child,WM_CHARTOITEM,0,0,FALSE)==-1);
    (void)wow_original_default_window_proc(&child,WM_HELP,0,99,FALSE);
    CHECK(sends==1 && last_message==WM_HELP && last_parameter==99);
    (void)wow_original_default_window_proc(&child,WM_WINDOWPOSCHANGED,0,(LPARAM)&position,FALSE);
    CHECK(sends==3 && last_message==WM_SIZE && last_parameter==MAKELONG(100,50));
    SetWF(&child,WFMINIMIZED);
    (void)wow_original_default_window_proc(&child,WM_WINDOWPOSCHANGED,0,(LPARAM)&position,FALSE);
    CHECK(last_parameter==MAKELONG(GetSystemMetrics(SM_CXMINIMIZED),GetSystemMetrics(SM_CYMINIMIZED)));
    SetWF(&child,WFWIN40COMPAT);
    context.server.argbSystem[COLOR_3DFACE]=RGB(11,22,33);
    context.server.argbSystem[COLOR_BTNTEXT]=RGB(44,55,66);
    wow_edit_system_brushes[COLOR_3DFACE]=(HBRUSH)42;
    CHECK(dc!=NULL);
    CHECK(wow_original_default_window_proc(&child,WM_CTLCOLORBTN,(WPARAM)dc,0,FALSE)==42);
    CHECK(GetBkColor(dc)==RGB(11,22,33) && GetTextColor(dc)==RGB(44,55,66));
    CHECK(wow_original_default_window_proc(&child,WM_KEYDOWN,VK_F10,0,FALSE)==71 && native_calls==1);
    CHECK(wow_original_default_window_proc(&child,WM_USER+8,0,0,FALSE)==0);
    destroy_on_move=TRUE;
    (void)wow_original_default_window_proc(&child,WM_WINDOWPOSCHANGED,0,(LPARAM)&position,FALSE);
    CHECK(last_message==WM_MOVE && TestWF(&child,WFDESTROYED));
    CHECK(DeleteDC(dc));
    printf("original-default failures=%u\n",failures);
    return failures ? 1 : 0;
}
