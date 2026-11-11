/* Independent bounded ownership witness. The exact extracted original body
 * runs with controlled native acceptance, never the user's Clipboard. */
#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
typedef WORD HAND16;
typedef uintptr_t VPVOID;
typedef struct { WORD f1,f2; } SETCLIPBOARDDATA16,*PSETCLIPBOARDDATA16;
typedef SETCLIPBOARDDATA16 *PVDMFRAME;
#pragma pack(push,2)
typedef struct {short mm,xExt,yExt;WORD hMF;} METAFILEPICT16,*LPMETAFILEPICT16;
#pragma pack(pop)
static UINT CFOLEObjectDescriptor=0xc001,CFOLELinkSrcDescriptor=0xc002;
#define CFOLE_ANSI_TO_UNICODE 1
static unsigned allocations,freed,metas,meta_freed,cached,cached_handle;
static BOOL native_accept;
static HANDLE copy_handle;
static METAFILEPICT16 guest={MM_ANISOTROPIC,10,20,7};
static HGLOBAL host_alloc(UINT flags,SIZE_T size){HGLOBAL h=GlobalAlloc(flags,size);if(h){++allocations;copy_handle=h;}return h;}
static HGLOBAL host_free(HGLOBAL h){HGLOBAL result=GlobalFree(h);if(!result&&h)++freed;return result;}
static BOOL host_delete_meta(HMETAFILE h){if(h)++meta_freed;return TRUE;}
static HANDLE host_set(UINT f,HANDLE h){(void)f;return native_accept?h:NULL;}
static VPVOID GlobalLock16(WORD h,int *bytes){if(bytes)*bytes=sizeof(guest);return h?(VPVOID)&guest:0;}
static void GlobalUnlock16(WORD h){(void)h;}
static HANDLE Copyh16Toh32(int n,void *p){HANDLE h=host_alloc(GMEM_MOVEABLE,n);(void)p;return h;}
static HANDLE CopyDropFilesFrom16(WORD h){(void)h;return host_alloc(GMEM_MOVEABLE,32);}
static HANDLE W32ConvertObjDescriptor(HANDLE h,int d){(void)h;(void)d;return host_alloc(GMEM_MOVEABLE,32);}
static HMETAFILE HMFFromWinMetaFile(WORD h,BOOL b){(void)h;(void)b;++metas;return (HMETAFILE)(uintptr_t)0x1234;}
static void WU32ICBStoreHandle(WORD f,WORD h){(void)f;++cached;cached_handle=h;}
#define FASTCALL
#define GETARGPTR(frame,size,p) ((p)=(frame))
#define FREEARGPTR(p) ((void)(p))
#define GETMISCPTR(v,p) ((p)=(LPBYTE)(v))
#define FREEMISCPTR(p) ((void)(p))
#define FETCHSHORT(v) (v)
#define FETCHWORD(v) (v)
#define WORD32(v) (v)
#define HBITMAP32(v) ((HBITMAP)(uintptr_t)(v))
#define HPALETTE32(v) ((HPALETTE)(uintptr_t)(v))
#define WOW32ASSERT(v) ((void)0)
#define LOGDEBUG(a,b) ((void)0)
#define RETURN(v) return (v)
#define GlobalAlloc host_alloc
#define GlobalFree host_free
#define DeleteMetaFile host_delete_meta
#define SetClipboardData host_set
#include "selected-set-clipboard.inc"
int main(int argc,char **argv){
 const WORD formats[]={CF_TEXT,CF_HDROP,CF_METAFILEPICT,CF_DSPMETAFILEPICT,0xc001,0xc002};
 unsigned i,mode,empty,failures=0,cases=0;
 BOOL strict=argc==2&&!strcmp(argv[1],"--expect-fixed");
 for(empty=0;empty<2;++empty)for(mode=0;mode<2;++mode)for(i=0;i<sizeof(formats)/sizeof(formats[0]);++i){
  SETCLIPBOARDDATA16 arg={formats[i],empty?0:0x55};ULONG result;
  unsigned expected_meta=!empty&&(formats[i]==CF_METAFILEPICT||formats[i]==CF_DSPMETAFILEPICT);
  allocations=freed=metas=meta_freed=cached=cached_handle=0;copy_handle=NULL;native_accept=(BOOL)mode;
  result=WU32SetClipboardData(&arg);++cases;
  printf("format=%u accepted=%u empty=%u result=%lu cached=%u guest=%u copies=%u freed=%u metafiles=%u meta_freed=%u\n",formats[i],mode,empty,result,cached,cached_handle,allocations,freed,metas,meta_freed);
  if(strict){
   unsigned expected_cache=empty?(formats[i]!=CF_TEXT):mode;
   if(result!=(!empty&&mode?0x55:0)||cached!=expected_cache||
      cached_handle!=(expected_cache&&!empty?0x55:0)||allocations!=!empty||
      freed!=(!empty&&!mode)||metas!=expected_meta||
      meta_freed!=(expected_meta&&!mode)){
    fprintf(stderr,"ownership assertion failed for format=%u accepted=%u empty=%u\n",formats[i],mode,empty);++failures;
   }
  }
  /* Fixture disposal after observation is not product cleanup evidence. */
  if(copy_handle&&allocations>freed)host_free(copy_handle);
 }
 printf("cases=%u strict=%u failures=%u\n",cases,strict,failures);
 return failures?1:0;
}
