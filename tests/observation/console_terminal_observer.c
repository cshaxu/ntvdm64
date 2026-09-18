/* Bounded real-ConPTY COMMAND -> MEM -> EDIT -> MEM twice regression.
 * The Job owns only the test process tree; native Console cell snapshots
 * distinguish renderer replay defects from upstream buffer corruption.
 * Usage: observer <columns> <rows> O:\\winnt\\logs\\<capture>.raw
 */
#define _WIN32_WINNT 0x0A00
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static HANDLE read_pipe, write_pipe, raw_log;
static HANDLE named_pipe_server;
static volatile LONG named_pipe_connected, named_pipe_sent, named_pipe_received;
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
static DWORD WINAPI copy_output(void *unused) {
    char data[8193]; DWORD n,w;
    (void)unused;
    while(ReadFile(read_pipe,data,sizeof(data)-1,&n,NULL)&&n) {
        data[n]=0;
        WriteFile(raw_log,data,n,&w,NULL);
        for(DWORD i=0;i+3<n;i++)if(!memcmp(data+i,"\x1b[6n",4))WriteFile(write_pipe,"\x1b[1;1R",6,&w,NULL);
    }
    return 0;
}
static void send_keys(const char *s) {DWORD w=0;if(!WriteFile(write_pipe,s,(DWORD)strlen(s),&w,NULL)||w!=strlen(s))fprintf(stderr,"console input write failed: %lu/%lu\n",GetLastError(),(unsigned long)w);}

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

static int guest_command_failed(const char *path) {
    return log_contains(path,"Bad command or filename") ||
        log_contains(path,"is not recognized as an internal or external command");
}

static DWORD WINAPI watch_cells(void *unused) {
    HANDLE h=CreateFileA("CONOUT$",GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
    (void)unused;
    char path[MAX_PATH];GetEnvironmentVariableA("TEST_CELL_LOG",path,sizeof(path));
    FILE *f=fopen(path,"w");if(!f)return 1;
    DWORD previous=0,begin=GetTickCount();WCHAR data[16000];
    while(GetTickCount()-begin<22000) {
        CONSOLE_SCREEN_BUFFER_INFO info;DWORD count=0,hash=2166136261u,mode=0;
        if(GetConsoleScreenBufferInfo(h,&info)) {
            DWORD total=(DWORD)info.dwSize.X*info.dwSize.Y;if(total>16000)total=16000;
            COORD origin={0,0};ReadConsoleOutputCharacterW(h,data,total,origin,&count);
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
    }fclose(f);CloseHandle(h);return 0;
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
        char runtime[MAX_PATH]="O:\\winnt",cmd[MAX_PATH+32];
        GetEnvironmentVariableA("TEST_RUNTIME_ROOT",runtime,sizeof(runtime));
        snprintf(cmd,sizeof(cmd),"%s\\run16.exe COMMAND.COM",runtime);
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
    COORD size;
    if(argc!=4 && (argc!=5 || (strcmp(argv[4],"--mouse") && strcmp(argv[4],"--resize") && strcmp(argv[4],"--video-int10") && strcmp(argv[4],"--vdmredir-pipe") && strcmp(argv[4],"--vdmredir-transact") && strcmp(argv[4],"--vdmredir-call") && strcmp(argv[4],"--vdmredir-timeout") && strcmp(argv[4],"--vdmredir-async") && strcmp(argv[4],"--vdmredir-async-write") && strcmp(argv[4],"--vdmredir-mailslot") && strcmp(argv[4],"--vdmredir-terminate") && strcmp(argv[4],"--vdmredir-netbios") && strcmp(argv[4],"--vdmredir-netbios-async") && strcmp(argv[4],"--vdmredir-dlc") && strcmp(argv[4],"--vdmredir-netapi") && strcmp(argv[4],"--vdmredir-net-enum") && strcmp(argv[4],"--vdmredir-wksta") && strcmp(argv[4],"--vdmredir-wksta-set") && strcmp(argv[4],"--vdmredir-message") && strcmp(argv[4],"--vdmredir-service") && strcmp(argv[4],"--vdmredir-assign") && strcmp(argv[4],"--vdmredir-use") && strcmp(argv[4],"--vdmredir-use-info") && strcmp(argv[4],"--vdmredir-use-lifecycle"))))return 64;
    size.X=(SHORT)atoi(argv[1]);size.Y=(SHORT)atoi(argv[2]);
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
    CreatePipe(&in_read,&write_pipe,NULL,0);CreatePipe(&read_pipe,&out_write,NULL,0);
    if(FAILED(CreatePseudoConsole(size,in_read,out_write,0,&pty)))return 65;
    CloseHandle(in_read);CloseHandle(out_write);
    thread=CreateThread(NULL,0,copy_output,NULL,0,NULL);
    InitializeProcThreadAttributeList(NULL,1,0,&bytes);
    si.lpAttributeList=HeapAlloc(GetProcessHeap(),0,bytes);
    InitializeProcThreadAttributeList(si.lpAttributeList,1,0,&bytes);
    UpdateProcThreadAttribute(si.lpAttributeList,0,PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE,pty,sizeof(pty),NULL,NULL);
    si.StartupInfo.cb=sizeof(si);
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
