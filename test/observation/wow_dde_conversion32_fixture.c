/* Exact conversion body with bounded allocation/registration doubles. */
#include <windows.h>
#include <dde.h>
#include <stdio.h>
#include <string.h>
typedef WORD HAND16;
typedef DWORD VPVOID;
typedef struct {WORD Msg,Format,Flags;HAND16 h16;} DDEINFO,*PDDEINFO;
typedef struct {short mm,xExt,yExt;HAND16 hMF;} METAFILEPICT16,*LPMETAFILEPICT16;
typedef struct {WORD status,cfFormat;HANDLE Value;} DDEDATA32;
typedef struct {HANDLE h[3];unsigned count;} DDECONVERSION,*PDDECONVERSION;
#define DDE_METAFILE 4
static BYTE guest[4][64],native[16][64],live[16];
static unsigned next_handle,allocs,fail_alloc,marks,fail_mark,invalid,read_after_mark;
static unsigned live_count(void){unsigned i,n=0;for(i=1;i<16;++i)n+=live[i];return n;}
static HANDLE alloc32(UINT flags,SIZE_T size){unsigned n;(void)flags;(void)size;++allocs;if(allocs==fail_alloc)return NULL;n=++next_handle;live[n]=1;return native[n];}
static void *lock32(HANDLE h){unsigned i;for(i=1;i<16;++i)if(h==native[i]){if(!live[i])++invalid;return h;}++invalid;return h;}
static HANDLE free32(HANDLE h){unsigned i;for(i=1;i<16;++i)if(h==native[i]){if(!live[i])++invalid;live[i]=0;return NULL;}++invalid;return h;}
static VPVOID lock16(HAND16 h,int *size){if(!h||h>3)++invalid;if(size)*size=16;return h;}
static WORD fetch(void *p){if(p==&((LPMETAFILEPICT16)guest[2])->hMF)read_after_mark=marks;return *(WORD *)p;}
static BOOL add(HAND16 t,HAND16 f,HAND16 h,HANDLE n,PDDEINFO i){(void)t;(void)f;(void)i;return h&&n&&++marks!=fail_mark;}
static BOOL track(PDDECONVERSION c,HAND16 t,HAND16 f,HAND16 h,HANDLE n,PDDEINFO i){if(!add(t,f,h,n,i))return FALSE;c->h[c->count++]=n;return TRUE;}
static void rollback(PDDECONVERSION c,HAND16 t,HAND16 f){(void)t;(void)f;while(c->count)free32(c->h[--c->count]);}
#define LOGDEBUG(level,args) ((void)0)
#define WOW32ASSERT(x) ((void)0)
#define GlobalAlloc alloc32
#define GlobalLock lock32
#define GlobalUnlock(h) ((void)(h))
#define GlobalFree free32
#define DeleteMetaFile(h) ((BOOL)(free32(h)==NULL))
#define GlobalLock16 lock16
#define GlobalUnlock16(h) ((void)(h))
#define GETMISCPTR(v,p) ((p)=(void *)guest[v])
#define FREEMISCPTR(p) ((void)(p))
#define FETCHWORD(s) fetch(&(s))
#define FETCHSHORT(s) (*(short *)&(s))
#define HBITMAP32(h) ((HANDLE)(ULONG_PTR)(h))
#define HPALETTE32(h) ((HANDLE)(ULONG_PTR)(h))
#define Copyh16Toh32(cb,p) alloc32(0,cb)
#define HMFFromWinMetaFile(h,f) alloc32(0,16)
#define DDEIsTargetMSDraw(h) FALSE
#define DDEAddhandle add
#define DDETrackhandle track
#define DDERollback32 rollback
#include "conversion32-body.h"
int main(void){unsigned which,errors=0;
 for(which=0;which<4;++which){DDEINFO info={WM_DDE_DATA,0,0,0};DDECONVERSION c={0};HANDLE result;
  memset(guest,0,sizeof guest);memset(native,0,sizeof native);memset(live,0,sizeof live);
  next_handle=allocs=marks=invalid=read_after_mark=0;fail_mark=which==1?1:which==2?2:0;fail_alloc=which==3?3:0;
  ((DDEDATA *)guest[1])->cfFormat=CF_METAFILEPICT;*(HAND16 *)((DDEDATA *)guest[1])->Value=2;
  ((LPMETAFILEPICT16)guest[2])->mm=1;((LPMETAFILEPICT16)guest[2])->hMF=3;
  result=DDECopyhData32(1,2,1,&info,&c);
  printf("case=%u result_nonzero=%u live=%u invalid=%u marks=%u hMF_read_after_marks=%u\n",which,result!=NULL,live_count(),invalid,marks,read_after_mark);
  if(invalid||(!which&&!result)||(which&&(result||live_count()))||read_after_mark!=1)++errors;
 }
 printf("conversion32_errors=%u guest_proof=0\n",errors);return errors?1:0;
}
