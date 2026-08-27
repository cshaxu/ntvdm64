#include "mvdm_xms_memory.h"

#include <string.h>

#include "adapter-bochs/machine_facade.h"
#include "session/session.h"

static int mvdm_xms_range_valid(uint32_t address, uint32_t byte_count)
{
    session *instance = session_thread_current();

    return instance != NULL && byte_count <= UINT32_MAX - address &&
        machine_facade_memory_readable(address, byte_count) &&
        machine_facade_memory_writable(address, byte_count);
}

int mvdm_xms_memory_copy(uint32_t destination, uint32_t source,
    uint32_t byte_count)
{
    session *instance = session_thread_current();
    guest_memory_lease *source_lease = NULL;
    guest_memory_lease *destination_lease = NULL;
    uint8_t *source_bytes = NULL;
    uint8_t *destination_bytes = NULL;
    int result = 0;

    if (instance == NULL || byte_count > UINT32_MAX - source ||
        byte_count > UINT32_MAX - destination ||
        !session_guest_memory_acquire(instance, source, byte_count,
            GUEST_MEMORY_ACCESS_READ, &source_lease, &source_bytes) ||
        !session_guest_memory_acquire(instance, destination, byte_count,
            GUEST_MEMORY_ACCESS_WRITE, &destination_lease, &destination_bytes)) {
        if (source_lease != NULL)
            (void)session_guest_memory_release(instance, source_lease, 0);
        return 0;
    }

    memcpy(destination_bytes, source_bytes, byte_count);
    result = session_guest_memory_release(instance, destination_lease, 1);
    (void)session_guest_memory_release(instance, source_lease, 0);
    return result;
}

int32_t xmsCommitBlock(uint32_t base_address, uint32_t byte_count)
{
    return mvdm_xms_range_valid(base_address, byte_count) ? 0 : -1;
}

int32_t xmsDecommitBlock(uint32_t base_address, uint32_t byte_count)
{
    /* Bochs owns one RAM backing.  The source suballocator owns allocation
       visibility; this same-shaped seam validates the released numeric span. */
    return mvdm_xms_range_valid(base_address, byte_count) ? 0 : -1;
}

void xmsMoveMemory(uint32_t destination, uint32_t source, uint32_t byte_count)
{
    (void)mvdm_xms_memory_copy(destination, source, byte_count);
}
