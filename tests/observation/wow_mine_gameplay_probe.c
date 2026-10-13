/* Test-owned English WINMINE fixture on an observer private desktop only.
 * Exercise ordinary mouse/key/close messages, never guest memory or code.
 * Pixel changes are operation-specific and captured for independent review. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static DWORD owner_pid;
static HWND mine;
static BOOL CALLBACK find_mine(HWND w, LPARAM unused)
{
    DWORD pid; WCHAR name[128]; RECT r;
    (void)unused; GetWindowThreadProcessId(w, &pid);
    if (pid != owner_pid || !IsWindowVisible(w) ||
        !GetClassNameW(w, name, 128) || wcscmp(name, L"Minesweeper")) return TRUE;
    if (IsIconic(w)) PostMessageW(w, WM_SYSCOMMAND, SC_RESTORE, 0);
    if (GetClientRect(w, &r) && r.right > 100 && r.bottom > 100) mine = w;
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
static BOOL wait_board(HANDLE process, DWORD *cell, DWORD *counter)
{
    ULONGLONG start=GetTickCount64(); DWORD previous_cell=0, previous_counter=0;
    do {
        BOOL cell_pattern=FALSE, counter_pattern=FALSE;
        DWORD current_cell, current_counter;
        if(WaitForSingleObject(process,0)!=WAIT_TIMEOUT || !IsWindow(mine))return FALSE;
        current_cell=region_hash_detail(mine,12,54,14,14,&cell_pattern);
        current_counter=region_hash_detail(mine,16,16,40,24,&counter_pattern);
        /* A visible restored HWND and WM_NULL reply can precede WM_PAINT.
         * Require actual patterned cells/counter, matching covered neighbours,
         * and two agreeing observations before arming the first operation.
         * Never accept a stable but uniformly grey startup surface. */
        if(cell_pattern && counter_pattern && current_cell && current_counter &&
           current_cell==region_hash(mine,28,54,14,14) &&
           current_cell==region_hash(mine,44,54,14,14)) {
            if(current_cell==previous_cell && current_counter==previous_counter) {
                *cell=current_cell;*counter=current_counter;
                printf("board-ready elapsed=%llu cell=%08lx counter=%08lx\n",GetTickCount64()-start,*cell,*counter);
                return TRUE;
            }
            previous_cell=current_cell;previous_counter=current_counter;
        } else {previous_cell=0;previous_counter=0;}
        Sleep(20);
    }while(GetTickCount64()-start<5000);
    puts("board-ready TIMEOUT");return FALSE;
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
static BOOL wait_cell(HANDLE process, DWORD before, BOOL equal, const char *phase)
{
    ULONGLONG start=GetTickCount64(); DWORD current;
    do {
        if(WaitForSingleObject(process,0)!=WAIT_TIMEOUT || !IsWindow(mine))return FALSE;
        current=region_hash(mine,12,54,14,14);
        if(current && (equal ? current==before : current!=before)) {
            printf("phase=%s elapsed=%llu cell=%08lx reference=%08lx\n",phase,GetTickCount64()-start,current,before); return TRUE;
        }
        Sleep(20);
    }while(GetTickCount64()-start<5000);
    printf("phase=%s TIMEOUT cell=%08lx reference=%08lx\n",phase,current,before); return FALSE;
}
static BOOL click(UINT down, UINT up, WPARAM held)
{
    return PostMessageW(mine,down,held,MAKELPARAM(19,61)) && PostMessageW(mine,up,0,MAKELPARAM(19,61));
}
int main(int argc,char **argv)
{
    HANDLE process; HDESK desk; ULONGLONG deadline; DWORD before, counter; DWORD_PTR reply;
    if((argc!=4 && argc!=5) || strncmp(argv[2],"NTVDMConsoleTest-",17))return 2;
    if(argc==5 && strcmp(argv[4],"0") && strcmp(argv[4],"1"))return 2;
    owner_pid=strtoul(argv[1],NULL,10);
    process=OpenProcess(SYNCHRONIZE|PROCESS_QUERY_LIMITED_INFORMATION,FALSE,owner_pid);
    desk=OpenDesktopA(argv[2],0,FALSE,DESKTOP_READOBJECTS|DESKTOP_WRITEOBJECTS|DESKTOP_ENUMERATE);
    if(!process || !desk || !SetThreadDesktop(desk))return 3;
    deadline=GetTickCount64()+10000;
    do {EnumDesktopWindows(desk,find_mine,0);if(mine)break;Sleep(25);}while(GetTickCount64()<deadline && WaitForSingleObject(process,0)==WAIT_TIMEOUT);
    if(!mine || !SendMessageTimeoutW(mine,WM_NULL,0,0,SMTO_ABORTIFHUNG,2000,&reply))return 4;
    printf("owned-window=%p pid=%lu\n",mine,owner_pid);
    if(argc==5){
        WCHAR caption[256];
        BOOL marked;
        if(!GetWindowTextW(mine,caption,256))return 15;
        marked=wcsncmp(caption,L"(Win16 Style) ",14)==0;
        if(marked!=(argv[4][0]=='1'))return 16;
        printf("PASS title marker=%d\n",marked);
    }
    if(!wait_board(process,&before,&counter) || !capture(mine,argv[3],"before"))return 5;
    if(!click(WM_RBUTTONDOWN,WM_RBUTTONUP,MK_RBUTTON) || !wait_cell(process,before,FALSE,"flag"))return 6;
    if(!capture(mine,argv[3],"flag"))return 7;
    printf("counter-before=%08lx counter-flag=%08lx\n",counter,region_hash(mine,16,16,40,24));
    if(counter==region_hash(mine,16,16,40,24))return 8;
    if(!PostMessageW(mine,WM_KEYDOWN,VK_F2,1) || !PostMessageW(mine,WM_KEYUP,VK_F2,0xc0000001) || !wait_cell(process,before,TRUE,"reset"))return 9;
    if(!capture(mine,argv[3],"reset"))return 10;
    if(!click(WM_LBUTTONDOWN,WM_LBUTTONUP,MK_LBUTTON) || !wait_cell(process,before,FALSE,"open"))return 11;
    if(!capture(mine,argv[3],"open"))return 12;
    printf("PASS flag changed cell+counter, F2 restored covered cell, left click changed cell\n");
    if(!PostMessageW(mine,WM_CLOSE,0,0))return 13;
    deadline=GetTickCount64()+5000;
    while(IsWindow(mine) && GetTickCount64()<deadline)Sleep(20);
    if(IsWindow(mine))return 14;
    printf("PASS owned game window closed after WM_CLOSE\n");
    CloseHandle(process);CloseDesktop(desk);return 0;
}
