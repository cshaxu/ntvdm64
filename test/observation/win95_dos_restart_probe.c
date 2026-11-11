/* Independent guest-side diagnostic. Never linked into the product.
 * Open the real Explorer shutdown dialog; log controls before selecting DOS.
 * The host runner owns the isolated installation and bounded lifetime. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
static HANDLE log_file;
static HWND dos_button, dialog;
static char line[768];
static unsigned ticks;
static int opened, execute_request, submitted, normal_shutdown, inspect_pif, execute_short;
static void record(const char *key,const char *value,DWORD id) {
    DWORD n,done,error=GetLastError();
    wsprintfA(line,"%s=%s id=%08lX\r\n",key,value,id);
    n=(DWORD)lstrlenA(line);WriteFile(log_file,line,n,&done,0);
    FlushFileBuffers(log_file);SetLastError(error);
}
/* Original shell232.src ordinals 9/10/13; pif.h byte-packed PROPPRG is
 * 658 bytes. This diagnostic opens an isolated-copy property object only. */
static void create_control(const char *name) {
    char path[MAX_PATH];HANDLE file;
    GetWindowsDirectoryA(path,sizeof(path));lstrcatA(path,"\\");lstrcatA(path,name);
    SetLastError(0);file=CreateFileA(path,GENERIC_WRITE,0,0,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,0);
    record("CREATE_CONTROL",name,(DWORD)file);record("CREATE_ERROR","last error",GetLastError());
    if(file!=INVALID_HANDLE_VALUE){CloseHandle(file);DeleteFileA(path);}
}
static void inspect_volume(void) {
    char root[MAX_PATH],label[MAX_PATH],fs[64],path[MAX_PATH],short_path[MAX_PATH];
    DWORD serial=0,max_component=0,flags=0,result;
    GetWindowsDirectoryA(root,sizeof(root));root[3]=0;
    SetLastError(0);result=GetVolumeInformationA(root,label,sizeof(label),&serial,&max_component,&flags,fs,sizeof(fs));
    record("VOLUME",result?fs:"failed",result);record("VOLUME_ERROR","last error",GetLastError());
    record("VOLUME_MAX_COMPONENT","length",max_component);record("VOLUME_FLAGS","flags",flags);
    GetWindowsDirectoryA(path,sizeof(path));lstrcatA(path,"\\ALIAS.TXT");
    {HANDLE file=CreateFileA(path,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,0,0);DWORD read;
     if(file!=INVALID_HANDLE_VALUE){
         if(ReadFile(file,path,sizeof(path)-1,&read,0)){path[read]=0;
             CloseHandle(file);SetLastError(0);
             file=CreateFileA(path,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,0,0);
             record("HOST_SHORT_ALIAS",path,(DWORD)file);record("HOST_ALIAS_ERROR","last error",GetLastError());
         }
         if(file!=INVALID_HANDLE_VALUE)CloseHandle(file);
     }}
    GetWindowsDirectoryA(path,sizeof(path));lstrcatA(path,"\\Long file name.tmp");
    SetLastError(0);result=GetShortPathNameA(path,short_path,sizeof(short_path));
    record("GUEST_SHORT_PATH",result?short_path:"failed",result);record("GUEST_SHORT_ERROR","last error",GetLastError());
}
static void inspect_properties(void) {
    typedef int (WINAPI *OPEN)(LPCSTR,LPCSTR,UINT,UINT);
    typedef int (WINAPI *GET)(int,LPCSTR,LPVOID,int,UINT);
    typedef int (WINAPI *CLOSE)(int,UINT);
    HMODULE module=LoadLibraryA("SHELL32.DLL");
    OPEN open;GET get;CLOSE close;char command[MAX_PATH],path[MAX_PATH],name[128];
    static BYTE properties[658];int handle,result;HANDLE file;
    if(!module){record("PIF","shell load failed",GetLastError());return;}
    open=(OPEN)GetProcAddress(module,(LPCSTR)9);
    get=(GET)GetProcAddress(module,(LPCSTR)10);
    close=(CLOSE)GetProcAddress(module,(LPCSTR)13);
    if(!open || !get || !close){record("PIF","missing original ordinal",0);return;}
    GetWindowsDirectoryA(path,sizeof(path));
    name[0]=0;result=LoadStringA(module,0x10b3,name,sizeof(name));
    record("PIF_RESOURCE",name,result);
    if(!result)return;
    create_control("DOSRET.TMP");create_control("Long file name.tmp");
    create_control(" LEAD.TMP");if(name[0]==' ')create_control(name+1);
    lstrcatA(path,"\\");lstrcatA(path,name);
    record("PIF_PATH",path,GetFileAttributesA(path));
    SetLastError(0);file=CreateFileA(path,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,0,0);
    record("PIF_EXISTING","handle",(DWORD)file);record("PIF_EXISTING_ERROR","last error",GetLastError());
    if(file!=INVALID_HANDLE_VALUE)CloseHandle(file);
    result=SearchPathA(0,"command.com",0,sizeof(command),command,0);
    record("COMMAND_PATH",result?command:"unresolved",result);
    if(!result)return;
    SetLastError(0);handle=open(command,path,0,6);
    record("PIF_OPEN","handle",handle);record("PIF_OPEN_ERROR","last error",GetLastError());
    if(!handle){
        SetLastError(0);file=CreateFileA(path,GENERIC_WRITE,0,0,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,0);
        record("PIF_RAW_CREATE","handle",(DWORD)file);record("PIF_RAW_ERROR","last error",GetLastError());
        if(file!=INVALID_HANDLE_VALUE){CloseHandle(file);DeleteFileA(path);}
        GetWindowsDirectoryA(path,sizeof(path));lstrcatA(path,"\\DOSRET.PIF");
        SetLastError(0);handle=open(command,path,0,6);
        record("PIF_ASCII_OPEN","handle",handle);record("PIF_ASCII_ERROR","last error",GetLastError());
        if(!handle){
            SetLastError(0);handle=open(command,0,0,6);
            record("PIF_NO_TARGET","handle",handle);record("PIF_NO_TARGET_ERROR","last error",GetLastError());
        }
        if(!handle)return;
    }
    SetLastError(0);result=get(handle,(LPCSTR)1,properties,sizeof(properties),0);
    record("PIF_GET","result",result);record("PIF_GET_ERROR","last error",GetLastError());
    close(handle,1);
}
/* Isolated experiment past the known long-name preparation failure.
 * This does not repair or replace Explorer's real menu path. */
static void execute_short_pif(void) {
    typedef int (WINAPI *OPEN)(LPCSTR,LPCSTR,UINT,UINT);
    typedef int (WINAPI *PROPERTIES)(int,LPCSTR,LPVOID,int,UINT);
    typedef int (WINAPI *CLOSE)(int,UINT);
    HMODULE module=LoadLibraryA("SHELL32.DLL");
    OPEN open;PROPERTIES get,set;CLOSE close;int handle,result;
    char command[MAX_PATH],path[MAX_PATH];static BYTE properties[658];
    static SHELLEXECUTEINFOA info;
    if(!module)return;
    open=(OPEN)GetProcAddress(module,(LPCSTR)9);
    get=(PROPERTIES)GetProcAddress(module,(LPCSTR)10);
    set=(PROPERTIES)GetProcAddress(module,(LPCSTR)11);
    close=(CLOSE)GetProcAddress(module,(LPCSTR)13);
    if(!open || !get || !set || !close)return;
    if(!SearchPathA(0,"command.com",0,sizeof(command),command,0))return;
    GetWindowsDirectoryA(path,sizeof(path));lstrcatA(path,"\\DOSRET.PIF");
    handle=open(command,path,0,6);record("SHORT_OPEN",command,handle);
    if(!handle)return;
    result=get(handle,(LPCSTR)1,properties,sizeof(properties),0);
    record("SHORT_GET","bytes",result);
    if(result!=sizeof(properties)){close(handle,1);return;}
    lstrcpynA((char *)properties+34,command,128);
    *(WORD *)(properties+2)|=0x0108;
    result=set(handle,(LPCSTR)1,properties,sizeof(properties),0);
    record("SHORT_SET","result",result);
    if(!result){close(handle,1);return;}
    result=close(handle,0);record("SHORT_CLOSE","result",result);
    info.cbSize=sizeof(info);info.lpFile=path;info.nShow=SW_SHOWNORMAL;
    record("SHORT_EXEC",path,*(WORD *)(properties+2));
    SetLastError(0);result=ShellExecuteExA(&info);
    record("SHORT_EXEC_RESULT","result",result);
    if(!result)record("SHORT_EXEC_ERROR","last error",GetLastError());
}
static BOOL CALLBACK child(HWND w,LPARAM parent) {
    char text[256],cls[64];
    GetWindowTextA(w,text,sizeof(text));GetClassNameA(w,cls,sizeof(cls));
    record(cls,text,(DWORD)GetDlgCtrlID(w));
    if(!lstrcmpiA(cls,"Button") && (GetWindowLongA(w,GWL_STYLE)&15)==BS_AUTORADIOBUTTON) {
        const char *s=text;
        if(normal_shutdown) {
            if(GetDlgCtrlID(w)==0x3800){dos_button=w;dialog=(HWND)parent;}
            return TRUE;
        }
        while(*s){if(lstrlenA(s)>=6 && s[0]=='M' && s[1]=='S' && s[2]=='-' && s[3]=='D' && s[4]=='O' && s[5]=='S'){dos_button=w;dialog=(HWND)parent;break;}++s;}
    }
    return TRUE;
}
static BOOL CALLBACK window(HWND w,LPARAM unused) {
    char text[256],cls[64];(void)unused;
    if(!IsWindowVisible(w))return TRUE;
    GetWindowTextA(w,text,sizeof(text));GetClassNameA(w,cls,sizeof(cls));
    if(!lstrcmpA(cls,"#32770")){record("DIALOG",text,(DWORD)w);EnumChildWindows(w,child,(LPARAM)w);}
    return TRUE;
}
static void CALLBACK timer(HWND w,UINT msg,UINT_PTR timer_id,DWORD time) {
    HWND shell;(void)w;(void)msg;(void)time;
    if(++ticks>120){record("RESULT",submitted?"post-request observation ended":"no selection",ticks);CloseHandle(log_file);ExitProcess(2);}
    if(!opened){HWND tray=FindWindowA("Shell_TrayWnd",0);
        shell=FindWindowA("Progman",0);if(!shell || !tray || !IsWindowVisible(tray))return;
        if(execute_short){opened=1;submitted=1;execute_short_pif();return;}
        if(inspect_pif){
            inspect_volume();inspect_properties();inspect_pif=0;
            if(!normal_shutdown){CloseHandle(log_file);ExitProcess(0);}
        }
        record("REQUEST","Progman WM_CLOSE",(DWORD)shell);
        if(!PostMessageA(shell,WM_CLOSE,0,0))return;opened=1;
    }
    dos_button=0;dialog=0;EnumWindows(window,0);
    if(dos_button && !submitted){
        if(!execute_request)KillTimer(0,timer_id);
        record("FOUND","MS-DOS radio",(DWORD)GetDlgCtrlID(dos_button));
        if(execute_request) {
            record("CHECK_BEFORE","radio",(DWORD)SendMessageA(dos_button,BM_GETCHECK,0,0));
            if(SendMessageA(dos_button,BM_GETCHECK,0,0)!=BST_CHECKED)
                SendMessageA(dos_button,BM_CLICK,0,0);
            record("CHECK_AFTER","radio",(DWORD)SendMessageA(dos_button,BM_GETCHECK,0,0));
            if(SendMessageA(dos_button,BM_GETCHECK,0,0)!=BST_CHECKED) {
                record("RESULT","selection failed",0);CloseHandle(log_file);ExitProcess(4);
            }
            record("SUBMIT",normal_shutdown?"normal shutdown selected":"MS-DOS selected",(DWORD)GetDlgCtrlID(dos_button));
            if(!PostMessageA(dialog,WM_COMMAND,IDOK,(LPARAM)GetDlgItem(dialog,IDOK))) {
                record("RESULT","submit failed",GetLastError());CloseHandle(log_file);ExitProcess(5);
            }
        }
        record("RESULT",execute_request?"DOS request posted":"dialog identified",(DWORD)dialog);
        submitted=1;
        if(!execute_request){CloseHandle(log_file);ExitProcess(0);}
    }
}
static LRESULT CALLBACK procedure(HWND w,UINT m,WPARAM wp,LPARAM lp) {
    if(m==WM_QUERYENDSESSION){record("QUERYENDSESSION","accept",(DWORD)lp);return TRUE;}
    if(m==WM_ENDSESSION){record("ENDSESSION","notification",(DWORD)wp);if(wp){CloseHandle(log_file);ExitProcess(0);}return 0;}
    return DefWindowProcA(w,m,wp,lp);
}
void __declspec(noreturn) entry(void) {
    char path[MAX_PATH];DWORD i,version=GetVersion();MSG msg;
    if(!(version&0x80000000u)||(version&0xffff)!=4)ExitProcess(90);
    GetModuleFileNameA(0,path,sizeof(path));
    for(i=(DWORD)lstrlenA(path);i&&path[i-1]!='\\';--i){}path[i]=0;
    lstrcatA(path,"DOSGO.TAG");
    execute_request=GetFileAttributesA(path)!=INVALID_FILE_ATTRIBUTES;
    path[i]=0;lstrcatA(path,"SHUTDOWN.TAG");
    normal_shutdown=GetFileAttributesA(path)!=INVALID_FILE_ATTRIBUTES;
    path[i]=0;lstrcatA(path,"PIFTEST.TAG");
    inspect_pif=GetFileAttributesA(path)!=INVALID_FILE_ATTRIBUTES;
    path[i]=0;lstrcatA(path,"SHORTGO.TAG");
    execute_short=GetFileAttributesA(path)!=INVALID_FILE_ATTRIBUTES;
    path[i]=0;lstrcatA(path,"DOSRET.LOG");
    log_file=CreateFileA(path,GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
    if(log_file==INVALID_HANDLE_VALUE)ExitProcess(91);
    record("VERSION","Win95 guest",version);
    {static WNDCLASSA c;HWND window_handle;
     c.lpfnWndProc=procedure;c.hInstance=GetModuleHandleA(0);c.lpszClassName="DOSRestartWitness";
     if(!RegisterClassA(&c))ExitProcess(93);
     window_handle=CreateWindowA(c.lpszClassName,"DOS restart witness",WS_OVERLAPPED,0,0,0,0,0,0,c.hInstance,0);
     if(!window_handle)ExitProcess(94);
    }
    if(!SetTimer(0,0,500,timer))ExitProcess(92);
    while(GetMessageA(&msg,0,0,0)>0){TranslateMessage(&msg);DispatchMessageA(&msg);}
    CloseHandle(log_file);ExitProcess(3);
}
