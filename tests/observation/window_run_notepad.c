/* Test-only input on an explicitly selected private desktop/frontend. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static DWORD wanted;static BOOL found;static const char *typed=NULL;
static BOOL enter_only;
static void tap(HWND w,UINT key,BOOL system) {
    LPARAM scan=(LPARAM)MapVirtualKeyA(key,MAPVK_VK_TO_VSC)<<16;
    if(system)scan|=0x20000000;
    PostMessageA(w,system?WM_SYSKEYDOWN:WM_KEYDOWN,key,scan|1);
    Sleep(40);
    PostMessageA(w,system?WM_SYSKEYUP:WM_KEYUP,key,scan|0xc0000001);
    Sleep(40);
}
static BOOL CALLBACK visit(HWND w,LPARAM unused) {
    DWORD pid;char kind[128];const char *text=typed?typed:"notepad.exe";(void)unused;
    GetWindowThreadProcessId(w,&pid);GetClassNameA(w,kind,sizeof(kind));
    if(pid!=wanted || strcmp(kind,"LibKvmWindow") || !IsWindowVisible(w))return TRUE;
    if(enter_only){tap(w,VK_RETURN,FALSE);found=TRUE;return FALSE;}
    /* Posted messages do not change GetKeyState's physical Alt state.
     * F10 activates the ordinary guest menu without synthesizing modifiers. */
    if(!typed){tap(w,VK_F10,FALSE);tap(w,'F',FALSE);tap(w,'R',FALSE);}
    for(;*text;text++){SHORT key=VkKeyScanA(*text);if(key==-1 || HIBYTE(key))return FALSE;tap(w,LOBYTE(key),FALSE);}
    if(!typed)tap(w,VK_RETURN,FALSE);found=TRUE;return FALSE;
}
int main(int argc,char **argv) {
    if((argc!=3&&argc!=4) || !(wanted=strtoul(argv[1],NULL,10)) || strncmp(argv[2],"NTVDMConsoleTest-",17))return 64;
    if(argc==4){enter_only=!strcmp(argv[3],"--enter");if(!enter_only)typed=argv[3];}
    HDESK desktop=OpenDesktopA(argv[2],0,FALSE,DESKTOP_READOBJECTS|DESKTOP_ENUMERATE);
    if(!desktop)return 65;
    EnumDesktopWindows(desktop,visit,0);CloseDesktop(desktop);
    puts(found?"Posted owned guest keys; consumption requires fresh image evidence":"No matching visible owned Window");
    return found?0:66;
}
