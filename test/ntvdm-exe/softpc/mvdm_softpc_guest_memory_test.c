#include "mvdm_softpc_guest_memory.h"
#include <stdio.h>
#include <string.h>

/* The production adapter and lease owner run unchanged. Only CCPU's memory
 * and translation services are fixtures; real VMM mappings need integration. */
static session owner;
static unsigned checks, reads, writes, translations;
static int paging, vm, deny_second, deny_write;
static uint32_t cpl = 3, second_physical = 0x1000;
static uint8_t last_access;
#define CHECK(x) do { ++checks; if (!(x)) { \
    fprintf(stderr, "line %u: %s\n", __LINE__, #x); return 1; } } while (0)
session *session_thread_current(void) { return &owner; }
int session_valid(const session *s) { return s == &owner; }
int session_guest_memory_begin(session *s, void *context,
    guest_memory_read_fn read, guest_memory_write_fn write)
{ return guest_memory_lease_begin(&s->guest_memory_lease, context, read, write); }
void session_guest_memory_end(session *s)
{ guest_memory_lease_end(&s->guest_memory_lease); }
int session_guest_memory_acquire(session *s, uint32_t address, uint32_t count,
    uint32_t access, guest_memory_lease **lease, uint8_t **bytes)
{ return guest_memory_lease_acquire(&s->guest_memory_lease, address, count, access, lease, bytes); }
int session_guest_memory_release(session *s, guest_memory_lease *lease, int commit)
{ return guest_memory_lease_release(&s->guest_memory_lease, lease, commit); }
uint32_t c_sas_memory_size(void) { return 0x4000; }
int c_getPG(void) { return paging; }
int c_getVM(void) { return vm; }
uint32_t c_getCPL(void) { return cpl; }
int xtrn2phy(uint32_t address, uint8_t access, uint32_t *physical)
{
    ++translations; last_access = access;
    if (deny_write && (access & 1u)) return 0;
    if ((address & ~4095u) == 0x80000000u) *physical = 0x3000 + (address & 4095u);
    else if ((address & ~4095u) == 0x80001000u && !deny_second)
        *physical = second_physical + (address & 4095u);
    else return 0;
    return 1;
}
void c_sas_loads(uint32_t address, uint8_t *bytes, uint32_t count)
{ (void)address; ++reads; memset(bytes, 0x5a, count); }
void c_sas_stores(uint32_t address, uint8_t *bytes, uint32_t count)
{ (void)address; (void)bytes; (void)count; ++writes; }
void c_sas_move_bytes_forward(uint32_t source, uint32_t destination, uint32_t count)
{ (void)source; (void)destination; (void)count; ++writes; }

int main(void)
{
    uint8_t bytes[32];
    uint8_t *leased;
    guest_memory_lease *lease;
    unsigned before;
    owner.state = SESSION_STATE_ACTIVE;
    CHECK(mvdm_softpc_guest_memory_begin(&owner));
    CHECK(mvdm_softpc_guest_memory_copy_from(0x3ff0, bytes, 16));
    CHECK(!translations && reads == 1 && bytes[0] == 0x5a);
    CHECK(!mvdm_softpc_guest_memory_copy_from(0x3ff0, bytes, 17));
    CHECK(!mvdm_softpc_guest_memory_copy_from(0xfffffff0, bytes, 32));
    CHECK(reads == 1);
    paging = 1;
    CHECK(mvdm_softpc_guest_memory_copy_from(0x80000ff0, bytes, 32));
    CHECK(translations == 2 && last_access == 0 && reads == 2);
    cpl = 0;
    CHECK(mvdm_softpc_guest_memory_copy_from(0x80000000, bytes, 1));
    CHECK(last_access == 2);
    cpl = 3; vm = 1;
    CHECK(mvdm_softpc_guest_memory_copy_to(0x80000ff0, bytes, 32));
    CHECK(last_access == 5 && writes == 1);
    before = reads; deny_second = 1;
    CHECK(!mvdm_softpc_guest_memory_copy_from(0x80000ff0, bytes, 32));
    CHECK(reads == before);
    CHECK(!mvdm_softpc_guest_memory_copy_forward(0x80000ff0, 0x80000000, 32));
    CHECK(writes == 1);
    deny_second = 0; second_physical = 0x4000;
    CHECK(!mvdm_softpc_guest_memory_copy_from(0x80000ff0, bytes, 32));
    CHECK(reads == before);
    second_physical = 0x1000;
    CHECK(mvdm_softpc_guest_memory_copy_from(0x80000ff0, bytes, 32));
    CHECK(session_guest_memory_acquire(&owner, 0x80000ff0, 32,
        GUEST_MEMORY_ACCESS_WRITE, &lease, &leased));
    memset(leased, 0xab, 32);
    deny_write = 1;
    CHECK(!session_guest_memory_release(&owner, lease, 1));
    CHECK(writes == 1 && !lease->active && !lease->bounce);
    for (unsigned i = 0; i < GUEST_MEMORY_LEASE_MAXIMUM; ++i)
        CHECK(!owner.guest_memory_lease.leases[i].active);
    mvdm_softpc_guest_memory_end(&owner);
    CHECK(!owner.guest_memory_lease.active);
    printf("PASS %u linear guest-memory assertions\n", checks);
    return 0;
}
