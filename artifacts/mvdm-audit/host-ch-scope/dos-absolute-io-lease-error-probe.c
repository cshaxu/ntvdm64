#include <windows.h>
#include <stdint.h>
#include <stdio.h>
typedef struct {uint16_t seg,off; uint32_t valid;} loc;
loc current_pdb_location={0,0,1};
void *session_thread_current(void){return NULL;}
int session_guest_memory_acquire(void *s, unsigned a, unsigned b, unsigned c, void *d, void *e){(void)s;(void)a;(void)b;(void)c;(void)d;(void)e;ExitProcess(96);return 0;}
int session_guest_memory_release(void *s, void *a, int b){(void)s;(void)a;(void)b;ExitProcess(97);return 0;}
extern ULONG nt_floppy_read(BYTE,ULONG,ULONG,BYTE *);
extern ULONG nt_floppy_write(BYTE,ULONG,ULONG,BYTE *);
#pragma pack(push,1)
typedef struct { BYTE drive,idle; CHAR letter; BOOLEAN locked; HANDLE fd; DWORD heads; LARGE_INTEGER cylinders; DWORD sectors,bytes,align; USHORT owner; CHAR name[9]; } disk_t;
#pragma pack(pop)
extern disk_t *fdisk_data_table; extern BYTE number_of_fdisk;
extern ULONG nt_fdisk_read(BYTE,LARGE_INTEGER *,ULONG,BYTE *);
extern ULONG nt_fdisk_write(BYTE,LARGE_INTEGER *,ULONG,BYTE *);
int main(void) { disk_t d={0}; LARGE_INTEGER off; off.QuadPart=0; fdisk_data_table=&d; number_of_fdisk=1;BYTE b=0;DWORD e;ULONG r; SetLastError(0x4a13);r=nt_floppy_read(0,0,1,&b);e=GetLastError();printf("read return=%lu last=%lu\n",r,e);if(r||e!=0x4a13)return 2;SetLastError(0x4a27);r=nt_floppy_write(0,0,1,&b);e=GetLastError();printf("write return=%lu last=%lu\n",r,e);if(r||e!=0x4a27)return 3; SetLastError(0x4a31); r=nt_fdisk_read(0,&off,1,&b); e=GetLastError(); printf("fd-read return=%lu last=%lu\n",r,e); if(r||e!=0x4a31)return 4; SetLastError(0x4a41); r=nt_fdisk_write(0,&off,1,&b); e=GetLastError(); printf("fd-write return=%lu last=%lu\n",r,e); return r||e!=0x4a41?5:0;}
#include <stdlib.h>
uintptr_t __security_cookie=0xbb40e64e;
void __attribute__((fastcall)) __security_check_cookie(uintptr_t x){(void)x;}
int RcMessageBox(void){ExitProcess(91);}
int mvdm_command_guest_state_add_fd_access(uint16_t x){(void)x;ExitProcess(92);}

void host_direct_access_error(void){ExitProcess(95);}
