#include <windows.h>
#include <stdio.h>
typedef unsigned long ULONG;
typedef struct { int pending; } wow_user_task_lifecycle_thread;
typedef struct { void *lifecycle; } runtime;
typedef struct { void *ppi; } thread;
typedef struct { runtime *runtime; thread *thread; } wow_user_runtime_thread;
typedef struct { ULONG returned_count; HWND returned_payload[4]; HWND returned_window, target_window; HMENU returned_menu; } wow_user_native_call;
#define wait_publication_pending pending
static int fail_stage, nwin, nmenu, ngraph, nall, traces; static DWORD trace_error; static const char *trace_stage;
static void retirement_trace(const char *stage,DWORD object,DWORD error) { (void)object; if (!traces) { trace_error=error;trace_stage=stage; } traces++; }
static BOOL wow_user_window_refresh_payload(HWND h,const HWND *p,ULONG c) { (void)h;(void)p;(void)c;nwin++; if(fail_stage==1){SetLastError(1400);return FALSE;} return TRUE; }
static BOOL wow_user_window_refresh_graph(HWND h) { return wow_user_window_refresh_payload(h,0,0); }
#define IsMenu my_is_menu
static BOOL my_is_menu(HMENU h) { return h != 0; }
static BOOL wow_menu_native_refresh_menu(HMENU h) { (void)h;nmenu++;if(fail_stage==2){SetLastError(1410);return FALSE;} return TRUE; }
static BOOL wow_user_borrow_client_graph(void *r,HWND h) { (void)r;(void)h;ngraph++;if(fail_stage==3){SetLastError(5023);return FALSE;}return TRUE; }
#define GetDesktopWindow my_desktop
static HWND my_desktop(void) {return (HWND)3;}
static BOOL wow_menu_client_refresh_all(void *p){(void)p;nall++;if(fail_stage==4){SetLastError(5);return FALSE;}return TRUE;}
static wow_user_task_lifecycle_thread task;
static wow_user_task_lifecycle_thread *current_task(void *p){(void)p;return &task;}
static BOOL publish_call_windows(const wow_user_native_call *call)
{
    HWND roots[6];
    ULONG count = call->returned_count;
    if (count > 4) { SetLastError(ERROR_INVALID_DATA); return FALSE; }
    CopyMemory(roots, call->returned_payload, count * sizeof(*roots));
    roots[count++] = call->returned_window;
    roots[count++] = call->target_window;
    if (!wow_user_window_refresh_payload(NULL, roots, count)) {
        retirement_trace("return-window-failed", (DWORD)(ULONG_PTR)call->target_window, GetLastError());
        return FALSE;
    }
    if (call->returned_menu && IsMenu(call->returned_menu) &&
            !wow_menu_native_refresh_menu(call->returned_menu)) {
        retirement_trace("return-returned-menu-failed", (DWORD)(ULONG_PTR)call->returned_menu, GetLastError());
        return FALSE;
    }
    return TRUE;
}

static BOOL publish_guest_return(wow_user_runtime_thread *binding,
    const wow_user_native_call *call)
{
    wow_user_task_lifecycle_thread *task = current_task(binding->runtime->lifecycle);
    BOOL published = (call ? publish_call_windows(call) : wow_user_window_refresh_graph(NULL));
    if (!published) retirement_trace("return-payload-failed", 0, GetLastError());
    if (published) {
        published = wow_user_borrow_client_graph(NULL, GetDesktopWindow());
        if (!published) retirement_trace("return-graph-failed", 0, GetLastError());
    }
    if (published) {
        published = wow_menu_client_refresh_all(binding->thread->ppi);
        if (!published) retirement_trace("return-menu-failed", 0, GetLastError());
    }
    /* An ordinary native return may have already discharged a prior wait.
     * Do not clear on partial failure or unwind. This is a call-boundary
     * obligation, not an independent HWND/graph dirty-state authority. */
    if (published && task) task->wait_publication_pending = FALSE;
    return published;
}


int main(void) {
 runtime r={0}; thread t={0}; wow_user_runtime_thread binding={&r,&t}; wow_user_native_call call={0};
 int i; DWORD errors[]={0,1400,1410,5023,5}; const char *stages[]={"","return-window-failed","return-returned-menu-failed","return-graph-failed","return-menu-failed"};
 call.target_window=(HWND)7;call.returned_menu=(HMENU)9;
 for(i=0;i<5;i++) {BOOL ok; fail_stage=i;nwin=nmenu=ngraph=nall=traces=0;trace_error=0;trace_stage="";task.pending=1;SetLastError(777);
 ok=publish_guest_return(&binding,&call);
 if ((i==0) != !!ok || task.pending != (i!=0) || nwin!=1 || nmenu!=(i!=1) || ngraph!=(i==0||i>=3) || nall!=(i==0||i==4)) return 10+i;
 if (i && (GetLastError()!=errors[i] || trace_error!=errors[i] || strcmp(trace_stage,stages[i]))) return 20+i;
 }
 call.returned_count=5;SetLastError(777);if(publish_call_windows(&call)||GetLastError()!=ERROR_INVALID_DATA) return 30;
 puts("RETURN_STAGE_FAILURE_CONTRACT_OK 5 stages plus invalid payload");return 0;
}
