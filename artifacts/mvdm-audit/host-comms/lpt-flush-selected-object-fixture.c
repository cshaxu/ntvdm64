/* Selected original nt_lpt object; event handle is intentionally not an LPT device. */
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
struct lpt {ULONG status;HANDLE handle;int counter,trigger,bytes,threshold;DWORD filetype;BOOLEAN active,dos_opened;BYTE *buf;BOOLEAN direct,no_device;};
_Static_assert(sizeof(struct lpt)==40,"x86 LPT stride");
_Static_assert(offsetof(struct lpt,bytes)==16 && offsetof(struct lpt,buf)==32 && offsetof(struct lpt,direct)==36,"x86 field offsets");
extern struct lpt host_lpt[]; extern int flushBuffer(int);
int main(void){BYTE data[2]={'A','B'};HANDLE h=CreateEventA(NULL,FALSE,FALSE,NULL);int a,b;DWORD e1,e2;
 if(!h)return 2;memset(&host_lpt[0],0,sizeof host_lpt[0]);host_lpt[0].handle=h;host_lpt[0].buf=data;
 host_lpt[0].direct=TRUE;host_lpt[0].bytes=2;SetLastError(0);a=flushBuffer(0);e1=GetLastError();
 printf("direct return=%d remaining=%d last_error=%lu\n",a,host_lpt[0].bytes,e1);
 if(a!=1||host_lpt[0].bytes!=0||e1==0){CloseHandle(h);return 3;}
 host_lpt[0].direct=FALSE;host_lpt[0].bytes=2;SetLastError(0);b=flushBuffer(0);e2=GetLastError();
 printf("buffered return=%d remaining=%d last_error=%lu\n",b,host_lpt[0].bytes,e2);
 CloseHandle(h);return b==0&&host_lpt[0].bytes==0&&e2!=0?0:4;
}
