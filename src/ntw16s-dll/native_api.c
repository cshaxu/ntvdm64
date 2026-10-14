#include "native_api.h"
#include <uxtheme.h>
static INIT_ONCE binding_once=INIT_ONCE_STATIC_INIT;
static BOOL WINAPI default_get(HWND w,int bar,LPSCROLLINFO info)
{return GetScrollInfo(w,bar,info);}
static int WINAPI default_set(HWND w,int bar,LPCSCROLLINFO info,BOOL redraw)
{return SetScrollInfo(w,bar,info,redraw);}
static HRESULT WINAPI default_theme(HWND w,LPCWSTR application,LPCWSTR ids)
{return SetWindowTheme(w,application,ids);}
static ntw16s_native_api native={sizeof(ntw16s_native_api),default_get,default_set,default_theme};
static BOOL CALLBACK bind_api(PINIT_ONCE once,void *value,void **context)
{(void)once;(void)context;native=*(const ntw16s_native_api *)value;return TRUE;}
BOOL __cdecl Ntw16sBindNativeApi(const ntw16s_native_api *api)
{
    if(!api || api->size!=sizeof(*api) || !api->get_scroll_info || !api->set_scroll_info || !api->set_window_theme){
        SetLastError(ERROR_INVALID_PARAMETER);return FALSE;
    }
    if(!InitOnceExecuteOnce(&binding_once,bind_api,(void *)api,NULL))return FALSE;
    if(native.get_scroll_info!=api->get_scroll_info || native.set_scroll_info!=api->set_scroll_info ||
        native.set_window_theme!=api->set_window_theme){SetLastError(ERROR_INVALID_STATE);return FALSE;}
    return TRUE;
}
BOOL ntw16s_native_get_scroll_info(HWND w,int bar,LPSCROLLINFO info)
{return native.get_scroll_info(w,bar,info);}
int ntw16s_native_set_scroll_info(HWND w,int bar,LPCSCROLLINFO info,BOOL redraw)
{return native.set_scroll_info(w,bar,info,redraw);}
HRESULT ntw16s_native_set_window_theme(HWND w,LPCWSTR application,LPCWSTR ids)
{return native.set_window_theme(w,application,ids);}
