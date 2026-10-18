/* Resource-only unit of the exact generated production full-copy body.
 * Lower-level projection throws deliberately. This is not a CCPU/guest,
 * window-identity, membership-algorithm or performance substitute. */
#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "ntvdm-exe/softpc/include/mvdm_softpc_wow_page_domain.h"
#define MVDM_SOFTPC_HANDLE_ENTRY_COUNT 0x10000u
typedef struct mvdm_softpc_wow_class_allocation {
    struct mvdm_softpc_wow_class_allocation *next;
    ULONG client;
} mvdm_softpc_wow_class_allocation;
typedef struct mvdm_softpc_wow_window_borrow {
    struct mvdm_softpc_wow_window_borrow *next;
    ULONG backing;
} mvdm_softpc_wow_window_borrow;
static struct { ULONG desktop_handle, guest_desktop_window, client_delta;
    ULONG guest_windows[MVDM_SOFTPC_HANDLE_ENTRY_COUNT]; } domain;
static mvdm_softpc_wow_class_allocation *class_allocations;
static mvdm_softpc_wow_window_borrow *window_borrows;
static unsigned errors, allocations, frees, mode;
static BOOL allocation_failure;
static void *live;
#define CHECK(x) do {if(!(x)){++errors;fprintf(stderr,"FAIL %d error=%lu\n",__LINE__,GetLastError());}}while(0)
static void *tracked_alloc(HANDLE heap,DWORD flags,SIZE_T bytes)
{
    void *result;
    if(allocation_failure){SetLastError(0x44729300);return NULL;}
    result=HeapAlloc(heap,flags,bytes);
    CHECK(result && !live);live=result;if(result)++allocations;return result;
}
static BOOL tracked_free(HANDLE heap,DWORD flags,void *value)
{
    BOOL result;
    CHECK(value && value==live);result=HeapFree(heap,flags,value);
    if(result){live=NULL;++frees;}SetLastError(0x4472ffff);return result;
}
static int page_domain_menu_context(void){return 1;}
static mvdm_softpc_wow_window_borrow **window_borrow_link(ULONG backing)
{(void)backing;return &window_borrows;}
static int window_borrow_is_current(const mvdm_softpc_wow_window_borrow *borrow)
{return borrow!=NULL;}
static int copy_window_projection(ULONG backing,
    const mvdm_softpc_wow_window_borrow *borrow,ULONG index,
    mvdm_softpc_wow_window_observation *rows,
    mvdm_softpc_wow_window_menu_observation *menus,const ULONG *classes,ULONG count)
{
    (void)borrow;
    if(classes)CHECK(count==2 && classes[0]==0x80001000u && classes[1]==0x80002000u);
    if(mode==1){SetLastError(0x44729301);return 0;}
    if(mode==2){SetLastError(0x44729302);RaiseException(0xe0447293,0,0,NULL);}
    if(menus){menus[index].window=backing;menus[index].style=0;menus[index].process_id=1;}
    else {ZeroMemory(&rows[index],sizeof(rows[index]));rows[index].links.window=backing;}
    return 1;
}
static int __cdecl compare_window_observation_slot(const void *a,const void *b)
{ULONG x=((const mvdm_softpc_wow_window_observation *)a)->links.window;
 ULONG y=((const mvdm_softpc_wow_window_observation *)b)->links.window;return(x>y)-(x<y);}
static int __cdecl compare_window_menu_slot(const void *a,const void *b)
{ULONG x=((const mvdm_softpc_wow_window_menu_observation *)a)->window;
 ULONG y=((const mvdm_softpc_wow_window_menu_observation *)b)->window;return(x>y)-(x<y);}
#define HeapAlloc tracked_alloc
#define HeapFree tracked_free
#include "class_copy_body.h"
#undef HeapAlloc
#undef HeapFree
int main(void)
{
    mvdm_softpc_wow_class_allocation a={NULL,0x1000},b={&a,0x2000};
    mvdm_softpc_wow_window_borrow borrow={NULL,2};
    mvdm_softpc_wow_window_observation rows[2];
    mvdm_softpc_wow_window_menu_observation menus[2];
    ULONG count=0;
    unsigned pass;
    BOOL result,caught;
    domain.desktop_handle=domain.guest_desktop_window=1;domain.client_delta=0x80000000u;
    domain.guest_windows[2]=2;class_allocations=&b;window_borrows=&borrow;
    for(pass=0;pass<3;++pass){
        mode=pass;caught=FALSE;result=FALSE;allocations=frees=0;
        SetLastError(0x44729303);
        __try{result=copy_current_window_view(2,rows,NULL,&count);}
        __except(GetExceptionCode()==0xe0447293?EXCEPTION_EXECUTE_HANDLER:EXCEPTION_CONTINUE_SEARCH){caught=TRUE;}
        CHECK(count==2 && allocations==1 && frees==1 && !live);
        CHECK(caught==(mode==2) && result==(mode==0));
        CHECK(GetLastError()==(mode==0?0x44729303u:mode==1?0x44729301u:0x44729302u));
        /* Clean an old-body negative after recording the leak as failure. */
        if(live){HeapFree(GetProcessHeap(),0,live);live=NULL;}
    }
    mode=0;allocation_failure=TRUE;allocations=frees=0;
    SetLastError(0x44729303);
    CHECK(copy_current_window_view(2,rows,NULL,&count));
    CHECK(!allocations && !frees && !live && GetLastError()==0x44729303);
    allocation_failure=FALSE;
    CHECK(copy_current_window_view(2,NULL,menus,&count));
    CHECK(!allocations && !frees && count==2);
    class_allocations=NULL;window_borrows=NULL;domain.guest_windows[2]=0;
    CHECK(copy_current_window_view(2,rows,NULL,&count) && count==1);
    CHECK(!allocations && !frees);
    printf("CLASS_COPY_BODY_UNWIND errors=%u normal=1 bool_failure=1 exception=1 allocation_fallback=1 menu=1 cold=1\n",errors);
    return errors!=0;
}
