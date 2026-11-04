#include <windows.h>
#include <stdio.h>
static int mode,probe,init,update,release,freed;
static int storage;
static DWORD error_code=1234;
static BOOL fake_init(LPPROC_THREAD_ATTRIBUTE_LIST list,DWORD count,DWORD flags,PSIZE_T bytes)
{
    (void)count;(void)flags;
    if(!list){++probe;*bytes=64;return FALSE;}
    ++init;return mode!=2;
}
static LPVOID fake_alloc(HANDLE heap,DWORD flags,SIZE_T bytes)
{ (void)heap;(void)flags;(void)bytes;return mode==1 ? NULL : &storage; }
static BOOL fake_update(LPPROC_THREAD_ATTRIBUTE_LIST list,DWORD flags,DWORD_PTR attribute,
    PVOID value,SIZE_T size,PVOID previous,PSIZE_T returned)
{ (void)list;(void)flags;(void)previous;(void)returned;++update;return mode!=3 && attribute==123 && value==&storage && size==sizeof(storage); }
static void fake_delete(LPPROC_THREAD_ATTRIBUTE_LIST list) { (void)list;++release; }
static BOOL fake_free(HANDLE heap,DWORD flags,LPVOID memory)
{ (void)heap;(void)flags;(void)memory;++freed;return TRUE; }
static DWORD fake_error(void) { return error_code; }
#define InitializeProcThreadAttributeList fake_init
#define UpdateProcThreadAttribute fake_update
#define DeleteProcThreadAttributeList fake_delete
#define HeapAlloc fake_alloc
#define HeapFree fake_free
#define GetLastError fake_error
#include "lib-win32/process-attributes/attributes.c"
#define CHECK(x) do{if(!(x)){fprintf(stderr,"process attributes failed line %d\n",__LINE__);return 1;}++checks;}while(0)
int main(void)
{
    win32_process_attributes state={0};int checks=0;
    CHECK(win32_process_attributes_init(NULL,123,&storage,sizeof(storage))==ERROR_INVALID_PARAMETER);
    for(mode=0;mode<=3;++mode){
        DWORD result;probe=init=update=release=freed=0;
        result=win32_process_attributes_init(&state,123,&storage,sizeof(storage));
        CHECK(result==(mode==0?ERROR_SUCCESS:mode==1?ERROR_NOT_ENOUGH_MEMORY:error_code));
        CHECK(probe==1);
        if(mode!=1) CHECK(win32_process_attributes_init(&state,123,&storage,sizeof(storage))==ERROR_INVALID_STATE);
        CHECK(init==(mode==1?0:1));
        CHECK(update==(mode<2?mode==0:mode==3));
        win32_process_attributes_destroy(&state);
        CHECK(release==(mode==0 || mode==3));
        CHECK(freed==(mode!=1));
        CHECK(state.list==NULL && !state.initialized);
        win32_process_attributes_destroy(&state);
        CHECK(release==(mode==0 || mode==3) && freed==(mode!=1));
    }
    win32_process_attributes_destroy(NULL);
    printf("PASS process attribute lifecycle checks=%d\n",checks);return 0;
}
