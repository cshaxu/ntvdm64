/* Read-only diagnostic of the selected x86 NTVDM's private palette cache.
 * Layout prefix is copied from console_client.c; not a production ABI. */
#include <windows.h>
#include <tlhelp32.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "ntvdm-exe/session/session.h"
#include "common/console/client.h"
typedef struct palette_prefix {
    session *owner;
    ntcon_worker_client channel;
    HANDLE ready,stop,shutdown;
    void *input_watch;
    HANDLE capability,input_identity,output_identity;
    CRITICAL_SECTION lock,palette_lock;
    void *publisher;
    console_io_request request;
    void *graphics;
    PALETTEENTRY palette[16];
    BOOL valid;
} palette_prefix;
static int read_exact(HANDLE process,uintptr_t address,void *out,SIZE_T size) {
    SIZE_T got=0;
    return ReadProcessMemory(process,(void *)address,out,size,&got) && got==size;
}
int main(int argc,char **argv) {
    DWORD pid;HANDLE process,snapshot;MODULEENTRY32 module;
    char path[32768];DWORD path_bytes=sizeof(path);uintptr_t base,client;
    palette_prefix prefix;DWORD mode,palette;
    if(argc!=3)return 2;
    pid=strtoul(argv[1],NULL,10);
    process=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION|PROCESS_VM_READ,FALSE,pid);
    if(!process)return 3;
    if(!QueryFullProcessImageNameA(process,0,path,&path_bytes) || _stricmp(path,argv[2])) {
        CloseHandle(process);return 4;
    }
    snapshot=CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,pid);
    module.dwSize=sizeof(module);
    if(snapshot==INVALID_HANDLE_VALUE || !Module32First(snapshot,&module)) {
        if(snapshot!=INVALID_HANDLE_VALUE)CloseHandle(snapshot);
        CloseHandle(process);return 5;
    }
    base=(uintptr_t)module.modBaseAddr;CloseHandle(snapshot);
    /* Exact T434 r001 MAP: worker_session=808830, sc=8600C0, base=400000.
     * sc offsets confirmed from original struct and final-paint instructions. */
    if(!read_exact(process,base+0x408830+offsetof(session,console_client),&client,sizeof(client)) ||
       !client || !read_exact(process,client,&prefix,sizeof(prefix)) ||
       !read_exact(process,base+0x4600C0+0x34,&mode,sizeof(mode)) ||
       !read_exact(process,base+0x4600C0+0x48,&palette,sizeof(palette))) {
        CloseHandle(process);return 6;
    }
    printf("pid=%lu base=%08lx client=%08lx session-offset=%lu valid-offset=%lu mode=%lu palette=%08lx cached=%ld\n",
        pid,(unsigned long)base,(unsigned long)client,(unsigned long)offsetof(session,console_client),
        (unsigned long)offsetof(palette_prefix,valid),mode,palette,prefix.valid);
    CloseHandle(process);return 0;
}
