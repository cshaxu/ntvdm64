#include "xms_block_move.h"

#include <stdint.h>

#include "adapter-bochs/machine_facade.h"
#include "adapter-mvdm-host-out/softpc/include/mvdm_command_registers.h"
#include "adapter-mvdm-host-out/softpc/include/mvdm_xms_memory.h"
#include "session/session.h"

int mvdm_xms_move_block_from_stack(void)
{
    session *instance = session_thread_current();
    guest_memory_lease *lease = NULL;
    uint8_t *bytes = NULL;
    uint32_t stack_address;
    uint32_t word_count;
    uint32_t source;
    uint32_t destination;
    uint16_t ss;
    uint16_t bp;

    if (instance == NULL || !machine_facade_copy_ss16(&ss) ||
        !machine_facade_copy_bp16(&bp) || bp < 12u)
        return 0;
    stack_address = ((uint32_t)ss << 4) + (uint32_t)bp - 12u;
    if (!session_guest_memory_acquire(instance, stack_address, 12u,
        GUEST_MEMORY_ACCESS_READ, &lease, &bytes)) return 0;
    word_count = (uint32_t)bytes[8] | ((uint32_t)bytes[9] << 8) |
        ((uint32_t)bytes[10] << 16) | ((uint32_t)bytes[11] << 24);
    source = (uint32_t)bytes[4] | ((uint32_t)bytes[5] << 8) |
        ((uint32_t)bytes[6] << 16) | ((uint32_t)bytes[7] << 24);
    destination = (uint32_t)bytes[0] | ((uint32_t)bytes[1] << 8) |
        ((uint32_t)bytes[2] << 16) | ((uint32_t)bytes[3] << 24);
    if (!session_guest_memory_release(instance, lease, 0) ||
        word_count > UINT32_MAX / 2u) return 0;
    return mvdm_xms_memory_copy(destination, source, word_count * 2u);
}
