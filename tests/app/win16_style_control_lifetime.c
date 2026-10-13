#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include "win16-style/controls.h"
#define CHECK(x) do{if(!(x)){fprintf(stderr,"LIFETIME FAIL line=%d error=%lu\n",__LINE__,GetLastError());return 1;}}while(0)
typedef struct action {BOOL destroy;unsigned calls;} action;
typedef struct nested_action {BOOL entered,installed;} nested_action;
static LRESULT CALLBACK nested_callback(HWND w,UINT m,WPARAM a,LPARAM b,UINT_PTR id,DWORD_PTR data)
{
    nested_action *state=(nested_action *)data;
    if(m==WM_GETFONT && !state->entered){state->entered=TRUE;state->installed=win16_style_control_install(w,NULL);}
    if(m==WM_NCDESTROY)RemoveWindowSubclass(w,nested_callback,id);
    return DefSubclassProc(w,m,a,b);
}
static LRESULT CALLBACK callback(HWND w,UINT m,WPARAM a,LPARAM b,UINT_PTR id,DWORD_PTR data)
{
    action *state=(action *)data;
    if(m==WM_SETFONT && !state->calls++){
        if(state->destroy)DestroyWindow(w);else win16_style_control_remove(w);
        return 0;
    }
    if(m==WM_NCDESTROY)RemoveWindowSubclass(w,callback,id);
    return DefSubclassProc(w,m,a,b);
}
int main(void)
{
    HDESK desktop;HWND root;unsigned mode;
    desktop=CreateDesktopW(L"StyleControlLifetime",NULL,NULL,0,GENERIC_ALL,NULL);
    CHECK(desktop && SetThreadDesktop(desktop));
    root=CreateWindowW(L"STATIC",L"Lifecycle probe",WS_OVERLAPPEDWINDOW,0,0,300,200,
        NULL,NULL,GetModuleHandleW(NULL),NULL);CHECK(root);
    for(mode=0;mode<2;++mode){
        action state={(BOOL)mode,0};BOOL installed;HWND child;
        child=CreateWindowW(L"BUTTON",L"test",WS_CHILD,0,0,200,40,root,NULL,GetModuleHandleW(NULL),NULL);
        CHECK(child && SetWindowSubclass(child,callback,1,(DWORD_PTR)&state));
        installed=win16_style_control_install(child,NULL);
        printf("INSTALL CALLBACK destroy=%u returned=%d callbacks=%u alive=%d\n",mode,installed,state.calls,IsWindow(child));
        CHECK(state.calls && !installed);
        if(!mode){CHECK(IsWindow(child));CHECK(!win16_style_control_remove(child));
            CHECK(RemoveWindowSubclass(child,callback,1));
            CHECK(win16_style_control_install(child,NULL));CHECK(win16_style_control_remove(child));CHECK(DestroyWindow(child));}
        else CHECK(!IsWindow(child));
    }
    {nested_action state={0};BOOL outer;HWND child;
        child=CreateWindowW(L"BUTTON",L"nested",WS_CHILD,0,0,200,40,root,NULL,GetModuleHandleW(NULL),NULL);
        CHECK(child && SetWindowSubclass(child,nested_callback,1,(DWORD_PTR)&state));
        outer=win16_style_control_install(child,NULL);
        printf("NESTED INSTALL inner=%d outer=%d\n",state.installed,outer);
        CHECK(state.entered && state.installed && !outer);
        CHECK(win16_style_control_remove(child));CHECK(!win16_style_control_remove(child));
        CHECK(RemoveWindowSubclass(child,nested_callback,1));CHECK(DestroyWindow(child));}
    CHECK(DestroyWindow(root));puts("CONTROL LIFETIME PASS removal/destruction/nested installation");return 0;
}
