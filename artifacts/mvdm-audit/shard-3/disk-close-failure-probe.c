#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#pragma pack(push,1)
typedef struct { BYTE drive,idle; CHAR letter; BOOLEAN locked; HANDLE fd; DWORD heads; LARGE_INTEGER cylinders; DWORD sectors,bytes,align; USHORT owner; CHAR name[9]; } disk_t;
#pragma pack(pop)
typedef struct { HANDLE fd; USHORT drive_type,flop_type,last_seek,last_head_seek; BOOLEAN change,locked; USHORT owner; SHORT motor,density; USHORT max_track,sectors,tracks; DWORD align; BYTE idle,c,h,r,n; CHAR name[MAX_PATH]; } floppy_t;
typedef char chkfl0[(offsetof(floppy_t,fd)==0)?1:-1]; typedef char chkfl1[(offsetof(floppy_t,locked)==13)?1:-1]; typedef char chkfl2[(offsetof(floppy_t,owner)==14)?1:-1]; typedef char chkfd0[(offsetof(disk_t,fd)==4)?1:-1]; typedef char chkfd1[(offsetof(disk_t,locked)==3)?1:-1]; typedef char chkfd2[(offsetof(disk_t,owner)==32)?1:-1];
extern floppy_t floppy_data[]; extern ULONG floppy_open_count; extern BOOL density_changed;
extern WORD fdisk_open_count; BOOL nt_floppy_close(unsigned char); BOOL close_fdisk(disk_t *);
static int fail_update=1; int mvdm_command_guest_state_add_fd_access(uint16_t delta) {(void)delta;return !fail_update;}
int main(void){ disk_t disk={0}; HANDLE first,second; BOOL a,b;
 first=CreateEventW(NULL,FALSE,FALSE,NULL); second=CreateEventW(NULL,FALSE,FALSE,NULL);if(!first||!second)return 2;
 floppy_data[0].fd=first;floppy_data[0].locked=1;floppy_data[0].owner=0x1234;floppy_open_count=1;density_changed=FALSE;
 disk.fd=second;disk.locked=1;disk.owner=0x1234;fdisk_open_count=1;
 fprintf(stderr,"before floppy event=%p disk=%p\n",first,second); __try { a=nt_floppy_close(0); fprintf(stderr,"after floppy=%d\n",a); b=close_fdisk(&disk); fprintf(stderr,"after disk=%d\n",b); fprintf(stderr,"after fd floppy=%p locked=%d owner=%x count=%lu disk=%p count=%u\n",floppy_data[0].fd,floppy_data[0].locked,floppy_data[0].owner,(unsigned long)floppy_open_count,disk.fd,fdisk_open_count); } __except(EXCEPTION_EXECUTE_HANDLER) { fprintf(stderr,"exception=%08lx\n",GetExceptionCode()); return 5; }
 fprintf(stderr,"failure floppy first=%d invalid=%d locked=%d owner=%04x count=%lu density=%d; disk first=%d invalid=%d owner=%04x count=%u\n",a,floppy_data[0].fd==INVALID_HANDLE_VALUE,floppy_data[0].locked,floppy_data[0].owner,(unsigned long)floppy_open_count,density_changed,b,disk.fd==INVALID_HANDLE_VALUE,disk.owner,fdisk_open_count);
 return (!a&&!b&&floppy_open_count==1&&fdisk_open_count==1&&floppy_data[0].locked&&floppy_data[0].owner==0x1234)?0:3;
}
