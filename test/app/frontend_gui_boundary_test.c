/* GUI-subsystem witness without windows: it must not inherit frontend routes.
 * Run via an inner run16 from actual frontend-owned CMD, not a mocked broker. */
#include <windows.h>
#include <string.h>
int WINAPI WinMain(HINSTANCE instance,HINSTANCE previous,LPSTR command,int show)
{
    char value[64];DWORD written;
    const char *message="GUI-NO-FRONTEND-CAPABILITY\r\n";
    (void)instance;(void)previous;(void)command;(void)show;
    SetLastError(0);
    if(GetEnvironmentVariableA("NTVDM_FRONTEND_CAPABILITY",value,sizeof(value)) ||
        GetLastError()!=ERROR_ENVVAR_NOT_FOUND)return 90;
    SetLastError(0);
    if(GetEnvironmentVariableA("NTVDM_EXECUTION_CONSOLE",value,sizeof(value)) ||
        GetLastError()!=ERROR_ENVVAR_NOT_FOUND)return 91;
    if(!WriteFile(GetStdHandle(STD_OUTPUT_HANDLE),message,(DWORD)strlen(message),&written,NULL) ||
        written!=strlen(message))return 92;
    if(*command){
        STARTUPINFOA startup={sizeof(startup)};PROCESS_INFORMATION child={0};
        SECURITY_ATTRIBUTES security={sizeof(security),NULL,TRUE};
        HANDLE input=CreateFileA("NUL",GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,
            &security,OPEN_EXISTING,0,NULL);
        DWORD result;
        if(input==INVALID_HANDLE_VALUE)return 93;
        startup.dwFlags=STARTF_USESTDHANDLES;startup.hStdInput=input;
        startup.hStdOutput=GetStdHandle(STD_OUTPUT_HANDLE);startup.hStdError=startup.hStdOutput;
        if(!CreateProcessA(NULL,command,NULL,NULL,TRUE,0,NULL,NULL,&startup,&child)){
            CloseHandle(input);return 94;
        }
        CloseHandle(input);CloseHandle(child.hThread);
        if(WaitForSingleObject(child.hProcess,15000)!=WAIT_OBJECT_0){
            TerminateProcess(child.hProcess,ERROR_TIMEOUT);CloseHandle(child.hProcess);return 95;
        }
        if(!GetExitCodeProcess(child.hProcess,&result)){CloseHandle(child.hProcess);return 96;}
        CloseHandle(child.hProcess);return (int)result;
    }
    return 37;
}
