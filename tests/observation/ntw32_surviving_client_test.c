/* Real native target: the direct target exits before its Console child reads
 * input. No helper or product hook; this is the workload under observation. */
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

int wmain(int argc,WCHAR **argv)
{
    if(argc==3 && !wcscmp(argv[1],L"--child")) {
        HANDLE parent=(HANDLE)(ULONG_PTR)_wcstoui64(argv[2],NULL,10);
        HANDLE input=GetStdHandle(STD_INPUT_HANDLE),output=GetStdHandle(STD_OUTPUT_HANDLE);
        WCHAR text[64]={0};DWORD count,mode,code;
        const WCHAR prompt[]=L"\r\nNATIVE-PARENT-EXIT-37\r\nC:\\> ";
        const WCHAR witness[]=L"NATIVE-SURVIVOR-INPUT-OK\r\n";
        if(WaitForSingleObject(parent,10000)!=WAIT_OBJECT_0 ||
            !GetExitCodeProcess(parent,&code) || code!=37)return 2;
        CloseHandle(parent);
        if(!GetConsoleMode(input,&mode) || !SetConsoleMode(input,
            mode|ENABLE_LINE_INPUT|ENABLE_ECHO_INPUT|ENABLE_PROCESSED_INPUT))return 3;
        /* The ordinary observer waits for a prompt, then delivers real input.
         * This prompt can only appear after the direct target has exited. */
        if(!WriteConsoleW(output,prompt,ARRAYSIZE(prompt)-1,&count,NULL))return 4;
        if(!ReadConsoleW(input,text,63,&count,NULL))return 5;
        if(wcscmp(text,L"survivor\r\n"))return 6;
        if(!WriteConsoleW(output,witness,ARRAYSIZE(witness)-1,&count,NULL))return 7;
        return 19;
    }
    if(argc==1) {
        WCHAR image[MAX_PATH],command[MAX_PATH+80];HANDLE parent=NULL;
        STARTUPINFOW startup={sizeof(startup)};PROCESS_INFORMATION child={0};
        if(!GetModuleFileNameW(NULL,image,MAX_PATH) ||
            !DuplicateHandle(GetCurrentProcess(),GetCurrentProcess(),GetCurrentProcess(),
                &parent,SYNCHRONIZE|PROCESS_QUERY_LIMITED_INFORMATION,TRUE,0))return 8;
        swprintf_s(command,ARRAYSIZE(command),L"\"%ls\" --child %llu",image,
            (unsigned long long)(ULONG_PTR)parent);
        if(!CreateProcessW(image,command,NULL,NULL,TRUE,0,NULL,NULL,&startup,&child)){
            CloseHandle(parent);return 9;
        }
        CloseHandle(child.hThread);CloseHandle(child.hProcess);CloseHandle(parent);
        return 37;
    }
    return 10;
}
