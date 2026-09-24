#include "wow_task_order_bindings.h"
#include "wow_task_init_bindings.h"
#include "wow_task_profile_bindings.h"
#include <stdio.h>
#include <stdarg.h>

/* The linked SoftPC host owns a silent production printf. Emit fixture
 * evidence through the native output handle, not that product diagnostic. */
static void fixture_print(const char *format, ...)
{
    char text[512];
    int length;
    DWORD written;
    va_list args;
    va_start(args, format);
    length = vsnprintf(text, sizeof(text), format, args);
    va_end(args);
    if (length < 0 || length >= sizeof(text) ||
        !WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), text, (DWORD)length,
            &written, NULL) || written != (DWORD)length)
        ExitProcess(97);
}
#define printf fixture_print

static unsigned errors;
#define CHECK(x) do { if (!(x)) { ++errors; printf("FAIL line=%d\n",__LINE__); } } while (0)
static void order(wow_task_order_state *state, wow_task_order_entry **expected, unsigned count)
{
    wow_task_order_entry *p=state->ptdbHead;
    unsigned i;
    for(i=0;i<count;++i) { CHECK(p==expected[i]); if(!p)return; p=p->ptdbNext; }
    CHECK(p==NULL);
}
static void transitions(void)
{
    wow_task_order_shared shared={0};
    wow_task_order_state sa={0},sb={0};
    wow_task_order_process pa={&sa,&shared},pb={&sb,&shared},none={0},pn={NULL,&shared};
    wow_task_order_entry a={NULL,10,0,NULL},b={NULL,10,0,NULL};
    wow_task_order_thread ta={&pa,&a,WOW_TASK_TIF_16BIT,NULL,NULL};
    wow_task_order_thread tb={&pb,&b,WOW_TASK_TIF_16BIT,NULL,NULL};
    wow_task_order_thread native={&pn,NULL,0,NULL,NULL};
    wow_task_order_message message={0};
    DWORD cookie;
    a.pti=&ta;b.pti=&tb;
    ta.pEventQueueServer=CreateEventW(NULL,FALSE,FALSE,NULL);
    tb.pEventQueueServer=CreateEventW(NULL,FALSE,FALSE,NULL);
    CHECK(ta.pEventQueueServer && tb.pEventQueueServer);
    sa.ptiScheduled=&ta;
    InsertTask(&pa,&a);InsertTask(&pb,&b);
    DirectedScheduleTask(&ta,&tb,TRUE,&message);
    CHECK(a.nEvents==1 && b.nEvents==1 && shared.nEvents==2);
    CHECK(sa.nSendLock==1 && sb.nRecvLock==1);
    CHECK(message.flags==(WOW_TASK_SMF_SEND|WOW_TASK_SMF_RECEIVE));
    CHECK(b.nPriority==10 && WaitForSingleObject(tb.pEventQueueServer,0)==WAIT_OBJECT_0);
    CHECK(WaitForSingleObject(tb.pEventQueueServer,0)==WAIT_TIMEOUT);
    sb.ptiScheduled=&tb;ta.psmsSent=&message;
    DirectedScheduleTask(&tb,&ta,FALSE,&message);
    CHECK(sa.nSendLock==0 && sb.nRecvLock==0 && message.flags==0);
    CHECK(a.nEvents==2 && b.nEvents==2 && shared.nEvents==4);
    WakeWowTask(&ta);
    CHECK(WaitForSingleObject(ta.pEventQueueServer,0)==WAIT_TIMEOUT);
    sa.ptiScheduled=NULL;WakeWowTask(&ta);
    CHECK(WaitForSingleObject(ta.pEventQueueServer,0)==WAIT_OBJECT_0);
    DirectedScheduleTask(&native,&tb,TRUE,&message);
    CHECK(sb.nRecvLock==1 && message.flags==WOW_TASK_SMF_RECEIVE && shared.nEvents==5);
    DirectedScheduleTask(&tb,&native,FALSE,&message);
    CHECK(sb.nRecvLock==0 && message.flags==0 && shared.nEvents==6);
    sa.ptiScheduled=&ta;
    cookie=CurrentTaskLock(0,&pa);
    CHECK(cookie==~(DWORD)&a && sa.nTaskLock==1);
    CHECK(CurrentTaskLock(0,&pa)==cookie && sa.nTaskLock==2);
    CHECK(CurrentTaskLock(cookie^1,&pa)==0 && sa.nTaskLock==2);
    CHECK(CurrentTaskLock(cookie,&pa)==0 && sa.nTaskLock==1);
    CHECK(CurrentTaskLock(cookie,&pa)==0 && sa.nTaskLock==0);
    CHECK(CurrentTaskLock(0,&none)==0);
    CHECK(CloseHandle(ta.pEventQueueServer));CHECK(CloseHandle(tb.pEventQueueServer));
}
static void same_worker_messages(void)
{
    wow_task_order_shared shared = {0};
    wow_task_order_state state = {0};
    wow_task_order_process process = {&state, &shared};
    wow_task_order_entry a = {NULL, 10}, b = {NULL, 10};
    wow_task_order_thread ta = {&process, &a, WOW_TASK_TIF_16BIT};
    wow_task_order_thread tb = {&process, &b, WOW_TASK_TIF_16BIT};
    wow_task_order_message outer = {0}, nested = {0};
    wow_task_order_entry *expected[2];
    a.pti = &ta; b.pti = &tb;
    ta.pEventQueueServer = CreateEventW(NULL, FALSE, FALSE, NULL);
    tb.pEventQueueServer = CreateEventW(NULL, FALSE, FALSE, NULL);
    CHECK(ta.pEventQueueServer && tb.pEventQueueServer);
    InsertTask(&process, &a); InsertTask(&process, &b);
    state.ptiScheduled = &ta;
    state.CSOwningThread = &ta;
    ta.psmsSent = &outer;
    DirectedScheduleTask(&ta, &tb, TRUE, &outer);
    expected[0] = &b; expected[1] = &a; order(&state, expected, 2);
    CHECK(a.nEvents == 1 && b.nEvents == 1 && shared.nEvents == 2);
    CHECK(state.nSendLock == 0 && state.nRecvLock == 0 && outer.flags == 0);
    /* Same-worker send does not itself deschedule the sender or wake the
     * receiver. The original caller must subsequently enter its wait path. */
    CHECK(state.ptiScheduled == &ta && state.CSOwningThread == &ta);
    CHECK(WaitForSingleObject(tb.pEventQueueServer, 0) == WAIT_TIMEOUT);
    CHECK(b.nPriority == 10);

    /* Model the receiver after that separate scheduler handoff. This is a
     * state-transition test, not a native SendMessage or wait acceptance. */
    state.ptiScheduled = &tb;
    tb.psmsSent = &nested;
    DirectedScheduleTask(&tb, &ta, TRUE, &nested);
    expected[0] = &a; expected[1] = &b; order(&state, expected, 2);
    CHECK(a.nEvents == 2 && b.nEvents == 2 && shared.nEvents == 4);
    CHECK(nested.flags == 0 && state.nSendLock == 0 && state.nRecvLock == 0);
    state.ptiScheduled = &ta;
    DirectedScheduleTask(&ta, &tb, FALSE, &nested);
    expected[0] = &b; expected[1] = &a; order(&state, expected, 2);
    CHECK(a.nEvents == 3 && b.nEvents == 3 && shared.nEvents == 6);
    CHECK(ta.psmsSent == &outer && tb.psmsSent == &nested);
    state.ptiScheduled = &tb;
    DirectedScheduleTask(&tb, &ta, FALSE, &outer);
    expected[0] = &a; expected[1] = &b; order(&state, expected, 2);
    CHECK(a.nEvents == 4 && b.nEvents == 4 && shared.nEvents == 8);
    CHECK(a.nPriority == 10 && b.nPriority == 10);
    CHECK(!outer.flags && !nested.flags && !state.nSendLock && !state.nRecvLock);
    CHECK(CloseHandle(ta.pEventQueueServer));
    CHECK(CloseHandle(tb.pEventQueueServer));
}

static void destruction(void)
{
    unsigned mode;
    for(mode=0;mode!=7;++mode) {
        wow_task_order_shared shared={0};
        wow_task_order_state state={0};
        wow_task_order_process process={&state,&shared};
        wow_task_order_entry successor={0};
        wow_task_order_thread next={0},victim={0};
        wow_task_order_waiter sentinel={0};
        wow_task_order_entry *owned;
        wow_task_order_waiter *waiter;
        HANDLE observer=NULL,retained=NULL;
        DWORD flags;
        owned=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*owned));
        waiter=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*waiter));
        CHECK(owned && waiter);
        if(!owned || !waiter) ExitProcess(99);
        observer=CreateEventW(NULL,FALSE,FALSE,NULL);
        CHECK(DuplicateHandle(GetCurrentProcess(),observer,GetCurrentProcess(),
            &retained,0,FALSE,DUPLICATE_SAME_ACCESS));
        waiter->pIdleEvent=retained;sentinel.pwtiNext=waiter;
        if(mode>=5) {
            CHECK(CloseHandle(retained));
            waiter->pIdleEvent=(mode==5 ? (HANDLE)-1 : NULL);
        }
        shared.pwtiFirst=&sentinel;
        owned->nEvents=3;owned->pwti=waiter;owned->pti=&victim;
        victim.ppi=&process;victim.ptdb=owned;
        next.ppi=&process;next.ptdb=&successor;successor.pti=&next;
        successor.nEvents=(mode==2 ? 0 : 1);
        next.pEventQueueServer=CreateEventW(NULL,FALSE,FALSE,NULL);
        state.pEventWowExec=CreateEventW(NULL,FALSE,FALSE,NULL);
        state.ptdbHead=owned;owned->ptdbNext=(mode==3 ? NULL : &successor);
        shared.nEvents=3+(mode==3 ? 0 : successor.nEvents);
        state.ptiScheduled=(mode==0 ? &next : &victim);
        state.CSOwningThread=state.ptiScheduled;state.CSLockCount=0;state.nTaskLock=2;
        if(mode==4) {
            /* Native thread without TDB can still be the active owner. */
            victim.ptdb=NULL;state.ptdbHead=&successor;shared.nEvents=1;
            CloseHandle(retained);HeapFree(GetProcessHeap(),0,waiter);
            HeapFree(GetProcessHeap(),0,owned);shared.pwtiFirst=NULL;
        }
        DestroyTask(&process,&victim);
        CHECK(state.ptdbHead==(mode==3 ? NULL : &successor));
        CHECK(shared.nEvents==(mode==3 ? 0 : successor.nEvents));
        if(mode!=4) {
            CHECK(shared.pwtiFirst==&sentinel && sentinel.pwtiNext==NULL);
            CHECK(WaitForSingleObject(observer,0)==(mode>=5 ? WAIT_TIMEOUT : WAIT_OBJECT_0));
            if(mode<5) CHECK(!GetHandleInformation(retained,&flags) && GetLastError()==ERROR_INVALID_HANDLE);
        }
        if(mode==0) CHECK(state.ptiScheduled==&next && state.nTaskLock==2 && state.CSLockCount==0);
        else CHECK(state.ptiScheduled==NULL && state.nTaskLock==0 &&
            state.CSOwningThread==NULL && state.CSLockCount==-1);
        CHECK(WaitForSingleObject(next.pEventQueueServer,0)==
            ((mode==1 || mode>=4) ? WAIT_OBJECT_0 : WAIT_TIMEOUT));
        CHECK(WaitForSingleObject(state.pEventWowExec,0)==(mode==2 ? WAIT_OBJECT_0 : WAIT_TIMEOUT));
        CHECK(CloseHandle(observer));CHECK(CloseHandle(next.pEventQueueServer));
        CHECK(CloseHandle(state.pEventWowExec));
    }
}

typedef struct wait_test {
    CRITICAL_SECTION lock;
    HANDLE begin;
    HANDLE interrupt;
    unsigned released,acquired,checked,received;
    DWORD target_id;
    wow_task_order_thread *target;
    unsigned lookups;
} wait_test;
static void WINAPI release_lock(wow_task_order_thread *t)
{
    wait_test *v=t->host_context;
    ++v->released;LeaveCriticalSection(&v->lock);
    if(v->begin) SetEvent(v->begin);
}
static void WINAPI acquire_lock(wow_task_order_thread *t)
{
    wait_test *v=t->host_context;
    EnterCriticalSection(&v->lock);++v->acquired;
}
static void WINAPI check_death(wow_task_order_thread *t)
{
    wait_test *v=t->host_context;++v->checked;
}
static void WINAPI unexpected(wow_task_order_thread *t) { (void)t;CHECK(FALSE); }
static BOOL WINAPI no_hook(wow_task_order_thread *t) { (void)t;return FALSE; }
static void WINAPI receive(wow_task_order_thread *t)
{
    wait_test *v=t->host_context;++v->received;
}
static wow_task_order_thread *WINAPI find_thread(wow_task_order_thread *t, DWORD id)
{
    wait_test *v=t->host_context;
    ++v->lookups;
    return id==v->target_id ? v->target : NULL;
}
static NTSTATUS WINAPI wait_events(wow_task_order_thread *thread, DWORD count,
    const HANDLE *events, BOOL alertable)
{
    DWORD result;
    (void)thread;
    /* Event-only mock for original scheduler mechanics, not the production
     * native message-queue bridge or a guest acceptance provider. */
    result = WaitForMultipleObjectsEx(count, events, FALSE, 3000, alertable);
    if (result < WAIT_OBJECT_0 + count) return (NTSTATUS)(result - WAIT_OBJECT_0);
    if (result == WAIT_IO_COMPLETION) return (NTSTATUS)0xC0;
    CHECK(FALSE);
    ExitProcess(98);
}
static const wow_task_host_ops test_ops={release_lock,acquire_lock,check_death,unexpected,
    no_hook,unexpected,unexpected,receive,wait_events,find_thread};
static DWORD WINAPI interrupt_thread(void *arg)
{
    wait_test *v=arg;
    if(WaitForSingleObject(v->begin,3000)!=WAIT_OBJECT_0)return 1;
    EnterCriticalSection(&v->lock);
    SetEvent(v->interrupt);
    LeaveCriticalSection(&v->lock);
    return 0;
}
static void sleeping(void)
{
    wait_test test={0};
    wow_task_order_shared shared={0};
    wow_task_order_state state={0};
    wow_task_order_process process={&state,&shared};
    wow_task_order_entry task={0};
    wow_task_order_queue queue={0};
    wow_task_order_thread thread={0};
    HANDLE helper;DWORD code;
    InitializeCriticalSection(&test.lock);
    test.interrupt=CreateEventW(NULL,FALSE,FALSE,NULL);
    state.pEventWowExec=state.hEventWowExecClient=test.interrupt;
    thread.ppi=&process;thread.ptdb=&task;thread.pcti=&queue;
    thread.host=&test_ops;thread.host_context=&test;thread.TIF_flags=WOW_TASK_TIF_16BIT;
    thread.pEventQueueServer=CreateEventW(NULL,FALSE,FALSE,NULL);
    task.pti=&thread;task.nPriority=10;task.nEvents=shared.nEvents=1;
    state.ptdbHead=&task;state.ptiScheduled=state.CSOwningThread=&thread;
    EnterCriticalSection(&test.lock);
    CHECK(!xxxSleepTask(TRUE,NULL,&thread));
    CHECK(task.nEvents==0 && shared.nEvents==0 && state.CSLockCount==0 && state.CSOwningThread==&thread);
    CHECK(test.released==1 && test.acquired==1 && test.checked==2);
    CHECK(xxxUserYield(&thread));
    CHECK(test.received==2 && test.released==2 && test.acquired==2);
    CHECK(task.nEvents==0 && shared.nEvents==0);
    test.begin=CreateEventW(NULL,FALSE,FALSE,NULL);
    helper=CreateThread(NULL,0,interrupt_thread,&test,0,NULL);CHECK(helper!=NULL);
    CHECK(xxxSleepTask(FALSE,test.interrupt,&thread));
    CHECK(queue.fsWakeMask==0x20ff);
    CHECK(state.CSOwningThread==&thread && state.CSLockCount==0);
    CHECK(task.nEvents==0 && shared.nEvents==0 && !(process.W32PF_Flags&0x10000));
    CHECK(test.released==3 && test.acquired==3 && test.checked==6);
    CHECK(WaitForSingleObject(helper,3000)==WAIT_OBJECT_0);
    CHECK(GetExitCodeThread(helper,&code) && code==0);
    CHECK(!xxxSleepTask(FALSE,(HANDLE)-1,&thread));
    CHECK(state.CSOwningThread==NULL && state.CSLockCount==-1 && state.ptiScheduled==NULL);
    CHECK(test.released==3 && test.acquired==3);
    LeaveCriticalSection(&test.lock);
    CHECK(CloseHandle(helper));CHECK(CloseHandle(test.begin));CHECK(CloseHandle(test.interrupt));
    CHECK(CloseHandle(thread.pEventQueueServer));
    CHECK(HeapFree(GetProcessHeap(),0,thread.apEvent));
    DeleteCriticalSection(&test.lock);
}

static DWORD WINAPI directed_target(void *arg)
{
    wow_task_order_thread *t=arg;
    wait_test *v=t->host_context;
    EnterCriticalSection(&v->lock);
    CHECK(t->ppi->pwpi->ptiScheduled==t && t->ptdb->nPriority==10);
    CHECK(!xxxSleepTask(TRUE,NULL,t));
    CHECK(t->ppi->pwpi->CSOwningThread==t && t->ptdb->nEvents==0);
    CHECK(!xxxSleepTask(FALSE,(HANDLE)-1,t));
    LeaveCriticalSection(&v->lock);
    return 0;
}

/* Native transport witness, not a WOW16 callback or production binding.
 * Task ordering is the linked original taskman; USER32 owns delivery. */
typedef struct native_send_test {
    wait_test wait;
    wow_task_order_thread *sender, *receiver;
    wow_task_order_message message;
    HANDLE ready;
    HWND window;
    unsigned delivered;
    BOOL handoff;
} native_send_test;

static LRESULT CALLBACK native_send_window(HWND window, UINT message,
    WPARAM wp, LPARAM lp)
{
    native_send_test *test = (native_send_test *)GetWindowLongPtrW(window, GWLP_USERDATA);
    if (message == WM_NCCREATE) {
        test = ((CREATESTRUCTW *)lp)->lpCreateParams;
        SetWindowLongPtrW(window, GWLP_USERDATA, (LONG_PTR)test);
    }
    if (message == WM_APP + 61) {
        EnterCriticalSection(&test->wait.lock);
        CHECK(test->receiver->ppi->pwpi->ptiScheduled == test->receiver);
        CHECK(test->receiver->ppi->pwpi->CSOwningThread == test->receiver);
        CHECK(InSendMessage() && wp == 41 && lp == 73);
        ++test->delivered;
        DirectedScheduleTask(test->receiver, test->sender, FALSE, &test->message);
        CHECK(!xxxSleepTask(FALSE, (HANDLE)-1, test->receiver));
        LeaveCriticalSection(&test->wait.lock);
        return 114;
    }
    return DefWindowProcW(window, message, wp, lp);
}

static DWORD WINAPI native_send_receiver(void *parameter)
{
    native_send_test *test = parameter;
    MSG message;
    DWORD wake;
    test->window = CreateWindowExW(0, L"WOW_TASK_NATIVE_SEND", L"", 0,
        0, 0, 1, 1, HWND_MESSAGE, NULL, GetModuleHandleW(NULL), test);
    CHECK(test->window != NULL);
    SetEvent(test->ready);
    if (!test->window) return 1;
    wake = WaitForSingleObject(test->receiver->pEventQueueServer, 3000);
    if (!test->handoff) {
        CHECK(wake == WAIT_TIMEOUT && !test->delivered);
        CHECK(DestroyWindow(test->window));
        return 0;
    }
    if (wake != WAIT_OBJECT_0) return 2;
    EnterCriticalSection(&test->wait.lock);
    CHECK(!xxxSleepTask(TRUE, NULL, test->receiver));
    CHECK(test->receiver->ppi->pwpi->CSOwningThread == test->receiver);
    LeaveCriticalSection(&test->wait.lock);
    wake = MsgWaitForMultipleObjectsEx(0, NULL, 3000, QS_SENDMESSAGE, MWMO_INPUTAVAILABLE);
    CHECK(wake == WAIT_OBJECT_0);
    (void)PeekMessageW(&message, NULL, 0, 0, PM_NOREMOVE | PM_QS_SENDMESSAGE);
    CHECK(test->delivered == 1);
    CHECK(DestroyWindow(test->window));
    return 0;
}

static void native_synchronous_send(BOOL handoff)
{
    native_send_test test = {0};
    wow_task_order_shared shared = {0};
    wow_task_order_state state = {0};
    wow_task_order_process process = {&state, &shared};
    wow_task_order_entry a = {NULL, 10}, b = {NULL, 10};
    wow_task_order_thread ta = {&process, &a, WOW_TASK_TIF_16BIT};
    wow_task_order_thread tb = {&process, &b, WOW_TASK_TIF_16BIT};
    WNDCLASSW cls = {0};
    HANDLE helper;
    DWORD code, send_error;
    LRESULT sent;
    DWORD_PTR result = 0;
    InitializeCriticalSection(&test.wait.lock);
    test.ready = CreateEventW(NULL, TRUE, FALSE, NULL);
    state.hEventWowExecClient = state.pEventWowExec = test.ready;
    ta.pEventQueueServer = CreateEventW(NULL, FALSE, FALSE, NULL);
    tb.pEventQueueServer = CreateEventW(NULL, FALSE, FALSE, NULL);
    ta.host = tb.host = &test_ops;
    ta.host_context = tb.host_context = &test.wait;
    a.pti = &ta; b.pti = &tb;
    test.sender = &ta; test.receiver = &tb;
    test.handoff = handoff;
    test.message.ptiSender = &ta; test.message.ptiReceiver = &tb;
    ta.psmsSent = &test.message;
    cls.lpfnWndProc = native_send_window;
    cls.hInstance = GetModuleHandleW(NULL);
    cls.lpszClassName = L"WOW_TASK_NATIVE_SEND";
    CHECK(RegisterClassW(&cls) != 0);
    InsertTask(&process, &a); InsertTask(&process, &b);
    state.ptiScheduled = state.CSOwningThread = &ta;
    helper = CreateThread(NULL, 0, native_send_receiver, &test, 0, NULL);
    CHECK(helper != NULL);
    if (!helper || WaitForSingleObject(test.ready, 3000) != WAIT_OBJECT_0)
        ExitProcess(96);
    if (handoff) {
        EnterCriticalSection(&test.wait.lock);
        DirectedScheduleTask(&ta, &tb, TRUE, &test.message);
        CHECK(!xxxSleepTask(FALSE, (HANDLE)-1, &ta));
        CHECK(state.ptiScheduled == &tb && state.CSOwningThread == NULL);
        LeaveCriticalSection(&test.wait.lock);
    }
    SetLastError(ERROR_SUCCESS);
    sent = SendMessageTimeoutW(test.window, WM_APP + 61, 41, 73,
        SMTO_ABORTIFHUNG, handoff ? 3000 : 500, &result);
    send_error = GetLastError();
    CHECK(handoff ? (sent != 0 && result == 114) :
        (sent == 0 && send_error == ERROR_TIMEOUT));
    EnterCriticalSection(&test.wait.lock);
    if (handoff) CHECK(!xxxSleepTask(TRUE, NULL, &ta));
    CHECK(state.ptiScheduled == &ta && state.CSOwningThread == &ta);
    CHECK(!state.nSendLock && !state.nRecvLock && !test.message.flags);
    LeaveCriticalSection(&test.wait.lock);
    CHECK(WaitForSingleObject(helper, 3000) == WAIT_OBJECT_0);
    CHECK(GetExitCodeThread(helper, &code) && code == 0);
    CHECK(test.delivered == (handoff ? 1u : 0u));
    CHECK(CloseHandle(helper)); CHECK(CloseHandle(test.ready));
    CHECK(CloseHandle(ta.pEventQueueServer)); CHECK(CloseHandle(tb.pEventQueueServer));
    if (ta.apEvent) CHECK(HeapFree(GetProcessHeap(), 0, ta.apEvent));
    if (tb.apEvent) CHECK(HeapFree(GetProcessHeap(), 0, tb.apEvent));
    CHECK(UnregisterClassW(cls.lpszClassName, cls.hInstance));
    DeleteCriticalSection(&test.wait.lock);
}
static void directed(void)
{
    wait_test test={0};
    wow_task_order_shared shared={0};
    wow_task_order_state state={0};
    wow_task_order_process process={&state,&shared};
    wow_task_order_entry a={0},b={0};
    wow_task_order_thread ta={0},tb={0},native={0};
    HANDLE helper;DWORD code;
    InitializeCriticalSection(&test.lock);
    state.pEventWowExec=state.hEventWowExecClient=CreateEventW(NULL,FALSE,FALSE,NULL);
    ta.ppi=tb.ppi=native.ppi=&process;ta.ptdb=&a;tb.ptdb=&b;
    ta.host=tb.host=&test_ops;ta.host_context=tb.host_context=&test;
    ta.TIF_flags=tb.TIF_flags=WOW_TASK_TIF_16BIT;
    a.pti=&ta;b.pti=&tb;a.nPriority=b.nPriority=10;
    ta.pEventQueueServer=CreateEventW(NULL,FALSE,FALSE,NULL);
    tb.pEventQueueServer=CreateEventW(NULL,FALSE,FALSE,NULL);
    InsertTask(&process,&a);InsertTask(&process,&b);
    state.ptiScheduled=state.CSOwningThread=&ta;
    EnterCriticalSection(&test.lock);
    xxxDirectedYield((DWORD)-1,&ta);
    CHECK(test.lookups==0 && shared.nEvents==0 && state.ptiScheduled==&ta);
    test.target_id=GetCurrentThreadId();test.target=&ta;
    xxxDirectedYield(test.target_id,&ta);
    CHECK(a.nEvents==1 && shared.nEvents==1 && a.nPriority==10);
    /* Drain the extra self-directed event through the original scheduler. */
    CHECK(!xxxSleepTask(TRUE,NULL,&ta));
    test.target=&native;
    xxxDirectedYield(test.target_id,&ta);
    CHECK(shared.nEvents==0 && state.ptiScheduled==&ta);
    test.target=NULL;
    xxxDirectedYield(test.target_id,&ta);
    /* Original missing-target return retains the caller's posted event. */
    CHECK(a.nEvents==1 && shared.nEvents==1 && state.CSOwningThread==&ta);
    CHECK(!xxxSleepTask(TRUE,NULL,&ta));
    xxxDirectedYield(test.target_id,&native);
    CHECK(shared.nEvents==0 && test.lookups==3);
    test.target=&tb;
    helper=CreateThread(NULL,0,directed_target,&tb,0,&test.target_id);
    CHECK(helper!=NULL);if(!helper)ExitProcess(99);
    xxxDirectedYield(test.target_id,&ta);
    CHECK(state.ptiScheduled==&ta && state.CSOwningThread==&ta && state.CSLockCount==0);
    CHECK(a.nEvents==0 && b.nEvents==0 && shared.nEvents==0 && b.nPriority==10);
    LeaveCriticalSection(&test.lock);
    CHECK(WaitForSingleObject(helper,3000)==WAIT_OBJECT_0);
    CHECK(GetExitCodeThread(helper,&code) && code==0);
    CHECK(CloseHandle(helper));CHECK(CloseHandle(ta.pEventQueueServer));
    CHECK(CloseHandle(tb.pEventQueueServer));CHECK(CloseHandle(state.pEventWowExec));
    CHECK(HeapFree(GetProcessHeap(),0,ta.apEvent));CHECK(HeapFree(GetProcessHeap(),0,tb.apEvent));
    DeleteCriticalSection(&test.lock);
}

static void WINAPI end_task(void) { CHECK(FALSE); }
static void registration(void)
{
    wow_task_order_shared shared={0};
    wow_task_order_process first={NULL,&shared},second={NULL,&shared},failed={NULL,&shared};
    HANDLE event,limited=NULL,semaphore,observer=NULL;
    DWORD before,after;unsigned i;
    event=CreateEventW(NULL,FALSE,FALSE,NULL);
    semaphore=CreateSemaphoreW(NULL,0,1,NULL);
    CHECK(event && semaphore);
    CHECK(DuplicateHandle(GetCurrentProcess(),event,GetCurrentProcess(),&limited,SYNCHRONIZE,FALSE,0));
    CHECK(GetProcessHandleCount(GetCurrentProcess(),&before));
    for(i=0;i<32;++i) {
        CHECK(!xxxRegisterUserHungAppHandlers(end_task,NULL,&failed));
        CHECK(!xxxRegisterUserHungAppHandlers(end_task,limited,&failed));
        CHECK(!xxxRegisterUserHungAppHandlers(end_task,semaphore,&failed));
        CHECK(!xxxRegisterUserHungAppHandlers(end_task,GetCurrentProcess(),&failed));
        CHECK(failed.pwpi==NULL && shared.pwpiFirstWow==NULL);
    }
    CHECK(GetProcessHandleCount(GetCurrentProcess(),&after) && after==before);
    CHECK(xxxRegisterUserHungAppHandlers(end_task,event,&first));
    CHECK(first.pwpi && shared.pwpiFirstWow==first.pwpi);
    CHECK(first.pwpi->lpfnWowExitTask==(DWORD)end_task && first.pwpi->hEventWowExecClient==event);
    CHECK(first.pwpi->ptdbHead==NULL && first.pwpi->nTaskLock==0 && first.pwpi->CSLockCount==0);
    /* Original registration records, but does not validate/call, callback. */
    CHECK(xxxRegisterUserHungAppHandlers(NULL,event,&second));
    CHECK(shared.pwpiFirstWow==second.pwpi && second.pwpi->pwpiNext==first.pwpi);
    CHECK(second.pwpi->lpfnWowExitTask==0);
    CHECK(DuplicateHandle(GetCurrentProcess(),event,GetCurrentProcess(),&observer,0,FALSE,DUPLICATE_SAME_ACCESS));
    CHECK(CloseHandle(event));
    CHECK(SetEvent(first.pwpi->pEventWowExec));
    CHECK(WaitForSingleObject(second.pwpi->pEventWowExec,0)==WAIT_OBJECT_0);
    CHECK(WaitForSingleObject(observer,0)==WAIT_TIMEOUT);
    /* Fixture-owned retirement, not a production process-teardown claim. */
    CHECK(CloseHandle(second.pwpi->pEventWowExec));
    shared.pwpiFirstWow=second.pwpi->pwpiNext;
    CHECK(HeapFree(GetProcessHeap(),0,second.pwpi));second.pwpi=NULL;
    CHECK(SetEvent(first.pwpi->pEventWowExec));
    CHECK(WaitForSingleObject(observer,0)==WAIT_OBJECT_0);
    CHECK(CloseHandle(first.pwpi->pEventWowExec));
    shared.pwpiFirstWow=first.pwpi->pwpiNext;
    CHECK(HeapFree(GetProcessHeap(),0,first.pwpi));first.pwpi=NULL;
    CHECK(shared.pwpiFirstWow==NULL);
    CHECK(CloseHandle(observer));CHECK(CloseHandle(limited));CHECK(CloseHandle(semaphore));
}

typedef struct init_test {
    wow_task_order_thread *thread;
    unsigned journal,attached,cursor,profile;
    BOOL long_value;
} init_test;
static BOOL WINAPI init_journal(wow_task_order_process *p,BOOL playback)
{
    init_test *v=p->init_context;
    CHECK((v->thread->TIF_flags&0x2002)==0x2002);
    CHECK(v->thread->pClientInfo->dwTIFlags==v->thread->TIF_flags);
    return v->journal==(playback ? 2u : 1u);
}
static BOOL WINAPI init_attach(wow_task_order_process *p,BOOL journal)
{
    init_test *v=p->init_context;
    CHECK(!journal && !v->cursor);++v->attached;return TRUE;
}
static void WINAPI init_cursor(wow_task_order_process *p,DWORD timeout)
{
    init_test *v=p->init_context;
    CHECK(timeout==(v->cursor ? 10000u : 5000u));
    if(v->cursor) {
        CHECK(p->pwpi->ptiScheduled==v->thread && p->pwpi->CSOwningThread==v->thread);
        CHECK(p->pwpi->CSLockCount==0 && v->thread->ptdb->nPriority==10);
    }
    ++v->cursor;
}
static UINT WINAPI init_profile(wow_task_order_process *p,UINT section,
    LPCWSTR key,LPCWSTR fallback,LPWSTR output,DWORD capacity)
{
    init_test *v=p->init_context;
    DWORD i;
    CHECK(v->cursor==1 && section==14 && !lstrcmpW(key,L"APP"));
    /* Original caller owns WCHAR szHex[80]. Reject an oversized promise
     * before writing: the red test must not corrupt its stack. */
    CHECK(!*fallback && capacity==80);
    if(capacity>80) {
        printf("PROFILE_CAPACITY_MISMATCH declared=80 requested=%lu\n",capacity);
        return 0;
    }
    CHECK(v->thread->dwExpWinVer==v->thread->pClientInfo->dwExpWinVer);
    ++v->profile;
    if(v->long_value) {
        /* Model a 199-character value truncated by the requested capacity;
         * like original FastGetProfileStringW, return its full data length. */
        for(i=0;i<capacity-1;++i)output[i]=L'0';
        output[capacity-1]=0;
        return 200;
    }
    lstrcpyW(output,L"0x200");return 5;
}
static void initialization(void)
{
    const wow_task_init_ops ops={init_journal,init_attach,init_cursor,init_profile};
    const wow_task_init_ops native_profile_ops={init_journal,init_attach,init_cursor,wow_task_profile};
    unsigned mode;
    for(mode=0;mode<8;++mode) {
        wow_task_order_shared shared={0};
        wow_task_order_process process={NULL,&shared};
        wow_task_order_thread thread={0};
        wow_task_client_info client={0};
        init_test test={0};
        UNICODE_STRING name={14,16,L"APP.EXE"};
        HANDLE event=CreateEventW(NULL,FALSE,FALSE,NULL);
        DWORD version=(mode==2 ? 0x0400 : 0x030a);
        DWORD task_id=(mode==1 ? 42 : mode==2 ? (DWORD)-1 : 0);
        DWORD position=mode==1 ? 12 : (DWORD)CW_USEDEFAULT;
        wow_task_order_waiter *waiter=NULL;
        CHECK(xxxRegisterUserHungAppHandlers(end_task,event,&process));
        test.thread=&thread;test.journal=(mode==3 ? 1 : mode==4 ? 2 : 0);
        test.long_value=(mode==6);
        process.init=(mode==7 ? &native_profile_ops : &ops);process.init_context=&test;
        process.usi.dwFlags=STARTF_USESHOWWINDOW|STARTF_USEPOSITION|STARTF_USESIZE;
        thread.ppi=&process;thread.pClientInfo=&client;
        if(mode==1) {
            waiter=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*waiter));
            CHECK(waiter!=NULL);if(!waiter)ExitProcess(99);
            waiter->idTask=42;shared.pwtiFirst=waiter;
        }
        if(mode==5) {
            WCHAR *invalid=VirtualAlloc(NULL,4096,MEM_RESERVE|MEM_COMMIT,PAGE_NOACCESS);
            CHECK(invalid!=NULL);name.Buffer=invalid;
            CHECK(xxxInitTask(version,&name,0x1234,7,0,position,20,position,40,0,&thread)==(NTSTATUS)0xc0000033);
            CHECK(thread.pstrAppName==NULL && thread.ptdb==NULL && shared.ppiStarting==NULL);
            CHECK(!test.attached && !test.cursor && !test.profile);
            CHECK(VirtualFree(invalid,0,MEM_RELEASE));name.Buffer=L"APP.EXE";
        }
        CHECK(xxxInitTask(version,&name,0x1234,7,task_id,position,20,position,40,0,&thread)==0);
        CHECK(thread.pstrAppName && thread.pstrAppName->Length==name.Length);
        CHECK(!memcmp(thread.pstrAppName->Buffer,name.Buffer,name.Length));
        CHECK(thread.ptdb->hTaskWow==0x1234 && thread.ptdb->pwti==waiter);
        CHECK(!!(thread.TIF_flags&0x1000)==!!task_id);
        CHECK(thread.dwExpWinVer==version && client.dwExpWinVer==version);
        CHECK(thread.dwCompatFlags==((mode==2 || mode>=6) ? 0u : 0x200u));
        CHECK(client.dwCompatFlags==thread.dwCompatFlags && process.dwCompatFlags==thread.dwCompatFlags);
        CHECK(test.profile==((mode==2 || mode==7) ? 0u : 1u) && test.cursor==2);
        CHECK(test.attached==(test.journal ? 0u : 1u));
        CHECK(shared.ppiStarting==&process && process.dwHotkey==7);
        CHECK(!(process.usi.dwFlags&STARTF_USESHOWWINDOW));
        CHECK(!!(process.usi.dwFlags&STARTF_USEPOSITION)==(mode==1));
        CHECK(!!(process.usi.dwFlags&STARTF_USESIZE)==(mode==1));
        if(mode==1) CHECK(process.usi.dwX==12 && process.usi.dwY==20 && process.usi.dwXSize==12 && process.usi.dwYSize==40);
        CHECK(!shared.allowForegroundActivate);
        CHECK(thread.ptdb->nEvents==0 && shared.nEvents==0);
        /* Native queue/window cleanup is mocked above, so retirement here
         * proves only original TDB/name/event ownership, not USER teardown. */
        DestroyTask(&process,&thread);thread.ptdb=NULL;
        CHECK(!process.pwpi->ptdbHead && shared.pwtiFirst==NULL);
        CHECK(HeapFree(GetProcessHeap(),0,thread.pstrAppName));
        CHECK(CloseHandle(process.pwpi->pEventWowExec));
        CHECK(HeapFree(GetProcessHeap(),0,process.pwpi));
        CHECK(CloseHandle(event));
    }
}

int __cdecl main(void)
{
    wow_task_order_entry a={NULL,10},b={NULL,10},c={NULL,9},d={NULL,11};
    wow_task_order_state state={0};
    wow_task_order_process process={&state};
    wow_task_order_entry *expected[4];
    InsertTask(&process,&a); InsertTask(&process,&b);
    InsertTask(&process,&c); InsertTask(&process,&d);
    expected[0]=&c;expected[1]=&a;expected[2]=&b;expected[3]=&d;order(&state,expected,4);
    InsertTask(&process,&a);
    expected[1]=&b;expected[2]=&a;order(&state,expected,4);
    d.nPriority=8;InsertTask(&process,&d);
    expected[0]=&d;expected[1]=&c;expected[2]=&b;expected[3]=&a;order(&state,expected,4);
    d.nPriority=12;InsertTask(&process,&d);
    expected[0]=&c;expected[1]=&b;expected[2]=&a;expected[3]=&d;order(&state,expected,4);
    InsertTask(&process,&d);order(&state,expected,4);
    transitions();
    same_worker_messages();
    destruction();
    sleeping();
    directed();
    native_synchronous_send(FALSE);
    native_synchronous_send(TRUE);
    registration();
    initialization();
    printf("WOW_ORIGINAL_TASK_ORDER errors=%u sequences=5 send_reply=4 same_worker_nested=4 native_send=2 locks=6 destruction=7 waits=4 directed=6 registration=6 init=8\n",errors);
    return errors!=0;
}
