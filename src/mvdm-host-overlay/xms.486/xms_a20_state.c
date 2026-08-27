#include "xms_a20_state.h"

#include <string.h>

#include "adapter-mvdm-host-out/softpc/include/mvdm_guest_location.h"
#include "session/session.h"

typedef struct mvdm_xms_himem_a20_state {
    session *owner;
    mvdm_guest_location location;
} mvdm_xms_himem_a20_state;

/* The state is thread-bound, as is the original monitor call context.  It
 * retains only guest numeric data and is invalidated if a different session
 * reaches the mirror. */
static __declspec(thread) mvdm_xms_himem_a20_state current_state;

int mvdm_xms_himem_a20_state_bind(uint16_t segment, uint16_t offset)
{
    session *instance = session_thread_current();

    memset(&current_state, 0, sizeof(current_state));
    if (instance == NULL || !mvdm_guest_location_set_real_mode(
        &current_state.location, segment, offset)) return 0;
    current_state.owner = instance;
    return 1;
}

int mvdm_xms_himem_a20_state_write(uint8_t value)
{
    session *instance = session_thread_current();

    if (instance == NULL || instance != current_state.owner ||
        current_state.location.valid != 1u) return 0;
    return mvdm_guest_location_copy_to_guest(&current_state.location, &value,
        1u);
}
