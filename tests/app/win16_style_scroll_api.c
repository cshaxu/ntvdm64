#include <windows.h>
#include <oleacc.h>
#include <stdio.h>

/* Read-only API-boundary probe. A system SCROLLBAR and a custom class receive
 * the same query handlers; count which public APIs actually dispatch them.
 * No product window, input, theme or global setting is touched. */
static WNDPROC native_proc;
static unsigned info_messages,bar_messages;
static LRESULT CALLBACK query_proc(HWND w,UINT m,WPARAM a,LPARAM b)
{
    if(m==SBM_GETSCROLLINFO){
        SCROLLINFO *info=(SCROLLINFO *)b;++info_messages;
        if(!info || info->cbSize!=sizeof(*info))return FALSE;
        if(info->fMask&SIF_RANGE){info->nMin=0;info->nMax=200000;}
        if(info->fMask&SIF_PAGE)info->nPage=20;
        if(info->fMask&SIF_POS)info->nPos=31;
        if(info->fMask&SIF_TRACKPOS)info->nTrackPos=123456;
        return TRUE;
    }
    if(m==SBM_GETSCROLLBARINFO){
        SCROLLBARINFO *info=(SCROLLBARINFO *)b;++bar_messages;
        if(!info || info->cbSize!=sizeof(*info))return FALSE;
        GetWindowRect(w,&info->rcScrollBar);info->dxyLineButton=17;
        info->xyThumbTop=50;info->xyThumbBottom=67;return TRUE;
    }
    return native_proc?CallWindowProcW(native_proc,w,m,a,b):DefWindowProcW(w,m,a,b);
}
static int frame_query_probe(void)
{
    HWND w=CreateWindowExW(0,L"Win16ScrollApiProbe",L"frame",WS_OVERLAPPEDWINDOW|WS_HSCROLL|WS_VSCROLL,
        0,0,400,300,NULL,NULL,GetModuleHandleW(NULL),NULL);int axis;
    if(!w)return 80;
    for(axis=SB_HORZ;axis<=SB_VERT;++axis){
        SCROLLINFO initial={sizeof(initial),SIF_RANGE|SIF_PAGE|SIF_POS,0,200000,20,31,0};
        SCROLLINFO result={sizeof(result),SIF_ALL};SCROLLINFO track={sizeof(track),SIF_TRACKPOS,0,0,0,0,123456};
        SetScrollInfo(w,axis,&initial,FALSE);info_messages=bar_messages=0;
        if(!GetScrollInfo(w,axis,&result) || info_messages || result.nPos!=31)return 81;
        printf("FRAME QUERY axis=%d dispatched=%u pos=%ld track=%ld\n",axis,info_messages,result.nPos,result.nTrackPos);
        SetScrollInfo(w,axis,&track,FALSE);
        if(!GetScrollInfo(w,axis,&result) || info_messages || result.nTrackPos==123456 || result.nPos!=31)return 82;
        printf("FRAME TRACK WRITE axis=%d requested=123456 actual=%ld (not a writable native tracking slot)\n",axis,result.nTrackPos);
    }
    DestroyWindow(w);return 0;
}

static int inspect(HWND w,const char *name)
{
    SCROLLINFO info={sizeof(info),SIF_ALL};SCROLLBARINFO bar={sizeof(bar)};
    BOOL got_info,got_bar;unsigned count_info,count_bar;
    info_messages=bar_messages=0;
    got_info=GetScrollInfo(w,SB_CTL,&info);count_info=info_messages;
    got_bar=GetScrollBarInfo(w,OBJID_CLIENT,&bar);count_bar=bar_messages;
    printf("%s API info=%d dispatched=%u pos=%ld track=%ld; bar=%d dispatched=%u thumb=%ld..%ld\n",
        name,got_info,count_info,info.nPos,info.nTrackPos,got_bar,count_bar,bar.xyThumbTop,bar.xyThumbBottom);
    if(!got_info || !got_bar)return 1;
    /* Explicit messages prove the installed procedure is live even if the
     * public API bypasses it for the built-in system class. */
    info_messages=bar_messages=0;
    if(!SendMessageW(w,SBM_GETSCROLLINFO,0,(LPARAM)&info) ||
        !SendMessageW(w,SBM_GETSCROLLBARINFO,0,(LPARAM)&bar))return 2;
    printf("%s direct info=%u track=%ld; bar=%u thumb=%ld..%ld\n",name,
        info_messages,info.nTrackPos,bar_messages,bar.xyThumbTop,bar.xyThumbBottom);
    if(info_messages!=1 || bar_messages!=1 || info.nTrackPos!=123456 ||
        bar.xyThumbTop!=50 || bar.xyThumbBottom!=67)return 3;
    if(!native_proc && (count_info!=1 || count_bar!=1))return 4;
    return 0;
}
static void list_state(HWND list,const char *kind,const char *step)
{
    SCROLLINFO info={sizeof(info),SIF_ALL};RECT client;
    BOOL got=GetScrollInfo(list,SB_VERT,&info);GetClientRect(list,&client);
    printf("LIST BOUNDARY %s %s styleBar=%d top=%ld selected=%ld range=%d/%ld/%ld/%u client=%ldx%ld\n",
        kind,step,(GetWindowLongPtrW(list,GWL_STYLE)&WS_VSCROLL)!=0,
        (LONG)SendMessageW(list,LB_GETTOPINDEX,0,0),(LONG)SendMessageW(list,LB_GETCURSEL,0,0),
        got,info.nMin,info.nMax,info.nPage,client.right,client.bottom);
}
static int hidden_bar_probe(void)
{
    HWND parent,list,combo=NULL;int kind,n,late;COMBOBOXINFO cb={sizeof(cb)};
    parent=CreateWindowExW(0,L"STATIC",L"private probe",WS_POPUP|WS_VISIBLE,
        0,0,400,400,NULL,NULL,GetModuleHandleW(NULL),NULL);
    if(!parent)return 20;
    for(late=0;late<2;++late)for(kind=0;kind<4;++kind){
        const char *name=kind==0?"list":kind==1?"dropdownlist":kind==2?"dropdown":"simple";
        DWORD bar_style=late?0:WS_VSCROLL;
        printf("LIST CREATION delayedBar=%d kind=%s\n",late,name);
        if(kind){
            combo=CreateWindowExW(0,L"COMBOBOX",L"",WS_CHILD|WS_VISIBLE|bar_style|
                (kind==1?CBS_DROPDOWNLIST:kind==2?CBS_DROPDOWN:CBS_SIMPLE),
                0,0,200,160,parent,NULL,GetModuleHandleW(NULL),NULL);
            if(!combo || !GetComboBoxInfo(combo,&cb))return 21;list=cb.hwndList;
        }else list=CreateWindowExW(0,L"LISTBOX",L"",WS_CHILD|WS_VISIBLE|WS_BORDER|bar_style,
            0,0,200,160,parent,NULL,GetModuleHandleW(NULL),NULL);
        if(!list)return 22;
        for(n=0;n<40;++n)SendMessageW(kind?combo:list,kind?CB_ADDSTRING:LB_ADDSTRING,0,(LPARAM)L"Boundary item");
        if(kind)SendMessageW(combo,CB_SHOWDROPDOWN,TRUE,0);
        if(late){
            SCROLLINFO external={sizeof(external),SIF_RANGE|SIF_PAGE|SIF_POS,0,39,8,0,0};
            SetScrollInfo(list,SB_VERT,&external,FALSE);
        }
        list_state(list,name,"before");
        if(!late){ShowScrollBar(list,SB_VERT,FALSE);list_state(list,name,"hidden");}
        SendMessageW(list,WM_MOUSEWHEEL,MAKEWPARAM(0,(WORD)-WHEEL_DELTA),0);list_state(list,name,"wheel");
        SendMessageW(list,LB_SETTOPINDEX,10,0);list_state(list,name,"settop");
        SendMessageW(list,LB_SETCURSEL,20,0);list_state(list,name,"select");
        SendMessageW(kind?combo:list,kind?CB_ADDSTRING:LB_ADDSTRING,0,(LPARAM)L"New item");list_state(list,name,"add");
        SendMessageW(list,WM_MOUSEMOVE,0,MAKELPARAM(8,40));list_state(list,name,"move");
        if(kind){SendMessageW(combo,CB_SHOWDROPDOWN,FALSE,0);SendMessageW(combo,CB_SHOWDROPDOWN,TRUE,0);list_state(list,name,"reopen");}
        DestroyWindow(kind?combo:list);
    }
    DestroyWindow(parent);return 0;
}
static int horizontal_list_probe(void)
{
    HWND parent,list;int multi,n,status=0;
    parent=CreateWindowExW(0,L"STATIC",L"horizontal boundary",WS_POPUP|WS_VISIBLE,
        0,0,400,400,NULL,NULL,GetModuleHandleW(NULL),NULL);
    if(!parent)return 30;
    for(multi=0;multi<2;++multi){
        SCROLLINFO info={sizeof(info),SIF_ALL};RECT before,after,client;
        LONG rows,item_height,expected;LRESULT top;
        list=CreateWindowExW(0,L"LISTBOX",L"",WS_CHILD|WS_VISIBLE|WS_BORDER|WS_HSCROLL|
            (multi?LBS_MULTICOLUMN:0),0,0,240,160,parent,NULL,GetModuleHandleW(NULL),NULL);
        if(!list){status=31;break;}
        SendMessageW(list,WM_SETFONT,(WPARAM)GetStockObject(SYSTEM_FONT),FALSE);
        if(multi)SendMessageW(list,LB_SETCOLUMNWIDTH,100,0);
        else SendMessageW(list,LB_SETHORIZONTALEXTENT,200000,0);
        for(n=0;n<100;++n)SendMessageW(list,LB_ADDSTRING,0,(LPARAM)L"Horizontal contract item");
        GetClientRect(list,&client);item_height=(LONG)SendMessageW(list,LB_GETITEMHEIGHT,0,0);
        if(item_height<=0){DestroyWindow(list);status=32;break;}
        rows=max(1,client.bottom/item_height);
        SendMessageW(list,WM_HSCROLL,MAKEWPARAM(SB_THUMBPOSITION,2),0);
        top=SendMessageW(list,LB_GETTOPINDEX,0,0);
        if(!GetScrollInfo(list,SB_HORZ,&info) || info.nPos!=2 ||
            (multi && top!=2*rows)){DestroyWindow(list);status=33;break;}
        printf("HORIZONTAL CONTRACT multi=%d pos=%ld top=%ld rows=%ld units=%s\n",
            multi,info.nPos,(LONG)top,rows,multi?"columns":"pixels");
        if(!multi){
            expected=70000&0xffff;
            SendMessageW(list,WM_HSCROLL,MAKEWPARAM(SB_THUMBPOSITION,expected),0);
            if(!GetScrollInfo(list,SB_HORZ,&info) || info.nPos!=expected){DestroyWindow(list);status=34;break;}
            if(SendMessageW(list,LB_GETITEMRECT,0,(LPARAM)&before)==LB_ERR){DestroyWindow(list);status=35;break;}
            info.fMask=SIF_POS;info.nPos=70000;SetScrollInfo(list,SB_HORZ,&info,FALSE);
            if(!GetScrollInfo(list,SB_HORZ,&info) ||
                SendMessageW(list,LB_GETITEMRECT,0,(LPARAM)&after)==LB_ERR){DestroyWindow(list);status=36;break;}
            printf("HORIZONTAL WIDTH packed70000=%ld publicPos=%ld itemLeftBefore=%ld after=%ld\n",
                expected,info.nPos,before.left,after.left);
            /* SetScrollInfo changes the bar, not the list's xOrigin. A wide
             * scrollbar update alone cannot implement wide content motion. */
            if(info.nPos!=70000 || !EqualRect(&before,&after)){DestroyWindow(list);status=37;break;}
            SendMessageW(list,WM_HSCROLL,SB_LINERIGHT,0);info.fMask=SIF_ALL;
            if(!GetScrollInfo(list,SB_HORZ,&info) || info.nPos>=70000){DestroyWindow(list);status=38;break;}
            printf("HORIZONTAL WIDTH next native line restores content-owned pos=%ld\n",info.nPos);
        }
        DestroyWindow(list);
    }
    DestroyWindow(parent);return status;
}
static LRESULT CALLBACK edit_parent_proc(HWND w,UINT m,WPARAM a,LPARAM b)
{
    unsigned *counts=(unsigned *)GetWindowLongPtrW(w,GWLP_USERDATA);
    if(m==WM_NCCREATE){counts=(unsigned *)((CREATESTRUCTW *)b)->lpCreateParams;
        SetWindowLongPtrW(w,GWLP_USERDATA,(LONG_PTR)counts);}
    if(m==WM_COMMAND && b && counts){
        if(HIWORD(a)==EN_VSCROLL)++counts[0];
        if(HIWORD(a)==EN_HSCROLL)++counts[1];
    }
    return DefWindowProcW(w,m,a,b);
}
static int edit_scroll_probe(void)
{
    HWND parent,edit;WCHAR text[22000];int row,col,offset=0,status=0;
    SCROLLINFO horizontal={sizeof(horizontal),SIF_ALL},vertical={sizeof(vertical),SIF_ALL};
    HDC dc;TEXTMETRICW metric;DWORD first,last;LONG edit_width;
    WNDCLASSW cls={0};unsigned counts[2]={0,0};
    for(row=0;row<128;++row){for(col=0;col<160;++col)text[offset++]=(WCHAR)(L'A'+col%26);
        text[offset++]=L'\r';text[offset++]=L'\n';}text[offset]=0;
    cls.lpfnWndProc=edit_parent_proc;cls.hInstance=GetModuleHandleW(NULL);cls.lpszClassName=L"Win16EditApiParent";
    if(!RegisterClassW(&cls))return 39;
    parent=CreateWindowExW(0,cls.lpszClassName,L"edit boundary",WS_POPUP|WS_VISIBLE,
        0,0,400,400,NULL,NULL,cls.hInstance,counts);if(!parent)return 40;
    edit=CreateWindowExW(0,L"EDIT",text,WS_CHILD|WS_VISIBLE|WS_BORDER|WS_HSCROLL|WS_VSCROLL|
        ES_MULTILINE|ES_AUTOHSCROLL|ES_AUTOVSCROLL,0,0,240,160,parent,NULL,GetModuleHandleW(NULL),NULL);
    if(!edit){DestroyWindow(parent);return 41;}
    SendMessageW(edit,WM_SETFONT,(WPARAM)GetStockObject(SYSTEM_FONT),FALSE);
    dc=GetDC(edit);if(!dc){DestroyWindow(parent);return 42;}
    {HGDIOBJ old=SelectObject(dc,GetStockObject(SYSTEM_FONT));SIZE alphabet;
        GetTextMetricsW(dc,&metric);edit_width=metric.tmAveCharWidth;
        /* Read-only reference: ECSetFont -> GdiGetCharDimensions uses the
         * 52-letter extent, rather than tmAveCharWidth, for variable pitch. */
        if(metric.tmPitchAndFamily&TMPF_FIXED_PITCH){
            GetTextExtentPoint32W(dc,L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ",52,&alphabet);
            edit_width=(alphabet.cx/26+1)/2;
        }
        SelectObject(dc,old);}
    ReleaseDC(edit,dc);
    SendMessageW(edit,EM_SETSEL,4,9);
    SendMessageW(edit,EM_LINESCROLL,2,3);
    if(!GetScrollInfo(edit,SB_HORZ,&horizontal) || !GetScrollInfo(edit,SB_VERT,&vertical))status=43;
    printf("EDIT SCROLL delta=2chars/3lines pos=%ldpx/%ldlines metricAverage=%ld editWidth=%ld firstVisible=%ld\n",
        horizontal.nPos,vertical.nPos,metric.tmAveCharWidth,edit_width,(LONG)SendMessageW(edit,EM_GETFIRSTVISIBLELINE,0,0));
    if(horizontal.nPos!=2*edit_width || vertical.nPos!=3 ||
        SendMessageW(edit,EM_GETFIRSTVISIBLELINE,0,0)!=3)status=44;
    SendMessageW(edit,WM_HSCROLL,MAKEWPARAM(SB_THUMBPOSITION,31),0);
    SendMessageW(edit,WM_VSCROLL,MAKEWPARAM(SB_THUMBPOSITION,17),0);
    if(!GetScrollInfo(edit,SB_HORZ,&horizontal) || !GetScrollInfo(edit,SB_VERT,&vertical))status=45;
    SendMessageW(edit,EM_GETSEL,(WPARAM)&first,(LPARAM)&last);
    printf("EDIT THUMB pos=%ldpx/%ldlines selected=%lu..%lu length=%d\n",
        horizontal.nPos,vertical.nPos,first,last,GetWindowTextLengthW(edit));
    if(horizontal.nPos!=31 || vertical.nPos!=17 || first!=4 || last!=9 || GetWindowTextLengthW(edit)!=offset)status=46;
    for(row=0;row<3;++row){
        counts[0]=counts[1]=0;
        if(!row)SendMessageW(edit,EM_LINESCROLL,0,3);
        else SendMessageW(edit,WM_VSCROLL,MAKEWPARAM(row==1?SB_THUMBTRACK:SB_THUMBPOSITION,23+row),0);
        printf("EDIT NOTIFICATION request=%s vertical=%u horizontal=%u first=%ld\n",
            row==0?"line":row==1?"track":"position",counts[0],counts[1],
            (LONG)SendMessageW(edit,EM_GETFIRSTVISIBLELINE,0,0));
        if(counts[0]!=(row==1?0u:1u) || counts[1])status=47;
    }
    {WCHAR *large=HeapAlloc(GetProcessHeap(),0,210001*sizeof(WCHAR));
        if(!large)status=48;
        else{for(row=0;row<70000;++row){large[row*3]=L'a';large[row*3+1]=L'\r';large[row*3+2]=L'\n';}
            large[210000]=0;SendMessageW(edit,WM_SETTEXT,0,(LPARAM)large);
            SendMessageW(edit,WM_VSCROLL,MAKEWPARAM(SB_THUMBTRACK,42),0);
            printf("EDIT WIDE lines=%ld thumb42 first=%ld\n",(LONG)SendMessageW(edit,EM_GETLINECOUNT,0,0),
                (LONG)SendMessageW(edit,EM_GETFIRSTVISIBLELINE,0,0));
            HeapFree(GetProcessHeap(),0,large);
        }
    }
    DestroyWindow(parent);return status;
}
int main(void)
{
    WCHAR desktop_name[80];HDESK desktop;HWND w;int status;
    WNDCLASSW cls={0};SCROLLINFO info={sizeof(info),SIF_RANGE|SIF_PAGE|SIF_POS,0,200000,20,31,0};
    swprintf_s(desktop_name,80,L"win16-scroll-api-%lu",GetCurrentProcessId());
    desktop=CreateDesktopW(desktop_name,NULL,NULL,0,GENERIC_ALL,NULL);
    if(!desktop || !SetThreadDesktop(desktop)){printf("desktop failed %lu\n",GetLastError());return 10;}
    w=CreateWindowExW(0,L"SCROLLBAR",L"",WS_POPUP|SBS_VERT,0,0,17,200,NULL,NULL,GetModuleHandleW(NULL),NULL);
    if(!w)return 11;
    SetScrollInfo(w,SB_CTL,&info,FALSE);
    native_proc=(WNDPROC)SetWindowLongPtrW(w,GWLP_WNDPROC,(LONG_PTR)query_proc);
    if(!native_proc)return 12;
    status=inspect(w,"native");DestroyWindow(w);native_proc=NULL;if(status)return status;
    cls.lpfnWndProc=query_proc;cls.hInstance=GetModuleHandleW(NULL);cls.lpszClassName=L"Win16ScrollApiProbe";
    if(!RegisterClassW(&cls))return 13;
    w=CreateWindowExW(0,cls.lpszClassName,L"",WS_POPUP,0,0,17,200,NULL,NULL,cls.hInstance,NULL);
    if(!w)return 14;
    status=inspect(w,"custom");DestroyWindow(w);
    /* The private desktop is released at process exit; it is still assigned
     * to this thread, so do not pretend CloseDesktop would release it here. */
    if(status)return status;
    status=frame_query_probe();if(status)return status;
    status=hidden_bar_probe();if(status)return status;
    status=horizontal_list_probe();return status?status:edit_scroll_probe();
}
