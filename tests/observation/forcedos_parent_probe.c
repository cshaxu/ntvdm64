/* Independent native caller for the supported, already-instrumented route. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
int wmain(int argc,wchar_t **argv)
{
    STARTUPINFOW si={sizeof(si)};PROCESS_INFORMATION pi={0};
    WCHAR command[2048];DWORD code=0,flags,wait;HANDLE output;
    SECURITY_ATTRIBUTES sa={sizeof(sa),NULL,TRUE};
    if(argc!=4 && argc!=5)return 90;
    flags=wcstoul(argv[2],NULL,0);
    output=CreateFileW(argv[3],GENERIC_WRITE,FILE_SHARE_READ,&sa,CREATE_ALWAYS,0,NULL);
    if(output==INVALID_HANDLE_VALUE)return 91;
    si.dwFlags=STARTF_USESTDHANDLES;si.hStdInput=GetStdHandle(STD_INPUT_HANDLE);
    si.hStdOutput=si.hStdError=output;
    BOOL sequence=argc==5 && !wcscmp(argv[4],L"sequence");
    for(unsigned step=0;step<(sequence?2u:1u);++step) {
        if(step)flags=0;
        swprintf_s(command,ARRAYSIZE(command),L"\"%ls\"",argv[1]);
        BOOL created;
        if(argc==5 && !wcscmp(argv[4],L"ansi")) {
            STARTUPINFOA ansi={sizeof(ansi)};
            char application_a[MAX_PATH],command_a[2048];
            if(!WideCharToMultiByte(CP_ACP,WC_NO_BEST_FIT_CHARS,argv[1],-1,
                application_a,sizeof(application_a),NULL,NULL) ||
               !WideCharToMultiByte(CP_ACP,WC_NO_BEST_FIT_CHARS,command,-1,
                command_a,sizeof(command_a),NULL,NULL)) {CloseHandle(output);return 95;}
            ansi.dwFlags=si.dwFlags;ansi.hStdInput=si.hStdInput;
            ansi.hStdOutput=ansi.hStdError=output;
            created=CreateProcessA(application_a,command_a,NULL,NULL,TRUE,flags,NULL,NULL,&ansi,&pi);
        } else created=CreateProcessW(argv[1],command,NULL,NULL,TRUE,flags,NULL,NULL,&si,&pi);
        if(!created) {
            code=GetLastError();printf("CREATE_ERROR=%lu\n",code);CloseHandle(output);return 92;
        }
        wait=WaitForSingleObject(pi.hProcess,20000);
        if(wait!=WAIT_OBJECT_0){TerminateProcess(pi.hProcess,93);code=93;}
        else if(!GetExitCodeProcess(pi.hProcess,&code))code=94;
        CloseHandle(pi.hThread);CloseHandle(pi.hProcess);
        printf("FORCEDOS flags=%08lx result=%lu wait=%lu\n",flags,code,wait);
        if(sequence && !step && code!=53){CloseHandle(output);return 96;}
    }
    CloseHandle(output);
    return (int)code;
}
