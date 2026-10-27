/* Actual selected original WHEAP x86 object, isolated host heap owner. */
#include <windows.h>
#include <stdio.h>
extern HANDLE hWOWHeapSmall;
BOOL __stdcall CreateSmallHeap(void);
void * __stdcall malloc_w_small(ULONG size);
BOOL __stdcall free_w_small(void *value);
int main(void) {
 void *a,*b; int pass;
 if(!CreateSmallHeap() || !hWOWHeapSmall)return 1;
 a=malloc_w_small(1);b=malloc_w_small(8192);
 pass=a && b && HeapValidate(hWOWHeapSmall,0,a) && HeapValidate(hWOWHeapSmall,0,b);
 if(a)pass=free_w_small(a)&&pass;
 if(b)pass=free_w_small(b)&&pass;
 pass=HeapValidate(hWOWHeapSmall,0,NULL)&&pass;
 pass=HeapDestroy(hWOWHeapSmall)&&pass;
 fprintf(stderr,"WHEAP production object allocation/free/growth pass=%d\n",pass);
 return pass?0:2;
}
