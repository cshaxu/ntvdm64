#define MVDM_REDIRECTOR_WORKER_EXPORTS
#include "common/redirector/async.h"
#include "mvdm-host/inc/vrnmpipe.h"
#include "ntvdm-exe/session/session.h"
#include <stdio.h>
#include <string.h>
#include "compat/include/thread_start_compat.h"

/* Exercise the production staging adapter and real session leases. Only the
 * guest-memory provider is controlled; failed writes must not look completed. */
typedef struct memory_fixture {
    uint8_t bytes[1024];
    uint32_t writes[8];
    unsigned write_count;
    uint32_t fail_address;
    uint32_t request_limit;
} memory_fixture;
static unsigned assertions, failures;
static void check(int ok, char const *name)
{
    ++assertions;
    if (!ok) ++failures;
    printf("%s %s\n", ok ? "PASS" : "FAIL", name);
}
static int read_memory(void *context, uint32_t address, uint8_t *bytes,
    uint32_t count)
{
    memory_fixture *m = context;
    if (address > sizeof(m->bytes) || count > sizeof(m->bytes) - address ||
        (address == 0x100 && count > m->request_limit))
        return 0;
    memcpy(bytes, m->bytes + address, count);
    return 1;
}
static int write_memory(void *context, uint32_t address, uint8_t const *bytes,
    uint32_t count)
{
    memory_fixture *m = context;
    if (m->write_count < 8) m->writes[m->write_count] = address;
    ++m->write_count;
    if (address == m->fail_address || address > sizeof(m->bytes) ||
        count > sizeof(m->bytes) - address) return 0;
    memcpy(m->bytes + address, bytes, count);
    return 1;
}
static void put32(uint8_t *p, uint32_t value)
{
    p[0] = (uint8_t)value; p[1] = (uint8_t)(value >> 8);
    p[2] = (uint8_t)(value >> 16); p[3] = (uint8_t)(value >> 24);
}
static int prepare(memory_fixture *m, DOS_ASYNC_NAMED_PIPE_INFO *request,
    DWORD kind)
{
    LPBYTE staging;
    WORD length;
    memset(m, 0xa5, sizeof(*m));
    m->write_count = 0; m->fail_address = UINT32_MAX;
    m->request_limit = kind == ANP_READ2 || kind == ANP_WRITE2 ? 24u : 18u;
    memset(request, 0, sizeof(*request));
    memset(m->bytes + 0x100, 0, 24);
    put32(m->bytes + 0x100, 0x180);
    m->bytes[0x104] = 3;
    put32(m->bytes + 0x106, 0x140);
    put32(m->bytes + 0x10a, 0x182);
    if (!mvdm_redirector_async_prepare(request, 0, 0x100, kind,
        &staging, &length) || length != 3 || staging == NULL) return 0;
    memcpy(staging, "xyz", 3);
    return 1;
}
static DWORD __cdecl complete_on_bound_thread(LPVOID opaque)
{
    return session_thread_current() != NULL &&
        mvdm_redirector_async_complete(opaque, 3, 0) ? 0u : 1u;
}
static int no_live_leases(session const *s)
{
    unsigned i;
    for (i = 0; i < GUEST_MEMORY_LEASE_MAXIMUM; ++i)
        if (s->guest_memory_lease.leases[i].bounce != NULL) return 0;
    return 1;
}
int main(void)
{
    memory_fixture m;
    session s;
    DOS_ASYNC_NAMED_PIPE_INFO request;
    unsigned i;
    static uint32_t const destinations[] = {0x140, 0x182, 0x180};
    session_initialize(&s, 1);
    if (!session_activate(&s) || !session_guest_memory_begin(&s, &m,
        read_memory, write_memory) || !session_thread_bind(&s)) return 2;

    if (!prepare(&m, &request, ANP_READ)) return 2;
    check(m.request_limit == 18u, "original request does not require Type2 extension");
    check(mvdm_redirector_async_complete(&request, 3, ERROR_MORE_DATA),
        "partial read completion succeeds");
    check(m.write_count == 3 && m.writes[0] == 0x140 &&
        m.writes[1] == 0x182 && m.writes[2] == 0x180,
        "payload precedes error then count publication");
    check(!memcmp(m.bytes + 0x140, "xyz", 3) && m.bytes[0x180] == 3 &&
        m.bytes[0x182] == (uint8_t)ERROR_MORE_DATA,
        "partial read preserves actual bytes and error");
    mvdm_redirector_async_release(&request);
    mvdm_redirector_async_release(&request);
    check(request.PrivateAsyncState == NULL && no_live_leases(&s),
        "release is idempotent and leases are closed");

    {
        LPBYTE staging;
        WORD length;
        request.PrivateAsyncState = (void *)1;
        check(mvdm_redirector_async_prepare(&request, 0, 0x100, ANP_READ,
            &staging, &length) && request.PrivateAsyncState != (void *)1,
            "fresh LocalAlloc request needs no private-field initialization");
        mvdm_redirector_async_release(&request);
    }

    for (i = 0; i < 3; ++i) {
        if (!prepare(&m, &request, ANP_READ2)) return 2;
        m.fail_address = destinations[i];
        check(!mvdm_redirector_async_complete(&request, 3, 5),
            "destination failure rejects completion");
        check(m.write_count == i + 1 &&
            m.bytes[0x180] == 0xa5 &&
            (i != 0 || m.bytes[0x182] == 0xa5),
            "failure does not publish later completion words");
        check(no_live_leases(&s), "failed commit closes lease");
        mvdm_redirector_async_release(&request);
    }
    {
        LPBYTE staging;
        WORD length;
        m.request_limit = 18u;
        check(!mvdm_redirector_async_prepare(&request, 0, 0x100, ANP_READ2,
            &staging, &length) && request.PrivateAsyncState == NULL,
            "Type2 rejects unreadable semaphore extension");
    }
    if (!prepare(&m, &request, ANP_READ)) return 2;
    check(!mvdm_redirector_async_complete(&request, 4, 0) && m.write_count == 0,
        "oversized read publishes nothing");
    mvdm_redirector_async_release(&request);

    if (!prepare(&m, &request, ANP_WRITE2)) return 2;
    check(mvdm_redirector_async_complete(&request, 2, 5) &&
        m.write_count == 2 && m.writes[0] == 0x182 && m.writes[1] == 0x180 &&
        m.bytes[0x140] == 0xa5, "write completion does not copy staging back");
    mvdm_redirector_async_release(&request);

    if (!prepare(&m, &request, ANP_READ)) return 2;
    check(mvdm_redirector_async_complete(&request, 0, 0) &&
        m.write_count == 2 && m.bytes[0x140] == 0xa5,
        "zero read publishes only completion words");
    mvdm_redirector_async_release(&request);

    if (!prepare(&m, &request, ANP_READ)) return 2;
    {
        DWORD result = 1;
        HANDLE worker = opennt_create_cdecl_thread_named(NULL, 0,
            complete_on_bound_thread, &request, 0, NULL, "redirector-owner-witness");
        if (worker == NULL) return 2;
        check(WaitForSingleObject(worker, 5000) == WAIT_OBJECT_0 &&
            GetExitCodeThread(worker, &result) && result == 0,
            "production thread bridge binds before direct completion");
        CloseHandle(worker);
        check(session_thread_current() == &s,
            "production thread bridge leaves parent binding unchanged");
    }
    m.write_count = 0;
    if (!session_thread_unbind(&s)) return 2;
    check(!mvdm_redirector_async_complete(&request, 3, 0) && m.write_count == 0,
        "unbound thread cannot publish");
    check(session_thread_current() == NULL, "failed completion does not bind caller");
    mvdm_redirector_async_release(&request);
    if (!session_thread_bind(&s)) return 2;

    if (!prepare(&m, &request, ANP_READ)) return 2;
    ++s.epoch;
    check(!mvdm_redirector_async_complete(&request, 3, 0) && m.write_count == 0,
        "stale owner epoch rejected before publication");
    --s.epoch;
    check(session_request_cancellation(&s, SESSION_CANCELLATION_REQUESTED) &&
        !mvdm_redirector_async_complete(&request, 3, 0) && m.write_count == 0,
        "cancelled owner rejects original completion entry");
    session_guest_memory_end(&s);
    check(!mvdm_redirector_async_complete(&request, 3, 0) && m.write_count == 0,
        "ended guest memory cannot publish");
    mvdm_redirector_async_release(&request);
    check(no_live_leases(&s) && session_thread_unbind(&s) && session_dispose(&s),
        "final session cleanup succeeds");
    printf("assertions=%u failures=%u\n", assertions, failures);
    return failures ? 1 : 0;
}
