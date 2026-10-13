#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "nthook32-dll/hook.h"
#include "opennt-abi/host-compat/include/command_process_compat.h"

/* Only the incoming DOS task/context is supplied by this fixture. Child
 * creation, suspension, payload installation and decoding are production code. */
static DWORD current_style;
static BOOL has_capture=TRUE;
int mvdm_command_native_child_window_style(DWORD *style)
{ if(!has_capture)return 0;*style=current_style;return 1; }
BOOL ntvdm_console_inherit_launch_capabilities(HANDLE *root,HANDLE *execution)
{ *root=NULL;*execution=NULL;return TRUE; }
DWORD ntvdm_console_handle_kind(HANDLE value) { (void)value;return 0; }
unsigned int mvdm_command_native_child_command_service(const char **tail)
{ *tail=NULL;return 0; }

#define CHECK(x) do { if(!(x)){printf("FAIL %d error=%lu\n",__LINE__,GetLastError());return 1;} } while(0)
int main(int argc,char **argv)
{
    char path[MAX_PATH],command[MAX_PATH+32];
    STARTUPINFOA startup={sizeof(startup)};
    PROCESS_INFORMATION process={0};
    DWORD suspended,code;
    if(argc==2 && !strcmp(argv[1],"--child")) {
        nthook_context context;BOOL found=FALSE;
        CHECK(!nthook_context_read(&context,&found) && found);
        CHECK(context.mode==NATIVE_HOOK_LAUNCHER);
        CHECK(!context.frontend && !context.execution);
        CHECK(!GetModuleHandleW(L"nthook32.dll") && !GetModuleHandleW(L"nthook64.dll"));
        return (int)(100+context.window_style);
    }
    CHECK(GetModuleFileNameA(NULL,path,sizeof(path)));
    for(current_style=WINDOW_STYLE_OFF;current_style<=WINDOW_STYLE_ALL;++current_style)
        for(suspended=0;suspended<2;++suspended) {
            sprintf_s(command,sizeof(command),"\"%s\" --child",path);
            CHECK(opennt_command_create_process_a(path,command,NULL,NULL,FALSE,
                CREATE_NO_WINDOW|(suspended?CREATE_SUSPENDED:0),NULL,NULL,&startup,&process));
            if(suspended) {
                CHECK(WaitForSingleObject(process.hProcess,0)==WAIT_TIMEOUT);
                CHECK(ResumeThread(process.hThread)==1);
            }
            CHECK(WaitForSingleObject(process.hProcess,10000)==WAIT_OBJECT_0);
            CHECK(GetExitCodeProcess(process.hProcess,&code) && code==100+current_style);
            CloseHandle(process.hThread);CloseHandle(process.hProcess);
        }
    current_style=3;
    sprintf_s(command,sizeof(command),"\"%s\" --child",path);
    CHECK(!opennt_command_create_process_a(path,command,NULL,NULL,FALSE,
        CREATE_NO_WINDOW,NULL,NULL,&startup,&process));
    CHECK(GetLastError()==ERROR_INVALID_PARAMETER && !process.hProcess && !process.hThread);
    has_capture=FALSE;
    CHECK(!opennt_command_create_process_a(path,command,NULL,NULL,FALSE,
        CREATE_NO_WINDOW,NULL,NULL,&startup,&process));
    CHECK(GetLastError()==ERROR_INVALID_STATE && !process.hProcess && !process.hThread);
    puts("PASS DOS adapter launcher payload: three policies, caller suspension and invalid-policy rollback");
    return 0;
}
