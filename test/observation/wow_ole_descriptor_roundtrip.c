/* Test-only native peer: calls the exact hash/map-bound product provider.
 * No clipboard access, product export, injected patch or replacement algorithm. */
#include <windows.h>
#include <string.h>
#include <wchar.h>
#ifndef OLE_PROVIDER_RVA
#error Require the verified current product map RVA
#endif
typedef struct { DWORD cbSize; GUID clsid; DWORD aspect; LONG width,height,x,y;
    DWORD status,user,source; } DESCRIPTOR;
typedef HGLOBAL (WINAPI *CONVERT)(HANDLE,UINT);
typedef DWORD (WINAPI *LOCK16)(WORD);
typedef BOOL (WINAPI *UNLOCK16)(WORD);
typedef void *(WINAPI *POINTER16)(DWORD,DWORD,BOOL);
__declspec(dllexport) DWORD WINAPI OleRoundtrip(DWORD guest,DWORD unused1,DWORD unused2)
{
    HMODULE module=GetModuleHandleA("WOW32.DLL");
    CONVERT convert;
    LOCK16 lock16;
    UNLOCK16 unlock16;
    POINTER16 pointer16;
    HGLOBAL host;
    WORD back=0;
    DWORD flags=0,vp;
    DESCRIPTOR *p;
    (void)unused1; (void)unused2;
    if(!module)return 0;
    convert=(CONVERT)((BYTE *)module+OLE_PROVIDER_RVA);
    lock16=(LOCK16)GetProcAddress(module,"WOWGlobalLock16");
    unlock16=(UNLOCK16)GetProcAddress(module,"WOWGlobalUnlock16");
    pointer16=(POINTER16)GetProcAddress(module,"WOWGetVDMPointer");
    if(!lock16||!unlock16||!pointer16)return 0;
    host=convert((HANDLE)(UINT_PTR)guest,1);
    if(!host)return 0;
    p=(DESCRIPTOR *)GlobalLock(host);
    if(p&&p->cbSize==76&&p->aspect==1&&p->width==100&&p->height==200&&
       p->user==52&&p->source==62&&
       !wcscmp((WCHAR *)((BYTE *)p+p->user),L"T478")&&
       !wcscmp((WCHAR *)((BYTE *)p+p->source),L"source"))flags|=1;
    if(p)GlobalUnlock(host);
    back=(WORD)(UINT_PTR)convert(host,0);
    if(back&&(vp=lock16(back))!=0){
        p=(DESCRIPTOR *)pointer16(vp,76,TRUE);
        if(p&&p->cbSize==76&&p->aspect==1&&p->width==100&&p->height==200&&
           p->user==52&&p->source==62&&
           !strcmp((char *)p+p->user,"T478")&&!strcmp((char *)p+p->source,"source"))flags|=2;
        unlock16(back);
    }
    GlobalFree(host);
    if(!convert(NULL,0)&&!convert(NULL,1))flags|=4;
    if(!convert((HANDLE)(UINT_PTR)guest,2))flags|=8;
    /* The Win16 caller owns and frees the returned block even on a check failure. */
    return ((flags&15)<<16)|back;
}
