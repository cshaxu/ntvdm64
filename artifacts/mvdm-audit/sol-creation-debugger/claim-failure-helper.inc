/* Four failure-only instruction sites, verified against the exact selected
 * project-owned claim_explicit disassembly. No target code bytes are changed. */
typedef struct error_watch {
    DWORD thread_id, address, visits;
    HANDLE thread;
    BOOL armed;
    CONTEXT saved;
} error_watch;
static const DWORD claim_failure_rva[4] = {0x72bcb,0x72bef,0x72c80,0x72cc5};
static const BYTE claim_failure_bytes[4][3] = {
    {0x89,0x47,0x40},{0xff,0x77,0x54},{0xff,0x77,0x54},{0x89,0x47,0x40}
};
static const char *claim_phase[4] = {"enrollment","encoding-or-facts","client-state","final-borrow"};

static DWORD remote_error_entry(DWORD pid)
{
    DWORD base=(DWORD)(ULONG_PTR)wow32_base(pid), i;
    HANDLE process;
    if(!base)return 0;
    process=OpenProcess(PROCESS_VM_READ|PROCESS_QUERY_INFORMATION,FALSE,pid);
    if(!process)return 0;
    for(i=0;i<4;++i){
        BYTE bytes[3];SIZE_T read=0;
        if(!ReadProcessMemory(process,(LPCVOID)(ULONG_PTR)(base+claim_failure_rva[i]),bytes,sizeof(bytes),&read)||
                read!=sizeof(bytes)||memcmp(bytes,claim_failure_bytes[i],sizeof(bytes))){base=0;break;}
    }
    CloseHandle(process);
    printf("SOL claim-watch: exact failure instructions verified=%d module=%08lx\n",base!=0,base);
    return base;
}

static BOOL arm_error_watch(error_watch *watch,DWORD tid)
{
    CONTEXT context={.ContextFlags=CONTEXT_DEBUG_REGISTERS};
    if(tid!=watch->thread_id)return TRUE;
    watch->thread=OpenThread(THREAD_GET_CONTEXT|THREAD_SET_CONTEXT|THREAD_QUERY_INFORMATION|THREAD_SUSPEND_RESUME,FALSE,tid);
    if(!watch->thread||!GetThreadContext(watch->thread,&context))return FALSE;
    if(context.Dr7&0xffu){puts("SOL claim-watch: existing debug-register owner; not replaced");return FALSE;}
    watch->saved=context;
    context.Dr0=watch->address+claim_failure_rva[0];context.Dr1=watch->address+claim_failure_rva[1];
    context.Dr2=watch->address+claim_failure_rva[2];context.Dr3=watch->address+claim_failure_rva[3];
    context.Dr6=0;context.Dr7=0x55u;
    watch->armed=SetThreadContext(watch->thread,&context);
    printf("SOL claim-watch: armed=%d tid=%lu\n",watch->armed,tid);
    return watch->armed;
}

static void clear_error_watch(error_watch *watch)
{
    CONTEXT context={.ContextFlags=CONTEXT_DEBUG_REGISTERS};
    if(watch->armed){
        DWORD suspended=SuspendThread(watch->thread);
        if(suspended!=(DWORD)-1){
            if(GetThreadContext(watch->thread,&context)){
                context.Dr0=watch->saved.Dr0;context.Dr1=watch->saved.Dr1;
                context.Dr2=watch->saved.Dr2;context.Dr3=watch->saved.Dr3;
                context.Dr6=watch->saved.Dr6;context.Dr7=watch->saved.Dr7;
                if(!SetThreadContext(watch->thread,&context))printf("SOL claim-watch: clear failed=%lu\n",GetLastError());
            }else printf("SOL claim-watch: clear-context failed=%lu\n",GetLastError());
            (void)ResumeThread(watch->thread);
        }else printf("SOL claim-watch: clear-suspend failed=%lu\n",GetLastError());
    }
    watch->armed=FALSE;
    if(watch->thread){CloseHandle(watch->thread);watch->thread=NULL;}
}

static int observe_error_watch(error_watch *watch,DWORD pid,DWORD tid)
{
    CONTEXT context={.ContextFlags=CONTEXT_DEBUG_REGISTERS|CONTEXT_CONTROL|CONTEXT_INTEGER};
    DWORD slot;
    HANDLE process;
    if(!watch->armed||tid!=watch->thread_id||!GetThreadContext(watch->thread,&context))return -1;
    for(slot=0;slot<4;++slot)
        if((context.Dr6&(1u<<slot))&&context.Eip==watch->address+claim_failure_rva[slot])break;
    if(slot==4)return -1;
    ++watch->visits;
    printf("SOL claim-watch: CLAIM_FAILURE phase=%s error=%lu window=%08lx scope=%08lx pc=%08lx\n",
        claim_phase[slot],context.Eax,context.Ebx,context.Edi,context.Eip);
    process=OpenProcess(PROCESS_VM_READ|PROCESS_QUERY_INFORMATION,FALSE,pid);
    if(process){
        /* Only the live project creation_scope, whose field offsets are in
         * the pinned disassembly; not a native USER/NT private structure. */
        DWORD words[22];SIZE_T read=0;unsigned i;
        if(ReadProcessMemory(process,(LPCVOID)(ULONG_PTR)context.Edi,words,sizeof(words),&read)&&read==sizeof(words)){
            printf("SOL claim-watch: scope-words");
            for(i=0;i<ARRAYSIZE(words);++i)printf(" %08lx",words[i]);
            puts("");
        }
        CloseHandle(process);
    }
    report_fault_stack(pid,tid);
    report_stack_words(pid,context.Esp);
    context.Dr6=0;context.EFlags|=0x10000u;
    if(!SetThreadContext(watch->thread,&context))printf("SOL claim-watch: resume-context failed=%lu\n",GetLastError());
    return 1;
}
