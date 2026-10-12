/* Native menu control: deliberately no win16-style, subclass or theme API.
 * --probe <bitmap> opens its own system menu and exits after 500 ms.
 * Without arguments this is a manual system/menu-bar comparison window. */
#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <wchar.h>

static const WCHAR *output;
static HWND popup;
static BOOL captured;
static int entry;

static void capture_popup(HWND w)
{
    RECT r;DWORD pid,bytes;HDC src,dc;HBITMAP bm,old;void *pixels;HANDLE file;
    BITMAPINFO bi={0};BITMAPFILEHEADER fh={0};int x,y;BOOL visible=TRUE;
    if(!GetWindowRect(w,&r) || !GetWindowThreadProcessId(w,&pid) || pid!=GetCurrentProcessId())return;
    /* Do not call occluded desktop pixels menu evidence. Check a grid before
     * copying; no PrintWindow or forced repaint that would hide the defect. */
    for(y=4;y<r.bottom-r.top;y+=12)for(x=4;x<r.right-r.left;x+=12){
        POINT p={r.left+x,r.top+y};if(WindowFromPoint(p)!=w)visible=FALSE;
    }
    printf("POPUP visible=%d size=%ld,%ld\n",visible,r.right-r.left,r.bottom-r.top);
    if(!visible)return;
    src=GetWindowDC(w);dc=CreateCompatibleDC(src);
    bi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);bi.bmiHeader.biWidth=r.right-r.left;
    bi.bmiHeader.biHeight=r.top-r.bottom;bi.bmiHeader.biPlanes=1;bi.bmiHeader.biBitCount=32;
    bm=CreateDIBSection(dc,&bi,DIB_RGB_COLORS,&pixels,NULL,0);
    if(!src || !dc || !bm){if(bm)DeleteObject(bm);if(dc)DeleteDC(dc);if(src)ReleaseDC(w,src);return;}
    old=SelectObject(dc,bm);
    if(BitBlt(dc,0,0,r.right-r.left,r.bottom-r.top,src,0,0,SRCCOPY)){
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
    SelectObject(dc,old);DeleteObject(bm);DeleteDC(dc);ReleaseDC(w,src);
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
        popup=(HWND)b;SetTimer(w,1,500,NULL);return 0;
    }
    if(m==WM_TIMER && a==1){KillTimer(w,1);capture_popup(popup);EndMenu();return 0;}
    if(m==WM_DESTROY){PostQuitMessage(0);return 0;}
    return DefWindowProcW(w,m,a,b);
}

int wmain(int argc,WCHAR **argv)
{
    WNDCLASSW c={0};HWND w;MSG m;HMENU bar,menu;HANDLE snap;MODULEENTRY32W mod={sizeof(mod)};
    if((argc==3 || argc==4) && !wcscmp(argv[1],L"--probe"))output=argv[2];
    else if(argc!=1)return 2;
    if(argc==4){
        if(!wcscmp(argv[3],L"--popup"))entry=1;
        else if(!wcscmp(argv[3],L"--popup-no-animation"))entry=2;
        else if(!wcscmp(argv[3],L"--file"))entry=3;
        else if(!wcscmp(argv[3],L"--file-no-animation"))entry=4;
        else if(!wcscmp(argv[3],L"--file-keyboard"))entry=5;
        else if(wcscmp(argv[3],L"--system-dpi"))return 2;
    }
    SetProcessDpiAwarenessContext(argc==4 && !wcscmp(argv[3],L"--system-dpi")?
        DPI_AWARENESS_CONTEXT_SYSTEM_AWARE:DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
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
    if(output)PostMessageW(w,WM_APP,0,0);
    while(GetMessageW(&m,NULL,0,0)>0){TranslateMessage(&m);DispatchMessageW(&m);}
    snap=CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,0);
    if(snap!=INVALID_HANDLE_VALUE){
        if(Module32FirstW(snap,&mod))do{printf("MODULE %ls\n",mod.szExePath);}while(Module32NextW(snap,&mod));
        CloseHandle(snap);
    }
    /* Unavailable visibility is not a pass, nor evidence of blank drawing. */
    return output&&!captured?5:0;
}
