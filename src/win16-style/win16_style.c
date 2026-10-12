/* Project-authored experimental non-client presentation, not imported USER.
 * See T445 S1 provenance/design: original NTUSER server is a stopping boundary.
 */
#include "win16_style.h"
#include "layout.h"
#include "frame_buttons.h"
#include <commctrl.h>
#include <dwmapi.h>
#include <windowsx.h>
#include <stdlib.h>
#include <string.h>

typedef struct style_window {
    win16_style_palette palette;
    HFONT font;
    UINT dpi, calls;
    BOOL active, detached, hot, system_menu;
    int pressed;
    DWORD old_policy;
    BOOL policy_saved;
} style_window;

static LRESULT CALLBACK style_proc(HWND,UINT,WPARAM,LPARAM,UINT_PTR,DWORD_PTR);
#define STYLE_ID ((UINT_PTR)style_proc)
/* UxTheme's non-client draw requests are not in the public SDK. They are
 * observed compatibility messages, not structures we interpret or APIs we
 * invoke. An attached custom frame must not delegate their drawing to USER.
 * See resources.md for the boundary and provenance. */
#define STYLE_NCUAHDRAWCAPTION 0x00ae
#define STYLE_NCUAHDRAWFRAME   0x00af

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
    fill(dc,r,kind==HTSYSMENU&&down?p->shadow:p->face);outline(dc,r,u,p->edge);
    if(kind==HTSYSMENU){
        /* CLOSE contains two side-by-side 18-pixel images. Original USER
         * uses the first for top-level windows, second for WS_CHILD.
         * Child/MDI frames are not yet part of this experiment. */
        bits=&system_menu_bitmap[0][0];width=18;stride=37;
        /* VGA selection inverts palette roles, not RGB bytes: light grey
         * becomes dark grey, black becomes white. No second CLOSE bitmap. */
        if(down){palette[0]=p->light;palette[1]=p->edge;
            palette[2]=p->face;palette[3]=p->shadow;}
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
    StretchDIBits(dc,r.left,r.top,r.right-r.left-(kind==HTSYSMENU?u:0),r.bottom-r.top,
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
    if(!l.border && IsRectEmpty(&l.caption)){if(!target)ReleaseDC(w,dc);return;}
    colors(s,&p);saved=SaveDC(dc);
    ExcludeClipRect(dc,l.client.left,l.client.top,l.client.right,l.client.bottom);
    fill(dc,l.outer,l.dialog_frame?(s->active?p.active:p.inactive):p.face);
    outline(dc,l.outer,l.unit,p.edge);
    /* The reference frame has a grey band between two dark outlines, not
     * the white upper/left bevel used by later Windows non-client frames. */
    r=l.outer;InflateRect(&r,-l.border+l.unit,-l.border+l.unit);
    outline(dc,r,l.unit,l.dialog_frame?p.light:p.edge);
    if(l.sizeable && !l.dialog_frame){
        /* Original USER 1:A06B..A1DE divides the flat sizing band into
         * corner and edge regions. These are strokes, not a bevel. */
        int corner=l.corner.cx-l.unit,vertical=l.corner.cy-l.unit,i;
        for(i=0;i<2;++i){
            int x=i?l.outer.right-corner-l.unit:l.outer.left+corner;
            int y=i?l.outer.bottom-vertical-l.unit:l.outer.top+vertical;
            if(l.outer.right-l.outer.left>2*corner+l.unit){
                SetRect(&r,x,l.outer.top+l.unit,x+l.unit,l.outer.top+l.border-l.unit);fill(dc,r,p.edge);
                OffsetRect(&r,0,l.outer.bottom-l.outer.top-l.border);fill(dc,r,p.edge);
            }
            if(l.outer.bottom-l.outer.top>2*vertical+l.unit){
                SetRect(&r,l.outer.left+l.unit,y,l.outer.left+l.border-l.unit,y+l.unit);fill(dc,r,p.edge);
                OffsetRect(&r,l.outer.right-l.outer.left-l.border,0);fill(dc,r,p.edge);
            }
        }
    }
    if(IsRectEmpty(&l.caption)){
        RestoreDC(dc,saved);if(!target)ReleaseDC(w,dc);return;
    }
    fill(dc,l.caption,s->active?p.active:p.inactive);
    r=l.caption;r.top=r.bottom;r.bottom+=l.unit;fill(dc,r,p.edge);
    button(dc,l.system,HTSYSMENU,s->system_menu||(s->pressed==HTSYSMENU&&s->hot),FALSE,l.unit,&p);
    button(dc,l.minimize,HTMINBUTTON,s->pressed==HTMINBUTTON&&s->hot,FALSE,l.unit,&p);
    button(dc,l.maximize,HTMAXBUTTON,s->pressed==HTMAXBUTTON&&s->hot,IsZoomed(w),l.unit,&p);
    len=GetWindowTextLengthW(w);
    title=(WCHAR *)calloc((size_t)len+1,sizeof(WCHAR));
    if(title){
        HDC text_dc;HBITMAP bitmap,prior;
        RECT logical;
        GetWindowTextW(w,title,len+1);
        r=l.title;
        /* System is a discrete raster font. Asking GDI for a 24-pixel cell
         * selects its 20-pixel strike, not a scaled 16-pixel VGA strike.
         * Render the base cell, then scale once with the rest of the metrics. */
        /* Owner-approved typography exception: retain the accepted 20-row
         * text raster's scaling phase, independently of the 18-row buttons. */
        SetRect(&logical,0,0,max(1,MulDiv(r.right-r.left,96,(int)s->dpi)),20);
        text_dc=CreateCompatibleDC(dc);
        bitmap=CreateCompatibleBitmap(dc,logical.right,logical.bottom);
        if(text_dc && bitmap){
            SIZE extent;
            prior=(HBITMAP)SelectObject(text_dc,bitmap);
            fill(text_dc,logical,s->active?p.active:p.inactive);
            SelectObject(text_dc,s->font?s->font:GetStockObject(SYSTEM_FONT));
            SetBkMode(text_dc,TRANSPARENT);
            SetTextColor(text_dc,s->active?p.active_text:p.inactive_text);
            /* USER 1:A526..A5DE measures, centres if it fits, otherwise
             * clips from the left border. It does not synthesize ellipses. */
            if(GetTextExtentPoint32W(text_dc,title,len,&extent))
                ExtTextOutW(text_dc,max(1,(logical.right-extent.cx)/2),
                    max(0,(logical.bottom-extent.cy)/2),ETO_CLIPPED,&logical,title,(UINT)len,NULL);
            {int text_height=MulDiv(20,(int)s->dpi,96),clip=SaveDC(dc);
            IntersectClipRect(dc,r.left,r.top,r.right,r.bottom);
            SetStretchBltMode(dc,COLORONCOLOR);
            StretchBlt(dc,r.left,r.top+(r.bottom-r.top-text_height)/2,
                max(0,r.right-r.left),text_height,text_dc,0,0,logical.right,logical.bottom,SRCCOPY);
            RestoreDC(dc,clip);}
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
    s->dpi=GetDpiForWindow(w);if(!s->dpi)s->dpi=96;
    font=CreateFontW(16,0,0,0,FW_BOLD,FALSE,FALSE,FALSE,
        DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,NONANTIALIASED_QUALITY,
        DEFAULT_PITCH,L"System");
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
    if((style&(WS_CHILD|WS_HSCROLL|WS_VSCROLL)) ||
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
    case WM_SETCURSOR:
        /* Default NC cursor handling can also repaint native caption buttons.
         * Only our frame belongs to us; preserve the application's client
         * cursor and error handling. No hover artwork exists in Win3.1. */
        if(!IsIconic(w) && (HWND)wp==w && (short)LOWORD(lp)>HTCLIENT){
            LPCWSTR cursor=IDC_ARROW;
            switch((short)LOWORD(lp)){
            case HTLEFT: case HTRIGHT: cursor=IDC_SIZEWE;break;
            case HTTOP: case HTBOTTOM: cursor=IDC_SIZENS;break;
            case HTSIZE: case HTTOPLEFT: case HTBOTTOMRIGHT: cursor=IDC_SIZENWSE;break;
            case HTTOPRIGHT: case HTBOTTOMLEFT: cursor=IDC_SIZENESW;break;
            }
            SetCursor(LoadCursorW(NULL,cursor));result=TRUE;break;
        }
        goto forward;
    case STYLE_NCUAHDRAWCAPTION:
    case STYLE_NCUAHDRAWFRAME:
    case WM_NCMOUSEMOVE:
    case WM_NCMOUSEHOVER:
    case WM_NCMOUSELEAVE:
        if(IsIconic(w))goto forward; /* S3 retains native minimized presentation. */
        /* DWMNCRP_DISABLED does not disable USER/UxTheme painting. In
         * particular default NC hover paints native caption buttons without
         * asking our WM_NCPAINT owner, potentially into our client area.
         * Win3.1 buttons have no hover drawing. Press tracking remains in
         * WM_MOUSEMOVE below; menu/drag/size commands still go to USER. */
        break;
    case WM_NCACTIVATE:
        s->active=(BOOL)wp;
        if(IsIconic(w))goto forward;
        /* Own activation drawing too. Forwarding even with lp=-1 leaves
         * native themed-caption pixels below the shorter custom caption.
         * TRUE permits deactivation; WM_ACTIVATE/focus still go downstream. */
        result=TRUE;paint(w,s,NULL);break;
    case WM_NCHITTEST:
        if(layout(w,s,&l)){
            GetWindowRect(w,&r);point.x=GET_X_LPARAM(lp)-r.left;point.y=GET_Y_LPARAM(lp)-r.top;
            result=win16_style_hit(&l,point);break;
        }
        goto forward;
    case WM_INITMENUPOPUP:
        result=DefSubclassProc(w,msg,wp,lp);
        if(!s->detached && HIWORD(lp)){
            s->system_menu=TRUE;paint(w,s,NULL);
        }
        break;
    case WM_EXITMENULOOP:
        result=DefSubclassProc(w,msg,wp,lp);
        if(!s->detached && s->system_menu){s->system_menu=FALSE;paint(w,s,NULL);}
        break;
    case WM_NCLBUTTONDOWN:
        if(wp==HTSYSMENU){
            MENUBARINFO menu={sizeof(menu)};
            /* Modern dialog frames can have a system menu but no native
             * icon rectangle. Their native mouse-menu entry has no button
             * to track; use the same system-menu entry as Alt+Space. */
            if(GetMenuBarInfo(w,OBJID_SYSMENU,0,&menu) && IsRectEmpty(&menu.rcBar)){
                /* Do not enter a keyboard menu with the mouse still down:
                 * its subsequent release would immediately dismiss it. */
                s->pressed=HTSYSMENU;s->hot=TRUE;SetCapture(w);paint(w,s,NULL);break;
            }
        }
        /* Native system-menu tracking owns the second click, cancellation
         * and disabled SC_CLOSE semantics. TrackPopupMenu is not equivalent
         * to the non-client menu loop. Double clicks also go to the owner. */
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
                if(invoke)PostMessageW(w,WM_SYSCOMMAND,pressed==HTSYSMENU?SC_KEYMENU:
                    pressed==HTMINBUTTON?SC_MINIMIZE:(IsZoomed(w)?SC_RESTORE:SC_MAXIMIZE),
                    pressed==HTSYSMENU?L' ':0);
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
    case WM_THEMECHANGED:
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
