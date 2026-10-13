#include "controls.h"
#include "control_glyphs.h"
#include "combo_glyph.h"
#include "scroll_glyphs.h"
#include "scroll_track.h"
#include "font.h"
#include <commctrl.h>
#include <oleacc.h>
#include <stdlib.h>
#include <limits.h>

typedef struct control_style {
    win16_style_palette palette;
    unsigned calls;
    BOOL detached,removing;
    HFONT font,original_font;
    BOOL edit_border;
    HWND combo_list;
    struct control_style *owner;
    win16_scroll_track scroll;
    UINT_PTR scroll_timer;
    unsigned scroll_epoch;
    BOOL scroll_reporting,scroll_key;
    LONG scroll_report;
    BOOL list_scrollbar,edit_scrollbars,frame_scrollbars,list_redraw_off;
    unsigned list_suppression;
    HWND list_capture_before;
    LONG embedded_object;
    BOOL scroll_caret,scroll_caret_shown;
    SIZE scroll_caret_size;
    int tab_count,*tab_positions;
} control_style;
static void dispose_control(control_style *s)
{if(s->font)win16_style_font_release(s->font);free(s->tab_positions);free(s);}
static LRESULT CALLBACK control_proc(HWND,UINT,WPARAM,LPARAM,UINT_PTR,DWORD_PTR);
static BOOL is_static(HWND w)
{WCHAR name[16];return GetClassNameW(w,name,16) && !lstrcmpiW(name,L"STATIC");}
static BOOL is_edit(HWND w)
{WCHAR name[16];return GetClassNameW(w,name,16) && !lstrcmpiW(name,L"EDIT");}
static BOOL is_list(HWND w)
{WCHAR name[16];return GetClassNameW(w,name,16) && (!lstrcmpiW(name,L"LISTBOX") || !lstrcmpiW(name,L"ComboLBox"));}
static BOOL is_combo(HWND w)
{WCHAR name[16];return GetClassNameW(w,name,16) && !lstrcmpiW(name,L"COMBOBOX");}
static BOOL is_scroll(HWND w)
{WCHAR name[16];return GetClassNameW(w,name,16) && !lstrcmpiW(name,L"SCROLLBAR");}

static BOOL supported(HWND w)
{
    WCHAR name[16];DWORD style=(DWORD)GetWindowLongPtrW(w,GWL_STYLE),type=style&BS_TYPEMASK;
    if(is_scroll(w))return !(style&SBS_SIZEGRIP);
    if(is_combo(w))return ((style&3)==CBS_DROPDOWNLIST || (style&3)==CBS_DROPDOWN || (style&3)==CBS_SIMPLE) &&
        !(style&(CBS_OWNERDRAWFIXED|CBS_OWNERDRAWVARIABLE));
    if(is_list(w))return !(style&(LBS_OWNERDRAWFIXED|LBS_OWNERDRAWVARIABLE|LBS_NODATA|LBS_USETABSTOPS));
    if(is_edit(w))return !(GetWindowLongPtrW(w,GWL_EXSTYLE)&WS_EX_CLIENTEDGE);
    if(is_static(w)){type=style&SS_TYPEMASK;return type==SS_LEFT || type==SS_CENTER || type==SS_RIGHT ||
        type==SS_ICON || type==SS_BITMAP || type==SS_SIMPLE || type==SS_LEFTNOWORDWRAP || (type>=SS_BLACKRECT && type<=SS_WHITEFRAME);}
    if(!GetClassNameW(w,name,16) || lstrcmpiW(name,L"BUTTON") || (style&(BS_ICON|BS_BITMAP|BS_PUSHLIKE)))return FALSE;
    if(type==BS_PUSHBUTTON || type==BS_DEFPUSHBUTTON || type==BS_GROUPBOX)return TRUE;
    return type==BS_CHECKBOX || type==BS_AUTOCHECKBOX || type==BS_RADIOBUTTON ||
        type==BS_AUTORADIOBUTTON || type==BS_3STATE || type==BS_AUTO3STATE;
}

static void frame(HDC dc,RECT r,COLORREF color,int width)
{
    HBRUSH brush=CreateSolidBrush(color);int i;
    if(!brush)return;for(i=0;i<width && r.right>r.left && r.bottom>r.top;++i){FrameRect(dc,&r,brush);InflateRect(&r,-1,-1);}
    DeleteObject(brush);
}

static void other_control(HWND w,control_style *s,HDC dc,RECT r,HFONT font,const WCHAR *text,BOOL label)
{
    DWORD style=(DWORD)GetWindowLongPtrW(w,GWL_STYLE);UINT type=style&(label?SS_TYPEMASK:BS_TYPEMASK);
    int u=max(1,MulDiv(1,(int)GetDpiForWindow(w),96));RECT bounds=r;TEXTMETRICW tm;
    UINT flags=DT_LEFT;BOOL pushed=!label && (SendMessageW(w,BM_GETSTATE,0,0)&BST_PUSHED);
    if(s->detached)return;
    SelectObject(dc,font);GetTextMetricsW(dc,&tm);SetBkMode(dc,TRANSPARENT);SetTextColor(dc,s->palette.edge);
    if(label && type>=SS_BLACKRECT && type<=SS_WHITEFRAME){
        COLORREF color=(type==SS_BLACKRECT || type==SS_BLACKFRAME)?s->palette.edge:
            (type==SS_GRAYRECT || type==SS_GRAYFRAME)?s->palette.shadow:s->palette.light;
        if(type>=SS_BLACKFRAME)frame(dc,r,color,u);
        else {SetDCBrushColor(dc,color);FillRect(dc,&r,(HBRUSH)GetStockObject(DC_BRUSH));}return;
    }
    if(!label && type==BS_GROUPBOX){
        RECT title=r;title.left+=8*u;DrawTextW(dc,text,-1,&title,DT_CALCRECT|DT_SINGLELINE);
        bounds.top+=tm.tmHeight/2;frame(dc,bounds,s->palette.shadow,u);
        InflateRect(&title,u,0);SetDCBrushColor(dc,s->palette.light);FillRect(dc,&title,(HBRUSH)GetStockObject(DC_BRUSH));
        title.left+=u;DrawTextW(dc,text,-1,&title,DT_SINGLELINE);return;
    }
    if(!label){
        /* Bounded Win3.x trial: hard outer edge and raised light/shadow bands.
         * Native default/pressed state remains the button's own state. */
        if(type==BS_DEFPUSHBUTTON){frame(dc,bounds,s->palette.edge,u);InflateRect(&bounds,-2*u,-2*u);}
        SetDCBrushColor(dc,s->palette.face);FillRect(dc,&bounds,(HBRUSH)GetStockObject(DC_BRUSH));
        frame(dc,bounds,s->palette.edge,u);InflateRect(&bounds,-u,-u);
        {RECT strip=bounds;COLORREF light=pushed?s->palette.shadow:s->palette.light;
            COLORREF dark=pushed?s->palette.light:s->palette.shadow;
            SetDCBrushColor(dc,light);strip.bottom=strip.top+2*u;FillRect(dc,&strip,(HBRUSH)GetStockObject(DC_BRUSH));
            strip=bounds;strip.right=strip.left+2*u;FillRect(dc,&strip,(HBRUSH)GetStockObject(DC_BRUSH));
            SetDCBrushColor(dc,dark);strip=bounds;strip.top=strip.bottom-2*u;FillRect(dc,&strip,(HBRUSH)GetStockObject(DC_BRUSH));
            strip=bounds;strip.left=strip.right-2*u;FillRect(dc,&strip,(HBRUSH)GetStockObject(DC_BRUSH));}
        InflateRect(&bounds,-3*u,-3*u);if(pushed)OffsetRect(&bounds,u,u);
        flags=DT_CENTER|DT_VCENTER|DT_SINGLELINE;
    }else {flags=type==SS_CENTER?DT_CENTER:type==SS_RIGHT?DT_RIGHT:DT_LEFT;
        /* StaticCallback expands tabs and preserves explicit line breaks.
         * LEFTNOWORDWRAP removes word wrapping, not multiline text. SIMPLE
         * is the distinct single-line, always-enabled original fast path. */
        flags|=type==SS_SIMPLE?DT_SINGLELINE:DT_EXPANDTABS|(type==SS_LEFTNOWORDWRAP?0:DT_WORDBREAK);
        if(style&SS_NOPREFIX)flags|=DT_NOPREFIX;}
    if(!IsWindowEnabled(w) && !(label && type==SS_SIMPLE))SetTextColor(dc,s->palette.shadow);
    DrawTextW(dc,text,-1,&bounds,flags);
    if(!label && GetFocus()==w)DrawFocusRect(dc,&bounds);
}

static void draw(HWND w,control_style *s,HDC dc)
{
    RECT r,glyph,label,focus,pixel;int saved,x,y,row,column,height,length;
    UINT dpi=GetDpiForWindow(w);DWORD style=(DWORD)GetWindowLongPtrW(w,GWL_STYLE);
    UINT type=style&BS_TYPEMASK;LRESULT state,check;WCHAR *text;HFONT font;TEXTMETRICW tm;
    BOOL radio=type==BS_RADIOBUTTON || type==BS_AUTORADIOBUTTON;
    BOOL label_control=is_static(w);
    if(label_control && (style&SS_TYPEMASK)==SS_ICON){
        /* statctl.c retains image ownership/sizing in native STATIC. The
         * painter borrows its current handle, never reads image data as text.
         * CENTERIMAGE is a host extension; its natural-size image is clipped. */
        HICON icon=(HICON)SendMessageW(w,STM_GETICON,0,0);int cx,cy;
        if(s->detached || !IsWindow(w))return;
        saved=SaveDC(dc);if(!saved)return;
        GetClientRect(w,&r);cx=r.right;cy=r.bottom;x=y=0;
        IntersectClipRect(dc,r.left,r.top,r.right,r.bottom);
        SetDCBrushColor(dc,s->palette.light);FillRect(dc,&r,(HBRUSH)GetStockObject(DC_BRUSH));
        if(icon && (style&SS_CENTERIMAGE)){
            ICONINFO info;BITMAP bits;
            if(GetIconInfo(icon,&info)){
                if(GetObjectW(info.hbmColor?info.hbmColor:info.hbmMask,sizeof(bits),&bits)){
                    cx=bits.bmWidth;cy=info.hbmColor?bits.bmHeight:bits.bmHeight/2;
                    x=(r.right-cx)/2;y=(r.bottom-cy)/2;
                }
                if(info.hbmColor)DeleteObject(info.hbmColor);
                if(info.hbmMask)DeleteObject(info.hbmMask);
            }
        }
        if(icon)DrawIconEx(dc,x,y,icon,cx,cy,0,NULL,DI_NORMAL);
        RestoreDC(dc,saved);return;
    }
    state=label_control?0:SendMessageW(w,BM_GETSTATE,0,0);check=label_control?0:SendMessageW(w,BM_GETCHECK,0,0);
    length=GetWindowTextLengthW(w);text=calloc((size_t)length+1,sizeof(WCHAR));
    if(!text)return;GetWindowTextW(w,text,length+1);
    font=(HFONT)SendMessageW(w,WM_GETFONT,0,0);
    if(s->detached || !IsWindow(w)){free(text);return;}
    if(!font)font=(HFONT)GetStockObject(SYSTEM_FONT);
    saved=SaveDC(dc);if(!saved){free(text);return;}
    GetClientRect(w,&r);SetDCBrushColor(dc,s->palette.light);
    /* A group box surrounds sibling controls, not an opaque child surface.
     * Repainting its interior erases those siblings during ordinary motion. */
    if(label_control || type!=BS_GROUPBOX)FillRect(dc,&r,(HBRUSH)GetStockObject(DC_BRUSH));
    if(label_control || type==BS_PUSHBUTTON || type==BS_DEFPUSHBUTTON || type==BS_GROUPBOX){
        other_control(w,s,dc,r,font,text,label_control);RestoreDC(dc,saved);free(text);return;}
    height=MulDiv(13,(int)dpi,96);glyph=r;glyph.top+=(r.bottom-height)/2;
    glyph.bottom=glyph.top+height;glyph.right=glyph.left+MulDiv(14,(int)dpi,96);
    if(style&BS_LEFTTEXT)OffsetRect(&glyph,r.right-glyph.right,0);
    row=radio?1:check==BST_INDETERMINATE?2:0;
    column=(check!=BST_UNCHECKED?1:0)+((state&BST_PUSHED)?2:0);
    for(y=0;y<13;++y)for(x=0;x<14;++x){
        SetDCBrushColor(dc,control_checks[row*13+y][column*14+x]=='0'?s->palette.edge:s->palette.light);
        SetRect(&pixel,glyph.left+MulDiv(x,(int)dpi,96),glyph.top+MulDiv(y,(int)dpi,96),
            glyph.left+MulDiv(x+1,(int)dpi,96),glyph.top+MulDiv(y+1,(int)dpi,96));
        FillRect(dc,&pixel,(HBRUSH)GetStockObject(DC_BRUSH));
    }
    label=r;if(style&BS_LEFTTEXT)label.right=glyph.left-MulDiv(4,(int)dpi,96);
    else label.left=glyph.right+MulDiv(4,(int)dpi,96);
    SelectObject(dc,font);GetTextMetricsW(dc,&tm);SetBkMode(dc,TRANSPARENT);SetTextColor(dc,s->palette.edge);
    label.top+=(r.bottom-tm.tmHeight)/2;label.bottom=label.top+tm.tmHeight;
    if(IsWindowEnabled(w))DrawTextW(dc,text,-1,&label,DT_LEFT|DT_SINGLELINE);
    else {HBRUSH brush=CreateSolidBrush(s->palette.edge);
        if(brush){DrawStateW(dc,brush,NULL,(LPARAM)text,0,label.left,label.top,
            max(0,label.right-label.left),tm.tmHeight,DST_PREFIXTEXT|DSS_UNION|DSS_MONO);DeleteObject(brush);}}
    if(state&BST_FOCUS){focus=label;DrawTextW(dc,text,-1,&focus,DT_CALCRECT|DT_SINGLELINE);
        focus.right=min(focus.right,label.right);InflateRect(&focus,1,1);DrawFocusRect(dc,&focus);}
    RestoreDC(dc,saved);free(text);
}

/* The native list owns strings, sorting, selection and scrolling. Query only
 * visible rows; never replace item data or convert application owner-draw. */
static void draw_list(HWND w,control_style *s,HDC dc)
{
    RECT client,row;int saved,index,count,caret;DWORD style;HFONT font;
    if(!dc)return;
    style=(DWORD)GetWindowLongPtrW(w,GWL_STYLE);
    index=(int)SendMessageW(w,LB_GETTOPINDEX,0,0);
    count=(int)SendMessageW(w,LB_GETCOUNT,0,0);
    caret=(int)SendMessageW(w,LB_GETCARETINDEX,0,0);
    font=(HFONT)SendMessageW(w,WM_GETFONT,0,0);
    if(s->detached)return;
    saved=SaveDC(dc);if(!saved)return;
    GetClientRect(w,&client);IntersectClipRect(dc,0,0,client.right,client.bottom);
    SetDCBrushColor(dc,s->palette.light);FillRect(dc,&client,(HBRUSH)GetStockObject(DC_BRUSH));
    SelectObject(dc,font?font:GetStockObject(SYSTEM_FONT));SetBkMode(dc,TRANSPARENT);
    for(;index>=0 && index<count;++index){
        LRESULT length,selected;WCHAR *text;RECT label,visible;int row_saved;
        if(SendMessageW(w,LB_GETITEMRECT,index,(LPARAM)&row)==LB_ERR)break;
        /* Multicolumn indices advance down a column, then to its right.
         * Keep native item geometry/scroll origin; a below-client item does
         * not imply that the following column is invisible. */
        if(style&LBS_MULTICOLUMN){if(row.left>=client.right)break;}
        else if(row.top>=client.bottom)break;
        /* USER's single-column item rect extends its right edge by xOrigin;
         * native drawing separately offsets the DC by that origin. Preserve
         * the same content coordinates, not the public scrollbar's position
         * (SetScrollInfo alone does not change the list's content origin). */
        if(!(style&LBS_MULTICOLUMN))OffsetRect(&row,client.right-row.right,0);
        if(!IntersectRect(&visible,&row,&client))continue;
        length=SendMessageW(w,LB_GETTEXTLEN,index,0);
        if(s->detached || length==LB_ERR)break;
        text=calloc((size_t)length+1,sizeof(WCHAR));if(!text)break;
        if(SendMessageW(w,LB_GETTEXT,index,(LPARAM)text)==LB_ERR){free(text);break;}
        selected=(style&(LBS_MULTIPLESEL|LBS_EXTENDEDSEL))?SendMessageW(w,LB_GETSEL,index,0):
            SendMessageW(w,LB_GETCURSEL,0,0)==index;
        if(s->detached){free(text);break;}
        row_saved=SaveDC(dc);if(!row_saved){free(text);break;}
        IntersectClipRect(dc,visible.left,visible.top,visible.right,visible.bottom);
        SetDCBrushColor(dc,selected>0?s->palette.active:s->palette.light);
        FillRect(dc,&row,(HBRUSH)GetStockObject(DC_BRUSH));
        SetTextColor(dc,!IsWindowEnabled(w)?s->palette.shadow:selected>0?s->palette.active_text:s->palette.edge);
        /* Original LBPrintCallback starts multi-column text at0, ordinary
         * single-column text at2, relative to the native item rectangle. */
        label=row;if(!(style&LBS_MULTICOLUMN))label.left+=2;
        if(style&LBS_USETABSTOPS)
            TabbedTextOutW(dc,label.left,label.top,text,(int)length,s->tab_count,s->tab_positions,row.left);
        else DrawTextW(dc,text,-1,&label,DT_LEFT|DT_SINGLELINE|DT_NOPREFIX);
        if(GetFocus()==w && caret==index)DrawFocusRect(dc,&row);
        RestoreDC(dc,row_saved);
        free(text);
    }
    RestoreDC(dc,saved);
}

static void edit_border(HWND w,control_style *s,HDC target)
{
    HDC dc;RECT bounds;int saved;
    if(!s->edit_border)return;
    dc=target?target:GetDC(w);if(!dc)return;
    saved=SaveDC(dc);if(saved){
        GetClientRect(w,&bounds);frame(dc,bounds,s->palette.edge,max(1,MulDiv(1,(int)GetDpiForWindow(w),96)));
        RestoreDC(dc,saved);
    }
    if(!target)ReleaseDC(w,dc);
}

static void list_border(HWND w,control_style *s,HDC target)
{
    RECT bounds,inside;HDC dc;int saved;BOOL edge=(GetWindowLongPtrW(w,GWL_EXSTYLE)&WS_EX_CLIENTEDGE)!=0;
    if(!(GetWindowLongPtrW(w,GWL_STYLE)&WS_BORDER) && !edge)return;
    if(!GetWindowRect(w,&bounds))return;
    OffsetRect(&bounds,-bounds.left,-bounds.top);
    dc=target?target:GetWindowDC(w);if(!dc)return;
    saved=SaveDC(dc);
    if(saved){
        if(edge){inside=bounds;InflateRect(&inside,-1,-1);frame(dc,inside,s->palette.light,1);}
        frame(dc,bounds,s->palette.edge,1);RestoreDC(dc,saved);}
    if(!target)ReleaseDC(w,dc);
}

static void draw_combo(HWND w,control_style *s,HDC dc)
{
    COMBOBOXINFO info={sizeof(info)};RECT client,button,text,pixel;HFONT font;
    WCHAR *label;int length,saved,x,y,left,top,u;BOOL selected,pressed,editable,simple;
    if(!dc || !GetComboBoxInfo(w,&info))return;
    length=GetWindowTextLengthW(w);label=calloc((size_t)length+1,sizeof(WCHAR));if(!label)return;
    GetWindowTextW(w,label,length+1);font=(HFONT)SendMessageW(w,WM_GETFONT,0,0);
    pressed=SendMessageW(w,CB_GETDROPPEDSTATE,0,0)!=0;
    simple=(GetWindowLongPtrW(w,GWL_STYLE)&3)==CBS_SIMPLE;
    editable=(GetWindowLongPtrW(w,GWL_STYLE)&3)!=CBS_DROPDOWNLIST;
    if(s->detached){free(label);return;}
    saved=SaveDC(dc);if(!saved){free(label);return;}
    u=max(1,MulDiv(1,(int)GetDpiForWindow(w),96));GetClientRect(w,&client);
    /* Native child EDIT owns its caret, text, selection and IME. Do not paint
     * over that child while drawing the combo's surrounding chrome. */
    if(editable)ExcludeClipRect(dc,info.rcItem.left,info.rcItem.top,info.rcItem.right,info.rcItem.bottom);
    if(simple){RECT list;
        if(GetWindowRect(info.hwndList,&list)){
            MapWindowPoints(NULL,w,(POINT *)&list,2);ExcludeClipRect(dc,list.left,list.top,list.right,list.bottom);}
        SetDCBrushColor(dc,s->palette.light);FillRect(dc,&client,(HBRUSH)GetStockObject(DC_BRUSH));
        text=info.rcItem;InflateRect(&text,u,u);frame(dc,text,s->palette.edge,u);
        RestoreDC(dc,saved);free(label);return;
    }
    SetDCBrushColor(dc,s->palette.light);FillRect(dc,&client,(HBRUSH)GetStockObject(DC_BRUSH));
    frame(dc,client,s->palette.edge,u);
    button=info.rcButton;SetDCBrushColor(dc,s->palette.face);FillRect(dc,&button,(HBRUSH)GetStockObject(DC_BRUSH));
    frame(dc,button,s->palette.edge,u);InflateRect(&button,-u,-u);
    pixel=button;pixel.bottom=pixel.top+u;SetDCBrushColor(dc,pressed?s->palette.shadow:s->palette.light);
    FillRect(dc,&pixel,(HBRUSH)GetStockObject(DC_BRUSH));pixel=button;pixel.right=pixel.left+u;
    FillRect(dc,&pixel,(HBRUSH)GetStockObject(DC_BRUSH));
    SetDCBrushColor(dc,pressed?s->palette.light:s->palette.shadow);pixel=button;pixel.top=pixel.bottom-u;
    FillRect(dc,&pixel,(HBRUSH)GetStockObject(DC_BRUSH));pixel=button;pixel.left=pixel.right-u;
    FillRect(dc,&pixel,(HBRUSH)GetStockObject(DC_BRUSH));
    left=(button.left+button.right-7*u)/2;top=(button.top+button.bottom-9*u)/2;
    if(pressed){left+=u;top+=u;}
    SetDCBrushColor(dc,IsWindowEnabled(w)?s->palette.edge:s->palette.shadow);
    for(y=0;y<9;++y)for(x=0;x<7;++x)if(combo_arrow[y][x]=='0'){
        SetRect(&pixel,left+x*u,top+y*u,left+(x+1)*u,top+(y+1)*u);FillRect(dc,&pixel,(HBRUSH)GetStockObject(DC_BRUSH));}
    if(editable){RestoreDC(dc,saved);free(label);return;}
    text=info.rcItem;selected=GetFocus()==w && !pressed;
    SetDCBrushColor(dc,selected?s->palette.active:s->palette.light);FillRect(dc,&text,(HBRUSH)GetStockObject(DC_BRUSH));
    SelectObject(dc,font?font:GetStockObject(SYSTEM_FONT));SetBkMode(dc,TRANSPARENT);
    SetTextColor(dc,!IsWindowEnabled(w)?s->palette.shadow:selected?s->palette.active_text:s->palette.edge);
    text.left+=2;DrawTextW(dc,label,-1,&text,DT_SINGLELINE|DT_VCENTER|DT_NOPREFIX);
    if(selected)DrawFocusRect(dc,&info.rcItem);
    RestoreDC(dc,saved);free(label);
}

static void detach_combo_list(control_style *s)
{
    DWORD_PTR data;HWND list=s->combo_list;s->combo_list=NULL;
    /* An independently removed/reinstalled child is no longer ours. */
    if(list && GetWindowSubclass(list,control_proc,(UINT_PTR)control_proc,&data) &&
        ((control_style *)data)->owner==s)win16_style_control_remove(list);
}

static void scroll_arrow(HDC dc,RECT r,const char bits[17][18],const win16_style_palette *p)
{
    int x,y;COLORREF colors[4]={p->edge,p->light,p->shadow,p->face};RECT pixel;
    for(y=0;y<17;++y)for(x=0;x<17;++x){
        SetDCBrushColor(dc,colors[bits[y][x]-'0']);
        SetRect(&pixel,r.left+MulDiv(x,r.right-r.left,17),r.top+MulDiv(y,r.bottom-r.top,17),
            r.left+MulDiv(x+1,r.right-r.left,17),r.top+MulDiv(y+1,r.bottom-r.top,17));
        FillRect(dc,&pixel,(HBRUSH)GetStockObject(DC_BRUSH));
    }
}

static BOOL scroll_layout(HWND w,win16_scroll_layout *layout)
{
    RECT r;SCROLLINFO info={sizeof(info),SIF_RANGE|SIF_PAGE|SIF_POS};
    int dpi=(int)GetDpiForWindow(w);
    win16_scroll_metrics metrics={MulDiv(17,dpi,96),MulDiv(17,dpi,96),max(1,MulDiv(1,dpi,96))};LONGLONG maximum;
    if(!GetClientRect(w,&r) || !GetScrollInfo(w,SB_CTL,&info))return FALSE;
    maximum=(LONGLONG)info.nMax-(info.nPage?info.nPage-1:0);
    if(maximum<info.nMin)maximum=info.nMin;
    return win16_scroll_measure(&r,(GetWindowLongPtrW(w,GWL_STYLE)&SBS_VERT)!=0,
        &metrics,info.nMin,(LONG)maximum,info.nPos,layout);
}
static BOOL embedded_scroll_layout(HWND w,LONG object,win16_scroll_layout *layout,POINT *screen_origin)
{
    BOOL vertical=object==OBJID_VSCROLL;
    SCROLLBARINFO bar={sizeof(bar)};SCROLLINFO info={sizeof(info),SIF_RANGE|SIF_PAGE|SIF_POS};
    RECT r;int dpi=(int)GetDpiForWindow(w);LONGLONG maximum;
    win16_scroll_metrics metrics={MulDiv(17,dpi,96),MulDiv(17,dpi,96),max(1,MulDiv(1,dpi,96))};
    if(!GetScrollBarInfo(w,vertical?OBJID_VSCROLL:OBJID_HSCROLL,&bar) || (bar.rgstate[0]&STATE_SYSTEM_INVISIBLE) ||
        !GetScrollInfo(w,vertical?SB_VERT:SB_HORZ,&info))return FALSE;
    SetRect(&r,0,0,bar.rcScrollBar.right-bar.rcScrollBar.left,bar.rcScrollBar.bottom-bar.rcScrollBar.top);
    screen_origin->x=bar.rcScrollBar.left;screen_origin->y=bar.rcScrollBar.top;
    maximum=(LONGLONG)info.nMax-(info.nPage?info.nPage-1:0);if(maximum<info.nMin)maximum=info.nMin;
    return win16_scroll_measure(&r,vertical,&metrics,info.nMin,(LONG)maximum,info.nPos,layout);
}

static BOOL horizontal_thumb_messages(HWND w)
{
    SCROLLINFO info={sizeof(info),SIF_RANGE|SIF_PAGE};
    return GetScrollInfo(w,SB_HORZ,&info) && info.nMin>=0 &&
        (LONGLONG)info.nMax-(info.nPage?info.nPage-1:0)<=0xffff;
}
static BOOL list_axis_owned(HWND w,LONG object)
{
    if(GetWindowLongPtrW(w,GWL_STYLE)&LBS_MULTICOLUMN)return object==OBJID_HSCROLL;
    return object==OBJID_VSCROLL || (object==OBJID_HSCROLL && horizontal_thumb_messages(w));
}
static BOOL edit_thumb_messages(HWND w,LONG object)
{
    /* Above this native EDIT boundary, thumb messages ignore HIWORD and read
     * USER-owned nTrackPos, which this public-API adapter cannot assign. */
    LRESULT lines=SendMessageW(w,EM_GETLINECOUNT,0,0);
    if(lines<=0 || lines>=0xffff)return FALSE;
    if(object==OBJID_HSCROLL && !horizontal_thumb_messages(w))return FALSE;
    return TRUE;
}

static BOOL draw_scroll_pixels(HWND w,control_style *s,HDC dc,LONG object,BOOL vertical)
{
    SCROLLBARINFO info={sizeof(info)};RECT r,first,last,thumb,pixel;int saved;
    if(!dc || !GetScrollBarInfo(w,object,&info) || (info.rgstate[0]&STATE_SYSTEM_INVISIBLE))return FALSE;
    saved=SaveDC(dc);if(!saved)return FALSE;
    if(object==OBJID_CLIENT)GetClientRect(w,&r);
    else {RECT outer;GetWindowRect(w,&outer);
        OffsetViewportOrgEx(dc,info.rcScrollBar.left-outer.left,info.rcScrollBar.top-outer.top,NULL);
        SetRect(&r,0,0,info.rcScrollBar.right-info.rcScrollBar.left,info.rcScrollBar.bottom-info.rcScrollBar.top);}
    IntersectClipRect(dc,r.left,r.top,r.right,r.bottom);
    /* Native state remains authoritative; opted-in bars replace geometry
     * below with the same fixed-thumb layout used for hit testing. */
    {static const WORD checks[8]={0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa};
        HBITMAP bitmap=CreateBitmap(8,8,1,1,checks);HBRUSH brush=bitmap?CreatePatternBrush(bitmap):NULL;
        SetTextColor(dc,s->palette.face);SetBkColor(dc,s->palette.light);
        if(brush){FillRect(dc,&r,brush);DeleteObject(brush);}
        if(bitmap)DeleteObject(bitmap);
        if(!brush){RestoreDC(dc,saved);return FALSE;}}
    first=last=thumb=r;
    if(vertical){first.bottom=first.top+info.dxyLineButton;last.top=last.bottom-info.dxyLineButton;
        thumb.top=info.xyThumbTop;thumb.bottom=info.xyThumbBottom;}
    else {first.right=first.left+info.dxyLineButton;last.left=last.right-info.dxyLineButton;
        thumb.left=info.xyThumbTop;thumb.right=info.xyThumbBottom;}
    if(object==OBJID_CLIENT || s->frame_scrollbars || (s->edit_scrollbars && edit_thumb_messages(w,object)) || (s->list_scrollbar &&
        list_axis_owned(w,object))){win16_scroll_layout layout;POINT origin;
        if(object==OBJID_CLIENT?scroll_layout(w,&layout):embedded_scroll_layout(w,object,&layout,&origin)){
            first=layout.first;last=layout.last;thumb=layout.thumb;
            if(s->scroll.active && (object==OBJID_CLIENT || object==s->embedded_object) && s->scroll.part==WIN16_SCROLL_THUMB){
                if(vertical)OffsetRect(&thumb,0,s->scroll.pixel-thumb.top);
                else OffsetRect(&thumb,s->scroll.pixel-thumb.left,0);}
            info.xyThumbBottom=layout.thumb_fits?1:0;info.xyThumbTop=0;
            info.rgstate[1]&=~STATE_SYSTEM_PRESSED;info.rgstate[5]&=~STATE_SYSTEM_PRESSED;
            if(s->scroll.active && (object==OBJID_CLIENT || object==s->embedded_object) && s->scroll.inside){
                if(s->scroll.part==WIN16_SCROLL_LINE_BEFORE)info.rgstate[1]|=STATE_SYSTEM_PRESSED;
                if(s->scroll.part==WIN16_SCROLL_LINE_AFTER)info.rgstate[5]|=STATE_SYSTEM_PRESSED;}
        }
    }
    frame(dc,r,s->palette.edge,1);
    scroll_arrow(dc,first,(info.rgstate[1]&STATE_SYSTEM_UNAVAILABLE)?(vertical?scroll_uparrowi:scroll_lfarrowi):
        vertical?(info.rgstate[1]&STATE_SYSTEM_PRESSED?scroll_uparrowd:scroll_uparrow):
        (info.rgstate[1]&STATE_SYSTEM_PRESSED?scroll_lfarrowd:scroll_lfarrow),&s->palette);
    scroll_arrow(dc,last,(info.rgstate[5]&STATE_SYSTEM_UNAVAILABLE)?(vertical?scroll_dnarrowi:scroll_rgarrowi):
        vertical?(info.rgstate[5]&STATE_SYSTEM_PRESSED?scroll_dnarrowd:scroll_dnarrow):
        (info.rgstate[5]&STATE_SYSTEM_PRESSED?scroll_rgarrowd:scroll_rgarrow),&s->palette);
    if(info.xyThumbBottom>info.xyThumbTop){
        SetDCBrushColor(dc,s->palette.face);FillRect(dc,&thumb,(HBRUSH)GetStockObject(DC_BRUSH));
        frame(dc,thumb,s->palette.edge,1);InflateRect(&thumb,-1,-1);
        SetDCBrushColor(dc,s->palette.light);pixel=thumb;pixel.bottom=pixel.top+1;FillRect(dc,&pixel,(HBRUSH)GetStockObject(DC_BRUSH));
        pixel=thumb;pixel.right=pixel.left+1;FillRect(dc,&pixel,(HBRUSH)GetStockObject(DC_BRUSH));
        SetDCBrushColor(dc,s->palette.shadow);pixel=thumb;pixel.top=pixel.bottom-1;FillRect(dc,&pixel,(HBRUSH)GetStockObject(DC_BRUSH));
        pixel=thumb;pixel.left=pixel.right-1;FillRect(dc,&pixel,(HBRUSH)GetStockObject(DC_BRUSH));
    }
    RestoreDC(dc,saved);return TRUE;
}

static void draw_scroll(HWND w,control_style *s,HDC dc,LONG object,BOOL vertical)
{
    RECT r;SCROLLBARINFO bar={sizeof(bar)};HDC memory;HBITMAP bitmap,old;POINT brush,device;BOOL painted;
    if(!dc || !GetScrollBarInfo(w,object,&bar) || (bar.rgstate[0]&STATE_SYSTEM_INVISIBLE))return;
    if(object==OBJID_CLIENT)GetClientRect(w,&r);
    else {RECT window;GetWindowRect(w,&window);r=bar.rcScrollBar;OffsetRect(&r,-window.left,-window.top);}
    if(IsRectEmpty(&r))return;
    memory=CreateCompatibleDC(dc);bitmap=CreateCompatibleBitmap(dc,r.right-r.left,r.bottom-r.top);
    if(!memory || !bitmap){if(memory)DeleteDC(memory);if(bitmap)DeleteObject(bitmap);
        draw_scroll_pixels(w,s,dc,object,vertical);return;}
    old=(HBITMAP)SelectObject(memory,bitmap);
    if(!old || (HGDIOBJ)old==HGDI_ERROR){DeleteObject(bitmap);DeleteDC(memory);
        draw_scroll_pixels(w,s,dc,object,vertical);return;}
    /* Preserve the checker phase while composing only this bar. One BitBlt
     * prevents the background erase and subsequent thumb draw being visible
     * as separate states. No persistent buffer or redraw timer is needed. */
    device.x=r.left;device.y=r.top;LPtoDP(dc,&device,1);GetBrushOrgEx(dc,&brush);
    SetBrushOrgEx(memory,brush.x-device.x,brush.y-device.y,NULL);
    SetViewportOrgEx(memory,-r.left,-r.top,NULL);
    painted=draw_scroll_pixels(w,s,memory,object,vertical);
    SetViewportOrgEx(memory,0,0,NULL);
    /* A failed painter has no complete image to publish. Keep the previous
     * target pixels rather than copying an uninitialized temporary bitmap. */
    if(painted)BitBlt(dc,r.left,r.top,r.right-r.left,r.bottom-r.top,memory,0,0,SRCCOPY);
    SelectObject(memory,old);DeleteObject(bitmap);DeleteDC(memory);
}

static void nonclient_scrollbars(HWND w,control_style *s,HDC target)
{
    DWORD style=(DWORD)GetWindowLongPtrW(w,GWL_STYLE);HDC dc;
    if(!(style&(WS_HSCROLL|WS_VSCROLL)))return;
    dc=target?target:GetWindowDC(w);if(!dc)return;
    if(style&WS_HSCROLL)draw_scroll(w,s,dc,OBJID_HSCROLL,FALSE);
    if(style&WS_VSCROLL)draw_scroll(w,s,dc,OBJID_VSCROLL,TRUE);
    if(!target)ReleaseDC(w,dc);
}

typedef struct list_view_state {
    LRESULT top,selection,caret;
} list_view_state;
static list_view_state list_view(HWND w)
{
    list_view_state v;
    v.top=SendMessageW(w,LB_GETTOPINDEX,0,0);
    v.selection=SendMessageW(w,LB_GETCURSEL,0,0);
    v.caret=SendMessageW(w,LB_GETCARETINDEX,0,0);return v;
}

void win16_style_scrollbars_paint(HWND w,HDC target,const win16_style_palette *palette)
{
    control_style s={0};DWORD_PTR attached;
    if(palette)s.palette=*palette;else win16_style_default_palette(&s.palette);
    if(GetWindowSubclass(w,control_proc,(UINT_PTR)control_proc,&attached)){
        control_style *owner=(control_style *)attached;
        s.list_scrollbar=owner->list_scrollbar;s.edit_scrollbars=owner->edit_scrollbars;s.scroll=owner->scroll;
        s.frame_scrollbars=owner->frame_scrollbars;
        s.embedded_object=owner->embedded_object;
    }
    nonclient_scrollbars(w,&s,target);
}

static UINT scroll_disabled(HWND w,LONG object)
{
    SCROLLBARINFO info={sizeof(info)};UINT disabled=0;
    if(!IsWindowEnabled(w) || !GetScrollBarInfo(w,object,&info))return ESB_DISABLE_BOTH;
    if(info.rgstate[1]&STATE_SYSTEM_UNAVAILABLE)disabled|=ESB_DISABLE_LTUP;
    if(info.rgstate[5]&STATE_SYSTEM_UNAVAILABLE)disabled|=ESB_DISABLE_RTDN;
    return disabled;
}
static void scroll_caret_destroy(HWND w,control_style *s)
{
    GUITHREADINFO info={sizeof(info)};
    if(s->scroll_caret && GetGUIThreadInfo(GetCurrentThreadId(),&info) && info.hwndCaret==w)DestroyCaret();
    s->scroll_caret=s->scroll_caret_shown=FALSE;
}
static void scroll_caret_sync(HWND w,control_style *s)
{
    win16_scroll_layout layout;RECT r;int inset=max(1,MulDiv(2,(int)GetDpiForWindow(w),96));
    if(s->detached || !is_scroll(w))return;
    if(GetFocus()!=w || !IsWindowEnabled(w) || !scroll_layout(w,&layout) || !layout.thumb_fits){
        scroll_caret_destroy(w,s);return;}
    r=layout.thumb;
    if(s->scroll.active && s->scroll.part==WIN16_SCROLL_THUMB){
        if(layout.vertical)OffsetRect(&r,0,s->scroll.pixel-r.top);
        else OffsetRect(&r,s->scroll.pixel-r.left,0);}
    InflateRect(&r,-inset,-inset);
    if(IsRectEmpty(&r)){scroll_caret_destroy(w,s);return;}
    if(s->scroll_caret && (s->scroll_caret_size.cx!=r.right-r.left || s->scroll_caret_size.cy!=r.bottom-r.top))
        scroll_caret_destroy(w,s);
    if(!s->scroll_caret){
        if(!CreateCaret(w,(HBITMAP)1,r.right-r.left,r.bottom-r.top))return;
        s->scroll_caret=TRUE;s->scroll_caret_size.cx=r.right-r.left;s->scroll_caret_size.cy=r.bottom-r.top;}
    SetCaretPos(r.left,r.top);
    if(s->scroll.active && s->scroll_caret_shown){HideCaret(w);s->scroll_caret_shown=FALSE;}
    else if(!s->scroll.active && !s->scroll_caret_shown){ShowCaret(w);s->scroll_caret_shown=TRUE;}
}
static void scroll_stop_timer(HWND w,control_style *s)
{if(s->scroll_timer){KillTimer(w,s->scroll_timer);s->scroll_timer=0;}}
static void scroll_actions(HWND w,control_style *s,const win16_scroll_actions *actions,unsigned epoch)
{
    UINT i;BOOL reporting=s->scroll_reporting;LONG previous=s->scroll_report;
    if(s->detached || s->scroll_epoch!=epoch)return;
    if(actions->timer_ms!=WIN16_SCROLL_KEEP_TIMER){
        scroll_stop_timer(w,s);
        if(actions->timer_ms && s->scroll.active)
            s->scroll_timer=SetTimer(w,(UINT_PTR)&s->scroll,actions->timer_ms,NULL);
    }
    for(i=0;i<actions->count;++i){
        HWND parent=GetParent(w);UINT message=(GetWindowLongPtrW(w,GWL_STYLE)&SBS_VERT)?WM_VSCROLL:WM_HSCROLL;
        s->scroll_reporting=TRUE;s->scroll_report=actions->notices[i].position;
        if(s->frame_scrollbars){
            SendMessageW(w,s->embedded_object==OBJID_HSCROLL?WM_HSCROLL:WM_VSCROLL,
                MAKEWPARAM(actions->notices[i].command,(WORD)s->scroll_report),0);
        }else if(s->edit_scrollbars){UINT command=actions->notices[i].command;
            SendMessageW(w,s->embedded_object==OBJID_HSCROLL?WM_HSCROLL:WM_VSCROLL,MAKEWPARAM(command,(WORD)s->scroll_report),0);
        }else if(s->list_scrollbar){UINT command=actions->notices[i].command;
            BOOL multi=(GetWindowLongPtrW(w,GWL_STYLE)&LBS_MULTICOLUMN)!=0;
            if(!multi && s->embedded_object==OBJID_HSCROLL)
                SendMessageW(w,WM_HSCROLL,MAKEWPARAM(command,(WORD)s->scroll_report),0);
            else if(command==SB_THUMBTRACK || command==SB_THUMBPOSITION){
                LONGLONG index=s->scroll_report;
                if(multi){RECT client;LRESULT height=SendMessageW(w,LB_GETITEMHEIGHT,0,0);
                    if(s->detached || s->scroll_epoch!=epoch)return;
                    if(height<=0 || !GetClientRect(w,&client))return;
                    index*=max(1,client.bottom/(LONG)height);
                }
                if(index>=0 && index<=MAXLONG)SendMessageW(w,LB_SETTOPINDEX,(WPARAM)index,0);
            }else SendMessageW(w,multi?WM_HSCROLL:WM_VSCROLL,MAKEWPARAM(command,0),0);
        }else if(parent)SendMessageW(parent,message,MAKEWPARAM(actions->notices[i].command,(WORD)s->scroll_report),(LPARAM)w);
        s->scroll_reporting=reporting;s->scroll_report=previous;
        /* Calls may destroy/remove the attachment, or start another gesture. */
        if(s->detached || s->scroll_epoch!=epoch)return;
    }
    if(actions->repaint && s->frame_scrollbars){HDC dc=GetWindowDC(w);
        if(dc){draw_scroll(w,s,dc,s->embedded_object,s->embedded_object==OBJID_VSCROLL);ReleaseDC(w,dc);}}
    else if(actions->repaint){RedrawWindow(w,NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW);
        if(!s->detached && (s->list_scrollbar || s->edit_scrollbars))nonclient_scrollbars(w,s,NULL);}
    scroll_caret_sync(w,s);
}
static void scroll_release_capture(HWND w,control_style *s)
{
    HWND previous=s->list_capture_before;s->list_capture_before=NULL;
    /* Do not release capture borrowed from the combo's own popup tracking,
     * and never take capture back from a window that already acquired it. */
    if(GetCapture()==w){
        if((s->list_scrollbar || s->edit_scrollbars || s->frame_scrollbars) && previous && IsWindow(previous) &&
            GetWindowThreadProcessId(previous,NULL)==GetCurrentThreadId()){
            if(previous!=w)SetCapture(previous);
        }else ReleaseCapture();
    }
}
static void scroll_finish(HWND w,control_style *s,BOOL cancel)
{
    win16_scroll_actions actions;unsigned epoch=s->scroll_epoch;
    if(!s->scroll.active)return;
    win16_scroll_end(&s->scroll,cancel,&actions);scroll_stop_timer(w,s);
    scroll_release_capture(w,s);
    scroll_actions(w,s,&actions,epoch);
}
static BOOL scroll_input(HWND w,control_style *s,UINT m,WPARAM a,LPARAM b)
{
    win16_scroll_layout layout;win16_scroll_actions actions;POINT point;
    point.x=(short)LOWORD(b);point.y=(short)HIWORD(b);
    if(m==WM_LBUTTONDOWN || m==WM_LBUTTONDBLCLK){
        RECT limit;unsigned epoch;
        if(!IsWindowEnabled(w) || s->scroll.active || !scroll_layout(w,&layout))return TRUE;
        /* USER18:17AE inflates the original bar by 4*CXBORDER, CYBORDER.
         * Do not substitute the much wider later-NT drag tolerance. */
        limit=layout.bar;{int unit=max(1,MulDiv(1,(int)GetDpiForWindow(w),96));InflateRect(&limit,4*unit,unit);}
        if(!win16_scroll_begin(&s->scroll,&layout,scroll_disabled(w,OBJID_CLIENT),point,&limit,&actions))return TRUE;
        epoch=++s->scroll_epoch;SetFocus(w);
        if(s->detached || epoch!=s->scroll_epoch || !s->scroll.active)return TRUE;
        SetCapture(w);
        if(s->detached || epoch!=s->scroll_epoch)return TRUE;
        if(GetCapture()!=w){scroll_finish(w,s,TRUE);return TRUE;}
        scroll_actions(w,s,&actions,epoch);return TRUE;
    }
    if(m==WM_MOUSEMOVE || (m==WM_TIMER && s->scroll_timer && a==s->scroll_timer)){
        if(s->scroll.active && scroll_layout(w,&layout)){
            if(m==WM_TIMER){if(!GetCursorPos(&point) || !ScreenToClient(w,&point))return TRUE;}
            win16_scroll_motion(&s->scroll,&layout,scroll_disabled(w,OBJID_CLIENT),point,m==WM_TIMER,&actions);
            scroll_actions(w,s,&actions,s->scroll_epoch);
        }return TRUE;
    }
    if(m==WM_LBUTTONUP || m==WM_CANCELMODE || m==WM_CAPTURECHANGED){
        scroll_finish(w,s,m!=WM_LBUTTONUP);return TRUE;
    }
    if(m==WM_MOUSELEAVE)return TRUE;
    if(m==WM_KEYDOWN){UINT command;
        if(!IsWindowEnabled(w))return TRUE;
        switch(a){case VK_LEFT:case VK_UP:command=SB_LINEUP;break;
        case VK_RIGHT:case VK_DOWN:command=SB_LINEDOWN;break;
        case VK_PRIOR:command=SB_PAGEUP;break;case VK_NEXT:command=SB_PAGEDOWN;break;
        case VK_HOME:command=SB_TOP;break;case VK_END:command=SB_BOTTOM;break;
        default:return FALSE;}
        ZeroMemory(&actions,sizeof(actions));actions.timer_ms=WIN16_SCROLL_KEEP_TIMER;
        actions.count=1;actions.notices[0].command=command;actions.repaint=TRUE;s->scroll_key=TRUE;
        scroll_actions(w,s,&actions,s->scroll_epoch);return TRUE;
    }
    if(m==WM_KEYUP && s->scroll_key){
        s->scroll_key=FALSE;ZeroMemory(&actions,sizeof(actions));actions.timer_ms=WIN16_SCROLL_KEEP_TIMER;
        actions.count=1;actions.notices[0].command=SB_ENDSCROLL;
        scroll_actions(w,s,&actions,s->scroll_epoch);return TRUE;
    }
    return FALSE;
}

static BOOL embedded_scroll_input(HWND w,control_style *s,UINT m,WPARAM a,LPARAM b)
{
    win16_scroll_layout layout;win16_scroll_actions actions;POINT point,origin;
    BOOL multi=s->list_scrollbar && (GetWindowLongPtrW(w,GWL_STYLE)&LBS_MULTICOLUMN)!=0;
    LONG object=multi?OBJID_HSCROLL:OBJID_VSCROLL;
    BOOL down=m==WM_NCLBUTTONDOWN || m==WM_NCLBUTTONDBLCLK;
    if(!s->list_scrollbar && !s->edit_scrollbars && !s->frame_scrollbars)return FALSE;
    /* A dropped combo already owns capture: its bar receives CLIENT clicks.
     * Native LISTBOX normally releases capture, calls the synchronous NC
     * tracker, then takes capture back. Our tracker returns between messages;
     * entering it through that native wrapper loses the borrowed ownership.
     * Normalize bar hits here, leaving item/outside clicks with native USER. */
    if(s->owner && s->list_scrollbar && !s->scroll.active && GetCapture()==w &&
        (m==WM_LBUTTONDOWN || m==WM_LBUTTONDBLCLK)){
        LRESULT hit;
        point.x=(short)LOWORD(b);point.y=(short)HIWORD(b);ClientToScreen(w,&point);
        hit=SendMessageW(w,WM_NCHITTEST,0,MAKELPARAM(point.x,point.y));
        if(s->detached)return TRUE;
        if(hit==HTVSCROLL || hit==HTHSCROLL){
            down=TRUE;a=(WPARAM)hit;b=MAKELPARAM(point.x,point.y);
        }
    }
    if(s->scroll.active)object=s->embedded_object;
    else if(down)object=a==HTHSCROLL?OBJID_HSCROLL:OBJID_VSCROLL;
    /* Querying EDIT's line count dispatches back through this subclass.
     * Only input/tracking messages may enter the eligibility query. */
    if(!down && m!=WM_MOUSEMOVE && m!=WM_LBUTTONUP && m!=WM_CANCELMODE &&
        m!=WM_CAPTURECHANGED && !(m==WM_TIMER && s->scroll_timer && a==s->scroll_timer))return FALSE;
    if((s->edit_scrollbars && !edit_thumb_messages(w,object)) ||
        (s->list_scrollbar && !list_axis_owned(w,object))){
        if(s->scroll.active)scroll_finish(w,s,TRUE);
        return FALSE; /* Keep native wide-text input, not truncated positioning. */
    }
    if(down && a==(WPARAM)(object==OBJID_HSCROLL?HTHSCROLL:HTVSCROLL)){RECT limit;unsigned epoch;
        if(s->scroll.active || !IsWindowEnabled(w) || !embedded_scroll_layout(w,object,&layout,&origin))return TRUE;
        point.x=(short)LOWORD(b)-origin.x;point.y=(short)HIWORD(b)-origin.y;
        limit=layout.bar;{int u=max(1,MulDiv(1,(int)GetDpiForWindow(w),96));InflateRect(&limit,4*u,u);}
        if(!win16_scroll_begin(&s->scroll,&layout,scroll_disabled(w,object),point,&limit,&actions))return TRUE;
        epoch=++s->scroll_epoch;s->embedded_object=object;s->list_capture_before=GetCapture();
        if(s->list_capture_before!=w)SetCapture(w);
        if(s->detached || s->scroll_epoch!=epoch)return TRUE;
        if(GetCapture()!=w){scroll_finish(w,s,TRUE);return TRUE;}
        scroll_actions(w,s,&actions,epoch);return TRUE;
    }
    if(!s->scroll.active)return FALSE;
    if(m==WM_MOUSEMOVE || (m==WM_TIMER && a==s->scroll_timer)){
        if(!embedded_scroll_layout(w,object,&layout,&origin)){scroll_finish(w,s,TRUE);return TRUE;}
        if(m==WM_TIMER){if(!GetCursorPos(&point))return TRUE;}
        else{point.x=(short)LOWORD(b);point.y=(short)HIWORD(b);ClientToScreen(w,&point);}
        point.x-=origin.x;point.y-=origin.y;
        win16_scroll_motion(&s->scroll,&layout,scroll_disabled(w,object),point,m==WM_TIMER,&actions);
        scroll_actions(w,s,&actions,s->scroll_epoch);return TRUE;
    }
    if(m==WM_LBUTTONUP || m==WM_CANCELMODE || m==WM_CAPTURECHANGED){
        scroll_finish(w,s,m!=WM_LBUTTONUP);return TRUE;}
    return FALSE;
}

/* Only screen WM_PAINT uses this transient surface. Keep WM_PRINTCLIENT on
 * its borrowed DC. Seed the dirty rectangle so group interiors and native
 * combo EDIT/list children excluded by the painter remain unchanged. */
static void present_control(HWND w,control_style *s,HDC target,
    void (*painter)(HWND,control_style *,HDC))
{
    RECT dirty,client;HDC memory=NULL;HBITMAP bitmap=NULL,old=NULL;BOOL ready=FALSE;
    if(!target || GetClipBox(target,&dirty)==ERROR || !GetClientRect(w,&client) ||
        !IntersectRect(&dirty,&dirty,&client))return;
    memory=CreateCompatibleDC(target);
    if(memory)bitmap=CreateCompatibleBitmap(target,dirty.right-dirty.left,dirty.bottom-dirty.top);
    if(bitmap)old=(HBITMAP)SelectObject(memory,bitmap);
    if(old && old!=(HBITMAP)HGDI_ERROR &&
        BitBlt(memory,0,0,dirty.right-dirty.left,dirty.bottom-dirty.top,target,dirty.left,dirty.top,SRCCOPY) &&
        SetWindowOrgEx(memory,dirty.left,dirty.top,NULL)){
        painter(w,s,memory);
        if(!s->detached)ready=BitBlt(target,dirty.left,dirty.top,dirty.right-dirty.left,
            dirty.bottom-dirty.top,memory,dirty.left,dirty.top,SRCCOPY);
    }
    if(old && old!=(HBITMAP)HGDI_ERROR)SelectObject(memory,old);
    if(bitmap)DeleteObject(bitmap);if(memory)DeleteDC(memory);
    if(!ready && !s->detached)painter(w,s,target);
}

/* Original LBSetTabStops stores set-time pixels. GdiGetCharDimensions uses
 * the alphabet average for proportional fonts, not tmAveCharWidth. Keep
 * that public-API arithmetic here; private PLBIV/CFONT are not imported. */
static LRESULT set_list_tabs(HWND w,control_style *s,WPARAM count,LPARAM values)
{
    int *pixels=NULL,average,i;LRESULT result;
    if(count>INT_MAX || (count && !values) || count>((size_t)-1)/sizeof(int))return FALSE;
    if(count){HDC dc;HFONT old;TEXTMETRICW tm;SIZE size;
        pixels=malloc((size_t)count*sizeof(int));if(!pixels)return FALSE;
        dc=GetDC(w);if(!dc){free(pixels);return FALSE;}
        old=SelectObject(dc,s->font);
        if(!GetTextMetricsW(dc,&tm)){SelectObject(dc,old);ReleaseDC(w,dc);free(pixels);return FALSE;}
        average=tm.tmAveCharWidth;
        if(tm.tmPitchAndFamily&TMPF_FIXED_PITCH){
            if(!GetTextExtentPoint32W(dc,L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ",52,&size)){
                SelectObject(dc,old);ReleaseDC(w,dc);free(pixels);return FALSE;}
            average=((size.cx/26)+1)/2;
        }
        SelectObject(dc,old);ReleaseDC(w,dc);
        for(i=0;i<(int)count;++i){LONGLONG value=((LONGLONG)((const int *)values)[i]*average+2)/4;
            if(value<INT_MIN || value>INT_MAX){free(pixels);return FALSE;}pixels[i]=(int)value;}
    }
    result=DefSubclassProc(w,LB_SETTABSTOPS,count,values);
    if(result && !s->detached){free(s->tab_positions);s->tab_positions=pixels;s->tab_count=(int)count;}
    else free(pixels);
    return result;
}
/* USER18:0208..027F: scrollbar-class size box shares the inner border
 * and fills with the system scrollbar brush, not a later sizing-grip image.
 * Native creation/alignment and parent size/maximize commands stay native. */
static void draw_sizebox(HWND w,control_style *s,HDC dc)
{
    RECT outer,client,r;UINT dpi=GetDpiForWindow(w);int saved,cx,cy,bx,by;
    (void)s;
    if(!dc || !GetWindowRect(w,&outer) || !GetClientRect(w,&client))return;
    saved=SaveDC(dc);if(!saved)return;
    cx=GetSystemMetricsForDpi(SM_CXVSCROLL,dpi);cy=GetSystemMetricsForDpi(SM_CYHSCROLL,dpi);
    bx=GetSystemMetricsForDpi(SM_CXBORDER,dpi);by=GetSystemMetricsForDpi(SM_CYBORDER,dpi);
    SetRect(&r,outer.right-outer.left-cx+bx,outer.bottom-outer.top-cy+by,
        outer.right-outer.left,outer.bottom-outer.top);
    IntersectClipRect(dc,client.left,client.top,client.right,client.bottom);
    FillRect(dc,&r,GetSysColorBrush(COLOR_SCROLLBAR));RestoreDC(dc,saved);
}
static LRESULT CALLBACK control_proc(HWND w,UINT m,WPARAM a,LPARAM b,UINT_PTR id,DWORD_PTR data)
{
    control_style *s=(control_style *)data;LRESULT result=0;(void)id;++s->calls;
    if(m==WM_SETFONT && s->font){
        /* Keep the application's latest borrowed font for detach; layout and
         * native EDIT/list input use the same effective font as our painter. */
        if((HFONT)a!=s->font)s->original_font=(HFONT)a;
        a=(WPARAM)s->font;
    }
    if(m==WM_NCDESTROY){RemoveWindowSubclass(w,control_proc,(UINT_PTR)control_proc);s->detached=TRUE;
        scroll_caret_destroy(w,s);
        scroll_stop_timer(w,s);
        detach_combo_list(s);
        result=DefSubclassProc(w,m,a,b);}
    else if(s->frame_scrollbars){
        if(embedded_scroll_input(w,s,m,a,b))result=0;
        else if(m==WM_NCMOUSEMOVE && (a==HTHSCROLL || a==HTVSCROLL))result=0;
        else {if(m==WM_ENABLE && !a)scroll_finish(w,s,TRUE);
            if(!s->detached)result=DefSubclassProc(w,m,a,b);}
    }
    else if(is_scroll(w) && (GetWindowLongPtrW(w,GWL_STYLE)&SBS_SIZEBOX)){
        if(m==WM_PAINT){PAINTSTRUCT ps;HDC dc=BeginPaint(w,&ps);present_control(w,s,dc,draw_sizebox);EndPaint(w,&ps);}
        else if(m==WM_PRINTCLIENT)draw_sizebox(w,s,(HDC)a);
        else if(m==WM_ERASEBKGND)result=1;
        else result=DefSubclassProc(w,m,a,b);
    }
    else if(is_scroll(w)){
        if(scroll_input(w,s,m,a,b))result=0;
        else if(m==WM_SETFOCUS || m==WM_SIZE)scroll_caret_sync(w,s);
        else if(m==WM_KILLFOCUS)scroll_caret_destroy(w,s);
        else if(m==SBM_GETSCROLLINFO){SCROLLINFO *info=(SCROLLINFO *)b;
            result=DefSubclassProc(w,m,a,b);
            if(result && info && info->cbSize==sizeof(*info) && (info->fMask&SIF_TRACKPOS)){
                if(s->scroll_reporting)info->nTrackPos=s->scroll_report;
                else if(s->scroll.active)info->nTrackPos=s->scroll.position;}}
        else if(m==WM_PAINT){PAINTSTRUCT ps;HDC dc=BeginPaint(w,&ps);draw_scroll(w,s,dc,OBJID_CLIENT,(GetWindowLongPtrW(w,GWL_STYLE)&SBS_VERT)!=0);EndPaint(w,&ps);scroll_caret_sync(w,s);}
        else if(m==WM_PRINTCLIENT)draw_scroll(w,s,(HDC)a,OBJID_CLIENT,(GetWindowLongPtrW(w,GWL_STYLE)&SBS_VERT)!=0);
        else if(m==WM_ERASEBKGND)result=1;
        else {BOOL redraw=m==SBM_SETPOS?b!=0:m==SBM_SETSCROLLINFO?a!=0:m!=SBM_SETRANGE;
            if(m==WM_ENABLE && !a)scroll_finish(w,s,TRUE);
            if(s->detached)goto finished;
            if(m==SBM_SETPOS)b=FALSE;if(m==SBM_SETSCROLLINFO)a=FALSE;
            if(m==SBM_SETRANGEREDRAW)m=SBM_SETRANGE;
            result=DefSubclassProc(w,m,a,b);
            if(redraw && !s->detached && (m==SBM_SETPOS || m==SBM_SETSCROLLINFO || m==SBM_SETRANGE || m==SBM_SETRANGEREDRAW ||
                m==SBM_ENABLE_ARROWS || m==WM_ENABLE || m==WM_SETFOCUS || m==WM_KILLFOCUS ||
                m==WM_LBUTTONDOWN || m==WM_LBUTTONUP || m==WM_KEYDOWN))RedrawWindow(w,NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW);
            if(redraw)scroll_caret_sync(w,s);}
    }
    else if(is_combo(w)){
        if(m==WM_SETREDRAW){s->list_redraw_off=!a;result=DefSubclassProc(w,m,a,b);}
        else if(m==WM_PAINT){PAINTSTRUCT ps;HDC dc=BeginPaint(w,&ps);present_control(w,s,dc,draw_combo);EndPaint(w,&ps);}
        else if(m==WM_PRINTCLIENT)draw_combo(w,s,(HDC)a);
        else if(m==WM_ERASEBKGND)result=1;
        else {BOOL suppress=!s->list_redraw_off &&
                (m==CB_SETCURSEL || m==CB_SHOWDROPDOWN || m==WM_COMMAND || m==WM_ENABLE ||
                 m==WM_SETFONT || m==WM_SETFOCUS || m==WM_KILLFOCUS || m==WM_KEYDOWN ||
                 m==WM_LBUTTONDOWN || m==WM_LBUTTONUP);
            if(suppress && s->list_suppression++==0)DefSubclassProc(w,WM_SETREDRAW,FALSE,0);
            result=DefSubclassProc(w,m,a,b);
            if(suppress && --s->list_suppression==0 && !s->detached && !s->list_redraw_off)
                DefSubclassProc(w,WM_SETREDRAW,TRUE,0);
            if(!s->detached && !s->list_suppression && !s->list_redraw_off && (m==CB_SETCURSEL || m==CB_SHOWDROPDOWN || m==WM_COMMAND || m==WM_ENABLE ||
                m==WM_SETFONT || m==WM_SETFOCUS || m==WM_KILLFOCUS || m==WM_KEYDOWN ||
                m==WM_LBUTTONDOWN || m==WM_LBUTTONUP))RedrawWindow(w,NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW);}
    }
    else if(is_list(w)){
        if(embedded_scroll_input(w,s,m,a,b))result=0;
        else if(m==LB_SETTABSTOPS && (GetWindowLongPtrW(w,GWL_STYLE)&LBS_USETABSTOPS))result=set_list_tabs(w,s,a,b);
        else if(m==WM_SETREDRAW){s->list_redraw_off=!a;result=DefSubclassProc(w,m,a,b);}
        else if(m==WM_PAINT){PAINTSTRUCT ps;HDC dc=BeginPaint(w,&ps);present_control(w,s,dc,draw_list);EndPaint(w,&ps);}
        else if(m==WM_PRINTCLIENT)draw_list(w,s,(HDC)a);
        else if(m==WM_NCPAINT || m==WM_PRINT){
            /* This subclass owns the complete list nonclient surface. Native
             * painting first exposes a second scrollbar skin before ours. */
            if(m==WM_PRINT && (b&PRF_CLIENT)){
                HDC dc=(HDC)a;int saved=SaveDC(dc);RECT wr;POINT origin={0,0};
                if(saved){GetWindowRect(w,&wr);ClientToScreen(w,&origin);
                    OffsetViewportOrgEx(dc,origin.x-wr.left,origin.y-wr.top,NULL);
                    draw_list(w,s,dc);RestoreDC(dc,saved);}
            }
            if(!s->detached && (m==WM_NCPAINT || (b&PRF_NONCLIENT))){
                list_border(w,s,m==WM_PRINT?(HDC)a:NULL);
                nonclient_scrollbars(w,s,m==WM_PRINT?(HDC)a:NULL);}
        }
        else if(m==WM_NCMOUSEMOVE || m==WM_NCMOUSELEAVE){
            /* Win3.x has no scrollbar hover skin. Do not start the host's
             * themed hover/leave repaint; clicks still use native tracking. */
            result=0;
        }
        else if(m==WM_ERASEBKGND)result=1;
        else {
            BOOL moving=m==WM_MOUSEMOVE,changed=FALSE;list_view_state before={0},after;
            BOOL suppress=!s->list_redraw_off &&
                (m==WM_MOUSEWHEEL || m==WM_VSCROLL || m==WM_HSCROLL || m==WM_KEYDOWN || m==WM_KEYUP ||
                m==WM_SETFOCUS || m==WM_KILLFOCUS || m==WM_ENABLE || m==WM_SETFONT ||
                m==WM_MOUSEMOVE || m==WM_LBUTTONDOWN || m==WM_LBUTTONUP ||
                m==LB_ADDSTRING || m==LB_INSERTSTRING || m==LB_DELETESTRING || m==LB_RESETCONTENT ||
                m==LB_SETCURSEL || m==LB_SETSEL || m==LB_SELITEMRANGE || m==LB_SELITEMRANGEEX ||
                m==LB_SETCARETINDEX || m==LB_SETTOPINDEX);
            if(moving)before=list_view(w);
            if(suppress && s->list_suppression++==0)DefSubclassProc(w,WM_SETREDRAW,FALSE,0);
            result=DefSubclassProc(w,m,a,b);
            if(suppress && --s->list_suppression==0 && !s->detached && !s->list_redraw_off)
                DefSubclassProc(w,WM_SETREDRAW,TRUE,0);
            if(!s->detached && moving){after=list_view(w);
                changed=before.top!=after.top || before.selection!=after.selection || before.caret!=after.caret;}
            if(!s->detached && !s->list_suppression && !s->list_redraw_off && (m==LB_SETCURSEL || m==LB_SETSEL || m==LB_SELITEMRANGE || m==LB_SELITEMRANGEEX ||
                m==LB_SETCARETINDEX || m==LB_SETTOPINDEX || m==LB_ADDSTRING || m==LB_INSERTSTRING ||
                m==LB_DELETESTRING || m==LB_RESETCONTENT || m==LB_SETCOLUMNWIDTH || m==LB_SETHORIZONTALEXTENT || m==WM_SETFONT || m==WM_ENABLE ||
                m==WM_SETFOCUS || m==WM_KILLFOCUS || m==WM_LBUTTONDOWN || m==WM_LBUTTONUP ||
                changed || m==WM_KEYDOWN || m==WM_KEYUP || m==WM_VSCROLL || m==WM_HSCROLL || m==WM_MOUSEWHEEL)){
                RedrawWindow(w,NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW);
                if(!s->detached)nonclient_scrollbars(w,s,NULL);}
        }
    }
    else if(is_edit(w)){
        /* EDIT retains its native buffer, selection, caret, undo and IME.
         * Original flat EDIT borders are painted inside the client area. */
        if(embedded_scroll_input(w,s,m,a,b))goto finished;
        if(s->edit_scrollbars && (m==WM_NCMOUSEMOVE || m==WM_NCMOUSELEAVE))goto finished;
        if(s->edit_scrollbars && m==WM_NCPAINT){nonclient_scrollbars(w,s,NULL);goto finished;}
        if(s->edit_scrollbars && m==WM_SETREDRAW)s->list_redraw_off=!a;
        {BOOL suppress=s->edit_scrollbars && !s->list_redraw_off &&
            (m==WM_VSCROLL || m==WM_HSCROLL || m==WM_MOUSEWHEEL || m==EM_LINESCROLL || m==WM_SETTEXT ||
            m==EM_REPLACESEL || m==EM_UNDO || m==WM_UNDO || m==WM_KEYDOWN || m==WM_CHAR ||
            (m==WM_MOUSEMOVE && GetCapture()==w && (a&MK_LBUTTON)) || m==WM_LBUTTONDOWN || m==WM_LBUTTONUP);
        if(suppress && s->list_suppression++==0)DefSubclassProc(w,WM_SETREDRAW,FALSE,0);
        result=DefSubclassProc(w,m,a,b);
        if(suppress && --s->list_suppression==0 && !s->detached && !s->list_redraw_off)
            DefSubclassProc(w,WM_SETREDRAW,TRUE,0);
        if(suppress && !s->detached)RedrawWindow(w,NULL,NULL,RDW_INVALIDATE|RDW_FRAME|RDW_UPDATENOW);
        }
        if(!s->detached && (m==WM_PRINTCLIENT || (m==WM_PRINT && (b&PRF_CLIENT)) || m==WM_PAINT || m==WM_ENABLE))
            edit_border(w,s,(m==WM_PRINT || m==WM_PRINTCLIENT)?(HDC)a:NULL);
        if(!s->detached && (m==WM_NCPAINT || (m==WM_PRINT && (b&PRF_NONCLIENT))))
            nonclient_scrollbars(w,s,m==WM_PRINT?(HDC)a:NULL);
        else if(!s->detached && (m==WM_VSCROLL || m==WM_HSCROLL || m==WM_MOUSEWHEEL ||
            m==EM_LINESCROLL || m==WM_SETTEXT || m==WM_KEYDOWN || m==WM_CHAR))
            nonclient_scrollbars(w,s,NULL);
    }
    /* Bitmap statics have no themed chrome or text. Keep the native image
     * painter and image ownership, rather than inventing a second blitter. */
    else if(is_static(w) && (GetWindowLongPtrW(w,GWL_STYLE)&SS_TYPEMASK)==SS_BITMAP){
        if(m==WM_PRINTCLIENT){
            HDC dc=(HDC)a;int saved=SaveDC(dc);RECT r;
            /* The centered native painter excludes its image rectangle while
             * filling the border. Do not leak that clip into the caller DC. */
            if(saved){GetClientRect(w,&r);IntersectClipRect(dc,r.left,r.top,r.right,r.bottom);
                result=DefSubclassProc(w,m,a,b);RestoreDC(dc,saved);}
        }else result=DefSubclassProc(w,m,a,b);
    }
    else if(m==WM_SETREDRAW){s->list_redraw_off=!a;result=DefSubclassProc(w,m,a,b);}
    else if(m==WM_PAINT){PAINTSTRUCT ps;HDC dc=BeginPaint(w,&ps);present_control(w,s,dc,draw);EndPaint(w,&ps);}
    else if(m==WM_PRINTCLIENT)draw(w,s,(HDC)a);
    else if(m==WM_ERASEBKGND)result=1;
    else {
        BOOL moving=m==WM_MOUSEMOVE && !is_static(w),changed=FALSE;
        LRESULT previous=moving?SendMessageW(w,BM_GETSTATE,0,0):0;
        /* Only bracket native drawing leaves, not BM_CLICK or mouse/key
         * dispatch: automatic radio-group traversal must see the original
         * visibility throughout the outer input operation. */
        BOOL suppress=!is_static(w) && !s->list_redraw_off &&
            (m==BM_SETCHECK || m==BM_SETSTATE || m==BM_SETSTYLE || m==WM_ENABLE ||
             m==WM_SETFOCUS || m==WM_KILLFOCUS || m==WM_SETTEXT || m==WM_SETFONT ||
             m==WM_MOUSEMOVE);
        if(suppress && s->list_suppression++==0)DefSubclassProc(w,WM_SETREDRAW,FALSE,0);
        result=DefSubclassProc(w,m,a,b);
        if(suppress && --s->list_suppression==0 && !s->detached && !s->list_redraw_off)
            DefSubclassProc(w,WM_SETREDRAW,TRUE,0);
        if(!s->detached && moving)changed=previous!=SendMessageW(w,BM_GETSTATE,0,0);
        if(!s->detached && s->calls==1 && !s->list_suppression && !s->list_redraw_off && (m==BM_SETCHECK || m==BM_SETSTATE || m==BM_SETSTYLE || m==BM_CLICK || m==WM_ENABLE ||
            m==WM_SETFOCUS || m==WM_KILLFOCUS || m==WM_SETTEXT || m==WM_SETFONT ||
            (is_static(w) && (m==STM_SETICON || m==STM_SETIMAGE)) ||
            m==WM_LBUTTONDOWN || m==WM_LBUTTONUP || changed || m==WM_KEYDOWN || m==WM_KEYUP))
            RedrawWindow(w,NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW);
    }
finished:
    if(!--s->calls && s->detached)dispose_control(s);return result;
}

static BOOL install(HWND w,const win16_style_palette *palette,BOOL edit,BOOL original_border,BOOL frame_bars,
    int tab_count,const int *tabs)
{
    DWORD_PTR existing;control_style *s;BOOL result=TRUE,tabbed=tab_count>=0;LONG_PTR style=GetWindowLongPtrW(w,GWL_STYLE);
    if(GetWindowThreadProcessId(w,NULL)!=GetCurrentThreadId()){SetLastError(ERROR_INVALID_PARAMETER);return FALSE;}
    if(GetWindowSubclass(w,control_proc,(UINT_PTR)control_proc,&existing)){SetLastError(ERROR_ALREADY_EXISTS);return FALSE;}
    if(tabbed){
        if(!is_list(w) || !(style&LBS_USETABSTOPS) || (style&(LBS_OWNERDRAWFIXED|LBS_OWNERDRAWVARIABLE|LBS_NODATA))){
            SetLastError(ERROR_NOT_SUPPORTED);return FALSE;}
        if((tab_count && !tabs) || (size_t)tab_count>((size_t)-1)/sizeof(int)){
            SetLastError(ERROR_INVALID_PARAMETER);return FALSE;}
    }else if(!frame_bars && (!supported(w) || is_edit(w)!=edit)){SetLastError(ERROR_NOT_SUPPORTED);return FALSE;}
    s=calloc(1,sizeof(*s));if(!s)return FALSE;
    if(tabbed && tab_count){s->tab_positions=malloc((size_t)tab_count*sizeof(int));
        if(!s->tab_positions){free(s);return FALSE;}
        CopyMemory(s->tab_positions,tabs,(size_t)tab_count*sizeof(int));s->tab_count=tab_count;}
    s->edit_border=edit && original_border;
    s->frame_scrollbars=frame_bars;
    if(!frame_bars){s->original_font=(HFONT)SendMessageW(w,WM_GETFONT,0,0);
        /* The font query may reenter installation. Do not replace that
         * successful attachment's subclass data with this outer attempt. */
        if(GetWindowSubclass(w,control_proc,(UINT_PTR)control_proc,&existing)){
            dispose_control(s);SetLastError(ERROR_ALREADY_EXISTS);return FALSE;}
        s->font=win16_style_font_acquire();if(!s->font){dispose_control(s);return FALSE;}}
    if(palette)s->palette=*palette;else win16_style_default_palette(&s->palette);
    if(!SetWindowSubclass(w,control_proc,(UINT_PTR)control_proc,(DWORD_PTR)s)){dispose_control(s);return FALSE;}
    /* Child attachment and WM_SETFONT can synchronously remove/destroy us.
     * Retain this attempt until its result is known, just like a callback. */
    ++s->calls;
    if(!frame_bars && is_combo(w)){
        COMBOBOXINFO info={sizeof(info)};
        if(!GetComboBoxInfo(w,&info) || !win16_style_control_install(info.hwndList,palette)){
            if(!s->detached)win16_style_control_remove(w);result=FALSE;goto installed;}
        if(s->detached){win16_style_control_remove(info.hwndList);goto installed;}
        s->combo_list=info.hwndList;
        if(GetWindowSubclass(info.hwndList,control_proc,(UINT_PTR)control_proc,&existing))
            ((control_style *)existing)->owner=s;
    }
    if(!frame_bars){SendMessageW(w,WM_SETFONT,(WPARAM)s->font,FALSE);
        if(!s->detached)RedrawWindow(w,NULL,NULL,RDW_INVALIDATE|RDW_FRAME);}
installed:
    if(s->detached){result=FALSE;SetLastError(ERROR_CANCELLED);}
    if(!--s->calls && s->detached)dispose_control(s);return result;
}
BOOL win16_style_control_install(HWND w,const win16_style_palette *palette)
{return install(w,palette,FALSE,FALSE,FALSE,-1,NULL);}
BOOL win16_style_tabbed_list_install(HWND w,const win16_style_palette *palette,int count,const int *pixels)
{if(count<0){SetLastError(ERROR_INVALID_PARAMETER);return FALSE;}return install(w,palette,FALSE,FALSE,FALSE,count,pixels);}
BOOL win16_style_frame_scrollbars_install(HWND w,const win16_style_palette *palette)
{return install(w,palette,FALSE,FALSE,TRUE,-1,NULL);}
void win16_style_frame_scrollbars_remove(HWND w)
{
    DWORD_PTR data;
    if(GetWindowSubclass(w,control_proc,(UINT_PTR)control_proc,&data) && ((control_style *)data)->frame_scrollbars)
        win16_style_control_remove(w);
}
void win16_style_frame_scrollbars_palette(HWND w,const win16_style_palette *palette)
{
    DWORD_PTR data;
    if(GetWindowSubclass(w,control_proc,(UINT_PTR)control_proc,&data) && ((control_style *)data)->frame_scrollbars)
        ((control_style *)data)->palette=*palette;
}
BOOL win16_style_get_scroll_info(HWND w,int bar,LPSCROLLINFO info)
{
    DWORD_PTR data;BOOL result=GetScrollInfo(w,bar,info);
    if(result && (bar==SB_HORZ || bar==SB_VERT) && info && info->cbSize==sizeof(*info) &&
        (info->fMask&SIF_TRACKPOS) && GetWindowSubclass(w,control_proc,(UINT_PTR)control_proc,&data)){
        control_style *s=(control_style *)data;
        if(s->frame_scrollbars && s->embedded_object==(bar==SB_HORZ?OBJID_HSCROLL:OBJID_VSCROLL)){
            if(s->scroll_reporting)info->nTrackPos=s->scroll_report;
            else if(s->scroll.active)info->nTrackPos=s->scroll.position;
        }
    }
    return result;
}
int win16_style_set_scroll_info(HWND w,int bar,LPCSCROLLINFO info,BOOL redraw)
{
    DWORD_PTR data;int result;
    BOOL styled=(bar==SB_HORZ || bar==SB_VERT) &&
        GetWindowSubclass(w,control_proc,(UINT_PTR)control_proc,&data) && ((control_style *)data)->frame_scrollbars;
    result=SetScrollInfo(w,bar,info,styled?FALSE:redraw);
    if(styled && redraw){
        if(GetWindowSubclass(w,control_proc,(UINT_PTR)control_proc,&data)){
            HDC dc=GetWindowDC(w);
            if(dc){draw_scroll(w,(control_style *)data,dc,bar==SB_HORZ?OBJID_HSCROLL:OBJID_VSCROLL,bar==SB_VERT);ReleaseDC(w,dc);}
        }else RedrawWindow(w,NULL,NULL,RDW_FRAME|RDW_INVALIDATE);
    }
    return result;
}
BOOL win16_style_list_scrollbar_enable(HWND w)
{
    DWORD_PTR data;
    if(GetWindowThreadProcessId(w,NULL)!=GetCurrentThreadId() || !is_list(w) ||
        !GetWindowSubclass(w,control_proc,(UINT_PTR)control_proc,&data)){
        SetLastError(ERROR_NOT_SUPPORTED);return FALSE;}
    ((control_style *)data)->list_scrollbar=TRUE;
    RedrawWindow(w,NULL,NULL,RDW_FRAME|RDW_INVALIDATE);return TRUE;
}
BOOL win16_style_edit_install(HWND w,BOOL original_border,const win16_style_palette *palette)
{return install(w,palette,TRUE,original_border,FALSE,-1,NULL);}
BOOL win16_style_edit_scrollbars_enable(HWND w)
{
    DWORD_PTR data;LONG_PTR style=GetWindowLongPtrW(w,GWL_STYLE);
    if(GetWindowThreadProcessId(w,NULL)!=GetCurrentThreadId() || !is_edit(w) ||
        !(style&ES_MULTILINE) || !(style&(WS_HSCROLL|WS_VSCROLL)) ||
        !GetWindowSubclass(w,control_proc,(UINT_PTR)control_proc,&data)){
        SetLastError(ERROR_NOT_SUPPORTED);return FALSE;}
    ((control_style *)data)->edit_scrollbars=TRUE;
    RedrawWindow(w,NULL,NULL,RDW_FRAME|RDW_INVALIDATE);return TRUE;
}
BOOL win16_style_control_remove(HWND w)
{
    DWORD_PTR data;control_style *s;
    if(GetWindowThreadProcessId(w,NULL)!=GetCurrentThreadId() ||
        !GetWindowSubclass(w,control_proc,(UINT_PTR)control_proc,&data))return FALSE;
    s=(control_style *)data;
    if(s->removing)return TRUE;
    ++s->calls;s->removing=TRUE;
    /* End the live gesture while queries still see its full-width report.
     * The application may remove again or destroy the HWND in this notice. */
    scroll_finish(w,s,TRUE);
    if(s->detached){if(!--s->calls)dispose_control(s);return TRUE;}
    if(!RemoveWindowSubclass(w,control_proc,(UINT_PTR)control_proc)){
        s->removing=FALSE;--s->calls;return FALSE;}
    s->detached=TRUE;detach_combo_list(s);
    if(s->font && IsWindow(w))SendMessageW(w,WM_SETFONT,(WPARAM)s->original_font,FALSE);
    scroll_caret_destroy(w,s);
    if(s->list_suppression && !s->list_redraw_off)SendMessageW(w,WM_SETREDRAW,TRUE,0);
    scroll_stop_timer(w,s);
    if(s->scroll.active){s->scroll.active=FALSE;scroll_release_capture(w,s);}
    if(!--s->calls)dispose_control(s);RedrawWindow(w,NULL,NULL,RDW_INVALIDATE|RDW_FRAME);
    if(is_scroll(w) && GetFocus()==w)SendMessageW(w,WM_SETFOCUS,0,0);
    return TRUE;
}
