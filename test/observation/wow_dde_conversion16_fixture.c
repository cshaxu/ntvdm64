/* Diagnostic only: exact conversion body, bounded memory/service doubles.
 * Detect use after guest release, not a guest allocator or transport test. */
#include <windows.h>
#include <dde.h>
#include <stdio.h>
#include <string.h>
typedef WORD HAND16;
typedef DWORD VPVOID;
typedef struct {WORD Msg,Format,Flags;HAND16 h16;} DDEINFO,*PDDEINFO;
typedef struct {short mm,xExt,yExt;HAND16 hMF;} METAFILEPICT16,*LPMETAFILEPICT16;
typedef struct {WORD status,cfFormat;HAND16 Value;} DDEDATA16;
typedef struct {HAND16 h[3];unsigned count;} DDECONVERSION,*PDDECONVERSION;
#define DDE_METAFILE 4
static BYTE guest[32][64],live[32];
static unsigned next_handle,allocs,fail_alloc,marks,fail_mark,invalid;
static BOOL draw;
static BYTE packet[64];
static METAFILEPICT meta;
static unsigned live_count(void){unsigned i,n=0;for(i=1;i<32;++i)n+=live[i];return n;}
static VPVOID alloc16(UINT flags,unsigned size,HAND16 *h){(void)flags;(void)size;++allocs;if(allocs==fail_alloc)return 0;*h=(HAND16)++next_handle;live[*h]=1;return *h;}
static void *view(VPVOID v){if(v>=32||!live[v])++invalid;return guest[v%32];}
static VPVOID lock16(HAND16 h,void *size){(void)size;if(!h||!live[h])++invalid;return h;}
static void release16(VPVOID v){if(!v||!live[v])++invalid;else live[v]=0;}
static HAND16 copy16(int size,LPBYTE data){HAND16 h=0;(void)data;alloc16(0,size,&h);return h;}
static BOOL add(HAND16 to,HAND16 from,HAND16 h,HANDLE n,PDDEINFO info){(void)to;(void)from;(void)n;(void)info;return h&&++marks!=fail_mark;}
static BOOL track(PDDECONVERSION c,HAND16 t,HAND16 f,HAND16 h,HANDLE n,PDDEINFO i){if(!add(t,f,h,n,i))return FALSE;c->h[c->count++]=h;return TRUE;}
static void rollback(PDDECONVERSION c,HAND16 t,HAND16 f){(void)t;(void)f;while(c->count)release16(c->h[--c->count]);}
static void fixmeta(LPMETAFILEPICT a,LPMETAFILEPICT16 b){b->mm=(short)a->mm;b->xExt=(short)a->xExt;b->yExt=(short)a->yExt;b->hMF=0;}
#define LOGDEBUG(level,args) ((void)0)
#define WOW32ASSERT(x) ((void)0)
#define GlobalSize(h) 32
#define GlobalLock(h) ((void *)(h))
#define GlobalUnlock(h) ((void)(h))
#define GlobalAllocLock16 alloc16
#define GlobalLock16 lock16
#define GlobalUnlockFree16 release16
#define GlobalUnlock16(h) ((void)(h))
#define GETMISCPTR(v,p) ((p)=view(v))
#define FREEMISCPTR(p) ((void)(p))
#define FLUSHVDMPTR(v,n,p) ((void)(v),(void)(n),(void)(p))
#define STOREWORD(d,s) (*(WORD *)&(d)=(WORD)(s))
#define GETHBITMAP16(h) ((HAND16)(ULONG_PTR)(h))
#define GETHPALETTE16(h) ((HAND16)(ULONG_PTR)(h))
#define Copyh32Toh16 copy16
#define WinMetaFileFromHMF(h,f) copy16(16,NULL)
#define DDEIsTargetMSDraw(h) draw
#define FixMetafile32To16 fixmeta
#define DDEAddhandle add
#define DDETrackhandle track
#define DDERollback16 rollback
#include "conversion16-body.h"
int main(void){
 unsigned which,errors=0;
 for(which=0;which<4;++which){
  DDEINFO info={WM_DDE_DATA,0,0,0};DDECONVERSION c={0};HAND16 result;
  memset(guest,0,sizeof guest);memset(live,0,sizeof live);memset(packet,0,sizeof packet);
  next_handle=allocs=marks=invalid=0;fail_mark=which==1?1:which==2?2:0;fail_alloc=which==3?2:0;draw=FALSE;
  ((DDEDATA *)packet)->cfFormat=CF_METAFILEPICT;
  *(HANDLE *)((DDEDATA *)packet)->Value=&meta;
  meta.mm=1;meta.xExt=2;meta.yExt=3;meta.hMF=(HMETAFILE)1;
  result=DDECopyhData16(1,2,packet,&info,&c);
  printf("case=%u result=%u live=%u invalid_after_release=%u marks=%u\n",which,result,live_count(),invalid,marks);
  if(invalid||(!which&&!result)||(which&&(result||live_count())))++errors;
 }
 printf("conversion16_errors=%u guest_proof=0\n",errors);return errors?1:0;
}
