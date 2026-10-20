/* Test-owned English WINMINE fixture on an observer private desktop only.
 * Exercise ordinary mouse/key/close messages, never guest memory or code.
 * Pixel changes are operation-specific and captured for independent review. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <dwmapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static DWORD owner_pid;
static HWND mine;
static BOOL exception_sibling, error_selected;
static BOOL ignore_exception;
static const char *startup_identity_path;
static BOOL CALLBACK close_owned_probe_error(HWND window, LPARAM unused)
{
    DWORD pid; WCHAR name[64], message[2048], label[64];
    HWND button;
    UINT response=ignore_exception?41003:41001;
    const WCHAR *expected=ignore_exception?L"Ignore":L"Close";
    (void)unused;
    GetWindowThreadProcessId(window,&pid);
    if(pid!=owner_pid || !GetClassNameW(window,name,64) || wcscmp(name,L"#32770"))return TRUE;
    if(!GetDlgItemTextW(window,40999,message,2048) ||
       (!wcsstr(message,L"e4470004") && !wcsstr(message,L"E4470004")))return TRUE;
    button=GetDlgItem(window,response);
    if(!button || !GetWindowTextW(button,label,64) || wcscmp(label,expected))return TRUE;
    printf("original-exception-dialog=%p worker=%lu response=%s\n",window,pid,ignore_exception?"Ignore":"Close");
    error_selected=PostMessageW(window,WM_COMMAND,MAKEWPARAM(response,BN_CLICKED),(LPARAM)button);
    return FALSE;
}
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
/* Diagnostic-only, opt-in message churn.  The ordinary acceptance path above
 * remains one flag/reset/open sequence; this mode exercises the same real
 * game window for a bounded interval while preserving its worker identity.
 * It is deliberately driven by an environment value so the public test
 * command line and guest stay unchanged. */
static BOOL stress_gameplay(HANDLE process)
{
    char value[32];
    DWORD rounds, index;
    if (!GetEnvironmentVariableA("MVDM_WOW_MINE_STRESS_ROUNDS", value, sizeof(value)))
        return TRUE;
    rounds = strtoul(value, NULL, 10);
    if (!rounds || rounds > 1000) return FALSE;
    for (index = 0; index < rounds; ++index) {
        int x = 19 + (int)((index % 8) * 16);
        int y = 61 + (int)(((index / 8) % 8) * 16);
        if (WaitForSingleObject(process, 0) != WAIT_TIMEOUT || !IsWindow(mine))
            return FALSE;
        if (index & 1u) {
            if (!PostMessageW(mine, WM_RBUTTONDOWN, MK_RBUTTON, MAKELPARAM(x, y)) ||
                !PostMessageW(mine, WM_RBUTTONUP, 0, MAKELPARAM(x, y))) return FALSE;
        } else {
            if (!PostMessageW(mine, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(x, y)) ||
                !PostMessageW(mine, WM_LBUTTONUP, 0, MAKELPARAM(x, y))) return FALSE;
        }
        if ((index % 8) == 7 &&
            (!PostMessageW(mine, WM_KEYDOWN, VK_F2, 1) ||
             !PostMessageW(mine, WM_KEYUP, VK_F2, 0xc0000001))) return FALSE;
        Sleep(20);
    }
    printf("stress-rounds=%lu complete\n", rounds);
    return WaitForSingleObject(process, 0) == WAIT_TIMEOUT && IsWindow(mine);
}
/* Launch a second ordinary Win16 task on the same unswitched desktop.
 * The script separately proves its worker/task identity from runtime traces;
 * launcher success alone is not proof of shared-worker retirement. */
static BOOL retire_sibling(const char *desktop, const char *launcher, const char *guest)
{
    STARTUPINFOA si = {0}; PROCESS_INFORMATION pi = {0};
    char command[2048], directory[MAX_PATH]; char *slash;
    DWORD status = STILL_ACTIVE, wait;
    HANDLE startup_thread = NULL;
    DWORD startup_tid = 0, startup_status = STILL_ACTIVE;
    if (sprintf_s(command, sizeof(command), "\"%s\" --wait \"%s\"", launcher, guest)<0 ||
        strcpy_s(directory, sizeof(directory), guest)) return FALSE;
    slash=strrchr(directory, '\\'); if(!slash)return FALSE; *slash=0;
    si.cb=sizeof(si); si.lpDesktop=(LPSTR)desktop;
    if(!CreateProcessA(launcher,command,NULL,NULL,FALSE,CREATE_NO_WINDOW,NULL,directory,&si,&pi)) {
        printf("sibling-launch-error=%lu\n",GetLastError());return FALSE;
    }
    printf("sibling-launcher=%lu command=%s\n",pi.dwProcessId,command);fflush(stdout);
    CloseHandle(pi.hThread);
    if (startup_identity_path) {
        ULONGLONG until = GetTickCount64()+5000;
        do {
            FILE *identity = NULL; DWORD pid = 0, task = 0;
            if (!fopen_s(&identity,startup_identity_path,"r")) {
                if (fscanf_s(identity,"%lu %lu %lu",&pid,&startup_tid,&task)==3 &&
                    pid==owner_pid && task && task<=0xffff) {
                    startup_thread=OpenThread(SYNCHRONIZE|THREAD_QUERY_LIMITED_INFORMATION,FALSE,startup_tid);
                    if (startup_thread && (GetProcessIdOfThread(startup_thread)!=owner_pid ||
                        WaitForSingleObject(startup_thread,0)!=WAIT_TIMEOUT)) {
                        CloseHandle(startup_thread);startup_thread=NULL;
                    }
                }
                fclose(identity);
            }
            if (startup_thread) break;
            Sleep(10);
        } while (GetTickCount64()<until && WaitForSingleObject(pi.hProcess,0)==WAIT_TIMEOUT);
        if (!startup_thread) {CloseHandle(pi.hProcess);return FALSE;}
        printf("startup-thread-live=%lu worker=%lu\n",startup_tid,owner_pid);
    }
    if(exception_sibling){
        ULONGLONG deadline=GetTickCount64()+10000;
        do {
            if(!error_selected)EnumDesktopWindows(GetThreadDesktop(GetCurrentThreadId()),close_owned_probe_error,0);
            wait=WaitForSingleObject(pi.hProcess,25);
        }while(wait==WAIT_TIMEOUT && GetTickCount64()<deadline);
    }else wait=WaitForSingleObject(pi.hProcess,10000);
    if(wait==WAIT_OBJECT_0)GetExitCodeProcess(pi.hProcess,&status);
    /* The owning package supervisor handles a timed-out child; never count
     * forced cleanup as ordinary task exit. */
    CloseHandle(pi.hProcess);
    if (startup_thread) {
        DWORD thread_wait=WaitForSingleObject(startup_thread,5000);
        if (thread_wait==WAIT_OBJECT_0)GetExitCodeThread(startup_thread,&startup_status);
        CloseHandle(startup_thread);
        printf("startup-thread-exit=%lu wait=%lu code=%lu\n",startup_tid,thread_wait,startup_status);
        if (thread_wait!=WAIT_OBJECT_0 || startup_status!=EXIT_FAILURE)return FALSE;
    }
    printf("sibling-wait=%lu exit=%lu survivor=%p worker=%lu\n",wait,status,mine,owner_pid);
    /* Original guest ForceTaskExit uses DOS 4CFF, but WK32KillTask explicitly
     * ends the host thread with EXIT_SUCCESS. Do not equate those contracts. */
    return wait==WAIT_OBJECT_0 && status==0 &&
        (!exception_sibling || error_selected) && IsWindow(mine);
}
int main(int argc,char **argv)
{
    HANDLE process; HDESK desk; ULONGLONG deadline, started; DWORD before, counter; DWORD_PTR reply;
    BOOL inherited_desktop;
    if((argc!=4 && argc!=5 && argc!=7 && argc!=8) ||
        (strcmp(argv[2],"-") && strncmp(argv[2],"NTVDMConsoleTest-",17)))return 2;
    inherited_desktop=!strcmp(argv[2],"-");
    if(inherited_desktop && argc>=7)return 2;
    if(argc==8){
        if(!strcmp(argv[7],"exception"))exception_sibling=TRUE;
        else if(!strcmp(argv[7],"exception-ignore"))exception_sibling=ignore_exception=TRUE;
        else startup_identity_path=argv[7];
    }
    if(argc>=5 && strcmp(argv[4],"0") && strcmp(argv[4],"1"))return 2;
    owner_pid=strtoul(argv[1],NULL,10);
    process=OpenProcess(SYNCHRONIZE|PROCESS_QUERY_LIMITED_INFORMATION,FALSE,owner_pid);
    desk=inherited_desktop ? GetThreadDesktop(GetCurrentThreadId()) :
        OpenDesktopA(argv[2],0,FALSE,DESKTOP_READOBJECTS|DESKTOP_WRITEOBJECTS|DESKTOP_ENUMERATE);
    if(!process || !desk || (!inherited_desktop && !SetThreadDesktop(desk)))return 3;
    started=GetTickCount64();deadline=started+10000;
    do {EnumDesktopWindows(desk,find_mine,0);if(mine)break;Sleep(25);}while(GetTickCount64()<deadline && WaitForSingleObject(process,0)==WAIT_TIMEOUT);
    if(!mine || !SendMessageTimeoutW(mine,WM_NULL,0,0,SMTO_ABORTIFHUNG,2000,&reply))return 4;
    printf("window-ready elapsed=%llu inherited-desktop=%u\n",GetTickCount64()-started,inherited_desktop);
    printf("owned-window=%p pid=%lu\n",mine,owner_pid);
    if(argc>=5){
        WCHAR caption[256];
        BOOL native_frame=FALSE;
        if(!GetWindowTextW(mine,caption,256))return 15;
        if(!wcsncmp(caption,L"(Win16 Style) ",14))return 16;
        if(FAILED(DwmGetWindowAttribute(mine,DWMWA_NCRENDERING_ENABLED,&native_frame,sizeof(native_frame))) ||
           (!native_frame)!=(argv[4][0]=='1'))return 16;
        printf("PASS unchanged title and native frame=%u\n",native_frame);
    }
    if(!wait_board(process,&before,&counter) || !capture(mine,argv[3],"before"))return 5;
    if(argc>=7) {
        if(!retire_sibling(argv[2],argv[5],argv[6]) ||
           WaitForSingleObject(process,0)!=WAIT_TIMEOUT ||
           !SendMessageTimeoutW(mine,WM_NULL,0,0,SMTO_ABORTIFHUNG,2000,&reply))return 17;
        if(!wait_board(process,&before,&counter))return 18;
    }
    if(!click(WM_RBUTTONDOWN,WM_RBUTTONUP,MK_RBUTTON) || !wait_cell(process,before,FALSE,"flag"))return 6;
    if(!capture(mine,argv[3],"flag"))return 7;
    printf("counter-before=%08lx counter-flag=%08lx\n",counter,region_hash(mine,16,16,40,24));
    if(counter==region_hash(mine,16,16,40,24))return 8;
    if(!PostMessageW(mine,WM_KEYDOWN,VK_F2,1) || !PostMessageW(mine,WM_KEYUP,VK_F2,0xc0000001) || !wait_cell(process,before,TRUE,"reset"))return 9;
    if(!capture(mine,argv[3],"reset"))return 10;
    if(!click(WM_LBUTTONDOWN,WM_LBUTTONUP,MK_LBUTTON) || !wait_cell(process,before,FALSE,"open"))return 11;
    if(!capture(mine,argv[3],"open"))return 12;
    printf("PASS flag changed cell+counter, F2 restored covered cell, left click changed cell\n");
    if(!stress_gameplay(process))return 19;
    if(!PostMessageW(mine,WM_CLOSE,0,0))return 13;
    deadline=GetTickCount64()+5000;
    while(IsWindow(mine) && GetTickCount64()<deadline)Sleep(20);
    if(IsWindow(mine))return 14;
    printf("PASS owned game window closed after WM_CLOSE\n");
    CloseHandle(process);if(!inherited_desktop)CloseDesktop(desk);return 0;
}
