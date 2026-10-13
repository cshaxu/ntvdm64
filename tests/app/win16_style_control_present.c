/* Exercise the real screen WM_PAINT path, not just WM_PRINT snapshots. */
#include <windows.h>
#include <stdio.h>
#include <string.h>
static BOOL observe;
static BOOL inject_leak;
static BOOL fail_next_save,fail_next_pattern;
static unsigned failed_saves,failed_patterns,scroll_copies;
static BOOL observe_scroll;
static int WINAPI observed_save(HDC dc)
{if(fail_next_save){fail_next_save=FALSE;++failed_saves;return 0;}return SaveDC(dc);}
static unsigned screen_fills,memory_fills,presents;
static unsigned native_paints;
/* Process totals also contain asynchronous USER/theme caches. Independently
 * require exact balance for every GDI allocation made by the real painter. */
static HGDIOBJ owned_objects[128];
static unsigned owned_live;
static BOOL owned_overflow;
static HGDIOBJ created(HGDIOBJ object)
{
    unsigned i;if(!object)return object;
    for(i=0;i<128;++i)if(!owned_objects[i]){owned_objects[i]=object;++owned_live;return object;}
    owned_overflow=TRUE;return object;
}
static void released(HGDIOBJ object)
{unsigned i;for(i=0;i<128;++i)if(owned_objects[i]==object){owned_objects[i]=NULL;--owned_live;return;}}
static HDC WINAPI observed_dc(HDC dc){return (HDC)created((HGDIOBJ)CreateCompatibleDC(dc));}
static HBITMAP WINAPI observed_bitmap(HDC dc,int x,int y){return (HBITMAP)created(CreateCompatibleBitmap(dc,x,y));}
static HBITMAP WINAPI observed_raw_bitmap(int x,int y,UINT planes,UINT bits,const void *data)
{return (HBITMAP)created(CreateBitmap(x,y,planes,bits,data));}
static HBRUSH WINAPI observed_pattern(HBITMAP bitmap)
{if(fail_next_pattern){fail_next_pattern=FALSE;++failed_patterns;return NULL;}
 return (HBRUSH)created(CreatePatternBrush(bitmap));}
static HBRUSH WINAPI observed_brush(COLORREF color){return (HBRUSH)created(CreateSolidBrush(color));}
static BOOL WINAPI observed_delete(HGDIOBJ object)
{BOOL result=DeleteObject(object);if(result)released(object);return result;}
static BOOL WINAPI observed_delete_dc(HDC dc)
{BOOL result=DeleteDC(dc);if(result)released((HGDIOBJ)dc);return result;}
static LRESULT CALLBACK witness(HWND w,UINT m,WPARAM a,LPARAM b)
{
    if(observe && m>=WM_CTLCOLORMSGBOX && m<=WM_CTLCOLORSTATIC)++native_paints;
    return DefWindowProcW(w,m,a,b);
}
static int WINAPI observed_fill(HDC dc,const RECT *r,HBRUSH brush)
{
    if(observe){if(GetObjectType(dc)==OBJ_MEMDC)++memory_fills;else ++screen_fills;}
    return FillRect(dc,r,brush);
}
static BOOL WINAPI observed_blt(HDC dc,int x,int y,int cx,int cy,HDC from,int sx,int sy,DWORD op)
{
    if(observe_scroll)++scroll_copies;
    if(observe && GetObjectType(dc)!=OBJ_MEMDC)++presents;
    if(observe && inject_leak){HGDIOBJ leak=created(CreateBitmap(1,1,1,1,NULL));
        if(leak){inject_leak=FALSE;printf("INJECTED PAINTER LEAK live=%u\n",owned_live);}}
    return BitBlt(dc,x,y,cx,cy,from,sx,sy,op);
}
#define FillRect observed_fill
#define SaveDC observed_save
#define BitBlt observed_blt
#define CreateCompatibleDC observed_dc
#define CreateCompatibleBitmap observed_bitmap
#define CreateBitmap observed_raw_bitmap
#define CreatePatternBrush observed_pattern
#define CreateSolidBrush observed_brush
#define DeleteObject observed_delete
#define DeleteDC observed_delete_dc
#include "win16-style/controls.c"
#undef FillRect
#undef SaveDC
#undef BitBlt
#undef CreateCompatibleDC
#undef CreateCompatibleBitmap
#undef CreateBitmap
#undef CreatePatternBrush
#undef CreateSolidBrush
#undef DeleteObject
#undef DeleteDC
#define CHECK(x) do{if(!(x)){fprintf(stderr,"CONTROL PRESENT FAIL line=%d error=%lu\n",__LINE__,GetLastError());return 1;}}while(0)
int main(int argc,char **argv)
{
    const WCHAR *classes[]={L"BUTTON",L"BUTTON",L"BUTTON",L"STATIC",L"LISTBOX",L"COMBOBOX"};
    DWORD types[]={BS_PUSHBUTTON,BS_AUTOCHECKBOX,BS_GROUPBOX,SS_LEFT,LBS_NOTIFY,CBS_DROPDOWNLIST};
    WNDCLASSW wc={0};WCHAR name[80];HDESK desktop;HWND root,w;unsigned i,n,warm;DWORD before,after;
    if(argc!=1){if(argc!=2 || strcmp(argv[1],"--leak-negative"))return 2;inject_leak=TRUE;}
    swprintf_s(name,80,L"StyleControlPresent-%lu",GetCurrentProcessId());
    desktop=CreateDesktopW(name,NULL,NULL,0,GENERIC_ALL,NULL);CHECK(desktop && SetThreadDesktop(desktop));
    wc.lpfnWndProc=witness;wc.lpszClassName=L"StyleControlPresent";wc.hInstance=GetModuleHandleW(NULL);
    CHECK(RegisterClassW(&wc));root=CreateWindowW(wc.lpszClassName,L"Presentation",WS_OVERLAPPEDWINDOW|WS_VISIBLE,
        0,0,500,300,NULL,NULL,wc.hInstance,NULL);CHECK(root);ShowWindow(root,SW_SHOWNORMAL);UpdateWindow(root);
    {HDC screen=GetDC(root),target;HBITMAP bitmap,old;control_style state={0};RECT area;
     SCROLLINFO range={sizeof(range),SIF_RANGE|SIF_PAGE|SIF_POS,0,100,10,25};int mode,x,y;
     CHECK(screen);target=CreateCompatibleDC(screen);bitmap=CreateCompatibleBitmap(screen,17,120);
     CHECK(target && bitmap);old=(HBITMAP)SelectObject(target,bitmap);CHECK(old);
     CHECK(ReleaseDC(root,screen));SetRect(&area,0,0,17,120);
     w=CreateWindowW(L"SCROLLBAR",L"",WS_CHILD|WS_VISIBLE|SBS_VERT,10,10,17,120,root,NULL,wc.hInstance,NULL);CHECK(w);
     SetScrollInfo(w,SB_CTL,&range,FALSE);win16_style_default_palette(&state.palette);
     for(mode=0;mode<2;++mode){
        SetDCBrushColor(target,RGB(17,39,61));FillRect(target,&area,(HBRUSH)GetStockObject(DC_BRUSH));
        fail_next_save=mode==0;fail_next_pattern=mode==1;scroll_copies=0;observe_scroll=TRUE;
        draw_scroll(w,&state,target,OBJID_CLIENT,TRUE);observe_scroll=FALSE;
        CHECK(!fail_next_save && !fail_next_pattern && !scroll_copies && !owned_live);
        for(y=0;y<120;++y)for(x=0;x<17;++x)CHECK(GetPixel(target,x,y)==RGB(17,39,61));
     }
     CHECK(failed_saves==1 && failed_patterns==1);scroll_copies=0;observe_scroll=TRUE;
     draw_scroll(w,&state,target,OBJID_CLIENT,TRUE);observe_scroll=FALSE;
     CHECK(scroll_copies==1 && !owned_live && GetPixel(target,0,0)!=RGB(17,39,61));
     SelectObject(target,old);CHECK(DeleteObject(bitmap) && DeleteDC(target) && DestroyWindow(w));
     printf("SCROLL FAILURE SaveDC=1 pattern=1 unchangedPixels=4080 normalPresent=1 objects=0\n");}
    for(i=0;i<sizeof(types)/sizeof(types[0]);++i){
        w=CreateWindowW(classes[i],L"Test",WS_CHILD|WS_VISIBLE|types[i],10,10,300,100,root,NULL,wc.hInstance,NULL);CHECK(w);
        CHECK(win16_style_control_install(w,NULL));
        if(i==4)SendMessageW(w,LB_ADDSTRING,0,(LPARAM)L"List item");
        if(i==5){SendMessageW(w,CB_ADDSTRING,0,(LPARAM)L"Combo item");SendMessageW(w,CB_SETCURSEL,0,0);}
        /* Native USER/theme resources initialize asynchronously even with no
         * style installed (r237 native/old/current phase oracle). Warm the
         * same interaction closure being measured, not paint alone. Keep the
         * no-growth and exact owned-object assertions below. */
        for(warm=0;warm<8;++warm){MSG message;
            for(n=0;n<20;++n)RedrawWindow(w,NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW);
            SetFocus(w);SetFocus(root);
            if(i<2){
                SendMessageW(w,BM_SETSTATE,TRUE,0);SendMessageW(w,BM_SETSTATE,FALSE,0);
                if(i==1)SendMessageW(w,BM_CLICK,0,0);
                SendMessageW(w,WM_LBUTTONDOWN,MK_LBUTTON,MAKELPARAM(12,12));
                SendMessageW(w,WM_LBUTTONUP,0,MAKELPARAM(12,12));
            }
            if(i>=4)SendMessageW(w,i==4?LB_SETCURSEL:CB_SETCURSEL,0,0);
            SetFocus(root);
            while(PeekMessageW(&message,NULL,0,0,PM_REMOVE)){TranslateMessage(&message);DispatchMessageW(&message);}
            GdiFlush();printf("CONTROL WARM class=%ls round=%u GDI=%lu\n",classes[i],warm,
                GetGuiResources(GetCurrentProcess(),GR_GDIOBJECTS));
        }
        GdiFlush();
        before=GetGuiResources(GetCurrentProcess(),GR_GDIOBJECTS);
        for(n=0;n<20;++n){
            screen_fills=memory_fills=presents=0;observe=TRUE;
            RedrawWindow(w,NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW);observe=FALSE;
            if(screen_fills || !memory_fills || presents!=1)
                printf("CONTROL PRESENT DIAGNOSTIC kind=%u round=%u screen=%u memory=%u present=%u\n",i,n,screen_fills,memory_fills,presents);
            CHECK(!screen_fills && memory_fills && presents==1 && !owned_live && !owned_overflow);
        }
        if(i<2){
            screen_fills=memory_fills=presents=native_paints=0;observe=TRUE;
            SendMessageW(w,BM_SETSTATE,TRUE,0);observe=FALSE;
            printf("BUTTON STATE nativePaint=%u screenFill=%u present=%u\n",native_paints,screen_fills,presents);
            CHECK((SendMessageW(w,BM_GETSTATE,0,0)&BST_PUSHED) && !screen_fills && presents && memory_fills && !native_paints);
            SendMessageW(w,BM_SETSTATE,FALSE,0);
            if(i==1){SendMessageW(w,BM_CLICK,0,0);CHECK(SendMessageW(w,BM_GETCHECK,0,0)==BST_CHECKED);}
            SetFocus(root);native_paints=0;observe=TRUE;
            SendMessageW(w,WM_LBUTTONDOWN,MK_LBUTTON,MAKELPARAM(12,12));observe=FALSE;
            CHECK(GetFocus()==w && GetCapture()==w && (SendMessageW(w,BM_GETSTATE,0,0)&BST_PUSHED));
            observe=TRUE;SendMessageW(w,WM_LBUTTONUP,0,MAKELPARAM(12,12));observe=FALSE;
            printf("BUTTON CLICK nativePaint=%u focus=%d captureReleased=%d\n",native_paints,GetFocus()==w,GetCapture()!=w);
            CHECK(!native_paints && GetCapture()!=w && !(SendMessageW(w,BM_GETSTATE,0,0)&BST_PUSHED));
            if(i==1)CHECK(SendMessageW(w,BM_GETCHECK,0,0)==BST_UNCHECKED);
        }
        if(i>=4){
            native_paints=0;observe=TRUE;
            SendMessageW(w,i==4?LB_SETCURSEL:CB_SETCURSEL,0,0);SetFocus(w);observe=FALSE;
            printf("SELECTION class=%ls nativePaint=%u focus=%d\n",classes[i],native_paints,GetFocus()==w);
            CHECK(!native_paints && GetFocus()==w);
        }
        /* Compare the same focus/queue state as the warm baseline; the input
         * assertions above deliberately leave the tested control focused. */
        SetFocus(root);
        {MSG message;while(PeekMessageW(&message,NULL,0,0,PM_REMOVE)){TranslateMessage(&message);DispatchMessageW(&message);}}
        GdiFlush();after=GetGuiResources(GetCurrentProcess(),GR_GDIOBJECTS);
        printf("CONTROL PRESENT class=%ls type=%lu frames=20 screenFill=%u memoryFill=%u present=%u GDI=%lu/%lu\n",
            classes[i],types[i],screen_fills,memory_fills,presents,before,after);
        printf("PAINTER OBJECTS live=%u overflow=%d\n",owned_live,owned_overflow);
        CHECK(after<=before && !owned_live && !owned_overflow && DestroyWindow(w));
    }
    {HWND first=CreateWindowW(L"BUTTON",L"Radio A",WS_CHILD|WS_VISIBLE|WS_GROUP|BS_AUTORADIOBUTTON,
        10,10,140,40,root,NULL,wc.hInstance,NULL);
     HWND second=CreateWindowW(L"BUTTON",L"Radio B",WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON,
        160,10,140,40,root,NULL,wc.hInstance,NULL);
     CHECK(first && second && win16_style_control_install(first,NULL) && win16_style_control_install(second,NULL));
     printf("RADIO GROUP begin\n");fflush(stdout);
     SendMessageW(first,BM_CLICK,0,0);SendMessageW(second,BM_CLICK,0,0);
     CHECK(SendMessageW(first,BM_GETCHECK,0,0)==BST_UNCHECKED && SendMessageW(second,BM_GETCHECK,0,0)==BST_CHECKED);
     printf("RADIO GROUP pass\n");}
    CHECK(DestroyWindow(root));return 0;
}
