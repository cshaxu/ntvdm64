/* Selected DEM ioctl object, controlled CPU and drive-type seams. */
#include <windows.h>
#include <stdio.h>
static WORD ax,dx,cf; static BYTE bl; static DWORD physical, fallback; static unsigned fallback_calls;
unsigned short getAL(void){return (unsigned char)ax;} unsigned short getBL(void){return bl;}
void setAX(unsigned short x){ax=x;} void setAL(unsigned short x){ax=(WORD)((ax&0xff00)|(x&0xff));}
void setDX(unsigned short x){dx=x;} void setCF(unsigned short x){cf=x;}
DWORD demGetPhysicalDriveType(BYTE n){(void)n;return physical;}
UINT WINAPI GetDriveTypeOem(const char *p){(void)p;fallback_calls++;return fallback;}
extern void demIoctlChangeable(void);
static int check(BYTE op,DWORD type,DWORD alt,WORD wantax,WORD wantdx,WORD wantcf,unsigned calls){
 ax=op;dx=0xeeee;cf=9;bl=0;physical=type;fallback=alt;fallback_calls=0;demIoctlChangeable();
 printf("op=%u type=%lu fallback=%lu ax=%u dx=%u cf=%u calls=%u\n",op,type,alt,ax,dx,cf,fallback_calls);
 return ax==wantax&&dx==wantdx&&cf==wantcf&&fallback_calls==calls;
}
int main(void){
 if(!check(8,DRIVE_REMOVABLE,DRIVE_UNKNOWN,0,0xeeee,0,0))return 1;
 if(!check(8,DRIVE_FIXED,DRIVE_UNKNOWN,1,0xeeee,0,0))return 2;
 if(!check(9,DRIVE_FIXED,DRIVE_UNKNOWN,0,0x0800,0,0))return 3;
 if(!check(9,DRIVE_REMOTE,DRIVE_UNKNOWN,0,0x1000,0,0))return 4;
 if(!check(9,DRIVE_CDROM,DRIVE_UNKNOWN,0,0x1000,0,0))return 5;
 if(!check(8,DRIVE_UNKNOWN,DRIVE_REMOVABLE,0,0xeeee,0,1))return 6;
 if(!check(8,DRIVE_UNKNOWN,DRIVE_NO_ROOT_DIR,ERROR_INVALID_DRIVE,0xeeee,1,1))return 7;
 return 0;
}
