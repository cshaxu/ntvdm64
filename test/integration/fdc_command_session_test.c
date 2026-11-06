#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "ntvdm-exe/session/session.h"
#include "compat/include/thread_start_compat.h"
#undef CreateThread
int mvdm_command_guest_state_set_scs_scalars(uint16_t,uint16_t,uint32_t,uint32_t,uint16_t,uint16_t,uint16_t,uint16_t);
int mvdm_command_guest_state_add_fd_access(uint16_t);
int mvdm_command_guest_state_read_fd_access(uint16_t *);
static int checks;
#define CHECK(x) do { ++checks; if (!(x)) {printf("FAIL line %d: %s\n",__LINE__,#x); return 1;} } while(0)
typedef struct memory {
    uint8_t bytes[4096];
    volatile LONG hold;
    HANDLE entered, release;
} memory;
static int read_memory(void *opaque,uint32_t address,uint8_t *bytes,uint32_t count)
{
    memory *m=opaque;
    if(address>4096 || count>4096-address) return 0;
    if(InterlockedCompareExchange(&m->hold,0,0)) {
        SetEvent(m->entered);
        if(WaitForSingleObject(m->release,5000)!=WAIT_OBJECT_0) return 0;
    }
    memcpy(bytes,m->bytes+address,count);return 1;
}
static int write_memory(void *opaque,uint32_t address,const uint8_t *bytes,uint32_t count)
{
    memory *m=opaque;
    if(address>4096 || count>4096-address) return 0;
    memcpy(m->bytes+address,bytes,count);return 1;
}
typedef struct work {session *owner; int result; uint16_t value;} work;
static void __cdecl increment(void *opaque)
{
    work *w=opaque;
    w->result=session_thread_current()==w->owner && mvdm_command_guest_state_add_fd_access(1);
}
static void __cdecl read_counter(void *opaque)
{
    work *w=opaque;
    w->result=session_thread_current()==w->owner && mvdm_command_guest_state_read_fd_access(&w->value);
}
static DWORD WINAPI retire(void *opaque)
{
    session_guest_memory_end(opaque);return 0;
}
static int register_memory(void)
{
    return mvdm_command_guest_state_set_scs_scalars(0,0x100,32,1,0,0x200,0,0x202);
}
int main(void)
{
    session a,b; memory m={0},n={0}; work w={0}; HANDLE thread,ender;
    guest_memory_lease *lease; uint8_t *bytes; uint16_t value;
    session_initialize(&a,1);session_initialize(&b,2);
    CHECK(session_activate(&a)&&session_activate(&b));
    CHECK(session_guest_memory_begin(&a,&m,read_memory,write_memory));
    CHECK(session_guest_memory_begin(&b,&n,read_memory,write_memory));
    CHECK(session_thread_bind(&a));CHECK(register_memory());
    CHECK(mvdm_command_guest_state_add_fd_access(1));
    CHECK(session_guest_memory_acquire(&a,0x300,2,GUEST_MEMORY_ACCESS_WRITE,&lease,&bytes));
    bytes[0]=0x5a;bytes[1]=0xa5;
    w.owner=&a;
    thread=opennt_create_void_cdecl_parameter_thread(NULL,0,increment,&w,0,NULL);
    CHECK(thread!=NULL);CHECK(WaitForSingleObject(thread,5000)==WAIT_OBJECT_0);CloseHandle(thread);
    CHECK(w.result&&m.bytes[0x202]==2);
    CHECK(session_guest_memory_release(&a,lease,1));CHECK(m.bytes[0x300]==0x5a&&m.bytes[0x301]==0xa5);
    CHECK(a.binding_count==1);
    CHECK(session_thread_unbind(&a));CHECK(!mvdm_command_guest_state_add_fd_access(1));
    CHECK(session_thread_bind(&b));CHECK(!mvdm_command_guest_state_add_fd_access(1));
    CHECK(register_memory());CHECK(mvdm_command_guest_state_add_fd_access(1));
    CHECK(n.bytes[0x202]==1&&m.bytes[0x202]==2);CHECK(session_thread_unbind(&b));
    CHECK(session_thread_bind(&a));CHECK(mvdm_command_guest_state_read_fd_access(&value)&&value==2);
    session_guest_memory_end(&a);CHECK(!mvdm_command_guest_state_add_fd_access(1));
    CHECK(session_guest_memory_begin(&a,&m,read_memory,write_memory));
    CHECK(!mvdm_command_guest_state_add_fd_access(1));CHECK(register_memory());
    CHECK(mvdm_command_guest_state_add_fd_access(UINT16_MAX)&&m.bytes[0x202]==1);
    /* A copied scalar read must finish before the provider is retired. */
    m.entered=CreateEvent(NULL,TRUE,FALSE,NULL);m.release=CreateEvent(NULL,TRUE,FALSE,NULL);
    CHECK(m.entered&&m.release);InterlockedExchange(&m.hold,1);
    thread=opennt_create_void_cdecl_parameter_thread(NULL,0,read_counter,&w,0,NULL);
    CHECK(thread!=NULL);CHECK(WaitForSingleObject(m.entered,5000)==WAIT_OBJECT_0);
    CHECK(a.binding_count==2);CHECK(!session_dispose(&a));
    ender=CreateThread(NULL,0,retire,&a,0,NULL);CHECK(ender!=NULL);
    CHECK(WaitForSingleObject(ender,100)==WAIT_TIMEOUT);
    SetEvent(m.release);
    CHECK(WaitForSingleObject(thread,5000)==WAIT_OBJECT_0);CHECK(w.result&&w.value==1);
    CHECK(WaitForSingleObject(ender,5000)==WAIT_OBJECT_0);
    CloseHandle(thread);CloseHandle(ender);CloseHandle(m.entered);CloseHandle(m.release);
    CHECK(!mvdm_command_guest_state_read_fd_access(&value));
    CHECK(session_thread_unbind(&a));CHECK(session_dispose(&a));
    CHECK(a.mvdm_command_guest_state==NULL);CHECK(session_dispose(&b));
    printf("FDC_COMMAND_SESSION_PASS checks=%d\n",checks);return 0;
}
/* The actual COMMAND object also exports its separate PIF helper. */
int IsFirstVDM; unsigned char cmdVDMEnvBlk[1024];
void cmdCheckForPIF(void *p){(void)p;ExitProcess(98);}
