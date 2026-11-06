/* Exercise the production C facade cleanup with native Windows exceptions. */
#include "lib-win32/process-hook/unwind.h"
#include <stdio.h>
#include <setjmp.h>
#define TEST_EXCEPTION 0xe0424242u
#define CHECK(x) do{if(!(x)){fprintf(stderr,"UNWIND FAIL line=%d error=%lu\n",__LINE__,GetLastError());return 1;}}while(0)
static _Thread_local unsigned cleaned,order;
static void cleanup(void *value)
{++cleaned;order=order*10+(unsigned)(ULONG_PTR)value;SetLastError(999);}
static PROCESS_HOOK_UNWIND_FUNCTION void inner(BOOL raises)
{
    process_hook_unwind_scope scope;
    PROCESS_HOOK_UNWIND_BEGIN(scope,cleanup,(void *)1);
    if(raises)RaiseException(TEST_EXCEPTION,0,0,NULL);
    PROCESS_HOOK_UNWIND_END(scope);
}
static PROCESS_HOOK_UNWIND_FUNCTION void outer(void)
{
    process_hook_unwind_scope scope;
    PROCESS_HOOK_UNWIND_BEGIN(scope,cleanup,(void *)2);
    inner(TRUE);
    PROCESS_HOOK_UNWIND_END(scope);
}
#if defined(_MSC_VER)
/* MSVC uses its native SEH syntax in the test harness too. */
#elif !defined(_WIN64)
typedef struct catcher {process_hook_unwind_record record;jmp_buf saved;} catcher;
static EXCEPTION_DISPOSITION NTAPI catch32(EXCEPTION_RECORD *exception,
    void *frame,CONTEXT *context,void *dispatcher)
{
    (void)context;(void)dispatcher;
    if(!(exception->ExceptionFlags&(EXCEPTION_UNWINDING|EXCEPTION_EXIT_UNWIND)) &&
        exception->ExceptionCode==TEST_EXCEPTION){
        RtlUnwind(frame,NULL,exception,NULL);longjmp(((catcher *)frame)->saved,1);
    }
    return ExceptionContinueSearch;
}
#else
static LONG __attribute__((used)) filter(EXCEPTION_POINTERS *exception,void *frame)
{
    (void)frame;
    return exception->ExceptionRecord->ExceptionCode==TEST_EXCEPTION?
        EXCEPTION_EXECUTE_HANDLER:EXCEPTION_CONTINUE_SEARCH;
}
#endif
static PROCESS_HOOK_UNWIND_FUNCTION int exercise(void)
{
    cleaned=order=0;SetLastError(1234);inner(FALSE);
    CHECK(GetLastError()==1234 && cleaned==1 && order==1);
#if defined(_MSC_VER)
    __try { outer(); } __except(GetExceptionCode()==TEST_EXCEPTION ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {}
#elif !defined(_WIN64)
    catcher c;c.record.handler=catch32;
    __asm__ __volatile__("movl %%fs:0,%0":"=r"(c.record.previous)::"memory");
    __asm__ __volatile__("movl %0,%%fs:0"::"r"(&c.record):"memory");
    if(!setjmp(c.saved))outer();
    __asm__ __volatile__("movl %0,%%fs:0"::"r"(c.record.previous):"memory");
#else
    __asm__ __volatile__(".Lcatch_start:\n.seh_handler __C_specific_handler, @except\n"
        ".seh_handlerdata\n.long 1\n.rva .Lcatch_start,.Lcatch_end,filter,.Lcatch_end\n.text\n":::"memory");
    outer();
    __asm__ __volatile__("nop\n.Lcatch_end: nop\n":::"memory");
#endif
    CHECK(cleaned==3 && order==112);
    /* Reuse on this same thread proves no stale scope remains after unwind. */
    inner(FALSE);CHECK(cleaned==4 && order==1121);
    return 0;
}
static DWORD WINAPI thread_main(void *unused)
{(void)unused;return (DWORD)exercise();}
int main(void)
{
    HANDLE thread[2];DWORD result;
    CHECK(!exercise());
    for(unsigned i=0;i<2;++i){thread[i]=CreateThread(NULL,0,thread_main,NULL,0,NULL);CHECK(thread[i]);}
    CHECK(WaitForMultipleObjects(2,thread,TRUE,10000)==WAIT_OBJECT_0);
    for(unsigned i=0;i<2;++i){CHECK(GetExitCodeThread(thread[i],&result) && !result);CloseHandle(thread[i]);}
    CHECK(cleaned==4 && order==1121);
    printf("C UNWIND PASS width=%u normal/nested native exception/reuse/thread isolation\n",(unsigned)(sizeof(void *)*8));
    return 0;
}
