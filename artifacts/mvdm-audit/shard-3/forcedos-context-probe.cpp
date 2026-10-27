#include "nthook32-dll/hook.h"
#include "nthook32-dll/detours/detours.h"
#include <string.h>
static BYTE packet[sizeof(native_hook_packet)+3*64];
static DWORD packet_bytes;
extern "C" PVOID WINAPI DetourFindPayloadEx(REFGUID, DWORD *size) { *size=packet_bytes; return packet; }
static int check(DWORD flags, DWORD expected, DWORD mode) {
 native_hook_packet *p=(native_hook_packet*)packet; nthook_context c; BOOL found=FALSE;
 const WCHAR path[]=L"C:\\x"; DWORD n=sizeof(path);
 memset(packet,0,sizeof(packet)); p->version=NATIVE_HOOK_VERSION; p->mode=mode;
 p->machine=sizeof(void*)==8?IMAGE_FILE_MACHINE_AMD64:IMAGE_FILE_MACHINE_I386; p->flags=flags;
 p->launcher_offset=sizeof(*p); p->launcher_bytes=n; p->hook_offset=p->launcher_offset+n; p->hook_bytes=n;
 p->hook64_offset=p->hook_offset+n; p->hook64_bytes=n; p->bytes=p->hook64_offset+n; packet_bytes=p->bytes;
 memcpy(packet+p->launcher_offset,path,n); memcpy(packet+p->hook_offset,path,n); memcpy(packet+p->hook64_offset,path,n);
 DWORD error=nthook_context_read(&c,&found); return found && error==expected && (error || c.creation_flags==flags);
}
int main() { return !(check(0,0,NATIVE_HOOK_LAUNCHER) && check(NATIVE_HOOK_SEPARATE_WOW,0,NATIVE_HOOK_LAUNCHER) && check(0x2000,ERROR_INVALID_DATA,NATIVE_HOOK_LAUNCHER) && check(0x2000|NATIVE_HOOK_SEPARATE_WOW,ERROR_INVALID_DATA,NATIVE_HOOK_LAUNCHER)); }
