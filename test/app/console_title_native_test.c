/* Native Console title contract evidence. Run only on the private desktop.
 * No product provider or guest code is substituted by this fixture. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"FAIL %d error=%lu\n",__LINE__,GetLastError()); return 1; } } while (0)
int main(int argc,char **argv)
{
    char saved[1024]={0},buffer[32];
    WCHAR image[MAX_PATH],command[MAX_PATH+32];
    STARTUPINFOW startup={sizeof(startup)};
    PROCESS_INFORMATION child={0};
    DWORD capacities[]={0,1,4,32},i,count,error,exit_code;
    if (argc==2 && !strcmp(argv[1],"--child")) {
        CHECK(SetConsoleTitleA("TITLE-CHILD"));
        return 0;
    }
    CHECK(argc==1);
    GetConsoleTitleA(saved,sizeof(saved));
    CHECK(SetConsoleTitleA("TITLE-PARENT"));
    for (i=0;i<sizeof(capacities)/sizeof(capacities[0]);++i) {
        memset(buffer,0x55,sizeof(buffer));
        SetLastError(0);
        count=GetConsoleTitleA(buffer,capacities[i]);error=GetLastError();
        printf("capacity=%lu result=%lu error=%lu first=%02x last=%02x\n",
            capacities[i],count,error,(unsigned char)buffer[0],
            capacities[i] ? (unsigned char)buffer[capacities[i]-1] : 0);
        CHECK(!capacities[i] || memchr(buffer,0,capacities[i]));
    }
    CHECK(GetModuleFileNameW(NULL,image,MAX_PATH));
    CHECK(swprintf_s(command,MAX_PATH+32,L"\"%s\" --child",image)>0);
    CHECK(CreateProcessW(NULL,command,NULL,NULL,FALSE,0,NULL,NULL,&startup,&child));
    CloseHandle(child.hThread);
    if (WaitForSingleObject(child.hProcess,10000)!=WAIT_OBJECT_0) {
        TerminateProcess(child.hProcess,99);CloseHandle(child.hProcess);
        SetConsoleTitleA(saved);return 2;
    }
    CHECK(GetExitCodeProcess(child.hProcess,&exit_code));CloseHandle(child.hProcess);
    CHECK(!exit_code);
    count=GetConsoleTitleA(buffer,sizeof(buffer));
    printf("after-child-exit result=%lu title=%s\n",count,buffer);
    CHECK(SetConsoleTitleA(""));
    memset(buffer,0x55,sizeof(buffer));SetLastError(0);
    count=GetConsoleTitleA(buffer,sizeof(buffer));error=GetLastError();
    printf("empty result=%lu error=%lu first=%02x\n",count,error,(unsigned char)buffer[0]);
    CHECK(SetConsoleTitleA(saved));
    puts("PASS native title observations; baseline title restored");
    return 0;
}
