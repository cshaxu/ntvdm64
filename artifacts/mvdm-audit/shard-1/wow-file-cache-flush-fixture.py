"""Run isolated fake-handle tests against unchanged extracted WOW cache functions."""
import hashlib,pathlib,re,subprocess,tempfile
src=pathlib.Path('src/mvdm/wow32/wkfileio.c').read_text(encoding='latin1')
def extract(n,expected):
 m=re.search(r'\b'+n+r'\s*\([^;{}]*\)\s*\{',src); assert m
 depth=0
 for i in range(m.end()-1,len(src)):
  if src[i]=='{':depth+=1
  elif src[i]=='}':
   depth-=1
   if depth==0:
    body=src[m.start():i+1]; assert hashlib.sha256(body.encode('latin1')).hexdigest()==expected; return body
 raise RuntimeError(n)
f=extract('FreeMapFileCache','b87a30e96d779dbf40f637d7f91d72128d6e6a242b589045e4c7f6b1b90da0c1')
m=extract('W32MapViewOfFile','2c649a81b2502279ea626a9b08f5bd65e2cec985a1f1a97c39789881233779ed')
find=extract('FindMapFileCache','6c18fa69bb42d401d319816b1ce311306c5b3114865e684f543af87f49b7ef8f')
alloc=extract('AllocMapFileCache','165aa808f3b391d6e2cbc4af6907d7997c523cbba4cadf0b13e9ac888e7d5908')
init=extract('InitMapFileCache','04fbcc5b91920aacc4a41a899d84f5002d5355d7a03addb4d97d627b308509d1')
flush=extract('FlushMapFileCaches','c1d38a0621a076d3005bc2f2a731786e2e00a2d8718f3a26187a2715e5e6b090')
code=r'''
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
typedef uintptr_t HANDLE; typedef uint32_t DWORD; typedef unsigned char *LPBYTE; typedef int BOOL; typedef void VOID;
typedef struct S {struct S *hpfNext;HANDLE hfile32,hMappedFileObject;LPBYTE lpStartingAddressOfView;DWORD lFilePointer,dwFileSize;BOOL fAccess;} HMAPPEDFILEALIAS,*PHMAPPEDFILEALIAS;
#define FALSE 0
#define TRUE 1
#define NULLPTR 0
#define PAGE_READONLY 2
#define FILE_MAP_READ 4
#define FILE_BEGIN 0
#define FILE_CURRENT 1
#define LOGDEBUG(...)
#define FINDMAPFILECACHE(f) FindMapFileCache(f)
typedef int INT;
#define MAX_MAPPED_FILES 12
#define WOW32ASSERT(x) do {if(!(x))abort();} while(0)
static DWORD dwTotalCacheBytes, dwTotalCacheAccess;
#define FREEMAPFILECACHE(h) FreeMapFileCache(h)
static HMAPPEDFILEALIAS aMappedFileCache[MAX_MAPPED_FILES]; static PHMAPPEDFILEALIAS gpCacheHead; static BOOL fCacheInit=TRUE;
void InitMapFileCache(void); void FreeMapFileCache(HANDLE); PHMAPPEDFILEALIAS FindMapFileCache(HANDLE);
static int fake_alive, close_count, unmap_count, mapping_fail, view_fail, last_close_was_live;

static HANDLE CreateFileMapping(HANDLE f,int a,int b,int c,int d,int e){(void)f;(void)a;(void)b;(void)c;(void)d;(void)e;if(mapping_fail)return 0;fake_alive=1;return 42;}
static LPBYTE MapViewOfFile(HANDLE h,int a,int b,int c,int d){(void)h;(void)a;(void)b;(void)c;(void)d;return view_fail?0:(LPBYTE)(uintptr_t)99;}
static int CloseHandle(HANDLE h){if(h!=42)abort();close_count++;last_close_was_live=fake_alive;fake_alive=0;return last_close_was_live;}
static int UnmapViewOfFile(LPBYTE p){if(p!=(LPBYTE)(uintptr_t)99)abort();unmap_count++;return 1;}
static DWORD SetFilePointer(HANDLE h,DWORD a,void*b,int c){(void)h;(void)a;(void)b;(void)c;return 4;}
static DWORD GetFileSize(HANDLE h,void*p){(void)h;(void)p;return 64;}
''' + 'PHMAPPEDFILEALIAS ' + find + '\nPHMAPPEDFILEALIAS ' + alloc + '\nVOID ' + init + '\nVOID ' + f + '\nBOOL ' + m + '\nVOID ' + flush + r'''
#define CHECK(x) do{if(!(x)){fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x);return 1;}}while(0)
static void reset(void){for(int i=0;i<MAX_MAPPED_FILES;i++)aMappedFileCache[i]=(HMAPPEDFILEALIAS){0}; fCacheInit=TRUE; gpCacheHead=0; fake_alive=close_count=unmap_count=mapping_fail=view_fail=last_close_was_live=0;}
int main(void){ PHMAPPEDFILEALIAS p;
 reset();p=AllocMapFileCache();view_fail=1;CHECK(!W32MapViewOfFile(p,7));CHECK(close_count==1 && !fake_alive && p->hMappedFileObject==42);fake_alive=1;FreeMapFileCache(7);CHECK(close_count==2 && last_close_was_live && !fake_alive);FreeMapFileCache(7);CHECK(close_count==2);puts("PASS failed map, simulated reuse closes unrelated handle");
 reset();p=AllocMapFileCache();view_fail=1;CHECK(!W32MapViewOfFile(p,7));FreeMapFileCache(7);CHECK(close_count==2 && !last_close_was_live);puts("PASS failed map, no reuse double close invalid handle");
 reset();p=AllocMapFileCache();CHECK(W32MapViewOfFile(p,7));FreeMapFileCache(7);CHECK(close_count==1 && unmap_count==1 && last_close_was_live);FreeMapFileCache(7);CHECK(close_count==1 && unmap_count==1);puts("PASS successful map exactly once cleanup");
 reset();p=AllocMapFileCache();view_fail=1;CHECK(!W32MapViewOfFile(p,7));for(int j=0;j<10;j++){p=AllocMapFileCache();p->hfile32=100+j;}fake_alive=1;p=AllocMapFileCache();CHECK(close_count==2 && last_close_was_live && !fake_alive);puts("PASS real cache eviction closes reused fake handle");
 reset();for(int j=0;j<11;j++){p=AllocMapFileCache();p->hfile32=100+j;}FlushMapFileCaches();int remaining=0;for(p=gpCacheHead;p;p=p->hpfNext)if(p->hfile32)remaining++;CHECK(remaining==1);puts("PASS full real list: flush leaves one populated tail");
 reset();p=AllocMapFileCache();mapping_fail=1;CHECK(!W32MapViewOfFile(p,7));FreeMapFileCache(7);CHECK(close_count==0 && unmap_count==0);puts("PASS mapping creation failure has no mapping close");return 0;}
'''
with tempfile.TemporaryDirectory(prefix='wow-cache-audit-',dir='build/t473-s10') as tmp:
 p=pathlib.Path(tmp);(p/'fixture.c').write_text(code,encoding='utf-8')
 subprocess.run(['gcc','-std=c11','-O0','-o',str(p/'fixture.exe'),str(p/'fixture.c')],check=True)
 subprocess.run([str(p/'fixture.exe')],check=True)
