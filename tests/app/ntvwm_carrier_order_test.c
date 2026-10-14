/* Test the actual main.c call ordering with finite typed boundary substitutes. */
#define _WIN32_WINNT 0x0A00
#include <windows.h>
/* Keep the unused worker entry local so this fixture links only the reached
 * production completion/close functions, not a second worker executable. */
static int unused_worker_entry(int,WCHAR **);
#define wmain unused_worker_entry
#include "../../src/ntvwm-exe/main.c"
#undef wmain

static char calls[64];static unsigned used;
static DWORD checkpoint_error,paint_error,hide_error,bind_error,route;
static void record(char c){calls[used++]=c;calls[used]=0;}
DWORD worker_base_io_checkpoint(DWORD reason,DWORD request,DWORD *decision)
{ (void)reason;(void)request;record('C');*decision=route;return checkpoint_error; }
DWORD worker_base_input_watch_bind(worker_base_input_watch *watch,HANDLE source)
{ (void)watch;(void)source;record('B');return bind_error; }
DWORD ntvwm_presentation_end(ntvwm_presentation *p,const console_text_style *font)
{ (void)p;(void)font;record('E');return paint_error; }
DWORD ntvwm_presentation_flush(ntvwm_presentation *p,const console_text_style *font)
{ (void)p;(void)font;record('F');return paint_error; }
void ntvwm_presentation_close(ntvwm_presentation *p){(void)p;record('P');}
DWORD worker_base_io_close(HANDLE *pipe,HANDLE *peer,HANDLE *ready)
{ (void)pipe;(void)peer;(void)ready;record('R');return 0; }
DWORD ntvwm_console_hide(HWND window){(void)window;record('H');return hide_error;}
void ntvwm_trace_error(const char *stage,DWORD operation,DWORD error)
{ (void)stage;(void)operation;(void)error; }
void ntvwm_console_guard_stop(ntvwm_console_guard *guard){(void)guard;record('S');}
DWORD ntvwm_console_close(void){record('X');return 123;}

int main(void)
{
    native_membership state={0};CRITICAL_SECTION lock;unsigned i;
    struct {DWORD route,checkpoint,paint,hide,bind,result;const char *calls;} cases[]={
        {WORKER_IO_KEEP,0,0,0,0,0,"CFH"},
        {WORKER_IO_KEEP,0,0,5,0,0,"CFH"},
        {WORKER_IO_KEEP,0,23,5,0,23,"CFH"},
        {WORKER_IO_RELEASE,0,0,0,0,0,"CBEHBPR"},
        {WORKER_IO_RELEASE,0,0,5,0,0,"CBEHBPR"},
        {WORKER_IO_RELEASE,0,23,5,0,23,"CBEH"},
        {WORKER_IO_RELEASE,0,0,5,6,6,"CBH"},
        {WORKER_IO_KEEP,1722,0,5,0,1722,"CH"}
    };
    InitializeCriticalSection(&lock);state.lock=&lock;
    state.admission_ready=CreateEventW(NULL,TRUE,FALSE,NULL);
    if(!state.admission_ready)return 1;
    for(i=0;i<ARRAYSIZE(cases);++i) {
        DWORD result;
        used=0;calls[0]=0;route=cases[i].route;checkpoint_error=cases[i].checkpoint;
        paint_error=cases[i].paint;hide_error=cases[i].hide;bind_error=cases[i].bind;
        state.presenting=TRUE;state.users=1;state.presentation=(ntvwm_presentation *)(ULONG_PTR)1;
        result=end_io(&state,1);
        if(result!=cases[i].result || strcmp(calls,cases[i].calls) || state.users){
            printf("FAIL case=%u result=%lu calls=%s\n",i,result,calls);return 2;
        }
    }
    used=0;calls[0]=0;state.guard=(ntvwm_console_guard *)(ULONG_PTR)1;
    if(close_console(&state)!=123 || state.guard || strcmp(calls,"SX"))return 3;
    CloseHandle(state.admission_ready);DeleteCriticalSection(&lock);
    puts("PASS 8 final-I/O/hide/release/error cases; stop-before-close preserves result");return 0;
}
