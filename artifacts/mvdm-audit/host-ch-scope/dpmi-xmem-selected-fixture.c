#include <windows.h>
#include <stdio.h>
static WORD bx,cx,dx,si,di;static int cf, allocs,reallocs,frees,fail_realloc,fail_free;static ULONG freed_address,freed_size;
WORD getBX(void){return bx;}WORD getCX(void){return cx;}WORD getDX(void){return dx;}WORD getSI(void){return si;}WORD getDI(void){return di;}
void setBX(WORD x){bx=x;}void setCX(WORD x){cx=x;}void setSI(WORD x){si=x;}void setDI(WORD x){di=x;}void setCF(int x){cf=x;}
LONG DpmiAllocateVirtualMemory(PVOID *addr,ULONG *sz){allocs++;(void)sz;*addr=(PVOID)0x40000;return 0;}
LONG DpmiFreeVirtualMemory(PVOID *addr,ULONG *sz){frees++;freed_address=(ULONG)(ULONG_PTR)*addr;freed_size=*sz;return fail_free ? (LONG)0xC0000001UL : 0;}
BOOL DpmiReallocateVirtualMemory(PVOID addr,ULONG old,PVOID *out,ULONG *sz){(void)addr;(void)old;(void)sz;reallocs++;if(fail_realloc)return FALSE;*out=(PVOID)0x50000;return TRUE;}
void DpmiAllocateXmem(void);void DpmiFreeXmem(void);void DpmiReallocateXmem(void);
static void request(DWORD n){bx=(WORD)(n>>16);cx=(WORD)n;}
int main(void){int k;for(k=0;k<3;k++){WORD hsi,hdi;allocs=reallocs=frees=cf=0;fail_realloc=k==0;fail_free=k==2;freed_address=freed_size=0;dx=0x27;request(0x1000);DpmiAllocateXmem();if(cf||allocs!=1)return 1;hsi=si;hdi=di;request(0x2000);DpmiReallocateXmem();fprintf(stderr,"XM case=%d realloc_cf=%d calls=%d handle_same=%d\n",k,cf,reallocs,si==hsi&&di==hdi);if(cf!=(k==0)||si!=hsi||di!=hdi || (k!=0 && (bx!=5 || cx!=0)))return 2;DpmiFreeXmem();fprintf(stderr,"XM case=%d free_cf=%d calls=%d addr=%08lx size=%08lx\n",k,cf,frees,(unsigned long)freed_address,(unsigned long)freed_size);if(cf!=(k==2)||freed_address!=(k==0?0x40000UL:0x50000UL)||freed_size!=(k==0?0x1000UL:0x2000UL))return 3;if(k==2){fail_free=0;DpmiFreeXmem();fprintf(stderr,"XM case=%d retry_cf=%d calls=%d\n",k,cf,frees);if(cf||frees!=2)return 4;}}return 0;}
