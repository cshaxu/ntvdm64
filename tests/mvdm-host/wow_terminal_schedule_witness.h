/* Test-only forced interleaving on an actual published CCPU window. */
typedef struct terminal_schedule_test {
    wow_user_runtime *runtime;
    session *session_owner;
    HWND window;
    wow_window_words_binding *words;
    HANDLE reached, release, attempting;
#ifdef WOW_CALLBACK_GRAPH_OVERLAP
    HANDLE locked, finish_graph, graph_requested;
    volatile LONG graph_gate, graph_acquired, hook_graph_completed, graph_raise;
    DWORD graph_thread;
#endif
    DWORD worker_id;
    volatile LONG first_phase;
    unsigned mutation_held, mutation_unheld;
    BOOL bound, retired, unbound;
} terminal_schedule_test;
static terminal_schedule_test *terminal_schedule_active;
#ifdef WOW_HOOK_GRAPH_OVERLAP
LONG WINAPI WU32StdHookProc(INT,LONG,LONG,INT);
void __cdecl test_hook_graph_ready(void)
{
    terminal_schedule_test *t=terminal_schedule_active;
    if (!t || !t->graph_gate || GetCurrentThreadId()!=t->graph_thread) return;
    SetEvent(t->release);
    if (WaitForSingleObject(t->locked,10000)!=WAIT_OBJECT_0) ExitProcess(5);
}
void __cdecl test_hook_graph_completed(void)
{
    terminal_schedule_test *t=terminal_schedule_active;
    if (t && t->graph_gate && GetCurrentThreadId()==t->graph_thread)
        InterlockedExchange(&t->hook_graph_completed,1);
}
#endif
#ifdef WOW_CALLBACK_GRAPH_OVERLAP
void __cdecl test_callback_graph_request(void)
{
    terminal_schedule_test *t=terminal_schedule_active;
    if(t && t->graph_gate && GetCurrentThreadId()==t->graph_thread)
        SetEvent(t->graph_requested);
}
void __cdecl test_callback_graph_acquired(void)
{
    terminal_schedule_test *t=terminal_schedule_active;
    if(t && t->graph_gate && GetCurrentThreadId()==t->graph_thread &&
            t->runtime->owner_thread_id==t->graph_thread)
        InterlockedExchange(&t->graph_acquired,1);
}
void __cdecl test_callback_graph_fault(void)
{
    terminal_schedule_test *t=terminal_schedule_active;
    if (t && GetCurrentThreadId()==t->graph_thread &&
            InterlockedExchange(&t->graph_raise,0))
        RaiseException(0xe0473111u,0,0,NULL);
}
static DWORD WINAPI release_graph_competitor(void *context)
{
    terminal_schedule_test *t=context;
    if (WaitForSingleObject(t->graph_requested,10000)!=WAIT_OBJECT_0 ||
            t->runtime->owner_thread_id!=t->worker_id) return 1;
    return SetEvent(t->finish_graph) ? 0 : 2;
}
#endif
void __cdecl test_terminal_schedule(unsigned phase)
{
    terminal_schedule_test *t=terminal_schedule_active;
    wow_user_runtime_thread *b;
    if(!t || GetCurrentThreadId()!=t->worker_id)return;
    b=wow_user_runtime_current();
    if(phase==2) {
        if(b && b->exclusive_held && t->runtime->owner_thread_id==GetCurrentThreadId())
            ++t->mutation_held;
        else ++t->mutation_unheld;
    }
#ifdef WOW_CALLBACK_GRAPH_OVERLAP
    if (phase==3 && t->locked) {
        SetEvent(t->locked);
        WaitForSingleObject(t->finish_graph,INFINITE);
    }
#endif
    if(InterlockedCompareExchange(&t->first_phase,(LONG)phase,0)==0) {
        SetEvent(t->reached);
        WaitForSingleObject(t->release,INFINITE);
        SetEvent(t->attempting);
    }
}
static DWORD WINAPI terminal_schedule_worker(void *context)
{
    terminal_schedule_test *t=context;
    wow_user_runtime_thread b={0};
    t->worker_id=GetCurrentThreadId();
    if (!session_thread_bind(t->session_owner)) { InterlockedExchange(&t->first_phase,-2);SetEvent(t->reached); return 1; }
    t->bound=wow_user_runtime_bind(&b,t->runtime,NULL,NULL);
    if(t->bound) {
        t->retired=wow_window_words_terminal(t->window,t->words);
        t->unbound=wow_user_runtime_unbind(&b);
    } else {InterlockedExchange(&t->first_phase,-1);SetEvent(t->reached);}
    if (!session_thread_unbind(t->session_owner)) return 2;
    return 0;
}
static void verify_terminal_schedule(wow_user_runtime_thread *main_binding)
{
    terminal_schedule_test t={0};
    WW words={0};
    HANDLE worker;
    DWORD wait, worker_exit = 0;
    unsigned before=borrow_errors;
    unsigned ownership_before=borrow_errors;
    BOOL graph_ok;
#ifdef WOW_CALLBACK_GRAPH_OVERLAP
#ifndef WOW_HOOK_GRAPH_OVERLAP
    wow_task_callback_scope callback;
#endif
    BOOL graph_result;
    HANDLE controller;
    DWORD controller_exit=0;
    unsigned overlap_before;
#endif
    t.runtime=main_binding->runtime;
    t.session_owner=session_thread_current();
    BORROW_CHECK(t.session_owner != NULL);
    /* Test both caller ownership states on success and on the actual
     * invalid-payload failure, including the public last-error contract. */
    BORROW_CHECK(!main_binding->exclusive_held);
    SetLastError(0x47311u);
    graph_ok=wow_user_window_refresh_payload(NULL,NULL,0);
    BORROW_CHECK(graph_ok && !main_binding->exclusive_held && GetLastError()==0x47311u);
    SetLastError(0x47312u);
    graph_ok=wow_user_window_refresh_payload(NULL,NULL,1);
    BORROW_CHECK(!graph_ok && !main_binding->exclusive_held && GetLastError()==ERROR_INVALID_PARAMETER);
    BORROW_CHECK(wow_user_runtime_enter(main_binding));
    SetLastError(0x47313u);
    graph_ok=wow_user_window_refresh_payload(NULL,NULL,0);
    BORROW_CHECK(graph_ok && main_binding->exclusive_held && GetLastError()==0x47313u);
    SetLastError(0x47314u);
    graph_ok=wow_user_window_refresh_payload(NULL,NULL,1);
    BORROW_CHECK(!graph_ok && main_binding->exclusive_held && GetLastError()==ERROR_INVALID_PARAMETER);
    BORROW_CHECK(wow_user_runtime_leave(main_binding));
    fprintf(stderr,"WOW_GRAPH_OWNER_RESTORE errors=%u success=2 failure=2 held=2 unheld=2\n",
        borrow_errors-ownership_before);
    t.reached=CreateEventW(NULL,TRUE,FALSE,NULL);
    t.release=CreateEventW(NULL,TRUE,FALSE,NULL);
    t.attempting=CreateEventW(NULL,TRUE,FALSE,NULL);
#ifdef WOW_CALLBACK_GRAPH_OVERLAP
    t.locked=CreateEventW(NULL,TRUE,FALSE,NULL);
    t.finish_graph=CreateEventW(NULL,TRUE,FALSE,NULL);
    t.graph_requested=CreateEventW(NULL,TRUE,FALSE,NULL);
    BORROW_CHECK(t.locked && t.finish_graph && t.graph_requested);
#endif
    t.window=CreateWindowExW(0,L"STATIC",L"",0,0,0,1,1,NULL,NULL,GetModuleHandleW(NULL),NULL);
    BORROW_CHECK(t.reached && t.release && t.attempting && t.window && wow_window_words_attach(t.window,&words));
    t.words=wow_window_words_acquire(t.window);
    BORROW_CHECK(t.words && main_binding->thread &&
        wow_window_words_publish_owner(t.window,t.words,main_binding->thread));
    BORROW_CHECK(!main_binding->exclusive_held && wow_user_runtime_enter(main_binding));
    BORROW_CHECK(wow_user_window_refresh_graph(NULL));
    BORROW_CHECK(mvdm_softpc_wow_page_domain_window_handle(LOWORD(t.window)) == (ULONG)(ULONG_PTR)t.window);
    terminal_schedule_active=&t;
    worker=CreateThread(NULL,0,terminal_schedule_worker,&t,0,NULL);
    BORROW_CHECK(worker!=NULL);
    wait=WaitForSingleObject(t.reached,10000); /* Test watchdog only. */
    BORROW_CHECK(wait==WAIT_OBJECT_0);
    BORROW_CHECK(main_binding->exclusive_held && t.runtime->owner_thread_id==GetCurrentThreadId());
    BORROW_CHECK(t.first_phase==1 && t.mutation_unheld==0);
    /* In the ordinary path release the resource thread before refreshing;
     * the hook path keeps its gate until callback admission to avoid blocking
     * that admission on the resource thread's test-only pause. */
#ifndef WOW_HOOK_GRAPH_OVERLAP
    BORROW_CHECK(SetEvent(t.release));
    BORROW_CHECK(WaitForSingleObject(t.attempting,10000)==WAIT_OBJECT_0);
#endif
    BORROW_CHECK(wow_user_window_refresh_graph(NULL));
    BORROW_CHECK(main_binding->exclusive_held && t.mutation_held==0 &&
        t.mutation_unheld==0 &&
        mvdm_softpc_wow_page_domain_window_handle(LOWORD(t.window)) ==
            (ULONG)(ULONG_PTR)t.window);
    fprintf(stderr,"WOW_TERMINAL_GRAPH_BEFORE_RETIRE errors=%u root_live=1 no_mutation=1\n",
        borrow_errors-before);
    BORROW_CHECK(wow_user_runtime_leave(main_binding));
#ifdef WOW_CALLBACK_GRAPH_OVERLAP
#ifndef WOW_HOOK_GRAPH_OVERLAP
    BORROW_CHECK(wow_task_callback_enter(&callback));
    BORROW_CHECK(!main_binding->exclusive_held);
#endif
#endif
#ifndef WOW_HOOK_GRAPH_OVERLAP
    BORROW_CHECK(SetEvent(t.release));
#endif
#ifdef WOW_CALLBACK_GRAPH_OVERLAP
#ifndef WOW_HOOK_GRAPH_OVERLAP
    wait=WaitForSingleObject(t.locked,10000);
    if (wait != WAIT_OBJECT_0) ExitProcess(3);
    overlap_before=borrow_errors;
    BORROW_CHECK(t.runtime->owner_thread_id==t.worker_id && !main_binding->exclusive_held);
#else
    overlap_before=borrow_errors;
#endif
    t.graph_thread=GetCurrentThreadId();
    InterlockedExchange(&t.graph_gate,1);
    controller=CreateThread(NULL,0,release_graph_competitor,&t,0,NULL);
    if (!controller) ExitProcess(4);
#ifdef WOW_HOOK_GRAPH_OVERLAP
    /* Slot zero is the original permanently-in-use sentinel. Slot one is
     * unused in this fixture, so original dispatch returns without a PTD. */
    graph_result=(WU32StdHookProc(-1,0,0,1)==0 && t.hook_graph_completed);
#else
    graph_result=wow_user_window_refresh_payload(NULL,NULL,0);
#endif
    InterlockedExchange(&t.graph_gate,0);
    BORROW_CHECK(WaitForSingleObject(controller,10000)==WAIT_OBJECT_0);
    BORROW_CHECK(GetExitCodeThread(controller,&controller_exit) && !controller_exit);
    BORROW_CHECK(graph_result && t.graph_acquired && !main_binding->exclusive_held);
    fprintf(stderr,"WOW_CALLBACK_GRAPH_OVERLAP errors=%u graph_succeeded=%u serialized=1 restored=1\n",
        borrow_errors-overlap_before,graph_result);
#ifdef WOW_HOOK_GRAPH_OVERLAP
    fprintf(stderr,"WOW_HOOK_GRAPH_OVERLAP errors=%u actual_hook=1 serialized=1 restored=1\n",
        borrow_errors-overlap_before);
#endif
    CloseHandle(controller);
#endif
    wait=WaitForSingleObject(worker,10000);
    if (wait != WAIT_OBJECT_0) {
        fprintf(stderr,"WOW_TERMINAL_SCHEDULE worker timeout=%lu\n",wait);
        ExitProcess(2); /* Do not release stack-owned state under a live worker. */
    }
    BORROW_CHECK(GetExitCodeThread(worker, &worker_exit) && worker_exit == 0);
#ifdef WOW_CALLBACK_GRAPH_OVERLAP
#ifndef WOW_HOOK_GRAPH_OVERLAP
    BORROW_CHECK(wow_task_callback_leave(&callback));
#endif
    {
        unsigned fault_before=borrow_errors, held;
        for (held=0;held<2;++held) {
            BOOL caught=FALSE;
            if (held) BORROW_CHECK(wow_user_runtime_enter(main_binding));
            InterlockedExchange(&t.graph_raise,1);
            __try { (void)wow_user_window_refresh_graph(NULL); }
            __except(GetExceptionCode()==0xe0473111u ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) { caught=TRUE; }
            BORROW_CHECK(caught && main_binding->exclusive_held==(BOOL)held);
            if (held) BORROW_CHECK(wow_user_runtime_leave(main_binding));
        }
        fprintf(stderr,"WOW_GRAPH_OWNER_EXCEPTION errors=%u caught=2 held=1 unheld=1\n",
            borrow_errors-fault_before);
    }
    CloseHandle(t.locked);CloseHandle(t.finish_graph);CloseHandle(t.graph_requested);
#endif
    terminal_schedule_active=NULL;
    BORROW_CHECK(t.bound && t.retired && t.unbound);
#ifdef WOW_CALLBACK_GRAPH_OVERLAP
    /* The native test window is still alive, so graph publication may
     * observe it again as a resource; its guest owner must be gone. */
    BORROW_CHECK(main_binding->thread->ppi->objects->entries[LOWORD(t.window)].bType != TYPE_WINDOW);
#else
    BORROW_CHECK(mvdm_softpc_wow_page_domain_window_handle(LOWORD(t.window)) == 0);
#endif
    BORROW_CHECK(wow_user_runtime_enter(main_binding));
    BORROW_CHECK(wow_user_window_refresh_graph(NULL));
    BORROW_CHECK(wow_user_runtime_leave(main_binding));
    BORROW_CHECK(t.mutation_held==2 && t.mutation_unheld==0);
    fprintf(stderr,"WOW_TERMINAL_SCHEDULE errors=%u first_phase=%ld held=%u unheld=%u retired=%u resource_thread=1\n",
        borrow_errors-before,t.first_phase,t.mutation_held,t.mutation_unheld,t.retired);
    wow_window_words_release(t.words);
    BORROW_CHECK(DestroyWindow(t.window));
    CloseHandle(worker);CloseHandle(t.reached);CloseHandle(t.release);CloseHandle(t.attempting);
}
