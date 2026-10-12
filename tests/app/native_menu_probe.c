/* Native menu control: deliberately no win16-style.
 * Subclass/theme APIs are explicit diagnostic controls, never default behavior.
 * --probe <bitmap> opens its own system menu and exits after 500 ms.
 * Without arguments this is a manual system/menu-bar comparison window. */
#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <wchar.h>
#include <stdlib.h>

static const WCHAR *output;
static HWND popup;
static BOOL captured;
static int entry;
static UINT observation_ms=500;
static struct {ULONGLONG time;HWND window;UINT message;WPARAM wp;LPARAM lp;BOOL visible;} paint_trace[256];
static unsigned trace_count;
static BOOL print_control;
static BOOL isolated,isolated_client,isolated_hidden,isolated_classic,capture_started;
static BOOL return_control;
static WNDPROC original_popup;
static void capture_popup(HWND w);
static LRESULT CALLBACK popup_return(HWND w,UINT m,WPARAM a,LPARAM b)
{
    LRESULT result;DWORD error;
    if(m==WM_PRINT)SetLastError(0);
    result=CallWindowProcW(original_popup,w,m,a,b);error=GetLastError();
    if(m==WM_PRINT)printf("NATIVE PRINT RETURN result=%lld error=%lu dc=%p flags=%llx\n",
        (long long)result,error,(void *)a,(unsigned long long)b);
    SetLastError(error);return result;
}
static LRESULT CALLBACK trace_paint(int code,WPARAM a,LPARAM b)
{
    DWORD incoming_error=GetLastError();
    if(code>=0 && trace_count<256){
        CWPSTRUCT *m=(CWPSTRUCT *)b;WCHAR name[32];
        if(m->message!=WM_NCHITTEST && GetClassNameW(m->hwnd,name,32) && !wcscmp(name,L"#32768")){
            if(return_control && !original_popup && m->message==WM_WINDOWPOSCHANGED && !IsWindowVisible(m->hwnd)){
                original_popup=(WNDPROC)SetWindowLongPtrW(m->hwnd,GWLP_WNDPROC,(LONG_PTR)popup_return);
                printf("CONTROL native popup return observer installed=%d\n",original_popup!=NULL);
            }
            if(m->message==WM_PRINT){
                HDC dc=(HDC)m->wParam;RECT clip={0};POINT origin={0},viewport={0};BITMAP bitmap={0};
                DWORD dc_type,dc_error,clip_error;int clip_type;HGDIOBJ object;
                SetLastError(0);dc_type=GetObjectType(dc);dc_error=GetLastError();
                SetLastError(0);clip_type=GetClipBox(dc,&clip);clip_error=GetLastError();
                object=GetCurrentObject(dc,OBJ_BITMAP);
                GetDCOrgEx(dc,&origin);GetViewportOrgEx(dc,&viewport);GetObjectW(object,sizeof(bitmap),&bitmap);
                printf("PRINT DC handle=%p type=%lu error=%lu clip-type=%d clip-error=%lu clip=%ld,%ld,%ld,%ld origin=%ld,%ld viewport=%ld,%ld bitmap=%ld,%ld,%u map=%d\n",
                    dc,dc_type,dc_error,clip_type,clip_error,clip.left,clip.top,clip.right,clip.bottom,origin.x,origin.y,
                    viewport.x,viewport.y,bitmap.bmWidth,bitmap.bmHeight,bitmap.bmBitsPixel,GetMapMode(dc));
            }
            paint_trace[trace_count].time=GetTickCount64();
            paint_trace[trace_count].window=m->hwnd;
            paint_trace[trace_count].message=m->message;
            paint_trace[trace_count].lp=m->lParam;
            paint_trace[trace_count].visible=IsWindowVisible(m->hwnd);
            paint_trace[trace_count++].wp=m->wParam;
            if(isolated && !capture_started && m->message==WM_WINDOWPOSCHANGED &&
               !!IsWindowVisible(m->hwnd)==!isolated_hidden){
                capture_started=TRUE;capture_popup(m->hwnd);
            }
        }
    }
    SetLastError(incoming_error);return CallNextHookEx(NULL,code,a,b);
}

static void capture_popup(HWND w)
{
    RECT r;DWORD pid,bytes;HDC src,dc;HBITMAP bm,old;void *pixels;HANDLE file;
    BITMAPINFO bi={0};BITMAPFILEHEADER fh={0};int x,y;BOOL visible=TRUE;
    if(!GetWindowRect(w,&r) || !GetWindowThreadProcessId(w,&pid) || pid!=GetCurrentProcessId())return;
    /* Do not call occluded desktop pixels menu evidence. Check a grid before
     * copying; no PrintWindow or forced repaint that would hide the defect. */
    for(y=4;!isolated && y<r.bottom-r.top;y+=12)for(x=4;x<r.right-r.left;x+=12){
        POINT p={r.left+x,r.top+y};if(WindowFromPoint(p)!=w)visible=FALSE;
    }
    printf("POPUP visible=%d window-visible=%d size=%ld,%ld delay=%u style=%llx exstyle=%llx\n",visible,IsWindowVisible(w),r.right-r.left,r.bottom-r.top,
        observation_ms,(unsigned long long)GetWindowLongPtrW(w,GWL_STYLE),
        (unsigned long long)GetWindowLongPtrW(w,GWL_EXSTYLE));
    if(!visible)return;
    src=isolated?NULL:GetWindowDC(w);dc=CreateCompatibleDC(src);
    bi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);bi.bmiHeader.biWidth=r.right-r.left;
    bi.bmiHeader.biHeight=r.top-r.bottom;bi.bmiHeader.biPlanes=1;bi.bmiHeader.biBitCount=32;
    bm=CreateDIBSection(dc,&bi,DIB_RGB_COLORS,&pixels,NULL,0);
    if((!isolated && !src) || !dc || !bm){if(bm)DeleteObject(bm);if(dc)DeleteDC(dc);if(src)ReleaseDC(w,src);return;}
    old=SelectObject(dc,bm);
    if(isolated){
        PatBlt(dc,0,0,r.right-r.left,r.bottom-r.top,WHITENESS);
        SendMessageW(w,isolated_client?WM_PRINTCLIENT:WM_PRINT,(WPARAM)dc,PRF_CLIENT|PRF_NONCLIENT|PRF_ERASEBKGND);
    }
    if(isolated || BitBlt(dc,0,0,r.right-r.left,r.bottom-r.top,src,0,0,SRCCOPY)){
        GdiFlush();fh.bfType=0x4d42;fh.bfOffBits=sizeof(fh)+sizeof(bi.bmiHeader);
        fh.bfSize=fh.bfOffBits+(r.right-r.left)*(r.bottom-r.top)*4;
        file=CreateFileW(output,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
        if(file!=INVALID_HANDLE_VALUE){
            captured=WriteFile(file,&fh,sizeof(fh),&bytes,NULL) &&
                WriteFile(file,&bi.bmiHeader,sizeof(bi.bmiHeader),&bytes,NULL) &&
                WriteFile(file,pixels,fh.bfSize-fh.bfOffBits,&bytes,NULL);
            CloseHandle(file);
        }
    }
    /* Separate off-screen diagnostic, after saving untouched visible pixels.
     * This asks the same menu to print while visible; it does not invalidate
     * or repaint its on-screen window. */
    if(print_control){
        unsigned dark=0;int width=r.right-r.left,height=r.bottom-r.top;DWORD *p=pixels;
        PatBlt(dc,0,0,width,height,WHITENESS);
        SendMessageW(w,WM_PRINT,(WPARAM)dc,PRF_CLIENT|PRF_NONCLIENT|PRF_ERASEBKGND);
        GdiFlush();
        for(y=40;y<height-4;++y)for(x=45;x<min(width-4,180);++x){
            DWORD c=p[y*width+x];
            if((c&255)<100 && ((c>>8)&255)<100 && ((c>>16)&255)<100)++dark;
        }
        printf("VISIBLE PRINT inner-dark-pixels=%u\n",dark);
    }
    SelectObject(dc,old);DeleteObject(bm);DeleteDC(dc);if(src)ReleaseDC(w,src);
}

static LRESULT CALLBACK proc(HWND w,UINT m,WPARAM a,LPARAM b)
{
    if(m==WM_APP){
        if(entry==5)SendMessageW(w,WM_SYSCOMMAND,SC_KEYMENU,L'f');
        else if(entry){RECT r;GetWindowRect(w,&r);TrackPopupMenuEx(
            entry>=3?GetSubMenu(GetMenu(w),0):GetSystemMenu(w,FALSE),
            entry==2 || entry==4?TPM_NOANIMATION:0,r.left+20,r.top+40,w,NULL);}
        else SendMessageW(w,WM_SYSCOMMAND,SC_KEYMENU,L' ');
        DestroyWindow(w);return 0;
    }
    if(m==WM_ENTERIDLE && a==MSGF_MENU && output && !popup){
        popup=(HWND)b;SetTimer(w,1,observation_ms,NULL);return 0;
    }
    if(m==WM_TIMER && a==1){KillTimer(w,1);if(!isolated)capture_popup(popup);EndMenu();return 0;}
    if(m==WM_DESTROY){PostQuitMessage(0);return 0;}
    return DefWindowProcW(w,m,a,b);
}

int wmain(int argc,WCHAR **argv)
{
    WNDCLASSW c={0};HWND w;MSG m;HMENU bar,menu;HANDLE snap;MODULEENTRY32W mod={sizeof(mod)};
    HHOOK trace=NULL;unsigned i;HDESK desktop=NULL;
    if(argc>=3 && argc<=6 && !wcscmp(argv[1],L"--probe"))output=argv[2];
    else if(argc!=1)return 2;
    if(argc>=4){
        if(!wcscmp(argv[3],L"--popup"))entry=1;
        else if(!wcscmp(argv[3],L"--popup-no-animation"))entry=2;
        else if(!wcscmp(argv[3],L"--file"))entry=3;
        else if(!wcscmp(argv[3],L"--file-no-animation"))entry=4;
        else if(!wcscmp(argv[3],L"--file-keyboard"))entry=5;
        else if(wcscmp(argv[3],L"--system-dpi"))return 2;
    }
    if(argc>=5){WCHAR *end;unsigned long value=wcstoul(argv[4],&end,10);
        if(*end || value<1 || value>10000)return 2;observation_ms=(UINT)value;}
    isolated_client=argc==6 && (!wcscmp(argv[5],L"--isolated-client") || !wcscmp(argv[5],L"--isolated-hidden-client"));
    isolated_hidden=argc==6 && (!wcscmp(argv[5],L"--isolated-hidden-print") || !wcscmp(argv[5],L"--isolated-hidden-client"));
    isolated_classic=argc==6 && !wcscmp(argv[5],L"--isolated-classic");
    return_control=argc==6 && !wcscmp(argv[5],L"--isolated-return");
    isolated=isolated_client || isolated_hidden || isolated_classic || return_control || (argc==6 && !wcscmp(argv[5],L"--isolated-print"));
    if(isolated){WCHAR name[80];swprintf_s(name,80,L"NativeMenuProbe-%lu",GetCurrentProcessId());
        desktop=CreateDesktopW(name,NULL,NULL,0,GENERIC_ALL,NULL);
        if(!desktop || !SetThreadDesktop(desktop))return 7;
        printf("ISOLATED off-screen message=%04x (not physical display evidence)\n",isolated_client?WM_PRINTCLIENT:WM_PRINT);
    }
    SetProcessDpiAwarenessContext(argc>=4 && !wcscmp(argv[3],L"--system-dpi")?
        DPI_AWARENESS_CONTEXT_SYSTEM_AWARE:DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    if(isolated_classic){
        HMODULE theme=LoadLibraryW(L"uxtheme.dll");
        typedef void (WINAPI *SET_THEME)(DWORD);SET_THEME set_theme;
        if(!theme)return 8;
        set_theme=(SET_THEME)(void *)GetProcAddress(theme,"SetThemeAppProperties");
        if(!set_theme)return 8;set_theme(0);
        printf("CONTROL process-local classic theme; no global setting changed\n");
    }
    c.lpfnWndProc=proc;c.hInstance=GetModuleHandleW(NULL);c.lpszClassName=L"NativeMenuControl";
    c.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);c.hCursor=LoadCursorW(NULL,IDC_ARROW);c.style=CS_DBLCLKS;
    if(!RegisterClassW(&c))return 3;
    bar=CreateMenu();menu=CreatePopupMenu();
    AppendMenuW(menu,MF_STRING,1,L"First native item");AppendMenuW(menu,MF_STRING,2,L"Second native item");
    AppendMenuW(menu,MF_STRING,3,L"Third native item");AppendMenuW(bar,MF_POPUP,(UINT_PTR)menu,L"&File");
    w=CreateWindowExW(output?WS_EX_TOPMOST:0,c.lpszClassName,L"Native menu - no style library",WS_OVERLAPPEDWINDOW,
        60,60,480,320,NULL,bar,c.hInstance,NULL);
    if(!w)return 4;
    ShowWindow(w,SW_SHOW);SetForegroundWindow(w);SetActiveWindow(w);UpdateWindow(w);
    printf("PID=%lu DPI=%u\n",GetCurrentProcessId(),GetDpiForWindow(w));
    {BOOL animation=FALSE,fade=FALSE;UINT delay=0;
        SystemParametersInfoW(SPI_GETMENUANIMATION,0,&animation,0);
        SystemParametersInfoW(SPI_GETMENUFADE,0,&fade,0);
        SystemParametersInfoW(SPI_GETMENUSHOWDELAY,0,&delay,0);
        printf("SETTINGS animation=%d fade=%d show-delay=%u observation-ms=%u\n",animation,fade,delay,observation_ms);}
    if(argc==6){
        print_control=!wcscmp(argv[5],L"--trace-print");
        if(!isolated && !print_control && wcscmp(argv[5],L"--trace"))return 2;
        trace=SetWindowsHookExW(WH_CALLWNDPROC,trace_paint,NULL,GetCurrentThreadId());
        if(!trace)return 6;
    }
    if(output)PostMessageW(w,WM_APP,0,0);
    while(GetMessageW(&m,NULL,0,0)>0){TranslateMessage(&m);DispatchMessageW(&m);}
    if(trace)UnhookWindowsHookEx(trace);
    for(i=0;i<trace_count;++i)printf("PAINT TRACE t=%llu hwnd=%p message=%04x wp=%llx lp=%llx visible=%d\n",
        paint_trace[i].time,paint_trace[i].window,paint_trace[i].message,(unsigned long long)paint_trace[i].wp,
        (unsigned long long)paint_trace[i].lp,paint_trace[i].visible);
    snap=CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,0);
    if(snap!=INVALID_HANDLE_VALUE){
        if(Module32FirstW(snap,&mod))do{printf("MODULE %ls\n",mod.szExePath);}while(Module32NextW(snap,&mod));
        CloseHandle(snap);
    }
    /* Unavailable visibility is not a pass, nor evidence of blank drawing. */
    return output&&!captured?5:0;
}
