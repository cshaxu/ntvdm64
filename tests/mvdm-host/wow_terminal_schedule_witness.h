/* Test-only forced interleaving on an actual published CCPU window. */
typedef struct terminal_schedule_test {
    wow_user_runtime *runtime;
    session *session_owner;
    HWND window;
    wow_window_words_binding *words;
    HANDLE reached, release;
#ifdef WOW_CALLBACK_GRAPH_OVERLAP
    HANDLE locked, finish_graph, graph_requested;
    volatile LONG graph_gate, graph_acquired;
    DWORD graph_thread;
#endif
    DWORD worker_id;
    volatile LONG first_phase;
    unsigned mutation_held, mutation_unheld;
    BOOL bound, retired, unbound;
} terminal_schedule_test;
static terminal_schedule_test *terminal_schedule_active;
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
#ifdef WOW_CALLBACK_GRAPH_OVERLAP
    wow_task_callback_scope callback;
    BOOL graph_result;
    HANDLE controller;
    DWORD controller_exit=0;
    unsigned overlap_before;
#endif
    t.runtime=main_binding->runtime;
    t.session_owner=session_thread_current();
    BORROW_CHECK(t.session_owner != NULL);
    t.reached=CreateEventW(NULL,TRUE,FALSE,NULL);
    t.release=CreateEventW(NULL,TRUE,FALSE,NULL);
#ifdef WOW_CALLBACK_GRAPH_OVERLAP
    t.locked=CreateEventW(NULL,TRUE,FALSE,NULL);
    t.finish_graph=CreateEventW(NULL,TRUE,FALSE,NULL);
    t.graph_requested=CreateEventW(NULL,TRUE,FALSE,NULL);
    BORROW_CHECK(t.locked && t.finish_graph && t.graph_requested);
#endif
    t.window=CreateWindowExW(0,L"STATIC",L"",0,0,0,1,1,NULL,NULL,GetModuleHandleW(NULL),NULL);
    BORROW_CHECK(t.reached && t.release && t.window && wow_window_words_attach(t.window,&words));
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
    BORROW_CHECK(wow_user_runtime_leave(main_binding));
#ifdef WOW_CALLBACK_GRAPH_OVERLAP
    BORROW_CHECK(wow_task_callback_enter(&callback));
    BORROW_CHECK(!main_binding->exclusive_held);
#endif
    BORROW_CHECK(SetEvent(t.release));
#ifdef WOW_CALLBACK_GRAPH_OVERLAP
    wait=WaitForSingleObject(t.locked,10000);
    if (wait != WAIT_OBJECT_0) ExitProcess(3);
    overlap_before=borrow_errors;
    BORROW_CHECK(t.runtime->owner_thread_id==t.worker_id && !main_binding->exclusive_held);
    t.graph_thread=GetCurrentThreadId();
    InterlockedExchange(&t.graph_gate,1);
    controller=CreateThread(NULL,0,release_graph_competitor,&t,0,NULL);
    if (!controller) ExitProcess(4);
    graph_result=wow_user_window_refresh_payload(NULL,NULL,0);
    InterlockedExchange(&t.graph_gate,0);
    BORROW_CHECK(WaitForSingleObject(controller,10000)==WAIT_OBJECT_0);
    BORROW_CHECK(GetExitCodeThread(controller,&controller_exit) && !controller_exit);
    BORROW_CHECK(graph_result && t.graph_acquired && !main_binding->exclusive_held);
    fprintf(stderr,"WOW_CALLBACK_GRAPH_OVERLAP errors=%u graph_succeeded=%u serialized=1 restored=1\n",
        borrow_errors-overlap_before,graph_result);
    CloseHandle(controller);
#endif
    wait=WaitForSingleObject(worker,10000);
    if (wait != WAIT_OBJECT_0) {
        fprintf(stderr,"WOW_TERMINAL_SCHEDULE worker timeout=%lu\n",wait);
        ExitProcess(2); /* Do not release stack-owned state under a live worker. */
    }
    BORROW_CHECK(GetExitCodeThread(worker, &worker_exit) && worker_exit == 0);
#ifdef WOW_CALLBACK_GRAPH_OVERLAP
    BORROW_CHECK(wow_task_callback_leave(&callback));
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
    CloseHandle(worker);CloseHandle(t.reached);CloseHandle(t.release);
}
