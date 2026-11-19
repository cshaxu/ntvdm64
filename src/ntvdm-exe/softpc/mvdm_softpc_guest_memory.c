#include "mvdm_softpc_guest_memory.h"

/* CPU40 `sas4gen.h` publishes these exact CCPU exports after its historical
 * `insignia.h` macro layer.  This adapter deliberately declares only the
 * fixed-width entries it consumes: importing `sas.h` here would make a
 * non-mirror translation unit depend on that source-era include ordering. */
extern uint32_t c_sas_memory_size(void);
extern void c_sas_loads(uint32_t address, uint8_t *bytes,
    uint32_t byte_count);
extern void c_sas_stores(uint32_t address, uint8_t *bytes,
    uint32_t byte_count);
extern void c_sas_move_bytes_forward(uint32_t source, uint32_t destination,
    uint32_t byte_count);
extern int c_getPG(void), c_getVM(void);
extern uint32_t c_getCPL(void);
extern int xtrn2phy(uint32_t address, uint8_t access, uint32_t *physical);

static int mvdm_softpc_guest_memory_range_valid(uint32_t address,
    uint32_t byte_count, int write)
{
    uint32_t memory_size = c_sas_memory_size();
    uint8_t access = write ? 1u : 0u;

    if (byte_count && byte_count - 1u > UINT32_MAX - address) return 0;
    if (!c_getPG())
        return address <= memory_size && byte_count <= memory_size - address;

    /* Match bios_read/write_* in original ccpusas4.c. xtrn2phy checks the
     * current TLB/page tables without taking a guest fault. Preflight every
     * page before SAS copies: a failed lease must not enter BIOS emulation
     * or partially write its destination. Called synchronously on the CPU
     * thread; no mapping or guest pointer survives this operation. */
    if (c_getCPL() == 3u) access |= 2u;
    if (c_getVM()) access |= 4u;
    while (byte_count) {
        uint32_t physical, chunk = 4096u - (address & 4095u);
        if (chunk > byte_count) chunk = byte_count;
        if (!xtrn2phy(address, access, &physical) ||
            physical > memory_size || chunk > memory_size - physical)
            return 0;
        address += chunk;
        byte_count -= chunk;
    }
    return 1;
}

static int mvdm_softpc_guest_memory_read(void *context, uint32_t address,
    uint8_t *bytes, uint32_t byte_count)
{
    (void)context;
    if (bytes == NULL || !mvdm_softpc_guest_memory_range_valid(address,
        byte_count, 0)) return 0;
    if (byte_count != 0u) c_sas_loads(address, bytes, byte_count);
    return 1;
}

static int mvdm_softpc_guest_memory_write(void *context, uint32_t address,
    const uint8_t *bytes, uint32_t byte_count)
{
    (void)context;
    if (bytes == NULL || !mvdm_softpc_guest_memory_range_valid(address,
        byte_count, 1)) return 0;
    if (byte_count != 0u) c_sas_stores(address, (uint8_t *)bytes, byte_count);
    return 1;
}

int mvdm_softpc_guest_memory_begin(session *owner)
{
    if (owner == NULL || !session_valid(owner) ||
        owner->guest_memory_lease.active != 0u) return 0;
    return session_guest_memory_begin(owner, NULL, mvdm_softpc_guest_memory_read,
        mvdm_softpc_guest_memory_write);
}

void mvdm_softpc_guest_memory_end(session *owner)
{
    if (owner != NULL) session_guest_memory_end(owner);
}

int mvdm_softpc_guest_memory_copy_forward(uint32_t destination,
    uint32_t source, uint32_t byte_count)
{
    session *owner = session_thread_current();

    if (byte_count > UINT32_MAX - source ||
        byte_count > UINT32_MAX - destination) return 0;
    if (byte_count == 0u) return 1;
    if (!session_valid(owner) || owner->state != SESSION_STATE_ACTIVE ||
        owner->guest_memory_lease.active != 1u ||
        !mvdm_softpc_guest_memory_range_valid(source, byte_count, 0) ||
        !mvdm_softpc_guest_memory_range_valid(destination, byte_count, 1)) return 0;

    /* XMS's original MoveBlock contract overwrites every destination byte in
     * forward order.  Keep the transfer as numeric CCPU addresses: unlike a
     * generic WRITE lease, it neither exposes a host alias nor hydrates a
     * destination bounce buffer that this operation cannot read. */
    c_sas_move_bytes_forward(source, destination, byte_count);
    return 1;
}
