/* Acceptance of immutable English SOL after bitmap registration recovery.
 * NE 1:0462 dispatches ordinary mouse messages; WM_COMMAND 1000 is the
 * original initial/deal request at 1:03fa. Card CORE resources are 71x96.
 * No guest memory access, injected procedures, desktop switch or product hook.
 */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static DWORD owner_pid;
static HWND game;
static BOOL CALLBACK report_window(HWND window, LPARAM unused)
{
    DWORD pid; WCHAR name[128], title[256]; RECT rect={0};
    (void)unused;
    GetWindowThreadProcessId(window,&pid);
    if(pid==owner_pid && IsWindowVisible(window)) {
        GetClassNameW(window,name,128);GetWindowTextW(window,title,256);
        GetClientRect(window,&rect);
        printf("visible=%p class=%ls title=%ls enabled=%d client=%ldx%ld\n",window,name,title,IsWindowEnabled(window),rect.right,rect.bottom);
    }
    return TRUE;
}
static BOOL CALLBACK find_game(HWND window, LPARAM unused)
{
    DWORD pid; WCHAR name[128]; RECT rect;
    (void)unused;
    GetWindowThreadProcessId(window, &pid);
    if (pid == owner_pid && IsWindowVisible(window) &&
        GetClassNameW(window, name, 128) && !wcscmp(name,L"Solitaire") &&
        GetClientRect(window,&rect) && rect.right >= 500 && rect.bottom >= 250)
        game=window;
    return TRUE;
}
static DWORD region_hash_detail(HWND w, int left, int top, int width, int height, BOOL *pattern)
{
    HDC src = GetDC(w), dc;
    HBITMAP bitmap; HGDIOBJ old; DWORD *pixels = NULL;
    BITMAPINFO info = {0}; DWORD hash = 2166136261u; int i;
    if (pattern) *pattern = FALSE;
    if (!src) return 0;
    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth = width; info.bmiHeader.biHeight = -height;
    info.bmiHeader.biPlanes = 1; info.bmiHeader.biBitCount = 32;
    dc = CreateCompatibleDC(src);
    bitmap = CreateDIBSection(src, &info, DIB_RGB_COLORS, (void **)&pixels, NULL, 0);
    if (!dc || !bitmap) { if (bitmap) DeleteObject(bitmap); if (dc) DeleteDC(dc); ReleaseDC(w,src); return 0; }
    old = SelectObject(dc, bitmap);
    if (!BitBlt(dc, 0, 0, width, height, src, left, top, SRCCOPY)) hash = 0;
    else { GdiFlush(); for (i=0; i<width*height; ++i) {
        hash = (hash ^ (pixels[i]&0xffffffu))*16777619u;
        if (pattern && (pixels[i]&0xffffffu)!=(pixels[0]&0xffffffu)) *pattern=TRUE;
    } }
    SelectObject(dc, old); DeleteObject(bitmap); DeleteDC(dc); ReleaseDC(w, src);
    return hash;
}
static DWORD region_hash(HWND w, int left, int top, int width, int height)
{
    return region_hash_detail(w,left,top,width,height,NULL);
}
static BOOL wait_deal_ready(HANDLE process)
{
    ULONGLONG deadline=GetTickCount64()+10000;
    do {
        HMENU menu=GetMenu(game); DWORD_PTR reply; UINT state;
        if(!menu || WaitForSingleObject(process,0)!=WAIT_TIMEOUT ||
           !SendMessageTimeoutW(game,WM_INITMENU,(WPARAM)menu,0,SMTO_ABORTIFHUNG,2000,&reply))return FALSE;
        /* Original SOL 1:075e--07ae updates command 1000 from its busy state.
         * Do not issue a command merely because a partly drawn board exists. */
        state=GetMenuState(menu,1000,MF_BYCOMMAND);
        if(state!=(UINT)-1 && !(state&(MF_DISABLED|MF_GRAYED))) {
            printf("deal-menu-state=%x enabled=%d\n",state,IsWindowEnabled(game));
            return IsWindowEnabled(game);
        }
        Sleep(30);
    }while(GetTickCount64()<deadline);
    return FALSE;
}
static BOOL capture(HWND w, const char *prefix, const char *phase)
{
    RECT r; HDC src, dc; HBITMAP bitmap; HGDIOBJ old;
    BITMAPINFO info={0}; BITMAPFILEHEADER header={0}; void *pixels=NULL;
    DWORD bytes; char path[MAX_PATH]; FILE *out; BOOL ok=FALSE;
    if (!GetClientRect(w,&r) || r.right<=0 || r.bottom<=0 || r.right>2048 || r.bottom>2048) return FALSE;
    bytes=(DWORD)(r.right*r.bottom*4); info.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth=r.right; info.bmiHeader.biHeight=-r.bottom;
    info.bmiHeader.biPlanes=1; info.bmiHeader.biBitCount=32;
    src=GetDC(w); if(!src)return FALSE; dc=CreateCompatibleDC(src);
    bitmap=CreateDIBSection(src,&info,DIB_RGB_COLORS,&pixels,NULL,0);
    if(!dc || !bitmap)goto done;
    old=SelectObject(dc,bitmap); ok=BitBlt(dc,0,0,r.right,r.bottom,src,0,0,SRCCOPY);
    GdiFlush(); SelectObject(dc,old); if(!ok)goto done;
    if(sprintf_s(path,sizeof(path),"%s-%s.bmp",prefix,phase)<0 || fopen_s(&out,path,"wb")){ok=FALSE;goto done;}
    header.bfType=0x4d42;header.bfOffBits=sizeof(header)+sizeof(info.bmiHeader);header.bfSize=header.bfOffBits+bytes;
    ok=fwrite(&header,sizeof(header),1,out)==1 && fwrite(&info.bmiHeader,sizeof(info.bmiHeader),1,out)==1 && fwrite(pixels,bytes,1,out)==1;
    fclose(out);
done:
    if(bitmap)DeleteObject(bitmap);if(dc)DeleteDC(dc);ReleaseDC(w,src);
    printf("capture=%s ok=%d\n",phase,ok); return ok;
}

static BOOL wait_pixels(HANDLE process, int x, int y, int w, int h,
    DWORD before, BOOL changed, const char *phase, DWORD *out)
{
    ULONGLONG deadline=GetTickCount64()+10000;
    DWORD previous=0, last=0; BOOL last_pattern=FALSE;
    do {
        BOOL pattern=FALSE;
        DWORD current;
        if(WaitForSingleObject(process,0)!=WAIT_TIMEOUT || !IsWindow(game))return FALSE;
        current=region_hash_detail(game,x,y,w,h,&pattern);
        last=current;last_pattern=pattern;
        if(current && pattern && (!changed || current!=before)) {
            if(current==previous) {
                *out=current;printf("phase=%s pixels=%08lx reference=%08lx\n",phase,current,before);
                return TRUE;
            }
            previous=current;
        } else previous=0;
        Sleep(30);
    }while(GetTickCount64()<deadline);
    printf("phase=%s TIMEOUT pixels=%08lx pattern=%d previous=%08lx\n",phase,last,last_pattern,previous);
    EnumDesktopWindows(GetThreadDesktop(GetCurrentThreadId()),report_window,0);
    return FALSE;
}
int main(int argc,char **argv)
{
    HANDLE process; HDESK desktop; RECT rect; DWORD_PTR reply;
    ULONGLONG deadline; DWORD initial, dealt, waste, drawn; int gap;
    if(argc!=4 || strncmp(argv[2],"NTVDMConsoleTest-",17))return 2;
    owner_pid=strtoul(argv[1],NULL,10);
    process=OpenProcess(SYNCHRONIZE|PROCESS_QUERY_LIMITED_INFORMATION,FALSE,owner_pid);
    desktop=OpenDesktopA(argv[2],0,FALSE,DESKTOP_READOBJECTS|DESKTOP_WRITEOBJECTS|DESKTOP_ENUMERATE);
    if(!process || !desktop || !SetThreadDesktop(desktop))return 3;
    deadline=GetTickCount64()+10000;
    do {EnumDesktopWindows(desktop,find_game,0);if(game)break;Sleep(25);}
    while(GetTickCount64()<deadline && WaitForSingleObject(process,0)==WAIT_TIMEOUT);
    if(!game || !GetClientRect(game,&rect) ||
       !SendMessageTimeoutW(game,WM_NULL,0,0,SMTO_ABORTIFHUNG,2000,&reply)) {
        printf("initial-window-failure game=%p process-wait=%lu error=%lu\n",game,WaitForSingleObject(process,0),GetLastError());
        EnumDesktopWindows(desktop,report_window,0);return 4;
    }
    printf("owned-window=%p pid=%lu client=%ldx%ld\n",game,owner_pid,rect.right,rect.bottom);
    gap=(rect.right-7*71)/8;if(gap<11)gap=11;
    /* Sample the tableau, excluding the status child and non-client menu. */
    if(!wait_deal_ready(process) ||
       !wait_pixels(process,0,120,rect.right,rect.bottom-144,0,FALSE,"ready",&initial) ||
       !capture(game,argv[3],"before")) {
        (void)capture(game,argv[3],"ready-failure");return 5;
    }
    /* SOL 1:09c7 calls entry 11 (8:00fc), time(NULL): DOS AH=2c
     * uses seconds DH, not hundredths DL. Entry 10 reseeds rand(). A deal
     * in the same second legitimately repeats the initial board. */
    Sleep(1100);
    if(!PostMessageW(game,WM_COMMAND,1000,0) ||
       !wait_pixels(process,0,120,rect.right,rect.bottom-144,initial,TRUE,"deal",&dealt) ||
       !capture(game,argv[3],"deal"))return 6;
    waste=region_hash(game,2*gap+71,20,71,70);
    if(!waste || !PostMessageW(game,WM_LBUTTONDOWN,MK_LBUTTON,MAKELPARAM(gap+30,40)) ||
       !PostMessageW(game,WM_LBUTTONUP,0,MAKELPARAM(gap+30,40)) ||
       !wait_pixels(process,2*gap+71,20,71,70,waste,TRUE,"draw-stock",&drawn) ||
       !capture(game,argv[3],"draw"))return 7;
    if(!PostMessageW(game,WM_CLOSE,0,0))return 8;
    deadline=GetTickCount64()+10000;
    while(IsWindow(game) && GetTickCount64()<deadline)Sleep(25);
    if(IsWindow(game))return 9;
    puts("PASS SOL deal changed tableau, stock click changed waste, WM_CLOSE destroyed owned window");
    CloseHandle(process);CloseDesktop(desktop);return 0;
}
