#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include "common/native-hook/context.h"
static DWORD packet(BYTE *data,DWORD machine)
{
    native_hook_packet *h=(native_hook_packet *)data;
    static const WCHAR a[]=L"C:\\run16.exe",b[]=L"C:\\nthook32.dll",c[]=L"C:\\nthook64.dll";
    ZeroMemory(data,sizeof(*h)+sizeof(a)+sizeof(b)+sizeof(c));
    h->version=NATIVE_HOOK_VERSION;h->mode=NATIVE_HOOK_LAUNCHER;h->machine=machine;
    h->launcher_offset=sizeof(*h);h->launcher_bytes=sizeof(a);
    h->hook_offset=h->launcher_offset+h->launcher_bytes;h->hook_bytes=sizeof(b);
    h->hook64_offset=h->hook_offset+h->hook_bytes;h->hook64_bytes=sizeof(c);
    h->bytes=h->hook64_offset+h->hook64_bytes;
    memcpy(data+h->launcher_offset,a,sizeof(a));memcpy(data+h->hook_offset,b,sizeof(b));memcpy(data+h->hook64_offset,c,sizeof(c));
    return h->bytes;
}
int main(void)
{
    BYTE data[sizeof(native_hook_packet)+3*MAX_PATH*sizeof(WCHAR)];nthook_context result;
    DWORD machine=sizeof(void*)==8?IMAGE_FILE_MACHINE_AMD64:IMAGE_FILE_MACHINE_I386;
    for(unsigned mutation=0;mutation<11;++mutation) {
        DWORD bytes=packet(data,machine);native_hook_packet *h=(native_hook_packet *)data;
        switch(mutation) {
        case 1:++h->version;break;case 2:h->mode=99;break;case 3:h->flags=1;break;
        case 4:h->frontend=1;break;case 5:h->hook_offset=MAXDWORD;break;
        case 6:h->launcher_bytes=MAXDWORD;break;case 7:data[bytes-2]=1;break;
        case 8:bytes=sizeof(*h)-1;break;
        case 9:h->flags=NATIVE_HOOK_SEPARATE_WOW;h->mode=NATIVE_HOOK_INTERCEPT;break;
        case 10:h->machine=machine==IMAGE_FILE_MACHINE_I386?IMAGE_FILE_MACHINE_AMD64:IMAGE_FILE_MACHINE_I386;break;
        }
        DWORD error=nthook_context_parse(&result,data,bytes,machine);
        if((mutation==0 && (error || wcscmp(result.launcher,L"C:\\run16.exe"))) || (mutation && error!=ERROR_INVALID_DATA)) {
            fprintf(stderr,"packet mutation %u returned %lu\n",mutation,error);return 1;
        }
    }
    {
        const DWORD flags[]={NATIVE_HOOK_FORCE_DOS,NATIVE_HOOK_SEPARATE_WOW,NATIVE_HOOK_CREATION_FLAGS};
        for(unsigned i=0;i<ARRAYSIZE(flags);++i) {
            DWORD bytes=packet(data,machine);native_hook_packet *h=(native_hook_packet *)data;
            h->flags=flags[i];
            if(nthook_context_parse(&result,data,bytes,machine) || result.creation_flags!=flags[i])return 2;
            h->mode=NATIVE_HOOK_INTERCEPT;
            if(nthook_context_parse(&result,data,bytes,machine)!=ERROR_INVALID_DATA)return 3;
        }
    }
    printf("CONTEXT_PACKET_PASS bits=%u valid=4 rejected=13\n",(unsigned)(sizeof(void*)*8));return 0;
}
