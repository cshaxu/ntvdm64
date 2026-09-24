#include "wow_cleanup_bindings.h"
#include "wow_class_words_binding.h"
#include "wow_window_creation_binding.h"
#include <stdio.h>

static unsigned errors, released, destroyed;
static BOOL fail_alloc;
static BOOL raise_alloc;
static unsigned live_callbacks;
static wow_cleanup_context *tree_context;
static wow_cleanup_window *tree_parent, *tree_child;
static BOOL expect_nested;
static unsigned tree_checks;
#define CHECK(x) do { if (!(x)) { ++errors; printf("FAIL line=%d error=%lu\n",__LINE__,GetLastError()); } } while (0)
static void verify_native_identity_binding(void)
{
    static wow_user_object_table table;
    wow_cleanup_window first={0}, second={0};
    wow_task_order_thread thread={0};
    HWND old_handle=(HWND)0x11111234, new_handle=(HWND)0x22221234;
    CHECK(sizeof(wow_cleanup_handle)==12 && sizeof(table.entries)==0xc0000);
    CHECK(!wow_user_window_publish(&table,NULL,&first,&thread));
    CHECK(!wow_user_window_publish(&table,(HWND)0xffff,&first,&thread));
    CHECK(table.last_handle==0);
    CHECK(wow_user_window_publish(&table,old_handle,&first,&thread));
    CHECK(!wow_user_window_publish(&table,new_handle,&second,&thread));
    CHECK(GetLastError()==ERROR_ALREADY_EXISTS);
    CHECK(!wow_user_window_retire(&table,new_handle,&first,NULL));
    CHECK(!wow_user_window_retire(&table,old_handle,&second,NULL));
    CHECK(table.entries[0x1234].phead==&first);
    CHECK(wow_user_window_retire(&table,old_handle,&first,NULL));
    CHECK(wow_user_window_publish(&table,new_handle,&second,&thread));
    CHECK(!wow_user_window_retire(&table,old_handle,&first,NULL));
    CHECK(table.entries[0x1234].phead==&second &&
        table.entries[0x1234].wUniq==HIWORD(new_handle));
    CHECK(wow_user_window_retire(&table,new_handle,&second,NULL));
    CHECK(table.entries[0x1234].bType==TYPE_FREE && table.last_handle==0x1234);
}
static PVOID WINAPI allocate(SIZE_T size)
{
    if (raise_alloc) RaiseException(0xE0420480,0,0,NULL);
    if (tree_context) {
        wow_cleanup_window *root=tree_context->thread->rpdesk->pDeskInfo->spwnd;
        ++tree_checks;
        if (expect_nested) {
            CHECK(root->spwndChild==tree_parent && tree_parent->spwndNext==NULL);
            CHECK(tree_parent->spwndChild==tree_child && tree_child->spwndNext==NULL);
        } else {
            CHECK(tree_parent->spwndChild==NULL && tree_child->spwndChild==NULL);
            CHECK((root->spwndChild==tree_parent && tree_parent->spwndNext==tree_child) ||
                (root->spwndChild==tree_child && tree_child->spwndNext==tree_parent));
        }
    }
    return fail_alloc ? NULL : HeapAlloc(GetProcessHeap(),0,size);
}
static void WINAPI release(PVOID memory)
{ ++released; CHECK(HeapFree(GetProcessHeap(),0,memory)); }
static BOOL WINAPI destroy_class(wow_cleanup_context *ctx, wow_class_lookup_entry **link)
{
    (void)ctx;
    CHECK((*link)->lpszClientAnsiMenuName==NULL &&
        (*link)->lpszClientUnicodeMenuName==NULL);
    return wow_class_words_destroy_native(link,(*link)->hModule,TRUE);
}
typedef struct resource_object { wow_cleanup_process_object head; HANDLE handle; } resource_object;
static void WINAPI destroy_object(wow_cleanup_context *ctx, wow_cleanup_handle *entry)
{
    resource_object *object=entry->phead;
    (void)ctx;
    if (entry->bType==TYPE_MENU) CHECK(DestroyMenu((HMENU)object->handle));
    else if (entry->bType==TYPE_CURSOR) CHECK(DestroyCursor((HCURSOR)object->handle));
    else if (entry->bType==TYPE_ACCELTABLE) CHECK(DestroyAcceleratorTable((HACCEL)object->handle));
    else CHECK(FALSE);
    entry->bType=0;
    ++destroyed;
}
static LRESULT CALLBACK wndproc(HWND window, UINT message, WPARAM wp, LPARAM lp)
{ return DefWindowProcA(window,message,wp,lp); }

static DWORD WINAPI live_callback(HWND window, UINT message, UINT wp, LONG lp,
    DWORD procedure, PVOID words)
{
    CHECK(procedure==0x92350020 && words!=NULL);
    if (message==WM_USER+42) { ++live_callbacks; return 0x7654; }
    return (DWORD)DefWindowProcA(window,message,wp,lp);
}

static void live_dispatch_cleanup(wow_cleanup_context *ctx, int cleanup_mode)
{
    WNDCLASSEXA wc={sizeof(wc)};
    CREATESTRUCTA creation={0};
    UNICODE_STRING name;
    DWORD class_words[2]={0x12350020,0}, procedure;
    WW words={0};
    wow_window_dispatch_target target={0x92350020,live_callback};
    wow_window_words_binding *borrow;
    wow_cleanup_window *live;
    HWND window;
    HWND later;
    HWND parent=NULL, helper=NULL, child=NULL;
    LONG helper_proc=0;
    NEMODULESEG segment={0};
    ATOM atom;
    unsigned before;
    wc.lpfnWndProc=wow_window_native_proc; wc.hInstance=(HINSTANCE)0x34561234;
    wc.lpszClassName="S40_CLEANUP_LIVE";
    RtlInitUnicodeString(&name,L"S40_CLEANUP_LIVE");
    atom=wow_class_words_register_target(ctx->thread->ppi->classes,&wc,&name,
        class_words,NULL,&target);
    CHECK(atom!=0); if (!atom) return;
    creation.hInstance=wc.hInstance; creation.lpszClass=wc.lpszClassName;
    creation.lpszName=""; creation.cx=creation.cy=32;
    if (cleanup_mode==2) {
        parent=CreateWindowExA(0,"STATIC","",0,0,0,32,32,NULL,NULL,GetModuleHandleA(NULL),NULL);
        CHECK(parent!=NULL);
        creation.style=WS_CHILD; creation.hwndParent=parent;
    }
    CHECK(!wow_window_create_boundA(NULL,atom,&creation,&words,NULL));
    CHECK(GetLastError()==ERROR_INVALID_PARAMETER);
    window=wow_window_create_boundA(ctx->thread,atom,&creation,&words,NULL);
    CHECK(window!=NULL); if (!window) return;
    borrow=wow_window_words_acquire(window); CHECK(borrow!=NULL);
    live=wow_window_words_cleanup_value(borrow);
    CHECK(live!=NULL && live->pcls==ctx->thread->ppi->classes->pclsPrivateList);
    CHECK(live->thread==ctx->thread);
    CHECK(ctx->thread->ppi->objects->entries[LOWORD(window)].phead==live &&
        ctx->thread->ppi->objects->entries[LOWORD(window)].pOwner==ctx->thread);
    if (cleanup_mode==0) {
        wow_cleanup_window *saved_root=ctx->thread->rpdesk->pDeskInfo->spwnd;
        BOOL caught=FALSE;
        fail_alloc=TRUE;
        CHECK(!wow_user_cleanup_bound((HANDLE)0x9999,0,&segment,1,ctx));
        fail_alloc=FALSE;
        CHECK(ctx->thread->rpdesk->pDeskInfo->spwnd==saved_root);
        CHECK(live->spwndChild==NULL && live->spwndNext==NULL);
        raise_alloc=TRUE;
        __try { wow_user_cleanup_bound((HANDLE)0x9999,0,&segment,1,ctx); }
        __except (GetExceptionCode()==0xE0420480 ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
            caught=TRUE;
        }
        raise_alloc=FALSE;
        CHECK(caught && ctx->thread->rpdesk->pDeskInfo->spwnd==saved_root);
        CHECK(live->spwndChild==NULL && live->spwndNext==NULL);
    }
    if (cleanup_mode==2) {
        helper=CreateWindowExA(0,"STATIC","",WS_CHILD,0,0,16,16,window,NULL,GetModuleHandleA(NULL),NULL);
        CHECK(helper!=NULL);
        helper_proc=GetWindowLongA(helper,GWL_WNDPROC);
        creation.hwndParent=helper;
        child=wow_window_create_boundA(ctx->thread,atom,&creation,&words,NULL);
        CHECK(child!=NULL);
        CHECK(SendMessageA(child,WM_USER+42,0,0)==0x7654);
    }
    CHECK(SendMessageA(window,WM_USER+42,0,0)==0x7654);
    before=live_callbacks;
    if (cleanup_mode==2) {
        wow_window_words_binding *child_borrow=wow_window_words_acquire(child);
        CHECK(child_borrow!=NULL);
        tree_context=ctx; tree_parent=live;
        tree_child=wow_window_words_cleanup_value(child_borrow);
        expect_nested=TRUE;
        CHECK(wow_user_cleanup_bound((HANDLE)0x9999,0,&segment,1,ctx));
        CHECK(SetParent(child,parent)==helper);
        expect_nested=FALSE;
        CHECK(wow_user_cleanup_bound((HANDLE)0x9999,0,&segment,1,ctx));
        CHECK(SetParent(child,helper)==parent);
        tree_context=NULL;
        wow_window_words_release(child_borrow);
        CHECK(wow_user_cleanup_bound((HANDLE)0x3456,0,NULL,0,ctx));
        CHECK((live->pcls->flags & CSF_WOWDEFERDESTROY)!=0);
        CHECK(SendMessageA(child,WM_USER+42,0,0)==0 && live_callbacks==before);
        CHECK(GetWindowLongA(helper,GWL_WNDPROC)==helper_proc);
    } else if (cleanup_mode==1) {
        segment.ns_handle=0x1234;
        CHECK(wow_user_cleanup_bound((HANDLE)0x9999,0,&segment,1,ctx));
        CHECK(live->pcls->lpfnWndProc==DefWindowProcW && live->pcls->adwWOW[0]==0);
        later=wow_window_create_boundA(ctx->thread,atom,&creation,&words,NULL);
        CHECK(later!=NULL);
        if (later) {
            CHECK(wow_window_procedure_read(later,&procedure) && procedure==(DWORD)DefWindowProcW);
            CHECK(SendMessageA(later,WM_USER+42,0,0)==0 && live_callbacks==before);
            CHECK(DestroyWindow(later));
        }
    } else {
        CHECK(wow_user_cleanup_bound((HANDLE)1,ctx->thread->ptdb->hTaskWow,NULL,0,ctx));
    }
    CHECK(live->lpfnWndProc==DefWindowProcA);
    CHECK(wow_window_procedure_read(window,&procedure) && procedure==(DWORD)DefWindowProcA);
    CHECK((WNDPROC)GetWindowLongA(window,GWL_WNDPROC)==wow_window_native_proc);
    CHECK(SendMessageA(window,WM_USER+42,0,0)==0 && live_callbacks==before);
    CHECK(live->spwndChild==NULL && live->spwndNext==NULL);
    if (parent) CHECK(DestroyWindow(parent));
    else CHECK(DestroyWindow(window));
    CHECK(live->pcls->cWndReferenceCount==0);
    CHECK(live->thread==NULL);
    CHECK(ctx->handles.aheList[LOWORD(window)].bType==TYPE_FREE);
    if (cleanup_mode==2) CHECK(ctx->destroy_class(ctx,&ctx->thread->ppi->classes->pclsPrivateList));
    else CHECK(wow_class_words_unregister(ctx->thread->ppi->classes,atom,wc.hInstance,NULL));
    CHECK(live->lpfnWndProc==DefWindowProcA); /* Outstanding borrow survives. */
    wow_window_words_release(borrow);
}

int __cdecl main(void)
{
    wow_class_lookup_context classes={0};
    wow_task_order_process process={0}, other_process={0};
    static wow_user_object_table objects;
    wow_task_order_entry task={0};
    wow_task_order_thread thread={0}, foreign={0};
    wow_input_desktop desktop={0};
    wow_cleanup_desktop_info deskinfo={0};
    wow_cleanup_window root={0}, window_view={0}, foreign_view={0};
    wow_cleanup_shared shared={0};
    wow_cleanup_handle handles[7]={0};
    wow_cleanup_context ctx={0};
    WNDCLASSEXA wc={sizeof(wc)};
    UNICODE_STRING name;
    DWORD words[2]={0x12350020,0x87654321};
    NEMODULESEG segment={0};
    resource_object menus[3], cursor={0}, accel={0};
    BYTE and_mask[128], xor_mask[128]={0};
    ACCEL accelerator={FVIRTKEY,'A',100};
    ATOM atom;
    HWND window;
    wow_class_lookup_entry *entry;
    PVOID inaccessible;
    unsigned before, i;
    verify_native_identity_binding();
    process.classes=&classes;
    process.objects=&objects;
    thread.ppi=&process; thread.ptdb=&task; thread.TIF_flags=WOW_TASK_TIF_16BIT;
    task.hTaskWow=0x4321;
    thread.rpdesk=&desktop; desktop.pDeskInfo=&deskinfo; deskinfo.spwnd=&root;
    foreign.ppi=&other_process; foreign.ptdb=&task; foreign.TIF_flags=WOW_TASK_TIF_16BIT;
    shared.apfnClientA.pfnDefWindowProc=(PROC)DefWindowProcA;
    shared.apfnClientW.pfnDefWindowProc=(PROC)DefWindowProcW;
    ctx.thread=&thread; ctx.shared=&shared; ctx.handles.aheList=handles;
    ctx.last_handle=6;
    ctx.destroy_class=destroy_class; ctx.destroy_object=destroy_object;
    ctx.allocate=allocate; ctx.release=release;
    wc.lpfnWndProc=wndproc; wc.hInstance=(HINSTANCE)0x23451234;
    wc.lpszClassName="S40_CLEANUP";
    RtlInitUnicodeString(&name,L"S40_CLEANUP");
    atom=wow_class_words_register(&classes,&wc,&name,words,NULL);
    CHECK(atom!=0); if (!atom) return 3;
    entry=classes.pclsPrivateList;
    window=CreateWindowExA(0,wc.lpszClassName,"",0,0,0,32,32,NULL,NULL,wc.hInstance,NULL);
    CHECK(window!=NULL); if (!window) return 3;
    /* Real native rejection, not a mock return: stop with the node retained.
     * No cleanup loop can claim to have removed the live native class. */
    CHECK(!_WOWCleanup((HANDLE)0x2345,0,NULL,0,&ctx));
    CHECK(GetLastError()==ERROR_CLASS_HAS_WINDOWS && classes.pclsPrivateList==entry);
    CHECK(DestroyWindow(window));
    CHECK(_WOWCleanup((HANDLE)0x2345,0,NULL,0,&ctx));
    CHECK(classes.pclsPrivateList==NULL);

    atom=wow_class_words_register(&classes,&wc,&name,words,NULL);
    CHECK(atom!=0); if (!atom) return 3;
    entry=classes.pclsPrivateList; entry->hTaskWow=task.hTaskWow;
    window=CreateWindowExA(0,wc.lpszClassName,"",0,0,0,32,32,NULL,NULL,wc.hInstance,NULL);
    CHECK(window!=NULL); if (!window) return 3;
    /* Controlled owner views exercise original policy, not production HWND
     * enumeration or a claim that live worker object publication exists. */
    root.spwndChild=&window_view; window_view.pcls=entry; window_view.thread=&thread;
    window_view.lpfnWndProc=(WNDPROC)0x92350020;
    entry->cWndReferenceCount=1;
    CHECK(_WOWCleanup((HANDLE)0x2345,0,NULL,0,&ctx));
    CHECK((entry->flags & CSF_WOWDEFERDESTROY)!=0);
    CHECK(window_view.lpfnWndProc==DefWindowProcA);

    segment.ns_handle=0x1234;
    entry->cWndReferenceCount=0;
    shared.atomSysClass[19]=atom; /* Not in the 13-entry procedure table. */
    handles[0].bType=1; handles[0].phead=&window_view; handles[0].pOwner=&thread;
    handles[1].bType=1; handles[1].phead=&foreign_view; handles[1].pOwner=&foreign;
    window_view.lpfnWndProc=(WNDPROC)0x92350020;
    foreign_view.lpfnWndProc=(WNDPROC)0x92350020;
    CHECK(_WOWCleanup((HANDLE)0x9999,0,&segment,1,&ctx));
    CHECK(window_view.lpfnWndProc==DefWindowProcA);
    CHECK(foreign_view.lpfnWndProc==(WNDPROC)0x92350020);
    CHECK(entry->adwWOW[0]==0 && entry->lpfnWndProc==DefWindowProcW);

    window_view.lpfnWndProc=wndproc;
    window_view.serverSideProc=TRUE;
    CHECK(_WOWCleanup((HANDLE)1,task.hTaskWow,NULL,0,&ctx));
    CHECK(window_view.lpfnWndProc==wndproc);
    window_view.serverSideProc=FALSE;
    CHECK(_WOWCleanup((HANDLE)1,task.hTaskWow,NULL,0,&ctx));
    CHECK(window_view.lpfnWndProc==DefWindowProcA);
    CHECK(foreign_view.lpfnWndProc==(WNDPROC)0x92350020);

    for (i=0;i<3;++i) {
        menus[i].head.hTaskWow=task.hTaskWow;
        menus[i].handle=CreateMenu(); CHECK(menus[i].handle!=NULL);
        handles[i+2].bType=2; handles[i+2].phead=&menus[i];
        handles[i+2].pOwner=&process;
    }
    handles[3].pOwner=&other_process;
    handles[4].bFlags=1;
    FillMemory(and_mask,sizeof(and_mask),0xff);
    cursor.head.hTaskWow=accel.head.hTaskWow=task.hTaskWow;
    cursor.handle=CreateCursor(GetModuleHandleA(NULL),0,0,32,32,and_mask,xor_mask);
    accel.handle=CreateAcceleratorTableA(&accelerator,1);
    CHECK(cursor.handle && accel.handle);
    handles[5].bType=TYPE_CURSOR; handles[5].phead=&cursor; handles[5].pOwner=&process;
    handles[6].bType=TYPE_ACCELTABLE; handles[6].phead=&accel; handles[6].pOwner=&process;
    CHECK(!_WOWCleanup(NULL,task.hTaskWow,NULL,0,&ctx));
    CHECK(GetLastError()==ERROR_CLASS_HAS_WINDOWS && classes.pclsPrivateList==entry);
    CHECK(destroyed==0 && IsMenu((HMENU)menus[0].handle));
    CHECK(DestroyWindow(window));
    CHECK(_WOWCleanup(NULL,task.hTaskWow,NULL,0,&ctx));
    CHECK(classes.pclsPrivateList==NULL && destroyed==3);
    CHECK(!IsMenu((HMENU)menus[0].handle) && IsMenu((HMENU)menus[1].handle) && IsMenu((HMENU)menus[2].handle));
    CHECK(handles[5].bType==TYPE_FREE && handles[6].bType==TYPE_FREE);
    CHECK(DestroyMenu((HMENU)menus[1].handle)); CHECK(DestroyMenu((HMENU)menus[2].handle));
    ZeroMemory(handles,sizeof(handles));

    fail_alloc=TRUE;
    CHECK(!_WOWCleanup(NULL,task.hTaskWow,&segment,1,&ctx));
    fail_alloc=FALSE;
    inaccessible=VirtualAlloc(NULL,4096,MEM_COMMIT|MEM_RESERVE,PAGE_NOACCESS);
    CHECK(inaccessible!=NULL);
    before=released;
    CHECK(!_WOWCleanup(NULL,task.hTaskWow,inaccessible,1,&ctx));
    CHECK(released==before+1);
    CHECK(VirtualFree(inaccessible,0,MEM_RELEASE));
    live_dispatch_cleanup(&ctx,FALSE);
    live_dispatch_cleanup(&ctx,TRUE);
    live_dispatch_cleanup(&ctx,2);
    CHECK(tree_checks==2);
    printf("WOW_CLEANUP errors=%u module=1 task=1 thread=1 native_failure=2 bound=13 probe_failure=1 live_dispatch=%u resource_kinds=3 native_membership=1 stale_retire=1 native_tree=%u\n",errors,live_callbacks,tree_checks);
    return errors ? 3 : 0;
}
