#include "mvdm_softpc_firmware.h"
#include "common/system_root.h"
#include <windows.h>
#include <stdio.h>

int main(int argc,char **argv)
{
    WCHAR provider[MAX_PATH];
    CHAR cwd[MAX_PATH];
    HANDLE file=INVALID_HANDLE_VALUE;
    HMODULE module=NULL;
    DWORD written,error;
    BOOL entered=FALSE,created=FALSE;
    int result=1;
    if(argc!=2 || !GetCurrentDirectoryA(sizeof(cwd),cwd))return 10;
    if(common_product_path_w(L"system32\\WOW32.DLL",provider,ARRAYSIZE(provider)) ||
        GetFileAttributesW(provider)!=INVALID_FILE_ATTRIBUTES)return 11;
    if(!CreateDirectoryA(argv[1],NULL))return 12;
    if(!SetCurrentDirectoryA(argv[1]))goto done;
    entered=TRUE;
    file=CreateFileA("WOW32.DLL",GENERIC_WRITE,0,NULL,CREATE_NEW,0,NULL);
    if(file==INVALID_HANDLE_VALUE)goto done;
    created=TRUE;
    if(!WriteFile(file,"not-a-DLL",9,&written,NULL) || written!=9)goto done;
    CloseHandle(file);file=INVALID_HANDLE_VALUE;
    module=mvdm_softpc_load_library("WOW32");error=GetLastError();
    if(module || error!=ERROR_MOD_NOT_FOUND)goto done;
    module=mvdm_softpc_load_library("wow32.dll");error=GetLastError();
    if(module || error!=ERROR_MOD_NOT_FOUND)goto done;
    /* Arbitrary native provider still follows the original loader. */
    module=mvdm_softpc_load_library("kernel32.dll");
    if(!module)goto done;
    FreeLibrary(module);module=NULL;
    result=0;
done:
    if(module)FreeLibrary(module);
    if(file!=INVALID_HANDLE_VALUE)CloseHandle(file);
    if(created && !DeleteFileA("WOW32.DLL"))result=2;
    if(entered && !SetCurrentDirectoryA(cwd))return 3;
    if(!RemoveDirectoryA(argv[1]))return 4;
    if(!result)puts("PRODUCT-LOADER-ROOT-PASS");
    return result;
}
