/* Actual DLL dialog palette and native SysLink behavior, no desktop switch. */
#define NTW16S_STATIC
#include "ntw16s-dll/ntw16s.h"
#include <commctrl.h>
#include <stdio.h>
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#define CHECK(x) do{if(!(x)){fprintf(stderr,"DIALOG FAIL line=%d error=%lu: %s\n",__LINE__,GetLastError(),#x);ExitProcess(1);}}while(0)
typedef ntw16s_creation_scope *(__cdecl *begin_fn)(BOOL);
typedef BOOL (__cdecl *end_fn)(ntw16s_creation_scope *,ntw16s_creation_result *);
typedef BOOL (__cdecl *window_fn)(HWND);
typedef win16_style_control_scope *(__cdecl *controls_begin_fn)(HWND,const win16_style_palette *);
typedef BOOL (__cdecl *controls_end_fn)(win16_style_control_scope *,win16_style_control_counts *);
static begin_fn begin_scope;
static end_fn end_scope;
static window_fn attached,remove_style;
static controls_begin_fn controls_begin;
static controls_end_fn controls_end;
static HWND link;
static unsigned clicks,returns;
static HBRUSH original_brush;
static INT_PTR CALLBACK dialog_proc(HWND w,UINT m,WPARAM a,LPARAM b)
{
    (void)a;
    if(m==WM_CTLCOLORDLG || m==WM_CTLCOLORSTATIC)return (INT_PTR)original_brush;
    if(m==WM_NOTIFY){NMLINK *n=(NMLINK *)b;
        if(n->hdr.hwndFrom==link && (n->hdr.code==NM_CLICK || n->hdr.code==NM_RETURN)){
            CHECK(n->item.iLink==0 && !wcscmp(n->item.szID,L"sample"));
            CHECK(!wcscmp(n->item.szUrl,L"https://example.invalid/"));
            if(n->hdr.code==NM_CLICK)++clicks;else ++returns;
            return TRUE;
        }
    }
    if(m==WM_COMMAND && LOWORD(a)==IDCANCEL){DestroyWindow(w);return TRUE;}
    return FALSE;
}
static void uniform_font(HWND w)
{
    HDC dc=GetDC(w);HFONT font=(HFONT)SendMessageW(w,WM_GETFONT,0,0),old;TEXTMETRICW tm;
    if(!font){WCHAR name[32];GetClassNameW(w,name,32);fwprintf(stderr,L"NO FONT class=%ls id=%d\n",name,GetDlgCtrlID(w));}
    CHECK(dc && font);old=SelectObject(dc,font);CHECK(GetTextMetricsW(dc,&tm));
    CHECK(tm.tmHeight==20);SelectObject(dc,old);ReleaseDC(w,dc);
}
static COLORREF background(HWND w)
{
    RECT r;HDC dc=CreateCompatibleDC(NULL);HBITMAP bitmap,old;COLORREF color;
    GetClientRect(w,&r);bitmap=CreateBitmap(r.right,r.bottom,1,32,NULL);CHECK(dc && bitmap);
    old=SelectObject(dc,bitmap);CHECK(SendMessageW(w,WM_ERASEBKGND,(WPARAM)dc,0));
    GdiFlush();color=GetPixel(dc,r.right-2,r.bottom-2);
    SelectObject(dc,old);DeleteObject(bitmap);DeleteDC(dc);return color;
}
static BOOL CALLBACK remove_child(HWND w,LPARAM unused)
{
    RECT before,after;WCHAR name[32];DWORD style=(DWORD)GetWindowLongPtrW(w,GWL_STYLE),ex=(DWORD)GetWindowLongPtrW(w,GWL_EXSTYLE);
    (void)unused;GetWindowRect(w,&before);
    if(attached(w))CHECK(remove_style(w));
    GetWindowRect(w,&after);GetClassNameW(w,name,32);
    if(!wcscmp(name,L"Static") && (style&SS_TYPEMASK)>=SS_ETCHEDHORZ && (style&SS_TYPEMASK)<=SS_ETCHEDFRAME)
        CHECK(EqualRect(&before,&after));
    CHECK((DWORD)GetWindowLongPtrW(w,GWL_STYLE)==style && (DWORD)GetWindowLongPtrW(w,GWL_EXSTYLE)==ex);
    return TRUE;
}
static void separator_pixels(HWND w)
{
    HDC dc=CreateCompatibleDC(NULL);HBITMAP bitmap=CreateBitmap(180,60,1,32,NULL),old;
    UINT messages[]={WM_PRINTCLIENT,WM_PRINT};unsigned i;RECT r={0,0,180,60};
    CHECK(dc && bitmap);old=SelectObject(dc,bitmap);
    {RECT c;GetClientRect(w,&c);printf("SEPARATOR client=%ld,%ld style=%lx ex=%lx\n",c.right,c.bottom,(DWORD)GetWindowLongPtrW(w,GWL_STYLE),(DWORD)GetWindowLongPtrW(w,GWL_EXSTYLE));}
    for(i=0;i<ARRAYSIZE(messages);++i){
        FillRect(dc,&r,(HBRUSH)GetStockObject(WHITE_BRUSH));
        SendMessageW(w,messages[i],(WPARAM)dc,PRF_CLIENT|PRF_ERASEBKGND);GdiFlush();
        printf("SEPARATOR message=%x pixel=%08lx\n",messages[i],GetPixel(dc,80,0));fflush(stdout);
        CHECK(GetPixel(dc,80,0)==RGB(0,0,0));
    }
    SelectObject(dc,old);DeleteObject(bitmap);DeleteDC(dc);
}
static void one_case(BOOL enabled,HWND owner)
{
    struct {DLGTEMPLATE d;WORD menu,cls,title;} t={0};
    ntw16s_creation_scope *scope;ntw16s_creation_result result;
    HWND w,control;LITEM item={0};LHITTESTINFO hit={0};HFONT latest;
    int i,x,y;BOOL found=FALSE;HDC dc;HBITMAP bitmap,old;
    static const struct {const WCHAR *name;DWORD type;} controls[]={
        {L"BUTTON",BS_PUSHBUTTON},{L"BUTTON",BS_DEFPUSHBUTTON},{L"BUTTON",BS_GROUPBOX},
        {L"BUTTON",BS_AUTOCHECKBOX},{L"BUTTON",BS_AUTO3STATE},{L"BUTTON",BS_AUTORADIOBUTTON},
        {L"STATIC",SS_LEFT},{L"STATIC",SS_CENTER},{L"STATIC",SS_RIGHT},{L"STATIC",SS_SIMPLE},
        {L"STATIC",SS_LEFTNOWORDWRAP},{L"STATIC",SS_BLACKFRAME},{L"STATIC",SS_GRAYRECT},
        {L"STATIC",SS_ETCHEDHORZ},{L"STATIC",SS_ETCHEDVERT},{L"STATIC",SS_ETCHEDFRAME},
        {L"EDIT",ES_MULTILINE},{L"EDIT",ES_READONLY},{L"LISTBOX",LBS_NOTIFY},
        {L"COMBOBOX",CBS_DROPDOWNLIST},{L"SCROLLBAR",SBS_HORZ}
    };
    t.d.style=WS_POPUP|WS_VISIBLE|WS_CAPTION|WS_SYSMENU|DS_MODALFRAME;
    t.d.cx=340;t.d.cy=200;
    scope=begin_scope(enabled);CHECK(scope);
    w=CreateDialogIndirectParamW(GetModuleHandleW(NULL),&t.d,owner,dialog_proc,0);CHECK(w);
    CHECK(attached(w)==enabled);
    for(i=0;i<ARRAYSIZE(controls);++i){
        control=CreateWindowW(controls[i].name,L"Sample",WS_CHILD|controls[i].type,
            10,70,180,60,w,(HMENU)(INT_PTR)(100+i),NULL,NULL);CHECK(control);
        wprintf(L"CLASS index=%d name=%ls type=%lx attached=%d enabled=%d\n",i,controls[i].name,controls[i].type,attached(control),enabled);fflush(stdout);
        CHECK(attached(control)==enabled);
        if(enabled && !wcscmp(controls[i].name,L"STATIC") && controls[i].type==SS_ETCHEDHORZ)separator_pixels(control);
        if(enabled && wcscmp(controls[i].name,L"SCROLLBAR") &&
           (wcscmp(controls[i].name,L"STATIC") || controls[i].type==SS_LEFT ||
            controls[i].type==SS_CENTER || controls[i].type==SS_RIGHT ||
            controls[i].type==SS_SIMPLE || controls[i].type==SS_LEFTNOWORDWRAP))uniform_font(control);
    }
    link=CreateWindowW(WC_LINK,L"Read <a id=\"sample\" href=\"https://example.invalid/\">terms</a> now",
        WS_CHILD|WS_VISIBLE|WS_TABSTOP,10,10,420,48,w,(HMENU)10,NULL,NULL);CHECK(link);
    CHECK(attached(link)==enabled);if(enabled)uniform_font(link);
    CHECK(end_scope(scope,&result) && !result.failure && !result.unsupported);
    if(enabled){
        win16_style_control_counts counts;win16_style_control_scope *child_scope=controls_begin(w,NULL);HWND late;
        CHECK(child_scope);late=CreateWindowW(WC_LINK,L"<a>Late child</a>",WS_CHILD,0,0,200,30,w,(HMENU)11,NULL,NULL);
        CHECK(late && controls_end(child_scope,&counts) && counts.installed==1 && !counts.failed && !counts.unsupported);
        uniform_font(late);CHECK(DestroyWindow(late));
    }
    CHECK(background(w)==(enabled?RGB(255,255,255):RGB(31,63,95)));
    dc=CreateCompatibleDC(NULL);bitmap=CreateBitmap(420,48,1,32,NULL);CHECK(dc && bitmap);
    old=SelectObject(dc,bitmap);
    SendMessageW(link,WM_PRINTCLIENT,(WPARAM)dc,PRF_CLIENT|PRF_ERASEBKGND);GdiFlush();
    CHECK(GetPixel(dc,419,47)==(enabled?RGB(255,255,255):RGB(31,63,95)));
    SelectObject(dc,old);DeleteObject(bitmap);DeleteDC(dc);
    item.mask=LIF_ITEMINDEX|LIF_STATE;item.iLink=0;item.stateMask=LIS_ENABLED|LIS_FOCUSED;
    item.state=LIS_ENABLED|LIS_FOCUSED;CHECK(SendMessageW(link,LM_SETITEM,0,(LPARAM)&item));
    CHECK(SendMessageW(link,LM_GETITEM,0,(LPARAM)&item) && (item.state&LIS_ENABLED));
    for(y=0;y<48 && !found;++y)for(x=0;x<420;++x){hit.pt.x=x;hit.pt.y=y;
        if(SendMessageW(link,LM_HITTEST,0,(LPARAM)&hit)){found=TRUE;break;}}
    CHECK(found);clicks=returns=0;
    SendMessageW(link,WM_MOUSEMOVE,0,MAKELPARAM(hit.pt.x,hit.pt.y));
    SendMessageW(link,WM_LBUTTONDOWN,MK_LBUTTON,MAKELPARAM(hit.pt.x,hit.pt.y));
    SendMessageW(link,WM_LBUTTONUP,0,MAKELPARAM(hit.pt.x,hit.pt.y));CHECK(clicks==1);
    SetFocus(link);CHECK(GetFocus()==link);
    SendMessageW(link,WM_KEYDOWN,VK_RETURN,0);CHECK(returns==1);
    EnableWindow(link,FALSE);CHECK(!IsWindowEnabled(link));EnableWindow(link,TRUE);
    latest=(HFONT)GetStockObject(ANSI_FIXED_FONT);SendMessageW(link,WM_SETFONT,(WPARAM)latest,FALSE);
    if(enabled){uniform_font(link);EnumChildWindows(w,remove_child,0);CHECK(remove_style(w));}
    CHECK((HFONT)SendMessageW(link,WM_GETFONT,0,0)==latest);
    CHECK(background(w)==RGB(31,63,95));CHECK(DestroyWindow(w));
    printf("DIALOG case enabled=%d owned=%d classes=%u link click=%u return=%u PASS\n",
        enabled,owner!=NULL,(unsigned)ARRAYSIZE(controls),clicks,returns);
}
int wmain(int argc,WCHAR **argv)
{
    HDESK desktop,original;WCHAR name[64];HMODULE dll;INITCOMMONCONTROLSEX cc={sizeof(cc),ICC_LINK_CLASS};
    HWND owner;DWORD before,after;int i;
    CHECK(argc==2);SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    original=GetThreadDesktop(GetCurrentThreadId());swprintf_s(name,64,L"StyleDialog-%lu",GetCurrentProcessId());
    desktop=CreateDesktopW(name,NULL,NULL,0,GENERIC_ALL,NULL);CHECK(desktop && SetThreadDesktop(desktop));
    CHECK(InitCommonControlsEx(&cc));
    dll=LoadLibraryExW(argv[1],NULL,LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR|LOAD_LIBRARY_SEARCH_SYSTEM32);CHECK(dll);
    begin_scope=(begin_fn)(void *)GetProcAddress(dll,"Ntw16sCreationBegin");
    end_scope=(end_fn)(void *)GetProcAddress(dll,"Ntw16sCreationEnd");
    attached=(window_fn)(void *)GetProcAddress(dll,"Ntw16sWindowAttached");
    remove_style=(window_fn)(void *)GetProcAddress(dll,"Ntw16sWindowRemove");
    controls_begin=(controls_begin_fn)(void *)GetProcAddress(dll,"win16_style_controls_begin");
    controls_end=(controls_end_fn)(void *)GetProcAddress(dll,"win16_style_controls_end");
    CHECK(begin_scope && end_scope && attached && remove_style && controls_begin && controls_end);
    original_brush=CreateSolidBrush(RGB(31,63,95));CHECK(original_brush);
    {WNDCLASSW cls={0};ntw16s_creation_scope *scope;ntw16s_creation_result result;HWND custom;
        cls.lpfnWndProc=DefWindowProcW;cls.hInstance=GetModuleHandleW(NULL);
        cls.lpszClassName=L"DialogCoverageCustom";cls.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);CHECK(RegisterClassW(&cls));
        scope=begin_scope(TRUE);CHECK(scope);
        custom=CreateWindowW(cls.lpszClassName,L"Custom client preserved",WS_OVERLAPPEDWINDOW,
            0,0,200,150,NULL,NULL,cls.hInstance,NULL);
        CHECK(custom && end_scope(scope,&result) && !result.failure && attached(custom));
        CHECK(background(custom)==RGB(0,0,0));CHECK(DestroyWindow(custom));
        CHECK(UnregisterClassW(cls.lpszClassName,cls.hInstance));
    }
    owner=CreateWindowW(L"STATIC",L"Native owner",WS_OVERLAPPEDWINDOW,0,0,100,100,NULL,NULL,NULL,NULL);CHECK(owner);
    one_case(FALSE,NULL);one_case(TRUE,NULL);one_case(TRUE,owner);
    for(i=0;i<10;++i)one_case(TRUE,owner);
    GdiFlush();before=GetGuiResources(GetCurrentProcess(),GR_GDIOBJECTS);
    for(i=0;i<10;++i)one_case(TRUE,owner);
    GdiFlush();after=GetGuiResources(GetCurrentProcess(),GR_GDIOBJECTS);
    printf("DIALOG GDI=%lu/%lu\n",before,after);CHECK(before==after);
    CHECK(DestroyWindow(owner) && DeleteObject(original_brush));CHECK(FreeLibrary(dll));
    CHECK(SetThreadDesktop(original) && CloseDesktop(desktop));puts("DIALOG PASS");return 0;
}
