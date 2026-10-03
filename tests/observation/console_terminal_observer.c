/* Bounded real-ConPTY COMMAND -> MEM -> EDIT -> MEM twice regression.
 * The Job owns only the test process tree; native Console cell snapshots
 * distinguish renderer replay defects from upstream buffer corruption.
 * Usage: observer <columns> <rows> O:\\winnt\\logs\\<capture>.raw
 */
#define _WIN32_WINNT 0x0A00
#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include "ntcon-exe/lib/kvm-window/frame_interface.h"
static HANDLE read_pipe, write_pipe, raw_log;
static HANDLE host_resize_seen;
static HANDLE s34_dir_complete_seen;
static HANDLE named_pipe_server;
static volatile LONG named_pipe_connected, named_pipe_sent, named_pipe_received;
static HANDLE lpt_pipe_server;
static volatile LONG lpt_pipe_connected, lpt_pipe_received;
static int lpt_mapping_active;
static volatile LONG comms_error_dialog_dismissed;
static int named_pipe_transact, named_pipe_async, named_pipe_async_write;
static DWORD WINAPI serve_named_pipe(void *unused) {
    DWORD written; (void)unused;
    if (ConnectNamedPipe(named_pipe_server,NULL) || GetLastError()==ERROR_PIPE_CONNECTED) {
        InterlockedExchange(&named_pipe_connected,1);
        if (named_pipe_transact) {
            char request[4];DWORD read;
            if(ReadFile(named_pipe_server,request,sizeof(request),&read,NULL)&&read==sizeof(request)&&!memcmp(request,"PING",4)) {
                InterlockedExchange(&named_pipe_received,1);
                if(WriteFile(named_pipe_server,"PONG",4,&written,NULL)&&written==4)InterlockedExchange(&named_pipe_sent,1);
            }
        } else if (named_pipe_async_write) {
            char request[4]; DWORD read;
            if (ReadFile(named_pipe_server,request,sizeof(request),&read,NULL) &&
                read == sizeof(request) && !memcmp(request,"PING",4))
                InterlockedExchange(&named_pipe_received,1);
        } else {
            if(named_pipe_async) {
                char delay_text[32];
                DWORD delay = 500;

                /* Keep the normal delayed-write assertion, but allow the
                 * guest async fixture to diagnose scheduling races without
                 * changing product behavior. */
                if (GetEnvironmentVariableA("MVDM_TEST_ASYNC_PIPE_DELAY_MS",
                                            delay_text,
                                            sizeof(delay_text)) &&
                    strspn(delay_text, "0123456789") == strlen(delay_text))
                    delay = (DWORD)strtoul(delay_text, NULL, 10);
                Sleep(delay);
            }
            if(WriteFile(named_pipe_server,named_pipe_async?"PONG":"VDMREDIR-TYPE-OK\r\n",
                named_pipe_async?4:18,&written,NULL)&&written==(DWORD)(named_pipe_async?4:18))
            InterlockedExchange(&named_pipe_sent,1);
        }
    } return 0;
}
static DWORD WINAPI serve_lpt_pipe(void *unused) {
    char value; DWORD read; (void)unused;
    if (ConnectNamedPipe(lpt_pipe_server,NULL) || GetLastError()==ERROR_PIPE_CONNECTED) {
        InterlockedExchange(&lpt_pipe_connected,1);
        if (ReadFile(lpt_pipe_server,&value,1,&read,NULL) && read==1 && value=='Z')
            InterlockedExchange(&lpt_pipe_received,1);
    }
    return 0;
}
static DWORD WINAPI copy_output(void *unused) {
    char data[8193]; DWORD n,w;
    const char resize_code[]="\x1b[8;28;80t";
    size_t matched=0;
    (void)unused;
    while(ReadFile(read_pipe,data,sizeof(data)-1,&n,NULL)&&n) {
        data[n]=0;
        WriteFile(raw_log,data,n,&w,NULL);
        for(DWORD i=0;i<n;i++) {
            if(data[i]==resize_code[matched]) {
                if(++matched==sizeof(resize_code)-1) {
                    if(host_resize_seen)SetEvent(host_resize_seen);
                    matched=0;
                }
            } else matched=data[i]==resize_code[0] ? 1 : 0;
            if(i+3<n && !memcmp(data+i,"\x1b[6n",4))
                WriteFile(write_pipe,"\x1b[1;1R",6,&w,NULL);
        }
    }
    return 0;
}
static void send_keys(const char *s) {DWORD w=0;if(!WriteFile(write_pipe,s,(DWORD)strlen(s),&w,NULL)||w!=strlen(s))fprintf(stderr,"console input write failed: %lu/%lu\n",GetLastError(),(unsigned long)w);}

/* The title sample is read from NTVWM's own Console, not from a Terminal tab
 * label or from the observer's ConPTY. This test-only process attaches just
 * long enough to read the title and never writes to that Console. */
static DWORD s36_process_pid(const char *runtime,const char *name)
{
    char expected[MAX_PATH];HANDLE image;
    BY_HANDLE_FILE_INFORMATION wanted;
    HANDLE snapshot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
    PROCESSENTRY32 entry={0};DWORD result=0;
    snprintf(expected,sizeof(expected),"%s\\%s",runtime,name);
    image=CreateFileA(expected,0,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,
        NULL,OPEN_EXISTING,0,NULL);
    if(snapshot==INVALID_HANDLE_VALUE || image==INVALID_HANDLE_VALUE ||
        !GetFileInformationByHandle(image,&wanted)) {
        if(snapshot!=INVALID_HANDLE_VALUE)CloseHandle(snapshot);
        if(image!=INVALID_HANDLE_VALUE)CloseHandle(image);
        return 0;
    }
    entry.dwSize=sizeof(entry);
    if(Process32First(snapshot,&entry))do {
        char path[MAX_PATH];DWORD size=sizeof(path);
        HANDLE process;
        if(_stricmp(entry.szExeFile,name))continue;
        process=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,FALSE,entry.th32ProcessID);
        if(process && QueryFullProcessImageNameA(process,0,path,&size)) {
            HANDLE candidate=CreateFileA(path,0,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,
                NULL,OPEN_EXISTING,0,NULL);
            BY_HANDLE_FILE_INFORMATION actual;
            if(candidate!=INVALID_HANDLE_VALUE) {
                if(GetFileInformationByHandle(candidate,&actual) &&
                    actual.dwVolumeSerialNumber==wanted.dwVolumeSerialNumber &&
                    actual.nFileIndexHigh==wanted.nFileIndexHigh &&
                    actual.nFileIndexLow==wanted.nFileIndexLow)
                    result=entry.th32ProcessID;
                CloseHandle(candidate);
            }
        }
        if(process)CloseHandle(process);
        if(result)break;
    }while(Process32Next(snapshot,&entry));
    CloseHandle(snapshot);CloseHandle(image);return result;
}
static void s36_note(const char *stage,const char *value)
{
    char line[256];DWORD written;
    int length=snprintf(line,sizeof(line),"\r\nS36-PROBE %s <%s>\r\n",stage,value);
    if(length>0 && length<(int)sizeof(line))WriteFile(raw_log,line,(DWORD)length,&written,NULL);
}
static int s36_sample_title(const char *runtime,const char *stage,char *title,DWORD capacity)
{
    DWORD pid=s36_process_pid(runtime,"ntvwm.exe"),length,error;
    if(!pid){s36_note(stage,"ntvwm-not-found");return 0;}
    FreeConsole();
    if(!AttachConsole(pid)){s36_note(stage,"attach-failed");return 0;}
    SetLastError(ERROR_SUCCESS);
    length=GetConsoleTitleA(title,capacity);
    error=GetLastError();
    if(capacity)title[capacity-1]=0;
    s36_note(stage,title);
    FreeConsole();
    return length || error==ERROR_SUCCESS;
}
static int s36_send_caf(DWORD console_pid)
{
    HANDLE input;INPUT_RECORD keys[2]={0};DWORD written=0;
    FreeConsole();
    if(!AttachConsole(console_pid)){s36_note("caf","attach-failed");return 0;}
    input=CreateFileW(L"CONIN$",GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
    if(input==INVALID_HANDLE_VALUE){s36_note("caf","conin-failed");FreeConsole();return 0;}
    keys[0].EventType=KEY_EVENT;keys[0].Event.KeyEvent.bKeyDown=TRUE;
    keys[0].Event.KeyEvent.wRepeatCount=1;keys[0].Event.KeyEvent.wVirtualKeyCode='F';
    keys[0].Event.KeyEvent.wVirtualScanCode=0x21;
    keys[0].Event.KeyEvent.dwControlKeyState=NUMLOCK_ON|LEFT_CTRL_PRESSED|LEFT_ALT_PRESSED;
    keys[1]=keys[0];keys[1].Event.KeyEvent.bKeyDown=FALSE;
    WriteConsoleInputW(input,keys,2,&written);
    CloseHandle(input);FreeConsole();s36_note("caf",written==2?"sent":"write-failed");return written==2;
}
static int s36_window_title(const char *runtime,const char *stage,const char *expected)
{
    DWORD pid=s36_process_pid(runtime,"ntcon.exe"),actual_pid=0;
    char caption[128]={0};HWND window=FindWindowW(L"LibKvmWindow",NULL);
    if(!pid || !window || !IsWindowVisible(window)){
        s36_note(stage,"window-not-visible");return 0;
    }
    GetWindowThreadProcessId(window,&actual_pid);
    if(pid!=actual_pid || !GetWindowTextA(window,caption,sizeof(caption))){
        s36_note(stage,"window-owner-or-title-failed");return 0;
    }
    s36_note(stage,caption);return !strcmp(caption,expected);
}
static int s36_close_window(void)
{
    HWND window=FindWindowW(L"LibKvmWindow",NULL);
    ULONGLONG deadline=GetTickCount64()+5000;
    if(!window || !PostMessageW(window,WM_CLOSE,0,0)){
        s36_note("close-window","not-found-or-post-failed");return 0;
    }
    while(IsWindow(window) && GetTickCount64()<deadline)Sleep(25);
    s36_note("close-window",IsWindow(window)?"still-open":"closed");
    return !IsWindow(window);
}
typedef struct s37_window_prefix {
    void *owner;
    kvm_window_frame frame;
} s37_window_prefix;
static int s37_unrequested_30,s37_requested_100x40,s37_returned_80x43;
static int s37_window_geometry(char *last,size_t capacity)
{
    HWND window=FindWindowW(L"LibKvmWindow",NULL);
    RECT client;DWORD pid;HANDLE process;SIZE_T copied;
    BYTE data[offsetof(kvm_window_frame,text)+sizeof(kvm_window_text_frame)];
    kvm_window_frame *frame=(kvm_window_frame *)data;
    ULONG_PTR address;
    char current[160],line[200];DWORD written;
    if(!window || !GetClientRect(window,&client))return 0;
    GetWindowThreadProcessId(window,&pid);
    process=OpenProcess(PROCESS_VM_READ,FALSE,pid);
    if(!process)return 0;
    address=(ULONG_PTR)GetWindowLongPtrW(window,GWLP_USERDATA)+
        offsetof(s37_window_prefix,frame);
    if(!ReadProcessMemory(process,(const void *)address,data,sizeof(data),&copied) ||
        copied!=sizeof(data) || !frame->valid){CloseHandle(process);return 0;}
    if(frame->graphics)
        snprintf(current,sizeof(current),"client=%ldx%ld graphics=%ux%u",
            client.right,client.bottom,frame->image.width,frame->image.height);
    else
        snprintf(current,sizeof(current),"client=%ldx%ld text=%ux%u font=%u",
            client.right,client.bottom,frame->text.base.text_columns,
            frame->text.base.text_rows,frame->text.base.font_height);
    if(!frame->graphics) {
        if(frame->text.base.text_columns==80 && frame->text.base.text_rows==30)
            s37_unrequested_30=1;
        if(frame->text.base.text_columns==100 && frame->text.base.text_rows==40)
            s37_requested_100x40=1;
        if(frame->text.base.text_columns==80 && frame->text.base.text_rows==43)
            s37_returned_80x43=1;
    }
    CloseHandle(process);
    if(strcmp(current,last)) {
        int length=snprintf(line,sizeof(line),"\r\nS37-GEOM %s\r\n",current);
        if(length>0 && length<(int)sizeof(line))
            WriteFile(raw_log,line,(DWORD)length,&written,NULL);
        strcpy_s(last,capacity,current);
    }
    return 1;
}
static int s37_window_text(const char *text)
{
    HWND window=FindWindowW(L"LibKvmWindow",NULL);
    if(!window)return 0;
    while(*text) {
        if(!PostMessageW(window,WM_CHAR,(WPARAM)(BYTE)*text,1))return 0;
        ++text;
    }
    return 1;
}

static int log_contains(const char *path,const char *needle) {
    HANDLE file; DWORD size,read; char *data; int found=0;
    file=CreateFileA(path,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
    if(file==INVALID_HANDLE_VALUE)return 0;
    size=GetFileSize(file,NULL);
    if(size!=INVALID_FILE_SIZE && size<1024*1024) {
        data=HeapAlloc(GetProcessHeap(),0,size+1);
        if(data && ReadFile(file,data,size,&read,NULL)) {
            data[read]=0;found=strstr(data,needle)!=NULL;
        }
        if(data)HeapFree(GetProcessHeap(),0,data);
    }
    CloseHandle(file);return found;
}
/* The first physical page containing the command output must still contain
 * the caller's banner.  NTCON may now project a 28-row DOS page into a
 * 30-row visible Console, so the physical buffer height is not a VGA mode. */
static int s33_first_command_page_kept_text(const char *raw_path,const char *expected) {
    char path[MAX_PATH],line[256];FILE *file=NULL;
    int in_page=0,banner=0,version=0;
    if(snprintf(path,sizeof(path),"%s.cells.txt",raw_path)<=0 ||
        fopen_s(&file,path,"r") || !file)return 0;
    while(fgets(line,sizeof(line),file)) {
        if(!strncmp(line,"t=",2)) {
            if(version)break;
            in_page=1;banner=0;version=0;
        } else if(in_page) {
            if(strstr(line,"Microsoft(R) Windows NT DOS"))banner=1;
            if(strstr(line,expected))version=1;
        }
    }
    fclose(file);return in_page && banner && version;
}

static int s34_prompt_has_directory_tail(const char *raw_path) {
    char path[MAX_PATH],line[256];FILE *file=NULL;
    int dirty=0;
    if(snprintf(path,sizeof(path),"%s.cells.txt",raw_path)<=0 ||
        fopen_s(&file,path,"r") || !file)return -1;
    while(fgets(line,sizeof(line),file))
        if(strstr(line,"Dir(s)") && strchr(line,'>') &&
            ((line[3]>='A' && line[3]<='Z') ||
             (line[3]>='a' && line[3]<='z')) && line[4]==':' && line[5]=='\\')
            dirty=1;
    fclose(file);return dirty;
}

static int guest_command_failed(const char *path) {
    return log_contains(path,"Bad command or filename") ||
        log_contains(path,"is not recognized as an internal or external command");
}

/* S29 alone exercises the original missing-COM error direction.  The product
 * keeps the original modal ERRORPANEL and its explicit Abort/Ignore choices;
 * this test-only observer selects its source-defined IDCANCEL/Ignore branch
 * so a headless ConPTY run can prove return and cleanup rather than hang. */
static BOOL CALLBACK dismiss_comms_error_dialog(HWND window, LPARAM unused) {
    char title[128], klass[32];
    (void)unused;
    if (InterlockedCompareExchange(&comms_error_dialog_dismissed,0,0)) return FALSE;
    if (!GetClassNameA(window,klass,sizeof(klass)) || strcmp(klass,"#32770")) return TRUE;
    if (!GetWindowTextA(window,title,sizeof(title)) ||
        !strstr(title,"16 bit MS-DOS Subsystem")) return TRUE;
    if (PostMessageA(window,WM_COMMAND,MAKEWPARAM(IDCANCEL,0),0)) {
        InterlockedExchange(&comms_error_dialog_dismissed,1);
        return FALSE;
    }
    return TRUE;
}

static void dismiss_comms_error_dialog_until_seen(void) {
    for (int attempt=0; attempt<20 &&
         !InterlockedCompareExchange(&comms_error_dialog_dismissed,0,0); ++attempt) {
        EnumWindows(dismiss_comms_error_dialog,0);
        Sleep(250);
    }
}

static DWORD WINAPI watch_cells(void *unused) {
    HANDLE h=CreateFileA("CONOUT$",GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
    HANDLE dir_event=NULL;
    char dir_event_name[80];
    (void)unused;
    if(GetEnvironmentVariableA("MVDM_TEST_S34_DIR_EVENT",dir_event_name,sizeof(dir_event_name)))
        dir_event=OpenEventA(EVENT_MODIFY_STATE,FALSE,dir_event_name);
    char path[MAX_PATH];GetEnvironmentVariableA("TEST_CELL_LOG",path,sizeof(path));
    FILE *f=fopen(path,"w");if(!f)return 1;
    DWORD previous=0,begin=GetTickCount();WCHAR data[16000];
    while(GetTickCount()-begin<45000) {
        CONSOLE_SCREEN_BUFFER_INFO info;DWORD count=0,hash=2166136261u,mode=0;
        if(GetConsoleScreenBufferInfo(h,&info)) {
            DWORD total=(DWORD)info.dwSize.X*info.dwSize.Y;if(total>16000)total=16000;
            COORD origin={0,0};ReadConsoleOutputCharacterW(h,data,total,origin,&count);
            if(dir_event && info.dwSize.X>0) {
                int summary=0,prompt=0;SHORT summary_row=-1,prompt_row=-1;
                for(DWORD row=0;row<count/(DWORD)info.dwSize.X;row++) {
                    WCHAR *line=data+row*(DWORD)info.dwSize.X;
                    for(DWORD col=0;col+6<(DWORD)info.dwSize.X;col++) {
                        if(!wcsncmp(line+col,L"Dir(s)",6)) {summary=1;summary_row=(SHORT)row;}
                        if(col>2 && line[0]>=L'A' && line[0]<=L'Z' &&
                            line[1]==L':' && line[2]==L'\\' &&
                            line[col]==L'>' &&
                            info.dwCursorPosition.Y==(SHORT)row &&
                            info.dwCursorPosition.X==(SHORT)(col+1)) {
                            DWORD tail=col+1;
                            while(tail<(DWORD)info.dwSize.X && line[tail]==L' ')tail++;
                            if(tail==(DWORD)info.dwSize.X) {prompt=1;prompt_row=(SHORT)row;}
                        }
                    }
                }
                if(summary && prompt && prompt_row>summary_row)SetEvent(dir_event);
            }
            for(DWORD i=0;i<count;i++)hash=(hash^data[i])*16777619u;
            hash^=info.dwCursorPosition.X+info.dwCursorPosition.Y*4096u;
            GetConsoleMode(h,&mode);hash^=mode;
            if(hash!=previous) {
                fprintf(f,"t=%lu buffer=%d,%d view=%d,%d,%d,%d cursor=%d,%d mode=%lu\n",GetTickCount()-begin,info.dwSize.X,info.dwSize.Y,info.srWindow.Left,info.srWindow.Top,info.srWindow.Right,info.srWindow.Bottom,info.dwCursorPosition.X,info.dwCursorPosition.Y,mode);
                for(DWORD row=0;row<count/info.dwSize.X;row++) {
                    fprintf(f,"%02lu|",row);
                    for(DWORD col=0;col<(DWORD)info.dwSize.X;col++){WCHAR c=data[row*info.dwSize.X+col];fputc(c>=32&&c<127?c:' ',f);}
                    fputc('\n',f);
                }
                fflush(f);previous=hash;
            }
        }Sleep(40);
    }fclose(f);if(dir_event)CloseHandle(dir_event);CloseHandle(h);return 0;
}

int main(int argc,char **argv) {
    if(argc==2) {
        SECURITY_ATTRIBUTES sa={sizeof(sa),NULL,TRUE};DWORD mode=0,n;
        HANDLE ci=CreateFileA("CONIN$",GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,&sa,OPEN_EXISTING,0,NULL);
        HANDLE co=CreateFileA("CONOUT$",GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,&sa,OPEN_EXISTING,0,NULL);
        char msg[128];snprintf(msg,sizeof(msg),"PTY child console input=%d output=%d\r\n",GetConsoleMode(ci,&mode),GetConsoleMode(co,&mode));WriteFile(co,msg,(DWORD)strlen(msg),&n,NULL);
        SetStdHandle(STD_INPUT_HANDLE,ci);SetStdHandle(STD_OUTPUT_HANDLE,co);SetStdHandle(STD_ERROR_HANDLE,co);
        CloseHandle(CreateThread(NULL,0,watch_cells,NULL,0,NULL));
        STARTUPINFOA startup={sizeof(startup)};PROCESS_INFORMATION child={0};
        char runtime[MAX_PATH]="O:\\winnt",cmd[MAX_PATH+32],system_directory[MAX_PATH];
        GetEnvironmentVariableA("TEST_RUNTIME_ROOT",runtime,sizeof(runtime));
        { char initial[MAX_PATH] = "COMMAND.COM";
          GetEnvironmentVariableA("MVDM_TEST_INITIAL_COMMAND",initial,sizeof(initial));
          if(GetEnvironmentVariableA("MVDM_TEST_DIRECT_CMD",NULL,0)) {
              if(!GetSystemDirectoryA(system_directory,sizeof(system_directory)))return 66;
              snprintf(cmd,sizeof(cmd),"%s\\cmd.exe /d /k",system_directory);
          }
          else snprintf(cmd,sizeof(cmd),"%s\\run16.exe %s",runtime,initial); }
        {
            char launch[2 * MAX_PATH + 32];
            snprintf(launch,sizeof(launch),"PTY child runtime=%s command=%s\r\n",runtime,cmd);
            WriteFile(co,launch,(DWORD)strlen(launch),&n,NULL);
        }
        startup.dwFlags=STARTF_USESTDHANDLES;startup.hStdInput=ci;startup.hStdOutput=startup.hStdError=co;
        if(!CreateProcessA(NULL,cmd,NULL,NULL,TRUE,0,NULL,runtime,&startup,&child))return 66;
        WaitForSingleObject(child.hProcess,INFINITE);GetExitCodeProcess(child.hProcess,&n);Sleep(400);return n;
    }
    HANDLE in_read,out_write,thread,job;
    HPCON pty;
    STARTUPINFOEXA si={0};PROCESS_INFORMATION pi={0};SIZE_T bytes=0;
    HDESK s36_desktop=NULL;char s36_desktop_name[64]={0};
    COORD size;
if(argc!=4 && (argc!=5 || (strcmp(argv[4],"--s38-window-reentry") && strcmp(argv[4],"--s38-idle-command") && strcmp(argv[4],"--s37-window-geometry") && strcmp(argv[4],"--s36-nested-title") && strcmp(argv[4],"--s33-first-ver") && strcmp(argv[4],"--s33-first-dir") && strcmp(argv[4],"--s34-full-dir") && strcmp(argv[4],"--mouse") && strcmp(argv[4],"--resize") && strcmp(argv[4],"--video-int10") && strcmp(argv[4],"--system-capability") && strcmp(argv[4],"--bios-capability") && strcmp(argv[4],"--support-capability") && strcmp(argv[4],"--disks-capability") && strcmp(argv[4],"--comms-capability") && strcmp(argv[4],"--comms-host-medium") && strcmp(argv[4],"--comms-loopback") && strcmp(argv[4],"--lpt-host-medium") && strcmp(argv[4],"--dosx-himem-capability") && strcmp(argv[4],"--pure-dos-capability") && strcmp(argv[4],"--ems-capability") && strcmp(argv[4],"--vdmredir-pipe") && strcmp(argv[4],"--vdmredir-transact") && strcmp(argv[4],"--vdmredir-call") && strcmp(argv[4],"--vdmredir-timeout") && strcmp(argv[4],"--vdmredir-async") && strcmp(argv[4],"--vdmredir-async-write") && strcmp(argv[4],"--vdmredir-mailslot") && strcmp(argv[4],"--vdmredir-terminate") && strcmp(argv[4],"--vdmredir-netbios") && strcmp(argv[4],"--vdmredir-netbios-async") && strcmp(argv[4],"--vdmredir-dlc") && strcmp(argv[4],"--vdmredir-netapi") && strcmp(argv[4],"--vdmredir-net-enum") && strcmp(argv[4],"--vdmredir-wksta") && strcmp(argv[4],"--vdmredir-wksta-set") && strcmp(argv[4],"--vdmredir-message") && strcmp(argv[4],"--vdmredir-service") && strcmp(argv[4],"--vdmredir-assign") && strcmp(argv[4],"--vdmredir-use") && strcmp(argv[4],"--vdmredir-use-info") && strcmp(argv[4],"--vdmredir-use-lifecycle"))))return 64;
    size.X=(SHORT)atoi(argv[1]);size.Y=(SHORT)atoi(argv[2]);
    if(argc==5 && !strcmp(argv[4],"--lpt-host-medium")) {
        lpt_pipe_server=CreateNamedPipeA("\\\\.\\pipe\\NTVDMLPTTEST",PIPE_ACCESS_INBOUND,PIPE_TYPE_BYTE|PIPE_WAIT,1,16,16,0,NULL);
        if(lpt_pipe_server==INVALID_HANDLE_VALUE)return 68;
        if(!DefineDosDeviceA(DDD_RAW_TARGET_PATH,"LPT1","\\Device\\NamedPipe\\NTVDMLPTTEST")){CloseHandle(lpt_pipe_server);return 69;}
        lpt_mapping_active=1; CloseHandle(CreateThread(NULL,0,serve_lpt_pipe,NULL,0,NULL));
    }
    if(argc==5 && (!strcmp(argv[4],"--vdmredir-pipe") || !strcmp(argv[4],"--vdmredir-transact") || !strcmp(argv[4],"--vdmredir-call") || !strcmp(argv[4],"--vdmredir-async") || !strcmp(argv[4],"--vdmredir-async-write"))) {
        char pipe_name[80]; snprintf(pipe_name,sizeof(pipe_name),"\\\\.\\pipe\\NTPTEST");
        named_pipe_transact=!strcmp(argv[4],"--vdmredir-transact") || !strcmp(argv[4],"--vdmredir-call");
        named_pipe_async=!strcmp(argv[4],"--vdmredir-async");
        named_pipe_async_write=!strcmp(argv[4],"--vdmredir-async-write");
        named_pipe_server=CreateNamedPipeA((pipe_name),
            (named_pipe_transact || named_pipe_async_write)?PIPE_ACCESS_DUPLEX:PIPE_ACCESS_OUTBOUND,
            (named_pipe_transact?PIPE_TYPE_MESSAGE|PIPE_READMODE_MESSAGE:PIPE_TYPE_BYTE|PIPE_READMODE_BYTE)|PIPE_WAIT,1,64,64,0,NULL);
        if(named_pipe_server==INVALID_HANDLE_VALUE)return 68;
        CloseHandle(CreateThread(NULL,0,serve_named_pipe,NULL,0,NULL));
    }
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION limits={0};
    char executable[MAX_PATH], command[MAX_PATH+16];
    GetModuleFileNameA(NULL,executable,sizeof(executable));
    snprintf(command,sizeof(command),"\"%s\" --child",executable);
    { char path[MAX_PATH];snprintf(path,sizeof(path),"%s.cells.txt",argv[3]);SetEnvironmentVariableA("TEST_CELL_LOG",path); }
    raw_log=CreateFileA(argv[3],GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,0,NULL);
    if(raw_log==INVALID_HANDLE_VALUE) {
        fprintf(stderr,"cannot create raw log (error %lu)\n",GetLastError());
        return 69;
    }
    host_resize_seen=CreateEventW(NULL,TRUE,FALSE,NULL);
    {
        char name[80];
        snprintf(name,sizeof(name),"Local\\MVDM-S34-DIR-%lu",GetCurrentProcessId());
        s34_dir_complete_seen=CreateEventA(NULL,TRUE,FALSE,name);
        SetEnvironmentVariableA("MVDM_TEST_S34_DIR_EVENT",name);
    }
    CreatePipe(&in_read,&write_pipe,NULL,0);CreatePipe(&read_pipe,&out_write,NULL,0);
    if(FAILED(CreatePseudoConsole(size,in_read,out_write,0,&pty)))return 65;
    CloseHandle(in_read);CloseHandle(out_write);
    thread=CreateThread(NULL,0,copy_output,NULL,0,NULL);
    InitializeProcThreadAttributeList(NULL,1,0,&bytes);
    si.lpAttributeList=HeapAlloc(GetProcessHeap(),0,bytes);
    InitializeProcThreadAttributeList(si.lpAttributeList,1,0,&bytes);
    UpdateProcThreadAttribute(si.lpAttributeList,0,PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE,pty,sizeof(pty),NULL,NULL);
    si.StartupInfo.cb=sizeof(si);
    if(argc==5 && (!strcmp(argv[4],"--s38-window-reentry") || !strcmp(argv[4],"--s36-nested-title") ||
        !strcmp(argv[4],"--s37-window-geometry"))) {
        snprintf(s36_desktop_name,sizeof(s36_desktop_name),"NTVDMConsoleTest-S36-%lu",GetCurrentProcessId());
        s36_desktop=CreateDesktopA(s36_desktop_name,NULL,NULL,0,GENERIC_ALL,NULL);
        if(!s36_desktop || !SetThreadDesktop(s36_desktop))return 79;
        si.StartupInfo.lpDesktop=s36_desktop_name;
    }
    job=CreateJobObjectA(NULL,NULL); limits.BasicLimitInformation.LimitFlags=JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
    SetInformationJobObject(job,JobObjectExtendedLimitInformation,&limits,sizeof(limits));
    HANDLE saved_in=GetStdHandle(STD_INPUT_HANDLE),saved_out=GetStdHandle(STD_OUTPUT_HANDLE),saved_err=GetStdHandle(STD_ERROR_HANDLE);
    SetStdHandle(STD_INPUT_HANDLE,NULL);SetStdHandle(STD_OUTPUT_HANDLE,NULL);SetStdHandle(STD_ERROR_HANDLE,NULL);
    if(!CreateProcessA(NULL,command,NULL,NULL,FALSE,EXTENDED_STARTUPINFO_PRESENT|CREATE_SUSPENDED,NULL,"O:\\winnt",&si.StartupInfo,&pi))return 66;
    SetStdHandle(STD_INPUT_HANDLE,saved_in);SetStdHandle(STD_OUTPUT_HANDLE,saved_out);SetStdHandle(STD_ERROR_HANDLE,saved_err);
    AssignProcessToJobObject(job,pi.hProcess);ResumeThread(pi.hThread);
    printf("launcher=%lu width=%d height=%d\n",pi.dwProcessId,size.X,size.Y);fflush(stdout);
    send_keys("\x1b[I");
    { char boot_wait_text[16]; DWORD boot_wait=6000;
      if(GetEnvironmentVariableA("MVDM_TEST_BOOT_WAIT_MS",boot_wait_text,
          sizeof(boot_wait_text))) boot_wait=(DWORD)strtoul(boot_wait_text,0,10);
      Sleep(boot_wait); }
    if(argc==5 && !strcmp(argv[4],"--s38-idle-command")) {
        char runtime[MAX_PATH]="O:\\winnt",launch[MAX_PATH+32];
        GetEnvironmentVariableA("TEST_RUNTIME_ROOT",runtime,sizeof(runtime));
        snprintf(launch,sizeof(launch),"%s\\run16 command\r",runtime);
        send_keys(launch);
        Sleep(15000);
        send_keys("echo S38-OUTER-STATUS=%errorlevel%\r");Sleep(500);
        send_keys("mem\r");Sleep(3000);
        send_keys("exit\r");Sleep(800);send_keys("exit\r");
        WaitForSingleObject(pi.hProcess,5000);
        CloseHandle(job);ClosePseudoConsole(pty);CloseHandle(write_pipe);
        WaitForSingleObject(thread,3000);CloseHandle(raw_log);
        printf("s38-idle-command log=%s\n",argv[3]);
        return 0;
    }
    if(argc==5 && !strcmp(argv[4],"--s38-window-reentry")) {
        char runtime[MAX_PATH]="O:\\winnt",launch[MAX_PATH+32];
        GetEnvironmentVariableA("TEST_RUNTIME_ROOT",runtime,sizeof(runtime));
        snprintf(launch,sizeof(launch),"%s\\run16 command\r",runtime);
        send_keys(launch);Sleep(3000);
        if(!s36_send_caf(pi.dwProcessId))return 90;
        Sleep(1000);
        if(!s37_window_text("exit\r"))return 91;
        Sleep(3000);
        s36_note("second-launch","begin");
        send_keys(launch);Sleep(12000);
        s36_note("second-launch","after-12s");
        send_keys("mem\r");Sleep(3000);
        send_keys("exit\r");Sleep(800);send_keys("exit\r");
        WaitForSingleObject(pi.hProcess,5000);
        CloseHandle(job);ClosePseudoConsole(pty);CloseHandle(write_pipe);
        WaitForSingleObject(thread,3000);CloseHandle(raw_log);
        printf("s38-window-reentry log=%s\n",argv[3]);
        return 0;
    }
    if(argc==5 && !strcmp(argv[4],"--s37-window-geometry")) {
            char runtime[MAX_PATH]="O:\\winnt",launch[MAX_PATH+32],last[160]={0};
            ULONGLONG until;unsigned samples=0;
            int roundtrip=GetEnvironmentVariableA("MVDM_S37_ROUNDTRIP",NULL,0)!=0;
            int resize=GetEnvironmentVariableA("MVDM_S37_RESIZE",NULL,0)!=0;
            GetEnvironmentVariableA("TEST_RUNTIME_ROOT",runtime,sizeof(runtime));
            snprintf(launch,sizeof(launch),"%s\\run16 command\r",runtime);
            send_keys(launch);Sleep(3000);
            if(!s36_send_caf(pi.dwProcessId))return 90;
            Sleep(800);
            if(!s37_window_geometry(last,sizeof(last)))return 91;
            if(!s37_window_text("ver\r"))return 92;
            until=GetTickCount64()+4000;
            while(GetTickCount64()<until) {
                samples+=(unsigned)s37_window_geometry(last,sizeof(last));
                Sleep(10);
            }
            if(roundtrip) {
                if(!s37_window_text("cmd\r"))return 94;
                until=GetTickCount64()+4000;
                while(GetTickCount64()<until) {
                    samples+=(unsigned)s37_window_geometry(last,sizeof(last));
                    Sleep(10);
                }
                if(resize) {
                    if(!s37_window_text("mode con cols=100 lines=40\r"))return 96;
                    until=GetTickCount64()+4000;
                    while(GetTickCount64()<until) {
                        samples+=(unsigned)s37_window_geometry(last,sizeof(last));
                        Sleep(10);
                    }
                }
                if(!s37_window_text("exit\r"))return 95;
                until=GetTickCount64()+4000;
                while(GetTickCount64()<until) {
                    samples+=(unsigned)s37_window_geometry(last,sizeof(last));
                    Sleep(10);
                }
            }
            if(!s36_close_window())return 93;
            send_keys("exit\r");Sleep(500);send_keys("exit\r");
            WaitForSingleObject(pi.hProcess,5000);
            CloseHandle(job);ClosePseudoConsole(pty);CloseHandle(write_pipe);
            WaitForSingleObject(thread,3000);CloseHandle(raw_log);
            printf("s37-window-geometry samples=%u unexpected30=%d native100x40=%d dos80x43=%d\n",
                samples,s37_unrequested_30,s37_requested_100x40,s37_returned_80x43);
            return samples && !s37_unrequested_30 &&
                (!resize || (s37_requested_100x40 && s37_returned_80x43)) ? 0 : 1;
    }
    if(argc==5 && !strcmp(argv[4],"--s36-nested-title")) {
        char runtime[MAX_PATH]="O:\\winnt",launch[MAX_PATH+32],title[128]={0};
        int outer,nested,inner,after,window_outer,window_nested,window_inner,window_after;
        GetEnvironmentVariableA("TEST_RUNTIME_ROOT",runtime,sizeof(runtime));
        snprintf(launch,sizeof(launch),"%s\\run16 cmd\r",runtime);
        send_keys(launch);Sleep(3000);
        send_keys("title S36-OUTER\r");Sleep(1000);
        outer=s36_sample_title(runtime,"outer",title,sizeof(title)) &&
            !strcmp(title,"S36-OUTER");
        if(!s36_send_caf(pi.dwProcessId))return 80;
        Sleep(1200);
        window_outer=s36_window_title(runtime,"outer","S36-OUTER");
        if(!s36_close_window())return 81;
        send_keys("cmd\r");Sleep(1500);
        nested=s36_sample_title(runtime,"nested-before-title",title,sizeof(title)) &&
            !strcmp(title,"S36-OUTER - cmd");
        if(!s36_send_caf(pi.dwProcessId))return 86;
        Sleep(1200);
        window_nested=s36_window_title(runtime,"nested-before-title","S36-OUTER - cmd");
        if(!s36_close_window())return 87;
        send_keys("title S36-INNER\r");Sleep(1000);
        inner=s36_sample_title(runtime,"inner",title,sizeof(title)) &&
            !strcmp(title,"S36-INNER");
        if(!s36_send_caf(pi.dwProcessId))return 82;
        Sleep(1200);
        window_inner=s36_window_title(runtime,"inner","S36-INNER");
        if(!s36_close_window())return 83;
        send_keys("exit\r");Sleep(1500);
        after=s36_sample_title(runtime,"after-inner-exit",title,sizeof(title));
        if(!s36_send_caf(pi.dwProcessId))return 84;
        Sleep(1200);
        window_after=after && s36_window_title(runtime,"after-inner-exit",title);
        if(!s36_close_window())return 85;
        send_keys("exit\r");Sleep(500);send_keys("exit\r");
        WaitForSingleObject(pi.hProcess,5000);
        CloseHandle(job);ClosePseudoConsole(pty);CloseHandle(write_pipe);
        WaitForSingleObject(thread,3000);CloseHandle(raw_log);
        printf("s36-title outer=%d nested=%d inner=%d after=%d window=%d/%d/%d/%d\n",
            outer,nested,inner,after,window_outer,window_nested,window_inner,window_after);
        return outer && nested && inner && after && window_outer && window_nested &&
            window_inner && window_after ? 0 : 1;
    }
    if(argc==5 && (!strcmp(argv[4],"--s33-first-ver") || !strcmp(argv[4],"--s33-first-dir"))) {
        char runtime[MAX_PATH]="O:\\winnt",launch[MAX_PATH+32];
        int dir=!strcmp(argv[4],"--s33-first-dir");
        DWORD wait,code=STILL_ACTIVE;
        GetEnvironmentVariableA("TEST_RUNTIME_ROOT",runtime,sizeof(runtime));
        snprintf(launch,sizeof(launch),"%s\\run16 command\r",runtime);
        send_keys(launch);Sleep(3000);
        send_keys(dir ? "dir AUTOEXEC.NT\r" : "ver\r");Sleep(7000);
        wait=WaitForSingleObject(pi.hProcess,0);
        send_keys("mem\r");Sleep(1500);
        send_keys("exit\r");Sleep(1200);send_keys("exit\r");
        WaitForSingleObject(pi.hProcess,5000);GetExitCodeProcess(pi.hProcess,&code);
        CloseHandle(job);ClosePseudoConsole(pty);CloseHandle(write_pipe);
        WaitForSingleObject(thread,3000);CloseHandle(raw_log);
        { int preserved=s33_first_command_page_kept_text(argv[3],
              dir ? "Directory of" : "Microsoft Windows [Version");
          int dos_alive=log_contains(argv[3],"bytes total conventional memory");
          printf("s33-child-after-command=%lu final=%lu preserved=%d dos-alive=%d\n",
              wait,code,preserved,dos_alive);
          return wait==WAIT_TIMEOUT && preserved && dos_alive ? 0 : 1; }
    }
    if(argc==5 && !strcmp(argv[4],"--s34-full-dir")) {
        char runtime[MAX_PATH]="O:\\winnt",launch[MAX_PATH+32];
        DWORD wait,code=STILL_ACTIVE;
        int dirty,entered;BOOL directory_complete;
        GetEnvironmentVariableA("TEST_RUNTIME_ROOT",runtime,sizeof(runtime));
        snprintf(launch,sizeof(launch),"%s\\run16 command\r",runtime);
        send_keys(launch);
        {
            char text[16];DWORD delay=3000;
            if(GetEnvironmentVariableA("MVDM_TEST_S34_GUEST_WAIT_MS",text,sizeof(text)))
                delay=(DWORD)strtoul(text,NULL,10);
            Sleep(delay);
        }
        {
            char probe[MAX_PATH];
            if(GetEnvironmentVariableA("MVDM_TEST_S34_SCROLL_MARGIN",probe,sizeof(probe))) {
                STARTUPINFOA start={sizeof(start)};PROCESS_INFORMATION helper={0};
                char line[3*MAX_PATH];
                if(WaitForSingleObject(host_resize_seen,7000)!=WAIT_OBJECT_0)return 76;
                snprintf(line,sizeof(line),"\"%s\" %lu \"%s.margin.txt\" vt-scroll-region-28",
                    probe,pi.dwProcessId,argv[3]);
                if(!CreateProcessA(NULL,line,NULL,NULL,FALSE,CREATE_NO_WINDOW,NULL,NULL,&start,&helper))
                    return 77;
                if(WaitForSingleObject(helper.hProcess,5000)!=WAIT_OBJECT_0 ||
                    !GetExitCodeProcess(helper.hProcess,&code) || code) {
                    CloseHandle(helper.hThread);CloseHandle(helper.hProcess);return 78;
                }
                CloseHandle(helper.hThread);CloseHandle(helper.hProcess);
            }
        }
        send_keys("dir\r");
        if((GetEnvironmentVariableA("MVDM_TEST_HOST_RETURNS_GEOMETRY",NULL,0) ||
            GetEnvironmentVariableA("MVDM_TEST_HOST_HONORS_GEOMETRY",NULL,0)) &&
            WaitForSingleObject(host_resize_seen,7000)==WAIT_OBJECT_0) {
            COORD host_size=size;
            if(GetEnvironmentVariableA("MVDM_TEST_HOST_HONORS_GEOMETRY",NULL,0))
                host_size.Y=28;
            ResizePseudoConsole(pty,host_size);
        }
        directory_complete=WaitForSingleObject(s34_dir_complete_seen,20000)==WAIT_OBJECT_0;
        if(!directory_complete)
            fprintf(stderr,"first directory prompt not observed before deadline\n");
        Sleep(500);
        {
            char probe[MAX_PATH];
            if(GetEnvironmentVariableA("MVDM_TEST_S34_BUFFER_TOGGLE",probe,sizeof(probe))) {
                STARTUPINFOA start={sizeof(start)};PROCESS_INFORMATION helper={0};
                char line[3*MAX_PATH];
                snprintf(line,sizeof(line),"\"%s\" %lu \"%s.toggle.txt\" buffer-toggle",
                    probe,pi.dwProcessId,argv[3]);
                if(!CreateProcessA(NULL,line,NULL,NULL,FALSE,CREATE_NO_WINDOW,NULL,NULL,&start,&helper))
                    return 74;
                if(WaitForSingleObject(helper.hProcess,5000)!=WAIT_OBJECT_0 ||
                    !GetExitCodeProcess(helper.hProcess,&code) || code) {
                    CloseHandle(helper.hThread);CloseHandle(helper.hProcess);return 75;
                }
                CloseHandle(helper.hThread);CloseHandle(helper.hProcess);
                send_keys("ver\r");Sleep(4000);
            } else if(GetEnvironmentVariableA("MVDM_TEST_S34_REPEAT_DIR",NULL,0)) {
                ResetEvent(s34_dir_complete_seen);
                send_keys("dir\r");
                directory_complete=directory_complete &&
                    WaitForSingleObject(s34_dir_complete_seen,20000)==WAIT_OBJECT_0;
                Sleep(500);
            }
        }
        wait=WaitForSingleObject(pi.hProcess,0);
        send_keys("exit\r");Sleep(1000);send_keys("exit\r");
        WaitForSingleObject(pi.hProcess,5000);GetExitCodeProcess(pi.hProcess,&code);
        CloseHandle(job);ClosePseudoConsole(pty);CloseHandle(write_pipe);
        WaitForSingleObject(thread,3000);CloseHandle(raw_log);
        dirty=s34_prompt_has_directory_tail(argv[3]);
        entered=log_contains(argv[3],"Microsoft(R) Windows NT DOS");
        printf("s34-child-after-dir=%lu final=%lu entered-dos=%d dirty-prompt=%d\n",
            wait,code,entered,dirty);
        return directory_complete && wait==WAIT_TIMEOUT && entered && dirty==0 ? 0 : 1;
    }
    if(argc==5 && !strcmp(argv[4],"--video-int10")) {
        char video_command[MAX_PATH];
        if (!GetEnvironmentVariableA("MVDM_TEST_VIDEO_COMMAND",video_command,
                sizeof(video_command))) return 70;
        send_keys(video_command); send_keys("\r"); Sleep(2500);
        send_keys("mem\r"); Sleep(2000); send_keys("exit\r");
        { DWORD video_wait=WaitForSingleObject(pi.hProcess,5000),video_code=0;
          GetExitCodeProcess(pi.hProcess,&video_code);
          printf("video-int10 wait=%lu exit=%lu\n",video_wait,video_code); fflush(stdout);
          Sleep(300); CloseHandle(write_pipe); WaitForSingleObject(thread,3000);
          CloseHandle(raw_log);
          { int passed=video_wait==WAIT_OBJECT_0 && video_code==1 &&
              log_contains(argv[3],"S23_INT10_WRITER_OK") &&
              log_contains(argv[3],"VVVV") &&
              log_contains(argv[3],"S23I") &&
              log_contains(argv[3],"bytes total conventional memory") &&
              !guest_command_failed(argv[3]);
            CloseHandle(job);ClosePseudoConsole(pty);return passed?0:1; }
        }
    }
    if(argc==5 && !strcmp(argv[4],"--system-capability")) {
        char system_command[MAX_PATH];
        if (!GetEnvironmentVariableA("MVDM_TEST_SYSTEM_COMMAND",system_command,
                sizeof(system_command))) return 70;
        send_keys(system_command); send_keys("\r"); Sleep(5000);
        { DWORD system_wait=WaitForSingleObject(pi.hProcess,5000),system_code=0;
          GetExitCodeProcess(pi.hProcess,&system_code);
          printf("system-capability wait=%lu exit=%lu\n",system_wait,system_code); fflush(stdout);
          Sleep(300); CloseHandle(write_pipe); WaitForSingleObject(thread,3000);
          CloseHandle(raw_log);
          { int passed=log_contains(argv[3],"S24_SYSTEM_OK") &&
              !guest_command_failed(argv[3]);
            CloseHandle(job);ClosePseudoConsole(pty);return passed?0:1; }
        }
    }
    if(argc==5 && !strcmp(argv[4],"--bios-capability")) {
        char bios_command[MAX_PATH];
        if (!GetEnvironmentVariableA("MVDM_TEST_BIOS_COMMAND",bios_command,
                sizeof(bios_command))) return 70;
        send_keys(bios_command); send_keys("\r"); Sleep(3000);
        { DWORD bios_wait=WaitForSingleObject(pi.hProcess,5000),bios_code=0;
          GetExitCodeProcess(pi.hProcess,&bios_code);
          printf("bios-capability wait=%lu exit=%lu\n",bios_wait,bios_code); fflush(stdout);
          Sleep(300); CloseHandle(write_pipe); WaitForSingleObject(thread,3000);
          CloseHandle(raw_log);
          { int marker=log_contains(argv[3],"S26_BIOS_OK"),failed=guest_command_failed(argv[3]);
            int passed=marker && !failed;
            printf("bios-capability marker=%d guest-failure=%d\n",marker,failed); fflush(stdout);
            CloseHandle(job);ClosePseudoConsole(pty);return passed?0:1; }
        }
    }
    if(argc==5 && !strcmp(argv[4],"--support-capability")) {
        char support_command[MAX_PATH];
        if (!GetEnvironmentVariableA("MVDM_TEST_SUPPORT_COMMAND",support_command,
                sizeof(support_command))) return 70;
        send_keys(support_command); send_keys("\r"); Sleep(3000);
        { DWORD support_wait=WaitForSingleObject(pi.hProcess,5000),support_code=0;
          GetExitCodeProcess(pi.hProcess,&support_code);
          printf("support-capability wait=%lu exit=%lu\n",support_wait,support_code); fflush(stdout);
          Sleep(300); CloseHandle(write_pipe); WaitForSingleObject(thread,3000);
          CloseHandle(raw_log);
          { int marker=log_contains(argv[3],"S27_SUPPORT_OK"),failed=guest_command_failed(argv[3]);
            int passed=marker && !failed;
            printf("support-capability marker=%d guest-failure=%d\n",marker,failed); fflush(stdout);
            CloseHandle(job);ClosePseudoConsole(pty);return passed?0:1; }
        }
    }
    if(argc==5 && !strcmp(argv[4],"--disks-capability")) {
        char disks_command[MAX_PATH];
        if (!GetEnvironmentVariableA("MVDM_TEST_DISKS_COMMAND",disks_command,
                sizeof(disks_command))) return 71;
        send_keys(disks_command); send_keys("\r"); Sleep(3000);
        { DWORD disks_wait=WaitForSingleObject(pi.hProcess,5000),disks_code=0;
          GetExitCodeProcess(pi.hProcess,&disks_code);
          printf("disks-capability wait=%lu exit=%lu\n",disks_wait,disks_code); fflush(stdout);
          Sleep(300); CloseHandle(write_pipe); WaitForSingleObject(thread,3000);
          CloseHandle(raw_log);
          { int marker=log_contains(argv[3],"S28_DISKS_OK"),failed=guest_command_failed(argv[3]);
            int passed=marker && !failed;
            printf("disks-capability marker=%d guest-failure=%d\n",marker,failed); fflush(stdout);
            CloseHandle(job);ClosePseudoConsole(pty);return passed?0:1; }
        }
    }
    if(argc==5 && !strcmp(argv[4],"--comms-capability")) {
        char comms_command[MAX_PATH];
        if (!GetEnvironmentVariableA("MVDM_TEST_COMMS_COMMAND",comms_command,
                sizeof(comms_command))) return 72;
        send_keys(comms_command); send_keys("\r"); dismiss_comms_error_dialog_until_seen(); Sleep(1000);
        send_keys("mem\r"); Sleep(2000); send_keys("exit\r");
        { DWORD comms_wait=WaitForSingleObject(pi.hProcess,5000),comms_code=0;
          GetExitCodeProcess(pi.hProcess,&comms_code);
          printf("comms-capability wait=%lu exit=%lu\n",comms_wait,comms_code); fflush(stdout);
          Sleep(300); CloseHandle(write_pipe); WaitForSingleObject(thread,3000);
          CloseHandle(raw_log);
          { int marker=log_contains(argv[3],"S29_COMMS_OK"),failed=guest_command_failed(argv[3]);
            int dismissed=InterlockedCompareExchange(&comms_error_dialog_dismissed,0,0);
            int passed=comms_wait==WAIT_OBJECT_0 && comms_code==1 && dismissed && marker && !failed &&
                log_contains(argv[3],"bytes total conventional memory");
            printf("comms-capability dialog-ignore=%d marker=%d guest-failure=%d\n",dismissed,marker,failed); fflush(stdout);
            CloseHandle(job);ClosePseudoConsole(pty);return passed?0:1; }
        }
    }
    if(argc==5 && !strcmp(argv[4],"--comms-host-medium")) {
        { DWORD comms_wait=WaitForSingleObject(pi.hProcess,12000),comms_code=0;
          GetExitCodeProcess(pi.hProcess,&comms_code);
          printf("comms-host-medium wait=%lu exit=%lu\n",comms_wait,comms_code); fflush(stdout);
          Sleep(300); CloseHandle(write_pipe); WaitForSingleObject(thread,3000);
          CloseHandle(raw_log);
          { int marker=log_contains(argv[3],"S30_COM3_OPEN_OK") && log_contains(argv[3],"S30_COM3_TX_OK");
            int failed=log_contains(argv[3],"S30_COM3_OPEN_OR_TX_FAIL") || guest_command_failed(argv[3]);
            printf("comms-host-medium marker=%d guest-failure=%d\n",marker,failed); fflush(stdout);
            CloseHandle(job);ClosePseudoConsole(pty);return comms_wait==WAIT_OBJECT_0 && comms_code==0 && marker && !failed?0:1; }
        }
    }
    if(argc==5 && !strcmp(argv[4],"--comms-loopback")) {
        { DWORD comms_wait=WaitForSingleObject(pi.hProcess,12000),comms_code=0;
          GetExitCodeProcess(pi.hProcess,&comms_code);
          printf("comms-loopback wait=%lu exit=%lu\n",comms_wait,comms_code); fflush(stdout);
          Sleep(300); CloseHandle(write_pipe); WaitForSingleObject(thread,3000);
          CloseHandle(raw_log);
          { int marker=log_contains(argv[3],"S30_COM3_LOOPBACK_TX_RX_OK");
            int failed=log_contains(argv[3],"S30_COM3_LOOPBACK_TX_RX_FAIL") || guest_command_failed(argv[3]);
            printf("comms-loopback marker=%d guest-failure=%d\n",marker,failed); fflush(stdout);
            CloseHandle(job);ClosePseudoConsole(pty);return comms_wait==WAIT_OBJECT_0 && comms_code==0 && marker && !failed?0:1; }
        }
    }
    if(argc==5 && !strcmp(argv[4],"--lpt-host-medium")) {
        DWORD lpt_wait=WaitForSingleObject(pi.hProcess,12000),lpt_code=0;
        GetExitCodeProcess(pi.hProcess,&lpt_code); Sleep(300); CloseHandle(write_pipe); WaitForSingleObject(thread,3000); CloseHandle(raw_log);
        if(lpt_mapping_active) DefineDosDeviceA(DDD_RAW_TARGET_PATH|DDD_REMOVE_DEFINITION|DDD_EXACT_MATCH_ON_REMOVE,"LPT1","\\Device\\NamedPipe\\NTVDMLPTTEST");
        printf("lpt-host-medium connected=%ld received=%ld wait=%lu exit=%lu\n",lpt_pipe_connected,lpt_pipe_received,lpt_wait,lpt_code); fflush(stdout);
        CloseHandle(lpt_pipe_server); CloseHandle(job); ClosePseudoConsole(pty);
        return lpt_wait==WAIT_OBJECT_0 && lpt_code==0 && lpt_pipe_connected && lpt_pipe_received && log_contains(argv[3],"S30_LPT1_WRITE_OK")?0:1;
    }
    if(argc==5 && (!strcmp(argv[4],"--dosx-himem-capability") || !strcmp(argv[4],"--pure-dos-capability"))) {
        const char *marker=!strcmp(argv[4],"--dosx-himem-capability") ? "S30_HIMEM_DOSX_OK" : "S30_PURE_DOS_OK";
        const char *failure=!strcmp(argv[4],"--dosx-himem-capability") ? "S30_HIMEM_DOSX_FAIL" : "S30_PURE_DOS_FAIL";
        DWORD profile_wait=WaitForSingleObject(pi.hProcess,12000),profile_code=0;
        GetExitCodeProcess(pi.hProcess,&profile_code);
        printf("profile-capability wait=%lu exit=%lu\n",profile_wait,profile_code); fflush(stdout);
        Sleep(300); CloseHandle(write_pipe); WaitForSingleObject(thread,3000);
        CloseHandle(raw_log);
        { int found=log_contains(argv[3],marker),failed=log_contains(argv[3],failure) || guest_command_failed(argv[3]);
          printf("profile-capability marker=%d guest-failure=%d\n",found,failed); fflush(stdout);
          CloseHandle(job);ClosePseudoConsole(pty);return profile_wait==WAIT_OBJECT_0 && profile_code==0 && found && !failed?0:1; }
    }
    if(argc==5 && !strcmp(argv[4],"--ems-capability")) {
        { DWORD ems_wait=WaitForSingleObject(pi.hProcess,12000),ems_code=0;
          GetExitCodeProcess(pi.hProcess,&ems_code);
          printf("ems-capability wait=%lu exit=%lu\n",ems_wait,ems_code); fflush(stdout);
          Sleep(300); CloseHandle(write_pipe); WaitForSingleObject(thread,3000);
          CloseHandle(raw_log);
          { int marker=log_contains(argv[3],"S30_HIMEM_ONLY_OK") && log_contains(argv[3],"S30_EMS_STATUS_OK") && log_contains(argv[3],"S30_EMS_MAP_OK") &&
                log_contains(argv[3],"S30_EMS_UNMAP_OK") && log_contains(argv[3],"S30_EMS_FREE_OK");
            int failed=log_contains(argv[3],"S30_EMS_FAIL");
            int passed=ems_wait==WAIT_OBJECT_0 && ems_code==0 && marker && !failed;
            printf("ems-capability marker=%d guest-failure=%d\n",marker,failed); fflush(stdout);
            CloseHandle(job);ClosePseudoConsole(pty);return passed?0:1; }
        }
    }
    if(argc==5 && !strcmp(argv[4],"--vdmredir-pipe")) {
        send_keys("REDIR.EXE\r"); Sleep(1500); send_keys("VDMPRDR.COM\r"); Sleep(3000); send_keys("mem\r");
        Sleep(2000); send_keys("exit\r");
        DWORD pipe_wait=WaitForSingleObject(pi.hProcess,5000),pipe_code=0; GetExitCodeProcess(pi.hProcess,&pipe_code);
        printf("pipe connected=%ld sent=%ld wait=%lu exit=%lu\n",named_pipe_connected,named_pipe_sent,pipe_wait,pipe_code); fflush(stdout);
        CloseHandle(job);ClosePseudoConsole(pty);CloseHandle(write_pipe);WaitForSingleObject(thread,3000);CloseHandle(raw_log);CloseHandle(named_pipe_server);
        return pipe_wait==WAIT_OBJECT_0 && pipe_code==1 && named_pipe_connected && named_pipe_sent &&
            log_contains(argv[3],"READ-RETURNED\r\nVDMREDIR-TYPE-OK") &&
            log_contains(argv[3],"bytes total conventional memory")?0:1;
    }
    if(argc==5 && !strcmp(argv[4],"--vdmredir-transact")) {
        send_keys("REDIR.EXE\r"); Sleep(1500); send_keys("VDMPTX.COM\r"); Sleep(3000); send_keys("mem\r");Sleep(2000);send_keys("exit\r");
        DWORD pipe_wait=WaitForSingleObject(pi.hProcess,5000),pipe_code=0; GetExitCodeProcess(pi.hProcess,&pipe_code);
        printf("transact connected=%ld received=%ld sent=%ld wait=%lu exit=%lu\n",named_pipe_connected,named_pipe_received,named_pipe_sent,pipe_wait,pipe_code); fflush(stdout);
        CloseHandle(job);ClosePseudoConsole(pty);CloseHandle(write_pipe);WaitForSingleObject(thread,3000);CloseHandle(raw_log);CloseHandle(named_pipe_server);
        return pipe_wait==WAIT_OBJECT_0 && pipe_code==1 && named_pipe_connected && named_pipe_received && named_pipe_sent &&
            log_contains(argv[3],"TRANSACT-OK") && log_contains(argv[3],"bytes total conventional memory")?0:1;
    }
    if(argc==5 && !strcmp(argv[4],"--vdmredir-call")) {
        send_keys("REDIR.EXE\r"); Sleep(1500); send_keys("VDMPCAL.COM\r"); Sleep(3000); send_keys("mem\r");Sleep(2000);send_keys("exit\r");
        DWORD pipe_wait=WaitForSingleObject(pi.hProcess,5000),pipe_code=0; GetExitCodeProcess(pi.hProcess,&pipe_code);
        printf("call connected=%ld received=%ld sent=%ld wait=%lu exit=%lu\n",named_pipe_connected,named_pipe_received,named_pipe_sent,pipe_wait,pipe_code); fflush(stdout);
        CloseHandle(job);ClosePseudoConsole(pty);CloseHandle(write_pipe);WaitForSingleObject(thread,3000);CloseHandle(raw_log);CloseHandle(named_pipe_server);
        return pipe_wait==WAIT_OBJECT_0 && pipe_code==1 && named_pipe_connected && named_pipe_received && named_pipe_sent &&
            log_contains(argv[3],"CALL-OK") && log_contains(argv[3],"bytes total conventional memory")?0:1;
    }
    if(argc==5 && !strcmp(argv[4],"--vdmredir-async")) {
        send_keys("REDIR.EXE\r"); Sleep(1500); send_keys("VDMPASY.COM\r"); Sleep(5000); send_keys("mem\r");Sleep(2000);send_keys("exit\r");
        DWORD pipe_wait=WaitForSingleObject(pi.hProcess,5000),pipe_code=0; GetExitCodeProcess(pi.hProcess,&pipe_code);
        printf("async connected=%ld sent=%ld wait=%lu exit=%lu\n",named_pipe_connected,named_pipe_sent,pipe_wait,pipe_code); fflush(stdout);
        CloseHandle(job);ClosePseudoConsole(pty);CloseHandle(write_pipe);WaitForSingleObject(thread,3000);CloseHandle(raw_log);CloseHandle(named_pipe_server);
        return pipe_wait==WAIT_OBJECT_0 && pipe_code==1 && named_pipe_connected && named_pipe_sent &&
            log_contains(argv[3],"ASYNC-OK") && log_contains(argv[3],"bytes total conventional memory")?0:1;
    }
    if(argc==5 && !strcmp(argv[4],"--vdmredir-async-write")) {
        send_keys("REDIR.EXE\r"); Sleep(1500); send_keys("VDMPASW.COM\r"); Sleep(5000); send_keys("mem\r");Sleep(2000);send_keys("exit\r");
        DWORD pipe_wait=WaitForSingleObject(pi.hProcess,5000),pipe_code=0; GetExitCodeProcess(pi.hProcess,&pipe_code);
        printf("async-write connected=%ld received=%ld wait=%lu exit=%lu\n",named_pipe_connected,named_pipe_received,pipe_wait,pipe_code); fflush(stdout);
        CloseHandle(job);ClosePseudoConsole(pty);CloseHandle(write_pipe);WaitForSingleObject(thread,3000);CloseHandle(raw_log);CloseHandle(named_pipe_server);
        return pipe_wait==WAIT_OBJECT_0 && pipe_code==1 && named_pipe_connected && named_pipe_received &&
            log_contains(argv[3],"ASYNC-WRITE-OK") && log_contains(argv[3],"bytes total conventional memory")?0:1;
    }
    if(argc==5 && !strcmp(argv[4],"--vdmredir-mailslot")) {
        send_keys("REDIR.EXE\r"); Sleep(1500); send_keys("VDMMAIL.COM\r"); Sleep(3000); send_keys("mem\r");Sleep(2000);send_keys("exit\r");
        DWORD pipe_wait=WaitForSingleObject(pi.hProcess,5000),pipe_code=0; GetExitCodeProcess(pi.hProcess,&pipe_code);
        printf("mailslot wait=%lu exit=%lu\n",pipe_wait,pipe_code); fflush(stdout);
        CloseHandle(job);ClosePseudoConsole(pty);CloseHandle(write_pipe);WaitForSingleObject(thread,3000);CloseHandle(raw_log);
        return pipe_wait==WAIT_OBJECT_0 && pipe_code==1 && log_contains(argv[3],"MAILSLOT-OK") &&
            log_contains(argv[3],"bytes total conventional memory")?0:1;
    }
    if(argc==5 && !strcmp(argv[4],"--vdmredir-terminate")) {
        send_keys("REDIR.EXE\r"); Sleep(1500); send_keys("VDMTRM1.COM\r"); Sleep(2000); send_keys("VDMTRM2.COM\r"); Sleep(2000);
        send_keys("mem\r");Sleep(2000);send_keys("exit\r");
        { DWORD pipe_wait=WaitForSingleObject(pi.hProcess,5000),pipe_code=0; GetExitCodeProcess(pi.hProcess,&pipe_code);
          Sleep(300); CloseHandle(write_pipe); WaitForSingleObject(thread,3000); CloseHandle(raw_log);
          int passed=pipe_wait==WAIT_OBJECT_0 && pipe_code==1 && log_contains(argv[3],"TERM-OWNER-EXIT-OK") &&
            log_contains(argv[3],"TERM-CLEANUP-OK") && log_contains(argv[3],"bytes total conventional memory");
          CloseHandle(job);ClosePseudoConsole(pty);return passed?0:1; }
    }
    if(argc==5 && !strcmp(argv[4],"--vdmredir-netbios")) {
        send_keys("REDIR.EXE\r"); Sleep(1500); send_keys("VDMNETB.COM\r"); Sleep(3000); send_keys("mem\r");Sleep(2000);send_keys("exit\r");
        { DWORD pipe_wait=WaitForSingleObject(pi.hProcess,5000),pipe_code=0; GetExitCodeProcess(pi.hProcess,&pipe_code);
          Sleep(300); CloseHandle(write_pipe); WaitForSingleObject(thread,3000); CloseHandle(raw_log);
          int passed=pipe_wait==WAIT_OBJECT_0 && pipe_code==1 && log_contains(argv[3],"NETBIOS-REAL-RESULT=") &&
            log_contains(argv[3],"bytes total conventional memory");
          CloseHandle(job);ClosePseudoConsole(pty);return passed?0:1; }
    }
    if(argc==5 && !strcmp(argv[4],"--vdmredir-netbios-async")) {
        send_keys("REDIR.EXE\r"); Sleep(1500); send_keys("VDMNETA.COM\r"); Sleep(5000); send_keys("mem\r");Sleep(2000);send_keys("exit\r");
        { DWORD pipe_wait=WaitForSingleObject(pi.hProcess,5000),pipe_code=0; GetExitCodeProcess(pi.hProcess,&pipe_code);
          Sleep(300); CloseHandle(write_pipe); WaitForSingleObject(thread,3000); CloseHandle(raw_log);
          int passed=pipe_wait==WAIT_OBJECT_0 && pipe_code==1 && log_contains(argv[3],"NETBIOS-ASYNC-POST-OK") &&
            log_contains(argv[3],"NETBIOS-ASYNC-STATUS AL=") &&
            log_contains(argv[3],"bytes total conventional memory");
          CloseHandle(job);ClosePseudoConsole(pty);return passed?0:1; }
    }
    if(argc==5 && !strcmp(argv[4],"--vdmredir-dlc")) {
        send_keys("REDIR.EXE\r"); Sleep(1500); send_keys("VDMDLC.COM\r"); Sleep(3000); send_keys("mem\r");Sleep(2000);send_keys("exit\r");
        { DWORD pipe_wait=WaitForSingleObject(pi.hProcess,5000),pipe_code=0; GetExitCodeProcess(pi.hProcess,&pipe_code);
          Sleep(300); CloseHandle(write_pipe); WaitForSingleObject(thread,3000); CloseHandle(raw_log);
          int passed=pipe_wait==WAIT_OBJECT_0 && pipe_code==1 && log_contains(argv[3],"DLC-REAL-RESULT=") &&
            log_contains(argv[3],"bytes total conventional memory");
          CloseHandle(job);ClosePseudoConsole(pty);return passed?0:1; }
    }
    if(argc==5 && !strcmp(argv[4],"--vdmredir-netapi")) {
        send_keys("REDIR.EXE\r"); Sleep(1500); send_keys("VDMNETAP.COM\r"); Sleep(3000); send_keys("mem\r");Sleep(2000);send_keys("exit\r");
        DWORD pipe_wait=WaitForSingleObject(pi.hProcess,5000),pipe_code=0; GetExitCodeProcess(pi.hProcess,&pipe_code);
        /* Let the ConPTY reader drain the child's final marker before the
         * bounded raw-log assertion below. */
        Sleep(300);
        printf("netapi wait=%lu exit=%lu\n",pipe_wait,pipe_code); fflush(stdout);
        CloseHandle(write_pipe);WaitForSingleObject(thread,3000);CloseHandle(raw_log);
        { int has_netapi=log_contains(argv[3],"NETAPI-OK");
          int has_mem=log_contains(argv[3],"bytes total conventional memory");
          int passed=pipe_wait==WAIT_OBJECT_0 && pipe_code==1 && has_netapi && has_mem;
          CloseHandle(job);ClosePseudoConsole(pty);return passed?0:1; }
    }
    if(argc==5 && !strcmp(argv[4],"--vdmredir-net-enum")) {
        send_keys("REDIR.EXE\r"); Sleep(1500); send_keys("VDMNEI.COM\r"); Sleep(3000); send_keys("mem\r");Sleep(2000);send_keys("exit\r");
        DWORD pipe_wait=WaitForSingleObject(pi.hProcess,5000),pipe_code=0; GetExitCodeProcess(pi.hProcess,&pipe_code);
        Sleep(300);
        printf("net-enum wait=%lu exit=%lu\n",pipe_wait,pipe_code); fflush(stdout);
        CloseHandle(write_pipe);WaitForSingleObject(thread,3000);CloseHandle(raw_log);
        { int has_net_enum=log_contains(argv[3],"NETENUM-OK");
          int has_mem=log_contains(argv[3],"bytes total conventional memory");
          int passed=pipe_wait==WAIT_OBJECT_0 && pipe_code==1 && has_net_enum && has_mem;
          CloseHandle(job);ClosePseudoConsole(pty);return passed?0:1; }
    }
    if(argc==5 && !strcmp(argv[4],"--vdmredir-wksta")) {
        send_keys("REDIR.EXE\r"); Sleep(1500); send_keys("VDMWKSTA.COM\r"); Sleep(3000); send_keys("mem\r");Sleep(2000);send_keys("exit\r");
        DWORD pipe_wait=WaitForSingleObject(pi.hProcess,5000),pipe_code=0; GetExitCodeProcess(pi.hProcess,&pipe_code);
        Sleep(300);
        printf("wksta wait=%lu exit=%lu\n",pipe_wait,pipe_code); fflush(stdout);
        CloseHandle(write_pipe);WaitForSingleObject(thread,3000);CloseHandle(raw_log);
        { int has_wksta=log_contains(argv[3],"NETWKSTA-OK");
          int has_mem=log_contains(argv[3],"bytes total conventional memory");
          int passed=pipe_wait==WAIT_OBJECT_0 && pipe_code==1 && has_wksta && has_mem;
          CloseHandle(job);ClosePseudoConsole(pty);return passed?0:1; }
    }
    if(argc==5 && !strcmp(argv[4],"--vdmredir-wksta-set")) {
        send_keys("REDIR.EXE\r"); Sleep(1500); send_keys("VDMWKSS.COM\r"); Sleep(3000); send_keys("mem\r");Sleep(2000);send_keys("exit\r");
        { DWORD pipe_wait=WaitForSingleObject(pi.hProcess,5000),pipe_code=0; GetExitCodeProcess(pi.hProcess,&pipe_code);
          Sleep(300); CloseHandle(write_pipe); WaitForSingleObject(thread,3000); CloseHandle(raw_log);
          int passed=pipe_wait==WAIT_OBJECT_0 && pipe_code==1 && log_contains(argv[3],"NETWKSTA-SET-ORIGINAL-OK") && log_contains(argv[3],"bytes total conventional memory");
          CloseHandle(job);ClosePseudoConsole(pty);return passed?0:1; }
    }
    if(argc==5 && !strcmp(argv[4],"--vdmredir-message")) {
        send_keys("REDIR.EXE\r"); Sleep(1500); send_keys("VDMMSG.COM\r"); Sleep(3000); send_keys("mem\r");Sleep(2000);send_keys("exit\r");
        { DWORD pipe_wait=WaitForSingleObject(pi.hProcess,5000),pipe_code=0; GetExitCodeProcess(pi.hProcess,&pipe_code);
          Sleep(300); CloseHandle(write_pipe); WaitForSingleObject(thread,3000); CloseHandle(raw_log);
          int passed=pipe_wait==WAIT_OBJECT_0 && pipe_code==1 && log_contains(argv[3],"NETMSG-REAL-ERROR-OK") && log_contains(argv[3],"bytes total conventional memory");
          CloseHandle(job);ClosePseudoConsole(pty);return passed?0:1; }
    }
    if(argc==5 && !strcmp(argv[4],"--vdmredir-service")) {
        send_keys("REDIR.EXE\r"); Sleep(1500); send_keys("VDMSVC.COM\r"); Sleep(3000); send_keys("mem\r");Sleep(2000);send_keys("exit\r");
        { DWORD pipe_wait=WaitForSingleObject(pi.hProcess,5000),pipe_code=0; GetExitCodeProcess(pi.hProcess,&pipe_code);
          Sleep(300); CloseHandle(write_pipe); WaitForSingleObject(thread,3000); CloseHandle(raw_log);
          int passed=pipe_wait==WAIT_OBJECT_0 && pipe_code==1 && log_contains(argv[3],"NETSVC-REAL-RESULT-OK") && log_contains(argv[3],"bytes total conventional memory");
          CloseHandle(job);ClosePseudoConsole(pty);return passed?0:1; }
    }
    if(argc==5 && !strcmp(argv[4],"--vdmredir-assign")) {
        send_keys("REDIR.EXE\r"); Sleep(1500); send_keys("VDMASGN.COM\r"); Sleep(4000); send_keys("mem\r");Sleep(2000);send_keys("exit\r");
        { DWORD pipe_wait=WaitForSingleObject(pi.hProcess,5000),pipe_code=0; GetExitCodeProcess(pi.hProcess,&pipe_code);
          Sleep(300); CloseHandle(write_pipe); WaitForSingleObject(thread,3000); CloseHandle(raw_log);
          int passed=pipe_wait==WAIT_OBJECT_0 && pipe_code==1 && log_contains(argv[3],"ASSIGN-MACRO-LIFECYCLE-OK") && log_contains(argv[3],"bytes total conventional memory");
          CloseHandle(job);ClosePseudoConsole(pty);return passed?0:1; }
    }
    if(argc==5 && !strcmp(argv[4],"--vdmredir-use")) {
        send_keys("REDIR.EXE\r"); Sleep(1500); send_keys("VDMUSE.COM\r"); Sleep(3000); send_keys("mem\r");Sleep(2000);send_keys("exit\r");
        DWORD pipe_wait=WaitForSingleObject(pi.hProcess,5000),pipe_code=0; GetExitCodeProcess(pi.hProcess,&pipe_code);
        Sleep(300);
        printf("use wait=%lu exit=%lu\n",pipe_wait,pipe_code); fflush(stdout);
        CloseHandle(write_pipe);WaitForSingleObject(thread,3000);CloseHandle(raw_log);
        { int has_use=log_contains(argv[3],"NETUSE-ENUM-OK");
          int has_mem=log_contains(argv[3],"bytes total conventional memory");
          int passed=pipe_wait==WAIT_OBJECT_0 && pipe_code==1 && has_use && has_mem;
          CloseHandle(job);ClosePseudoConsole(pty);return passed?0:1; }
    }
    if(argc==5 && !strcmp(argv[4],"--vdmredir-timeout")) {
        send_keys("REDIR.EXE\r"); Sleep(1500); send_keys("VDMPTMO.COM\r"); Sleep(3000); send_keys("mem\r");Sleep(2000);send_keys("exit\r");
        { DWORD pipe_wait=WaitForSingleObject(pi.hProcess,5000),pipe_code=0; GetExitCodeProcess(pi.hProcess,&pipe_code);
          Sleep(300); CloseHandle(write_pipe); WaitForSingleObject(thread,3000); CloseHandle(raw_log);
          int passed=pipe_wait==WAIT_OBJECT_0 && pipe_code==1 && log_contains(argv[3],"PIPE-TIMEOUT-OK") && log_contains(argv[3],"bytes total conventional memory");
          CloseHandle(job);ClosePseudoConsole(pty);return passed?0:1; }
    }
    if(argc==5 && !strcmp(argv[4],"--vdmredir-use-info")) {
        send_keys("REDIR.EXE\r"); Sleep(1500); send_keys("VDMUSI.COM\r"); Sleep(3000); send_keys("mem\r");Sleep(2000);send_keys("exit\r");
        { DWORD pipe_wait=WaitForSingleObject(pi.hProcess,5000),pipe_code=0; GetExitCodeProcess(pi.hProcess,&pipe_code);
          Sleep(300); CloseHandle(write_pipe); WaitForSingleObject(thread,3000); CloseHandle(raw_log);
          int passed=pipe_wait==WAIT_OBJECT_0 && pipe_code==1 && log_contains(argv[3],"NETUSE-INFO-OK") && log_contains(argv[3],"bytes total conventional memory");
          CloseHandle(job);ClosePseudoConsole(pty);return passed?0:1; }
    }
    if(argc==5 && !strcmp(argv[4],"--vdmredir-use-lifecycle")) {
        send_keys("REDIR.EXE\r"); Sleep(1500); send_keys("VDMUSL.COM\r"); Sleep(4000); send_keys("mem\r");Sleep(2000);send_keys("exit\r");
        { DWORD pipe_wait=WaitForSingleObject(pi.hProcess,5000),pipe_code=0; GetExitCodeProcess(pi.hProcess,&pipe_code);
          Sleep(300); CloseHandle(write_pipe); WaitForSingleObject(thread,3000); CloseHandle(raw_log);
          int passed=pipe_wait==WAIT_OBJECT_0 && pipe_code==1 && log_contains(argv[3],"NETUSE-LIFECYCLE-OK") && log_contains(argv[3],"bytes total conventional memory");
          CloseHandle(job);ClosePseudoConsole(pty);return passed?0:1; }
    }
    send_keys("mem\r");Sleep(2500);
    send_keys("edit\r");Sleep(3500);send_keys("\x1b");Sleep(500);
    if(argc==5 && !strcmp(argv[4],"--resize")) {
        COORD narrow={45,10}, wide={120,30};
        if(FAILED(ResizePseudoConsole(pty,narrow)))return 67; Sleep(400);
        if(FAILED(ResizePseudoConsole(pty,wide)))return 67; Sleep(400);
        if(FAILED(ResizePseudoConsole(pty,size)))return 67; Sleep(400);
    }
    if(argc==5 && !strcmp(argv[4],"--mouse")) {
        send_keys("\x1b[<35;20;10M");Sleep(250);
        send_keys("\x1b[<0;20;10M");Sleep(250);
        send_keys("\x1b[<0;20;10m");Sleep(250);
    }
    send_keys("\x1b" "f");Sleep(500);send_keys("x");Sleep(2000);
    send_keys("mem\r");Sleep(2500);send_keys("mem\r");Sleep(2500);
    send_keys("exit\r");
    DWORD wait=WaitForSingleObject(pi.hProcess,5000),code=0;GetExitCodeProcess(pi.hProcess,&code);
    printf("wait=%lu exit=%lu\n",wait,code);fflush(stdout);
    CloseHandle(job);ClosePseudoConsole(pty);CloseHandle(write_pipe);
    WaitForSingleObject(thread,3000);CloseHandle(raw_log);
    return wait==WAIT_OBJECT_0 && code==1?0:1;
}
