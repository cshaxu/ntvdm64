#include "layout.h"
#include <string.h>

BOOL win16_style_measure(int width,int height,UINT dpi,DWORD style,DWORD ex_style,
    BOOL maximized,win16_style_layout *l)
{
    int top,button,caption_height,right;BOOL caption=(style&WS_CAPTION)==WS_CAPTION;
    if(!l || width<1 || height<1 || dpi<48 || dpi>768)return FALSE;
    memset(l,0,sizeof(*l));
    l->unit=max(1,(int)dpi/96);
    l->sizeable=(style&WS_THICKFRAME)!=0 && !maximized;
    l->dialog_frame=(ex_style&WS_EX_DLGMODALFRAME)!=0;
    l->border=MulDiv(l->dialog_frame?6:l->sizeable?4:caption?3:
        (style&WS_DLGFRAME)?3:(style&WS_BORDER)?1:0,(int)dpi,96);
    top=l->border;button=MulDiv(19,(int)dpi,96);
    /* Original VGA: 18-pixel artwork, 19-pixel button pitch. USER's
     * 20-pixel CYCAPTION includes both boundary lines, not 20 content rows. */
    caption_height=MulDiv(18,(int)dpi,96);
    SetRect(&l->outer,0,0,width,height);
    if(!caption){
        SetRect(&l->client,l->border,l->border,max(l->border,width-l->border),
            max(l->border,height-l->border));
        return TRUE;
    }
    SetRect(&l->caption,l->border,top,max(l->border,width-l->border),top+caption_height);
    SetRect(&l->client,l->border,top+caption_height+l->unit,
        max(l->border,width-l->border),max(top+caption_height+l->unit,height-l->border));
    l->title=l->caption;
    if(style&WS_SYSMENU) {
        l->system=l->caption;l->system.right=l->system.left+button;
        l->title.left=l->system.right;
    }
    right=l->caption.right;
    if(style&WS_MAXIMIZEBOX) {
        l->maximize=l->caption;l->maximize.left=max(l->caption.left,right-button);
        right=l->maximize.left;
    }
    if(style&WS_MINIMIZEBOX) {
        l->minimize=l->caption;l->minimize.right=right;
        l->minimize.left=max(l->caption.left,right-button);right=l->minimize.left;
    }
    l->title.right=max(l->title.left,right);
    return TRUE;
}

LRESULT win16_style_hit(const win16_style_layout *l,POINT p)
{
    BOOL left,right,top,bottom;
    if(!l || !PtInRect(&l->outer,p))return HTNOWHERE;
    if(PtInRect(&l->system,p))return HTSYSMENU;
    if(PtInRect(&l->minimize,p))return HTMINBUTTON;
    if(PtInRect(&l->maximize,p))return HTMAXBUTTON;
    if(PtInRect(&l->caption,p))return HTCAPTION;
    if(PtInRect(&l->client,p))return HTCLIENT;
    if(!l->sizeable)return HTBORDER;
    left=p.x<l->border;right=p.x>=l->outer.right-l->border;
    top=p.y<l->border;bottom=p.y>=l->outer.bottom-l->border;
    if(top)return left?HTTOPLEFT:right?HTTOPRIGHT:HTTOP;
    if(bottom)return left?HTBOTTOMLEFT:right?HTBOTTOMRIGHT:HTBOTTOM;
    return left?HTLEFT:right?HTRIGHT:HTBORDER;
}
