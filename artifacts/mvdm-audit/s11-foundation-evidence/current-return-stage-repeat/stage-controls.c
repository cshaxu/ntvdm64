#include <windows.h>
#include <stdio.h>
#include <string.h>
typedef struct {int wait_publication_pending;} wow_user_task_lifecycle_thread;
typedef struct {void *lifecycle;} runtime_t;
typedef struct {void *ppi;} thread_t;
typedef struct {runtime_t *runtime;thread_t *thread;} wow_user_runtime_thread;
typedef struct {int unused;} wow_user_native_call;
static wow_user_task_lifecycle_thread task;static int fail,windows_calls,borrow_calls,menu_calls;
static wow_user_task_lifecycle_thread *current_task(void *p){(void)p;return &task;}
static BOOL publish_call_windows(const wow_user_native_call *p){(void)p;windows_calls++;if(fail==1){SetLastError(1400);return FALSE;}return TRUE;}
static BOOL wow_user_window_refresh_graph(void *p){return publish_call_windows((const wow_user_native_call*)p);}
static BOOL wow_user_borrow_client_graph(void *p,HWND w){(void)p;(void)w;borrow_calls++;if(fail==2){SetLastError(1400);return FALSE;}return TRUE;}
static BOOL wow_menu_client_refresh_all(void *p){(void)p;menu_calls++;if(fail==3){SetLastError(1400);return FALSE;}return TRUE;}
static void retirement_trace(const char *stage, DWORD task, DWORD status)
{
    char path[MAX_PATH], line[160];
    DWORD error = GetLastError(), size, written, path_length;
    HANDLE file;
    path_length = GetEnvironmentVariableA("MVDM_WOW_WINDOW_TRACE_PATH", path, sizeof(path));
    if (path_length && path_length < sizeof(path)) {
        file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (file != INVALID_HANDLE_VALUE) {
            size = (DWORD)sprintf_s(line, sizeof(line),
                "%lu TaskRetirement tid=%lu task=%08lX stage=%s status=%lu\r\n",
                GetCurrentProcessId(), GetCurrentThreadId(), task, stage, status);
            if (size) (void)WriteFile(file, line, size, &written, NULL);
            CloseHandle(file);
        }
    }
    SetLastError(error);
}

static BOOL publish_guest_return(wow_user_runtime_thread *binding,
    const wow_user_native_call *call)
{
    wow_user_task_lifecycle_thread *task = current_task(binding->runtime->lifecycle);
    BOOL published = call ? publish_call_windows(call) : wow_user_window_refresh_graph(NULL);
    if (!published) retirement_trace("diag-publish-window", 0, GetLastError());
    if (published) { published = wow_user_borrow_client_graph(NULL, GetDesktopWindow());
        if (!published) retirement_trace("diag-publish-borrow", 0, GetLastError()); }
    if (published) { published = wow_menu_client_refresh_all(binding->thread->ppi);
        if (!published) retirement_trace("diag-publish-menu", 0, GetLastError()); }
    /* An ordinary native return may have already discharged a prior wait.
     * Do not clear on partial failure or unwind. This is a call-boundary
     * obligation, not an independent HWND/graph dirty-state authority. */
    if (published && task) task->wait_publication_pending = FALSE;
    return published;
}

int main(int argc,char **argv){runtime_t r={0};thread_t t={0};wow_user_runtime_thread b={&r,&t};wow_user_native_call call={0};const char *names[]={"","diag-publish-window","diag-publish-borrow","diag-publish-menu"}; int i,j; FILE *f;char content[4096];size_t n;BOOL ok;DWORD err;
if(argc!=2)return 1;
SetEnvironmentVariableA("MVDM_WOW_WINDOW_TRACE_PATH",argv[1]);
for(i=0;i<=3;i++)for(j=0;j<2;j++){DeleteFileA(argv[1]);fail=i;windows_calls=borrow_calls=menu_calls=0;task.wait_publication_pending=1;SetLastError(777);ok=publish_guest_return(&b,j?&call:NULL);err=GetLastError();if(ok!=(i==0)||task.wait_publication_pending!=(i!=0)||windows_calls!=1||borrow_calls!=(i!=1)||menu_calls!=(i==0||i==3)||(i&&err!=1400))return 10+i*2+j;
f=fopen(argv[1],"rb");if(i==0){if(f){fclose(f);return 30;}continue;} if(!f)return 31;n=fread(content,1,sizeof(content)-1,f);fclose(f);content[n]=0;if(!strstr(content,names[i])||!strstr(content,"status=1400"))return 32;printf("stage=%s path=%s error=%lu pending=%d windows=%d borrow=%d menu=%d\n",names[i],j?"call":"initial",(unsigned long)err,task.wait_publication_pending,windows_calls,borrow_calls,menu_calls);}
DeleteFileA(argv[1]);return 0;}
