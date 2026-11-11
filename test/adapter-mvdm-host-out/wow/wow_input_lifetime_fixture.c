/* Supplementary actual native service test. Original Recalc2 is linked from
 * the selected input.c; the current production input owner is included below.
 * Only allocation failure and one native-call failure are injected. All
 * successful attachments and keyboard observations use real private USER. */
#include <windows.h>
#include <stdio.h>
#include <string.h>
static LONG allocations, fail_allocation, live_allocations, attaches, detaches;
static BOOL fail_native;
static void *WINAPI input_alloc(HANDLE heap, DWORD flags, SIZE_T size)
{
    void *value;
    if (++allocations == fail_allocation) return NULL;
    value = HeapAlloc(heap, flags, size);
    if (value) ++live_allocations;
    return value;
}
static BOOL WINAPI input_free(HANDLE heap, DWORD flags, void *value)
{
    BOOL result = HeapFree(heap, flags, value);
    if (result) --live_allocations;
    return result;
}
static BOOL WINAPI input_attach(DWORD first, DWORD second, BOOL attach)
{
    BOOL result;
    if (attach) {
        ++attaches;
        if (fail_native && attaches == 3) { SetLastError(ERROR_ACCESS_DENIED); return FALSE; }
    } else ++detaches;
    result = AttachThreadInput(first, second, attach);
    fprintf(stderr,"INPUT_NATIVE first=%lu second=%lu attach=%u result=%u\n",first,second,attach,result);
    return result;
}
#define HeapAlloc input_alloc
#define HeapFree input_free
#define AttachThreadInput input_attach
#include "wow32-dll/source/wow_input_group_lifetime.c"
#undef HeapAlloc
#undef HeapFree
#undef AttachThreadInput

static unsigned errors;
#define CHECK(x) do { if (!(x)) { ++errors; fprintf(stderr,"FAIL line=%d error=%lu\n",__LINE__,GetLastError()); } } while (0)
/* Failure callback is an explicit test seam, not the production fail-fast
 * implementation. It proves refusal after a partially successful mutation;
 * native lifecycle fixtures separately link the actual production owner. */
__declspec(noreturn) void WINAPI wow_user_native_call_failed(const char *stage, DWORD error)
{
    fprintf(stderr,"INPUT_TERMINAL stage=%s error=%lu attaches=%ld\n",stage,error,attaches);
    ExitProcess(91);
}
typedef struct peer {
    HDESK desktop;
    HANDLE ready, request, reply, stop, thread;
    DWORD id;
    BOOL ok;
    volatile LONG write_key;
    BYTE keys[256];
} peer;
static DWORD WINAPI peer_main(void *argument)
{
    peer *p = argument;
    HANDLE events[2] = {p->stop,p->request};
    MSG msg;
    p->ok = SetThreadDesktop(p->desktop);
    if (p->ok) {
        (void)IsGUIThread(TRUE); p->ok = IsGUIThread(FALSE);
        PeekMessageW(&msg,NULL,0,0,PM_NOREMOVE);
    }
    SetEvent(p->ready);
    for (;;) {
        DWORD result = MsgWaitForMultipleObjects(2,events,FALSE,10000,QS_ALLINPUT);
        if (result == WAIT_OBJECT_0) return 0;
        if (result == WAIT_OBJECT_0+2) {
            while (PeekMessageW(&msg,NULL,0,0,PM_REMOVE)) DispatchMessageW(&msg);
            continue;
        }
        if (result != WAIT_OBJECT_0+1) return 2;
        {
            LONG key = InterlockedExchange(&p->write_key,0);
            if (key) { BYTE state[256] = {0}; state[key] = 0x80; CHECK(SetKeyboardState(state)); }
        }
        p->ok = GetKeyboardState(p->keys); SetEvent(p->reply);
    }
}
static void start_peer(peer *p, HDESK desktop)
{
    p->desktop = desktop;
    p->ready = CreateEventW(NULL,FALSE,FALSE,NULL);
    p->request = CreateEventW(NULL,FALSE,FALSE,NULL);
    p->reply = CreateEventW(NULL,FALSE,FALSE,NULL);
    p->stop = CreateEventW(NULL,TRUE,FALSE,NULL);
    p->thread = CreateThread(NULL,0,peer_main,p,0,&p->id);
    if (!p->thread || WaitForSingleObject(p->ready,5000) != WAIT_OBJECT_0 || !p->ok) ExitProcess(92);
}
static void sample(peer *p, int key)
{
    InterlockedExchange(&p->write_key,key);
    CHECK(SetEvent(p->request));
    if (WaitForSingleObject(p->reply,5000) != WAIT_OBJECT_0) ExitProcess(93);
    CHECK(p->ok);
    fprintf(stderr,"INPUT_KEYS tid=%lu write=%d A=%u B=%u C=%u D=%u E=%u F=%u\n",p->id,key,p->keys['A'],p->keys['B'],p->keys['C'],p->keys['D'],p->keys['E'],p->keys['F']);
}
static void insert(wow_input_desktop *desktop, wow_task_order_thread *thread)
{
    thread->PtiLink.Flink = &desktop->PtiList;
    thread->PtiLink.Blink = desktop->PtiList.Blink;
    desktop->PtiList.Blink->Flink = &thread->PtiLink;
    desktop->PtiList.Blink = &thread->PtiLink;
}
static void remove_member(wow_input_group_owner *owner, wow_task_order_thread *thread)
{
    SetLastError(0x44708401);
    wow_input_group_retire(owner,thread);
    CHECK(GetLastError() == 0x44708401);
    thread->PtiLink.Blink->Flink = thread->PtiLink.Flink;
    thread->PtiLink.Flink->Blink = thread->PtiLink.Blink;
    thread->PtiLink.Flink = thread->PtiLink.Blink = NULL;
    CHECK(!thread->pq && !thread->pqAttach);
}
static void commit(wow_input_group_owner *owner, wow_task_order_thread *current)
{
    wow_input_group_plan *plan;
    ULONG i;
    SetLastError(0x44708402);
    plan = wow_input_group_prepare(owner);
    if (!plan) ExitProcess(94);
    CHECK(GetLastError() == 0x44708402);
    wow_input_group_commit(owner,plan,current);
    CHECK(GetLastError() == 0x44708402);
    for (i = 0; i < plan->count; ++i) {
        fprintf(stderr,"INPUT_COMPONENT tid=%lu flags=%08lx group=%lu\n",plan->rows[i].thread->native_thread_id,plan->rows[i].thread->TIF_flags,plan->rows[i].group);
        CHECK(!plan->rows[i].thread->pq && !plan->rows[i].thread->pqAttach);
        CHECK(plan->rows[i].thread->pcti->fsChangeBits == 0x77);
        CHECK(plan->rows[i].group && plan->rows[i].group <= plan->count);
    }
    SetLastError(0x44708403);
    wow_input_group_release_plan(plan);
    CHECK(GetLastError() == 0x44708403);
}
static DWORD WINAPI exercise(void *desktop)
{
    wow_input_desktop view = {0};
    wow_input_group_owner owner;
    wow_task_order_process process = {0};
    wow_task_order_thread members[4] = {0};
    wow_task_order_queue clients[4] = {0};
    peer peers[4] = {0};
    BYTE state[256] = {0};
    BYTE local[256];
    MSG queue_probe;
    LONG before;
    unsigned i;
    if (!SetThreadDesktop((HDESK)desktop)) return 95;
    (void)IsGUIThread(TRUE); if (!IsGUIThread(FALSE)) return 95;
    PeekMessageW(&queue_probe,NULL,0,0,PM_NOREMOVE);
    state['Z'] = 0x80;
    CHECK(SetKeyboardState(state) && GetKeyboardState(local));
    fprintf(stderr,"INPUT_NATIVE_CONTROL own_Z=%u before_group=1\n",local['Z']);
    if (!(local['Z'] & 0x80)) return 95;
    ZeroMemory(state,sizeof(state)); CHECK(SetKeyboardState(state));
    view.PtiList.Flink = view.PtiList.Blink = &view.PtiList;
    wow_input_group_initialize(&owner,&view);
    peers[0].id = GetCurrentThreadId();
    for (i = 1; i < 4; ++i) start_peer(&peers[i],(HDESK)desktop);
    for (i = 0; i < 4; ++i) {
        members[i].ppi = &process; members[i].rpdesk = &view;
        members[i].pcti = &clients[i]; clients[i].fsChangeBits = 0x77;
        members[i].native_thread_id = peers[i].id;
        members[i].native_thread = OpenThread(SYNCHRONIZE|THREAD_QUERY_LIMITED_INFORMATION,FALSE,peers[i].id);
        CHECK(members[i].native_thread);
    }
    insert(&view,&members[0]); members[0].TIF_flags = TIF_16BIT;
    commit(&owner,&members[0]); CHECK(!owner.links && !live_allocations);
    insert(&view,&members[1]);
    for (i = 1; i <= 3; ++i) {
        wow_input_group_plan *failed;
        before = attaches; allocations = 0; fail_allocation = (LONG)i;
        SetLastError(0x7777); failed = wow_input_group_prepare(&owner);
        CHECK(!failed && GetLastError() == ERROR_NOT_ENOUGH_MEMORY);
        CHECK(attaches == before && !live_allocations && !owner.links);
        CHECK(!members[0].pqAttach && !members[1].pqAttach);
    }
    fail_allocation = 0;
    members[1].TIF_flags = TIF_16BIT; commit(&owner,&members[1]);
    before = attaches; commit(&owner,&members[1]); CHECK(attaches == before);
    CHECK(SetKeyboardState(state)); state['A'] = 0x80; CHECK(SetKeyboardState(state));
    sample(&peers[1],0); CHECK(peers[1].keys['A'] & 0x80);
    if (!fail_native) {
        /* An application-owned duplicate is not ours to remove. */
        CHECK(AttachThreadInput(peers[0].id,peers[1].id,TRUE));
        remove_member(&owner,&members[1]); CHECK(!owner.links && !live_allocations);
        ZeroMemory(state,sizeof(state)); state['B'] = 0x80; CHECK(SetKeyboardState(state));
        sample(&peers[1],0); CHECK(peers[1].keys['B'] & 0x80);
        CHECK(AttachThreadInput(peers[0].id,peers[1].id,FALSE));
        ZeroMemory(state,sizeof(state)); state['C'] = 0x80; CHECK(SetKeyboardState(state));
        sample(&peers[1],0); CHECK(!(peers[1].keys['C'] & 0x80));
        insert(&view,&members[1]); commit(&owner,&members[1]);
    }
    insert(&view,&members[2]); members[2].TIF_flags = TIF_16BIT;
    commit(&owner,&members[2]); /* injected native call3 must terminate here */
    if (fail_native) return 96;
    remove_member(&owner,&members[0]);
    sample(&peers[1],'D'); sample(&peers[2],0); CHECK(peers[2].keys['D'] & 0x80);
    insert(&view,&members[3]); members[3].TIF_flags = TIF_16BIT;
    commit(&owner,&members[3]);
    sample(&peers[1],'E'); sample(&peers[2],0); sample(&peers[3],0);
    CHECK((peers[2].keys['E'] & 0x80) && (peers[3].keys['E'] & 0x80));
    before = attaches; commit(&owner,&members[2]); CHECK(before == attaches);
    remove_member(&owner,&members[1]);
    sample(&peers[2],'F'); sample(&peers[3],0); CHECK(peers[3].keys['F'] & 0x80);
    remove_member(&owner,&members[2]); remove_member(&owner,&members[3]);
    CHECK(!owner.links && !live_allocations);
    for (i = 0; i < 4; ++i) CloseHandle(members[i].native_thread);
    for (i = 1; i < 4; ++i) {
        DWORD code;
        SetEvent(peers[i].stop);
        if (WaitForSingleObject(peers[i].thread,5000) != WAIT_OBJECT_0) ExitProcess(97);
        CHECK(GetExitCodeThread(peers[i].thread,&code) && !code);
        CloseHandle(peers[i].thread); CloseHandle(peers[i].ready);
        CloseHandle(peers[i].request); CloseHandle(peers[i].reply); CloseHandle(peers[i].stop);
    }
    fprintf(stderr,"WOW_INPUT_LIFETIME errors=%u original_components=1 private_native=1 allocation_failures=3 idempotent=1 external_reference=1 origin_exit=1 rejoin=1 middle_exit=1 last_exit=1 scratch_escape=0 live=%ld\n",errors,live_allocations);
    return errors != 0;
}
int __cdecl main(int argc, char **argv)
{
    WCHAR name[80];
    HDESK desktop;
    HANDLE thread;
    DWORD code;
    BOOL owned_desktop;
    fail_native = argc == 2 && !strcmp(argv[1],"--native-failure");
    owned_desktop = argc == 2 && !strcmp(argv[1],"--private-desktop");
    wsprintfW(name,L"WowInputLifetime-%lu",GetCurrentProcessId());
    /* Keyboard witness follows the retained native-input probe: manipulate
     * only these new threads' USER tables, never SendInput or another app.
     * Nonactive-private-desktop control remains available but is not silently
     * counted as passed when SetKeyboardState cannot be observed there. */
    desktop = owned_desktop ? CreateDesktopW(name,NULL,NULL,0,GENERIC_ALL,NULL) :
        GetThreadDesktop(GetCurrentThreadId());
    fprintf(stderr,"INPUT_SCOPE owned_threads=1 private_desktop=%u send_input=0 windows=0\n",owned_desktop);
    if (!desktop) return 98;
    thread = CreateThread(NULL,0,exercise,desktop,0,NULL);
    if (!thread || WaitForSingleObject(thread,45000) != WAIT_OBJECT_0) ExitProcess(99);
    if (!GetExitCodeThread(thread,&code)) return 98;
    CloseHandle(thread); if (owned_desktop) CHECK(CloseDesktop(desktop));
    return code || errors;
}
