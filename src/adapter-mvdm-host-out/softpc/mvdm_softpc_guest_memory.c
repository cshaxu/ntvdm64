#include "mvdm_softpc_guest_memory.h"

/* CPU40 `sas4gen.h` publishes these exact CCPU exports after its historical
 * `insignia.h` macro layer.  This adapter deliberately declares only the
 * three fixed-width entries it consumes: importing `sas.h` here would make a
 * non-mirror translation unit depend on that source-era include ordering. */
extern uint32_t c_sas_memory_size(void);
extern void c_sas_loads(uint32_t address, uint8_t *bytes,
    uint32_t byte_count);
extern void c_sas_stores(uint32_t address, uint8_t *bytes,
    uint32_t byte_count);

static int mvdm_softpc_guest_memory_range_valid(uint32_t address,
    uint32_t byte_count)
{
    uint32_t memory_size = c_sas_memory_size();

    return address <= memory_size && byte_count <= memory_size - address;
}

static int mvdm_softpc_guest_memory_read(void *context, uint32_t address,
    uint8_t *bytes, uint32_t byte_count)
{
    (void)context;
    if (bytes == NULL || !mvdm_softpc_guest_memory_range_valid(address,
        byte_count)) return 0;
    if (byte_count != 0u) c_sas_loads(address, bytes, byte_count);
    return 1;
}

static int mvdm_softpc_guest_memory_write(void *context, uint32_t address,
    const uint8_t *bytes, uint32_t byte_count)
{
    (void)context;
    if (bytes == NULL || !mvdm_softpc_guest_memory_range_valid(address,
        byte_count)) return 0;
    if (byte_count != 0u) c_sas_stores(address, (uint8_t *)bytes, byte_count);
    return 1;
}

int mvdm_softpc_guest_memory_begin(session *owner)
{
    if (owner == NULL || !session_valid(owner) ||
        owner->guest_memory_lease.active != 0u) return 0;
    return session_guest_memory_begin(owner, owner, mvdm_softpc_guest_memory_read,
        mvdm_softpc_guest_memory_write);
}

void mvdm_softpc_guest_memory_end(session *owner)
{
    if (owner != NULL) session_guest_memory_end(owner);
}
