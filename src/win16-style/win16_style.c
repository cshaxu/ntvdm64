/* Project-authored experimental non-client presentation, not imported USER.
 * See T445 S1 provenance/design: original NTUSER server is a stopping boundary.
 */
#include "win16_style.h"
#include "layout.h"
#include "frame_buttons.h"
#include "system_font.h"
#include <commctrl.h>
#include <dwmapi.h>
#include <windowsx.h>
#include <stdlib.h>
#include <string.h>

typedef struct style_window {
    win16_style_palette palette;
    HFONT font;
    HANDLE font_resource;
    UINT dpi, calls;
    BOOL active, detached, hot;
    int pressed;
    DWORD old_policy;
    BOOL policy_saved;
} style_window;

static LRESULT CALLBACK style_proc(HWND,UINT,WPARAM,LPARAM,UINT_PTR,DWORD_PTR);
#define STYLE_ID ((UINT_PTR)style_proc)

void win16_style_default_palette(win16_style_palette *p)
{
    if(!p)return;
    /* Owner reference preset, not a claim about universal Win3.1 defaults. */
    p->active=RGB(164,200,240);p->inactive=RGB(255,255,255);
    p->active_text=p->inactive_text=RGB(0,0,0);
    p->face=RGB(192,192,192);p->light=RGB(255,255,255);
    p->shadow=RGB(128,128,128);p->edge=RGB(0,0,0);
}

static style_window *state(HWND w)
{
    DWORD_PTR data=0;
    if(GetWindowSubclass(w,style_proc,STYLE_ID,&data))return (style_window *)data;
    return NULL;
}

static BOOL local_window(HWND w)
{
    if(GetWindowThreadProcessId(w,NULL)!=GetCurrentThreadId()){
        SetLastError(ERROR_INVALID_THREAD_ID);return FALSE;
    }
    return TRUE;
}

static BOOL measure_window(HWND w,style_window *s,const RECT *outer,win16_style_layout *l)
{
    RECT visible=*outer;BOOL zoom=IsZoomed(w);int dx,dy;
    if(zoom){
        MONITORINFO monitor={sizeof(monitor)};RECT intersection;
        if(GetMonitorInfoW(MonitorFromWindow(w,MONITOR_DEFAULTTONEAREST),&monitor) &&
            IntersectRect(&intersection,outer,&monitor.rcWork))visible=intersection;
    }
    if(!win16_style_measure(visible.right-visible.left,visible.bottom-visible.top,
        s->dpi,(DWORD)GetWindowLongPtr(w,GWL_STYLE),
        (DWORD)GetWindowLongPtr(w,GWL_EXSTYLE),zoom,l))return FALSE;
    /* Native maximized resize borders can lie outside the work area. Keep
     * Windows' geometry, but place every custom non-client element and the
     * actual client inside its visible part using one layout calculation. */
    dx=visible.left-outer->left;dy=visible.top-outer->top;
    OffsetRect(&l->outer,dx,dy);OffsetRect(&l->client,dx,dy);
    OffsetRect(&l->caption,dx,dy);OffsetRect(&l->title,dx,dy);
    OffsetRect(&l->system,dx,dy);OffsetRect(&l->minimize,dx,dy);OffsetRect(&l->maximize,dx,dy);
    return TRUE;
}

static BOOL layout(HWND w,style_window *s,win16_style_layout *l)
{
    RECT r;
    return GetWindowRect(w,&r) && measure_window(w,s,&r,l);
}

static void colors(style_window *s,win16_style_palette *p)
{
    HIGHCONTRASTW hc={sizeof(hc)};
    *p=s->palette;
    if(SystemParametersInfoW(SPI_GETHIGHCONTRAST,sizeof(hc),&hc,0) &&
        (hc.dwFlags&HCF_HIGHCONTRASTON)){
        p->active=GetSysColor(COLOR_ACTIVECAPTION);
        p->inactive=GetSysColor(COLOR_INACTIVECAPTION);
        p->active_text=GetSysColor(COLOR_CAPTIONTEXT);
        p->inactive_text=GetSysColor(COLOR_INACTIVECAPTIONTEXT);
        p->face=GetSysColor(COLOR_BTNFACE);p->light=GetSysColor(COLOR_BTNHIGHLIGHT);
        p->shadow=GetSysColor(COLOR_BTNSHADOW);p->edge=GetSysColor(COLOR_WINDOWTEXT);
    }
}

static void fill(HDC dc,RECT r,COLORREF c)
{
    SetDCBrushColor(dc,c);FillRect(dc,&r,(HBRUSH)GetStockObject(DC_BRUSH));
}

static void outline(HDC dc,RECT r,int u,COLORREF c)
{
    RECT q=r;q.bottom=q.top+u;fill(dc,q,c);
    q=r;q.top=q.bottom-u;fill(dc,q,c);
    q=r;q.right=q.left+u;fill(dc,q,c);
    q=r;q.left=q.right-u;fill(dc,q,c);
}

static void button(HDC dc,RECT r,int kind,BOOL down,BOOL zoom,int u,
    const win16_style_palette *p)
{
    BITMAPINFO bi={0};DWORD pixels[19*18];int x,y,width,stride;
    const char *bits;
    COLORREF palette[4]={p->edge,p->light,p->shadow,p->face};
    if(IsRectEmpty(&r))return;
    fill(dc,r,p->face);outline(dc,r,u,p->edge);
    if(kind==HTSYSMENU){
        /* CLOSE contains two side-by-side 18-pixel images. Original USER
         * uses the first for top-level windows, second for WS_CHILD.
         * Child/MDI frames are not yet part of this experiment. */
        bits=&system_menu_bitmap[0][0];width=18;stride=37;r.left+=u;
    }else{
        int index=(kind==HTMINBUTTON?0:zoom?2:1)+(down?3:0);
        bits=&frame_buttons[index][0][0];width=19;stride=20;
    }
    for(y=0;y<18;++y)for(x=0;x<width;++x){
        COLORREF c=palette[bits[y*stride+x]-'0'];
        pixels[y*width+x]=((DWORD)GetRValue(c)<<16)|((DWORD)GetGValue(c)<<8)|GetBValue(c);
    }
    bi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth=width;bi.bmiHeader.biHeight=-18;
    bi.bmiHeader.biPlanes=1;bi.bmiHeader.biBitCount=32;
    /* Arrow OEM artwork includes its left separator, CLOSE does not.
     * The caption supplies the other separators. Scale the raster once. */
    SetStretchBltMode(dc,COLORONCOLOR);
    StretchDIBits(dc,r.left,r.top+u,r.right-r.left-u,r.bottom-r.top-2*u,
        0,0,width,18,pixels,&bi,DIB_RGB_COLORS,SRCCOPY);
}

static void paint(HWND w,style_window *s,HDC target)
{
    win16_style_layout l;
    win16_style_palette p;
    HDC dc=target?target:GetWindowDC(w);
    RECT r;int saved,len;
    WCHAR *title;
    if(!dc)return;
    if(!layout(w,s,&l)){if(!target)ReleaseDC(w,dc);return;}
    colors(s,&p);saved=SaveDC(dc);
    ExcludeClipRect(dc,l.client.left,l.client.top,l.client.right,l.client.bottom);
    fill(dc,l.outer,l.dialog_frame?(s->active?p.active:p.inactive):p.face);
    outline(dc,l.outer,l.unit,p.edge);
    /* The reference frame has a grey band between two dark outlines, not
     * the white upper/left bevel used by later Windows non-client frames. */
    r=l.outer;InflateRect(&r,-l.border+l.unit,-l.border+l.unit);
    outline(dc,r,l.unit,l.dialog_frame?p.light:p.edge);
    fill(dc,l.caption,s->active?p.active:p.inactive);
    r=l.caption;r.top=r.bottom;r.bottom+=l.unit;fill(dc,r,p.edge);
    button(dc,l.system,HTSYSMENU,s->pressed==HTSYSMENU&&s->hot,FALSE,l.unit,&p);
    button(dc,l.minimize,HTMINBUTTON,s->pressed==HTMINBUTTON&&s->hot,FALSE,l.unit,&p);
    button(dc,l.maximize,HTMAXBUTTON,s->pressed==HTMAXBUTTON&&s->hot,IsZoomed(w),l.unit,&p);
    len=GetWindowTextLengthW(w);
    title=(WCHAR *)calloc((size_t)len+1,sizeof(WCHAR));
    if(title){
        HDC text_dc;HBITMAP bitmap,prior;
        RECT logical;
        GetWindowTextW(w,title,len+1);
        r=l.title;InflateRect(&r,-2*l.unit,0);
        /* System is a discrete raster font. Asking GDI for a 24-pixel cell
         * selects its 20-pixel strike, not a scaled 16-pixel VGA strike.
         * Render the base cell, then scale once with the rest of the metrics. */
        SetRect(&logical,0,0,max(1,MulDiv(r.right-r.left,96,(int)s->dpi)),20);
        text_dc=CreateCompatibleDC(dc);
        bitmap=CreateCompatibleBitmap(dc,logical.right,logical.bottom);
        if(text_dc && bitmap){
            prior=(HBITMAP)SelectObject(text_dc,bitmap);
            fill(text_dc,logical,s->active?p.active:p.inactive);
            SelectObject(text_dc,s->font?s->font:GetStockObject(SYSTEM_FONT));
            SetBkMode(text_dc,TRANSPARENT);
            SetTextColor(text_dc,s->active?p.active_text:p.inactive_text);
            DrawTextW(text_dc,title,-1,&logical,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX|DT_END_ELLIPSIS);
            SetStretchBltMode(dc,COLORONCOLOR);
            StretchBlt(dc,r.left,r.top,max(0,r.right-r.left),r.bottom-r.top,
                text_dc,0,0,logical.right,logical.bottom,SRCCOPY);
            SelectObject(text_dc,prior);
        }
        if(text_dc)DeleteDC(text_dc);
        if(bitmap)DeleteObject(bitmap);
        free(title);
    }
    RestoreDC(dc,saved);if(!target)ReleaseDC(w,dc);
}

static BOOL refresh_font(HWND w,style_window *s)
{
    HFONT font;
    /* A private face alias prevents GDI selecting the host's same-named
     * System font. Only size/face metadata in this temporary copy changes;
     * original widths, bitmaps and copyright remain unchanged. */
    if(!s->font_resource){
        static const char face[]="Win31 VGA System";
        DWORD size=(DWORD)sizeof(style_system_font)+sizeof(face);
        DWORD offset=(DWORD)sizeof(style_system_font),count=0;
        BYTE *copy=(BYTE *)malloc(size);
        if(!copy)return FALSE;
        memcpy(copy,style_system_font,sizeof(style_system_font));
        memcpy(copy+offset,face,sizeof(face));
        memcpy(copy+2,&size,sizeof(size));memcpy(copy+105,&offset,sizeof(offset));
        s->font_resource=AddFontMemResourceEx(copy,size,NULL,&count);free(copy);
        if(!s->font_resource)return FALSE;
    }
    s->dpi=GetDpiForWindow(w);if(!s->dpi)s->dpi=96;
    font=CreateFontW(16,0,0,0,FW_BOLD,FALSE,FALSE,FALSE,
        ANSI_CHARSET,OUT_RASTER_PRECIS,CLIP_DEFAULT_PRECIS,NONANTIALIASED_QUALITY,
        DEFAULT_PITCH,L"Win31 VGA System");
    if(font){if(s->font)DeleteObject(s->font);s->font=font;}
    return font!=NULL;
}

static void frame_changed(HWND w)
{
    SetWindowPos(w,NULL,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER|
        SWP_NOACTIVATE|SWP_FRAMECHANGED);
    RedrawWindow(w,NULL,NULL,RDW_FRAME|RDW_INVALIDATE|RDW_UPDATENOW);
}

static void client_screen(HWND w,RECT *r)
{
    POINT p={0,0};GetClientRect(w,r);ClientToScreen(w,&p);OffsetRect(r,p.x,p.y);
}

static void restore_client(HWND w,const RECT *wanted)
{
    RECT r,outer;client_screen(w,&r);GetWindowRect(w,&outer);
    SetWindowPos(w,NULL,outer.left+wanted->left-r.left,outer.top+wanted->top-r.top,
        outer.right-outer.left+(wanted->right-wanted->left)-(r.right-r.left),
        outer.bottom-outer.top+(wanted->bottom-wanted->top)-(r.bottom-r.top),
        SWP_NOZORDER|SWP_NOACTIVATE);
}

static void dispose(style_window *s)
{
    if(s->font)DeleteObject(s->font);
    if(s->font_resource)RemoveFontMemResourceEx(s->font_resource);
    free(s);
}

BOOL win16_style_install(HWND w,const win16_style_palette *p)
{
    style_window *s;
    DWORD style,ex;
    RECT client;
    DWORD policy=DWMNCRP_DISABLED;
    if(!local_window(w))return FALSE;
    if(state(w)){SetLastError(ERROR_ALREADY_EXISTS);return FALSE;}
    style=(DWORD)GetWindowLongPtr(w,GWL_STYLE);ex=(DWORD)GetWindowLongPtr(w,GWL_EXSTYLE);
    if((style&WS_CAPTION)!=WS_CAPTION || (style&(WS_CHILD|WS_HSCROLL|WS_VSCROLL)) ||
        (ex&(WS_EX_CLIENTEDGE|WS_EX_TOOLWINDOW|WS_EX_LAYERED)) || GetMenu(w) ||
        IsIconic(w) || IsZoomed(w)){
        SetLastError(ERROR_NOT_SUPPORTED);return FALSE;
    }
    s=(style_window *)calloc(1,sizeof(*s));
    if(!s){SetLastError(ERROR_NOT_ENOUGH_MEMORY);return FALSE;}
    if(p)s->palette=*p;else win16_style_default_palette(&s->palette);
    s->active=GetActiveWindow()==w;
    if(!refresh_font(w,s)){dispose(s);SetLastError(ERROR_INVALID_DATA);return FALSE;}
    client_screen(w,&client);
    s->policy_saved=SUCCEEDED(DwmGetWindowAttribute(w,DWMWA_NCRENDERING_POLICY,
        &s->old_policy,sizeof(s->old_policy)));
    if(!SetWindowSubclass(w,style_proc,STYLE_ID,(DWORD_PTR)s)){dispose(s);return FALSE;}
    DwmSetWindowAttribute(w,DWMWA_NCRENDERING_POLICY,&policy,sizeof(policy));
    ++s->calls;
    frame_changed(w);
    if(!s->detached)restore_client(w,&client);
    --s->calls;
    if(s->detached){dispose(s);SetLastError(ERROR_INVALID_WINDOW_HANDLE);return FALSE;}
    return TRUE;
}

BOOL win16_style_remove(HWND w)
{
    style_window *s;RECT client;BOOL normal;
    if(!local_window(w))return FALSE;
    s=state(w);if(!s){SetLastError(ERROR_NOT_FOUND);return FALSE;}
    normal=!IsIconic(w)&&!IsZoomed(w);client_screen(w,&client);
    if(!RemoveWindowSubclass(w,style_proc,STYLE_ID))return FALSE;
    ++s->calls;
    s->detached=TRUE;
    if(s->pressed && GetCapture()==w)ReleaseCapture();
    if(s->policy_saved)DwmSetWindowAttribute(w,DWMWA_NCRENDERING_POLICY,
        &s->old_policy,sizeof(s->old_policy));
    frame_changed(w);if(normal)restore_client(w,&client);
    if(!--s->calls)dispose(s);
    return TRUE;
}

BOOL win16_style_set_palette(HWND w,const win16_style_palette *p)
{
    style_window *s;
    if(!p){SetLastError(ERROR_INVALID_PARAMETER);return FALSE;}
    if(!local_window(w))return FALSE;
    s=state(w);if(!s){SetLastError(ERROR_NOT_FOUND);return FALSE;}
    ++s->calls;s->palette=*p;paint(w,s,NULL);
    if(!--s->calls && s->detached)dispose(s);
    return TRUE;
}

static void system_menu(HWND w,style_window *s)
{
    RECT r;UINT command;DWORD style=(DWORD)GetWindowLongPtr(w,GWL_STYLE);
    HMENU menu=GetSystemMenu(w,FALSE);
    if(!menu)return;
    EnableMenuItem(menu,SC_RESTORE,MF_BYCOMMAND|(IsZoomed(w)||IsIconic(w)?MF_ENABLED:MF_GRAYED));
    EnableMenuItem(menu,SC_SIZE,MF_BYCOMMAND|((style&WS_THICKFRAME)&&!IsZoomed(w)?MF_ENABLED:MF_GRAYED));
    EnableMenuItem(menu,SC_MINIMIZE,MF_BYCOMMAND|((style&WS_MINIMIZEBOX)?MF_ENABLED:MF_GRAYED));
    EnableMenuItem(menu,SC_MAXIMIZE,MF_BYCOMMAND|((style&WS_MAXIMIZEBOX)&&!IsZoomed(w)?MF_ENABLED:MF_GRAYED));
    GetWindowRect(w,&r);
    command=TrackPopupMenu(menu,TPM_RETURNCMD|TPM_LEFTALIGN|TPM_TOPALIGN,
        r.left,r.top+MulDiv(21,(int)s->dpi,96),0,w,NULL);
    if(command)PostMessageW(w,WM_SYSCOMMAND,command,0);
}

static LRESULT CALLBACK style_proc(HWND w,UINT msg,WPARAM wp,LPARAM lp,
    UINT_PTR id,DWORD_PTR data)
{
    style_window *s=(style_window *)data;
    LRESULT result=0;win16_style_layout l;POINT point;RECT r;
    (void)id;++s->calls;
    switch(msg){
    case WM_NCCALCSIZE:
        r=wp?((NCCALCSIZE_PARAMS *)lp)->rgrc[0]:*(RECT *)lp;
        if(measure_window(w,s,&r,&l)){
            OffsetRect(&l.client,r.left,r.top);
            if(wp)((NCCALCSIZE_PARAMS *)lp)->rgrc[0]=l.client;else *(RECT *)lp=l.client;
            break;
        }
        goto forward;
    case WM_NCPAINT: paint(w,s,NULL);break;
    case WM_NCACTIVATE:
        s->active=(BOOL)wp;result=DefSubclassProc(w,msg,wp,-1);
        if(!s->detached)paint(w,s,NULL);break;
    case WM_NCHITTEST:
        if(layout(w,s,&l)){
            GetWindowRect(w,&r);point.x=GET_X_LPARAM(lp)-r.left;point.y=GET_Y_LPARAM(lp)-r.top;
            result=win16_style_hit(&l,point);break;
        }
        goto forward;
    case WM_NCLBUTTONDBLCLK:
        if(wp==HTSYSMENU){PostMessageW(w,WM_SYSCOMMAND,SC_CLOSE,0);break;}
        goto forward;
    case WM_NCLBUTTONDOWN:
        if(wp==HTSYSMENU){system_menu(w,s);break;}
        if(wp==HTMINBUTTON || wp==HTMAXBUTTON){
            s->pressed=(int)wp;s->hot=TRUE;SetCapture(w);paint(w,s,NULL);break;
        }
        goto forward;
    case WM_MOUSEMOVE:
    case WM_LBUTTONUP:
        if(s->pressed && layout(w,s,&l)){
            int pressed=s->pressed;
            point.x=GET_X_LPARAM(lp);point.y=GET_Y_LPARAM(lp);ClientToScreen(w,&point);
            GetWindowRect(w,&r);point.x-=r.left;point.y-=r.top;
            s->hot=win16_style_hit(&l,point)==pressed;
            if(msg==WM_LBUTTONUP){
                BOOL invoke=s->hot;s->pressed=0;ReleaseCapture();
                if(invoke)PostMessageW(w,WM_SYSCOMMAND,pressed==HTMINBUTTON?SC_MINIMIZE:
                    (IsZoomed(w)?SC_RESTORE:SC_MAXIMIZE),0);
            }
            paint(w,s,NULL);break;
        }
        goto forward;
    case WM_CANCELMODE:
    case WM_CAPTURECHANGED:
        s->pressed=0;s->hot=FALSE;
        if(msg==WM_CANCELMODE && GetCapture()==w)ReleaseCapture();
        paint(w,s,NULL);goto forward;
    case WM_PRINT:
        result=DefSubclassProc(w,msg,wp,lp&~PRF_NONCLIENT);
        if(!s->detached && (lp&PRF_NONCLIENT))paint(w,s,(HDC)wp);break;
    case WM_SETTEXT:
        result=DefSubclassProc(w,msg,wp,lp);
        if(!s->detached)paint(w,s,NULL);break;
    case WM_DPICHANGED:
        result=DefSubclassProc(w,msg,wp,lp);
        if(!s->detached){refresh_font(w,s);frame_changed(w);}break;
    case WM_SETTINGCHANGE:
    case WM_SYSCOLORCHANGE:
        result=DefSubclassProc(w,msg,wp,lp);
        if(!s->detached)paint(w,s,NULL);break;
    case WM_NCDESTROY:
        RemoveWindowSubclass(w,style_proc,STYLE_ID);s->detached=TRUE;
        result=DefSubclassProc(w,msg,wp,lp);break;
    default:
forward:
        result=DefSubclassProc(w,msg,wp,lp);break;
    }
    if(!--s->calls && s->detached)dispose(s);
    return result;
}
