/* Runs the selected WOW cache object; only Win32 imports are redirected.
 * Normal cases use real private files/mappings. Failure reuse is simulated. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdint.h>
typedef struct Cache {struct Cache *next; HANDLE file,mapping; BYTE *view; DWORD offset,size; BOOL access;} Cache;
extern Cache * __stdcall AllocMapFileCache(void);
extern Cache * __stdcall FindMapFileCache(HANDLE);
extern void __stdcall InitMapFileCache(void),FlushMapFileCaches(void),FreeMapFileCache(HANDLE);
extern BOOL __stdcall W32MapViewOfFile(Cache *,HANDLE);
extern Cache *gpCacheHead;
static int checks,failures,fail_view,fail_create,alive,closes,unmaps;
#define CHECK(x) do {++checks;if(!(x)){++failures;printf("FAIL line=%d %s\n",__LINE__,#x);}}while(0)
static HANDLE WINAPI create_map(HANDLE h,LPSECURITY_ATTRIBUTES a,DWORD p,DWORD hi,DWORD lo,LPCSTR n) {
    if(fail_create)return NULL;
    if(fail_view){alive=1;return (HANDLE)42;}
    return CreateFileMappingA(h,a,p,hi,lo,n);
}
static LPVOID WINAPI map_view(HANDLE h,DWORD a,DWORD hi,DWORD lo,SIZE_T n) {
    return fail_view ? NULL : MapViewOfFile(h,a,hi,lo,n);
}
static BOOL WINAPI close_map(HANDLE h) {
    ++closes;if(fail_view){BOOL was=alive;CHECK(h==(HANDLE)42);alive=0;return was;}
    return CloseHandle(h);
}
static BOOL WINAPI unmap_view(LPCVOID p){++unmaps;return UnmapViewOfFile(p);}
void *cache_imp_Create=(void*)create_map,*cache_imp_Map=(void*)map_view;
void *cache_imp_Close=(void*)close_map,*cache_imp_Unmap=(void*)unmap_view;
static HANDLE make_file(int i) {
    char name[80];BYTE data[64];DWORD done;HANDLE h;
    sprintf(name,"wow-cache-%lu-%d.tmp",GetCurrentProcessId(),i);
    h=CreateFileA(name,GENERIC_READ|GENERIC_WRITE,0,NULL,CREATE_NEW,
        FILE_ATTRIBUTE_TEMPORARY|FILE_FLAG_DELETE_ON_CLOSE,NULL);
    CHECK(h!=INVALID_HANDLE_VALUE);if(h==INVALID_HANDLE_VALUE)ExitProcess(90);
    for(int j=0;j<64;j++)data[j]=(BYTE)(j+i);
    CHECK(WriteFile(h,data,64,&done,NULL)&&done==64);
    CHECK(SetFilePointer(h,2,NULL,FILE_BEGIN)==2);return h;
}
int main(void) {
    Cache *p;HANDLE files[11],failed_file;BYTE fallback[3];DWORD read;int count,remaining;
    FlushMapFileCaches(); /* original uninitialized no-op */
    for(count=1;count<=11;count++) {
        InitMapFileCache();closes=unmaps=0;
        for(int i=0;i<count;i++) {
            files[i]=make_file(i);p=AllocMapFileCache();CHECK(W32MapViewOfFile(p,files[i]));
            CHECK(p->size==64&&p->offset==2&&p->view[9]==(BYTE)(9+i));
            p->offset=3+i; /* selected read path's logical file position */
        }
        /* Lookup promotes a non-head entry, as normal reads do. */
        CHECK(FindMapFileCache(files[0])!=NULL);
        FlushMapFileCaches();remaining=0;
        for(p=gpCacheHead;p;p=p->next)if(p->file)remaining++;
        CHECK(remaining==0);CHECK(closes==count&&unmaps==count);
        for(int i=0;i<count;i++) {
            CHECK(SetFilePointer(files[i],0,NULL,FILE_CURRENT)==(DWORD)(3+i));
            FreeMapFileCache(files[i]); /* also retires baseline skipped tail */
            CHECK(CloseHandle(files[i]));
        }
        FlushMapFileCaches();CHECK(closes==count&&unmaps==count);
    }
    /* Holes and reordered nodes must also flush all live ownership. */
    InitMapFileCache();closes=unmaps=0;
    for(int i=0;i<11;i++){files[i]=make_file(i);p=AllocMapFileCache();CHECK(W32MapViewOfFile(p,files[i]));}
    for(int i=0;i<11;i+=2)FreeMapFileCache(files[i]);
    FlushMapFileCaches();CHECK(closes==11&&unmaps==11);
    for(int i=0;i<11;i++){FreeMapFileCache(files[i]);CHECK(CloseHandle(files[i]));}
    InitMapFileCache();fail_view=1;closes=unmaps=0;
    failed_file=make_file(0);p=AllocMapFileCache();CHECK(!W32MapViewOfFile(p,failed_file));
    CHECK(ReadFile(failed_file,fallback,3,&read,NULL)&&read==3&&fallback[0]==2&&fallback[2]==4);
    CHECK(closes==1&&!alive&&p->mapping==NULL);
    alive=1; /* deterministic reuse of numeric 42 by an unrelated owner */
    FreeMapFileCache(failed_file);CHECK(closes==1&&alive&&unmaps==0);
    FreeMapFileCache(failed_file);CHECK(closes==1);CHECK(CloseHandle(failed_file));
    InitMapFileCache();closes=0;alive=0;p=AllocMapFileCache();CHECK(!W32MapViewOfFile(p,(HANDLE)7));
    for(int i=0;i<10;i++){p=AllocMapFileCache();p->file=(HANDLE)(intptr_t)(100+i);}
    alive=1;p=AllocMapFileCache();CHECK(closes==1&&alive); /* eviction */
    InitMapFileCache();fail_view=0;fail_create=1;closes=unmaps=0;
    p=AllocMapFileCache();CHECK(!W32MapViewOfFile(p,(HANDLE)7));FreeMapFileCache((HANDLE)7);
    CHECK(closes==0&&unmaps==0);
    printf("WOW_FILE_CACHE checks=%d failures=%d\n",checks,failures);
    return failures?1:0;
}
