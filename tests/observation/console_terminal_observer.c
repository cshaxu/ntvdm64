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
static DWORD WINAPI copy_output(void *unused) {
    char data[8193]; DWORD n,w;
    while(ReadFile(read_pipe,data,sizeof(data)-1,&n,NULL)&&n) {
        data[n]=0;
        WriteFile(raw_log,data,n,&w,NULL);
        for(DWORD i=0;i+3<n;i++)if(!memcmp(data+i,"\x1b[6n",4))WriteFile(write_pipe,"\x1b[1;1R",6,&w,NULL);
    }
    return 0;
}
static void send_keys(const char *s) {DWORD w;WriteFile(write_pipe,s,(DWORD)strlen(s),&w,NULL);}

static DWORD WINAPI watch_cells(void *unused) {
    HANDLE h=CreateFileA("CONOUT$",GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
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
        STARTUPINFOA startup={sizeof(startup)};PROCESS_INFORMATION child={0};char cmd[]="O:\\winnt\\run16.exe COMMAND.COM";
        startup.dwFlags=STARTF_USESTDHANDLES;startup.hStdInput=ci;startup.hStdOutput=startup.hStdError=co;
        if(!CreateProcessA(NULL,cmd,NULL,NULL,TRUE,0,NULL,"O:\\winnt",&startup,&child))return 66;
        WaitForSingleObject(child.hProcess,INFINITE);GetExitCodeProcess(child.hProcess,&n);Sleep(400);return n;
    }
    HANDLE in_read,out_write,thread,job;
    HPCON pty;
    STARTUPINFOEXA si={0};PROCESS_INFORMATION pi={0};SIZE_T bytes=0;
    COORD size;
    if(argc!=4 && (argc!=5 || strcmp(argv[4],"--mouse")))return 64;
    size.X=(SHORT)atoi(argv[1]);size.Y=(SHORT)atoi(argv[2]);
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION limits={0};
    char executable[MAX_PATH], command[MAX_PATH+16];
    GetModuleFileNameA(NULL,executable,sizeof(executable));
    snprintf(command,sizeof(command),"\"%s\" --child",executable);
    if(argc==5) { char path[MAX_PATH];snprintf(path,sizeof(path),"%s.mouse.txt",argv[3]);SetEnvironmentVariableA("MVDM_CONSOLE_PRESENTATION_REPORT_PATH",path); }
    { char path[MAX_PATH]; snprintf(path,sizeof(path),"%s.rpc.txt",argv[3]);SetEnvironmentVariableA("MVDM_BASESRV_TRACE_PATH",path);snprintf(path,sizeof(path),"%s.stream.txt",argv[3]);SetEnvironmentVariableA("MVDM_STREAM_IO_REPORT_PATH",path); }
    { char path[MAX_PATH];snprintf(path,sizeof(path),"%s.cells.txt",argv[3]);SetEnvironmentVariableA("TEST_CELL_LOG",path); }
    raw_log=CreateFileA(argv[3],GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,0,NULL);
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
    Sleep(3000);
    send_keys("mem\r");Sleep(2500);
    send_keys("edit\r");Sleep(3500);send_keys("\x1b");Sleep(500);
    if(argc==5) {
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
