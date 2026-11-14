/* Exercise the exact source-owned loader with native test DLL lifetimes.
 * Only host API failures and guest register storage are test seams. */
#include <windows.h>
#include <stdio.h>
#include <string.h>
typedef ULONG (*MYFARPROC)();
static const char *dll_path;
static unsigned load_calls, proc_calls, bad_close_calls, fail_export;
static int missing_library;
static LONG detached;
static USHORT guest_ip, guest_ax, guest_cf;
static unsigned failures, checks;
#define CHECK(x) do { ++checks; if(!(x)){++failures;printf("FAIL line=%d %s\n",__LINE__,#x);} }while(0)
static HMODULE fixture_load(const char *name)
{
    HMODULE module;
    void (__cdecl *configure)(LONG *);
    ++load_calls;
    if(strcmp(name,"VDMREDIR")) return NULL;
    module=LoadLibraryA(missing_library?"t478_s12_intentionally_absent.dll":dll_path);
    if(module) {
        configure=(void (__cdecl *)(LONG *))GetProcAddress(module,"Configure");
        if(!configure){FreeLibrary(module);return NULL;}
        configure(&detached);
    }
    return module;
}
static FARPROC WINAPI fixture_proc(HMODULE module,LPCSTR name)
{
    ++proc_calls;
    if(proc_calls==fail_export){SetLastError(ERROR_PROC_NOT_FOUND);return NULL;}
    return GetProcAddress(module,name);
}
static BOOL WINAPI fixture_close(HANDLE handle)
{
    ++bad_close_calls;return CloseHandle(handle);
}
static USHORT getCS(void){return 0;}
static USHORT getIP(void){return guest_ip;}
static USHORT getMSW(void){return 0;}
static USHORT getVM(void){return 0;}
static void setIP(USHORT value){guest_ip=value;}
static void setCF(USHORT value){guest_cf=value;}
static void setAX(USHORT value){guest_ax=value;}
static unsigned char *Sim32GetVDMPointer(unsigned long address,int count,int protected_mode)
{static unsigned char service; (void)address;(void)count;(void)protected_mode;return &service;}
#define SEGOFF(s,o) (((unsigned long)(s)<<16)|(o))
#define MSW_PE 1
#define SafeLoadLibrary fixture_load
#define GetProcAddress fixture_proc
#define CloseHandle fixture_close
#include "redirector_loader_selected.inc"
#undef GetProcAddress
#undef CloseHandle
static void reset_fixture(void)
{
    HMODULE module=GetModuleHandleA(dll_path);
    /* Release the baseline's leaked reference so each characterization is isolated. */
    if(module) FreeLibrary(module);
    hVdmRedir=NULL;VdmRedirLoaded=FALSE;
    VrDispatch=NULL;VrInitialized=DefaultVrInitialized;VrReadNamedPipe=NULL;
    VrWriteNamedPipe=NULL;VrIsNamedPipeName=NULL;VrIsNamedPipeHandle=NULL;
    VrAddOpenNamedPipeInfo=NULL;VrConvertLocalNtPipeName=NULL;VrRemoveOpenNamedPipeInfo=NULL;
    load_calls=proc_calls=bad_close_calls=0;fail_export=0;missing_library=0;detached=0;
}
int main(int argc,char **argv)
{
    unsigned i;
    if(argc!=2)return 2;dll_path=argv[1];
    reset_fixture();missing_library=1;
    CHECK(!LoadVdmRedir());CHECK(!IsVdmRedirLoaded());CHECK(!hVdmRedir);
    CHECK(load_calls==1 && proc_calls==0 && bad_close_calls==0);
    for(i=1;i<=9;++i){
        reset_fixture();fail_export=i;
        CHECK(!LoadVdmRedir());CHECK(!IsVdmRedirLoaded());CHECK(!hVdmRedir);
        CHECK(detached==1);CHECK(!GetModuleHandleA(dll_path));
        CHECK(load_calls==1 && proc_calls==i && bad_close_calls==0);
        fail_export=0;
        CHECK(LoadVdmRedir());CHECK(IsVdmRedirLoaded());
        CHECK(VrDispatch()==0 && VrInitialized()==0 && VrReadNamedPipe()==0 &&
            VrWriteNamedPipe()==0 && VrIsNamedPipeName()==0 && VrIsNamedPipeHandle()==0 &&
            VrAddOpenNamedPipeInfo()==0 && VrConvertLocalNtPipeName()==0 &&
            VrRemoveOpenNamedPipeInfo()==0);
    }
    reset_fixture();CHECK(LoadVdmRedir());CHECK(IsVdmRedirLoaded());CHECK(hVdmRedir!=NULL);
    CHECK(load_calls==1 && proc_calls==9 && detached==0 && bad_close_calls==0);
    CHECK(LoadVdmRedir());CHECK(load_calls==1 && proc_calls==9);
    reset_fixture();missing_library=1;guest_ip=100;
    MS_bop_7();CHECK(guest_cf==1 && guest_ax==ERROR_INVALID_FUNCTION && guest_ip==101);
    MS_bop_7();CHECK(guest_cf==1 && guest_ax==ERROR_INVALID_FUNCTION && guest_ip==102);
    CHECK(load_calls==1 && proc_calls==0);
    printf("REDIR_LOADER_RESULT checks=%u failures=%u missing_exports=9 native_unload=1 no_bop_retry=1\n",checks,failures);
    return failures?1:0;
}
