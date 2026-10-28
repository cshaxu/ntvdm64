/* Test-only object probe; renamed import relocations point to local fake seams. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdint.h>

typedef struct Cache {struct Cache *next; HANDLE file, mapping; BYTE *view; DWORD offset, size; BOOL access;} Cache;
extern Cache * __stdcall AllocMapFileCache(void);
extern void __stdcall InitMapFileCache(void), FlushMapFileCaches(void);
extern void __stdcall FreeMapFileCache(HANDLE);
extern BOOL __stdcall W32MapViewOfFile(Cache *, HANDLE);
extern Cache *gpCacheHead;
static int closes, fake_alive;
static HANDLE WINAPI fake_create(HANDLE a, LPSECURITY_ATTRIBUTES b, DWORD c, DWORD d, DWORD e, LPCSTR f) { (void)a;(void)b;(void)c;(void)d;(void)e;(void)f;fake_alive=1;return (HANDLE)42; }
static LPVOID WINAPI fake_map(HANDLE a,DWORD b,DWORD c,DWORD d,SIZE_T e) { (void)a;(void)b;(void)c;(void)d;(void)e;return NULL; }
static BOOL WINAPI fake_close(HANDLE h) { if (h!=(HANDLE)42) ExitProcess(20); ++closes; { int was=fake_alive;fake_alive=0;return was; } }
static BOOL WINAPI fake_unmap(LPCVOID p) { (void)p; return TRUE; }
static DWORD WINAPI fake_seek(HANDLE a,LONG b,PLONG c,DWORD d){(void)a;(void)b;(void)c;(void)d;return 4;}
static DWORD WINAPI fake_size(HANDLE a,LPDWORD b){(void)a;(void)b;return 64;}
void *fake_imp_Create=(void*)fake_create, *fake_imp_Map=(void*)fake_map, *fake_imp_Close=(void*)fake_close;
void *fake_imp_Unmap=(void*)fake_unmap, *fake_imp_Seek=(void*)fake_seek, *fake_imp_Size=(void*)fake_size;
int main(void) {
    Cache *p; int remaining=0;
    InitMapFileCache(); p=AllocMapFileCache();
    if (W32MapViewOfFile(p,(HANDLE)7) || closes!=1 || p->mapping!=(HANDLE)42 || fake_alive) return 2;
    fake_alive=1; FreeMapFileCache((HANDLE)7);
    printf("failed-map closes=%d reused-handle-alive=%d\n",closes,fake_alive);
    if (closes!=2 || fake_alive) return 3;
    InitMapFileCache();
    for (int i=0;i<11;i++){ p=AllocMapFileCache();p->file=(HANDLE)(intptr_t)(100+i); }
    FlushMapFileCaches(); for(p=gpCacheHead;p;p=p->next) if(p->file)++remaining;
    printf("flush remaining=%d\n",remaining);
    return remaining==1 ? 0:4;
}
