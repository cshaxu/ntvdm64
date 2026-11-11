/* Exercise the already-built witness DLL, including a real replacement
 * generation. Rearm the latest owned HWND; no fixed dead-slot promise. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
typedef DWORD (WINAPI *identity_fn)(DWORD);
int main(int argc,char **argv)
{
    HMODULE module=NULL;
    HWND anchor=NULL,old=NULL,next=NULL;
    FARPROC address;
    identity_fn invoke;
    DWORD full,request,started=GetTickCount(),attempt;
    int result=1;
    if(argc!=2 || !(module=LoadLibraryA(argv[1])))goto done;
    address=GetProcAddress(module,"CommonDialogIdentity");
    if(!address)goto done;
    CopyMemory(&invoke,&address,sizeof(invoke));
    anchor=CreateWindowExA(0,"STATIC","native identity lifetime",WS_POPUP,
        0,0,24,24,NULL,NULL,GetModuleHandle(NULL),NULL);
    if(!anchor)goto done;
    for(attempt=1;attempt<=128 && GetTickCount()-started<15000;++attempt){
        old=CreateWindowExA(0,"STATIC","old identity",WS_POPUP,
            0,0,24,24,NULL,NULL,GetModuleHandle(NULL),NULL);
        if(!old)goto done;
        full=(DWORD)old;request=LOWORD(full);
        if(invoke(0x10000|request)!=0x447a0001)goto done;
        if(invoke(0x20000|request)!=0 || invoke(0x30000|request)!=0 || !IsWindow(old))goto done;
        if(!DestroyWindow(old))goto done;
        old=NULL;
        next=CreateWindowExA(0,"STATIC","new identity",WS_POPUP,
            0,0,24,24,NULL,NULL,GetModuleHandle(NULL),NULL);
        if(!next)goto done;
        if(LOWORD((DWORD)next)==request && (DWORD)next!=full){
            if(IsWindow((HWND)full) || !IsWindow(next) || invoke(0x30000|request)!=0x447a0003)goto done;
            printf("COMMON_IDENTITY_REUSE attempts=%lu old=%08lX new=%08lX omitted_destroy=rejected stale_true=rejected new_generation=accepted\n",attempt,full,(DWORD)next);
            result=0;goto done;
        }
        if(!DestroyWindow(next))goto done;
        next=NULL;
    }
    puts("COMMON_IDENTITY_REUSE prerequisite not acquired; not a pass");
done:
    if(old)DestroyWindow(old);
    if(next)DestroyWindow(next);
    if(anchor)DestroyWindow(anchor);
    if(module)FreeLibrary(module);
    return result;
}
