#if 0
/* DEFERRED EXPERIMENT MVDM-SOFTPC-PATCH-002:
 * Do not enable this mapping/lease prototype until the respective original
 * CCPU CPU-frame, PhysicalPageREC and BOP-route contracts are separately
 * recovered.  The active fmstubs.c contract is an immediate debugger break,
 * not session cancellation, a zero result or an implicit mapping substitution.
 */

#include "mvdm_softpc_patch_hooks.h"

#include "mvdm_host_identity.h"
#include "session/session.h"

/* A PBYTE returned by the historical c_sas_touch form is only a synchronous
 * CCPU implementation view.  It is never placed in an MVDM field or returned
 * to guest code.  The underlying address remains registered in the one
 * session guest-memory mapping instance and is released before the next view. */
typedef struct mvdm_softpc_touch_view {
    session *owner;
    guest_memory_lease *lease;
    uint32_t native_identity;
} mvdm_softpc_touch_view;

static __declspec(thread) mvdm_softpc_touch_view current_touch;

static void clear_touch(int commit)
{
    mapping_manager *mappings;

    if (current_touch.owner == NULL) return;
    mappings = session_guest_memory_mappings(current_touch.owner);
    if (current_touch.lease != NULL)
        (void)session_guest_memory_release(current_touch.owner,
            current_touch.lease, commit);
    if (mappings != NULL && current_touch.native_identity != 0u)
        (void)mapping_manager_release(mappings, current_touch.native_identity);
    current_touch.owner = NULL;
    current_touch.lease = NULL;
    current_touch.native_identity = 0u;
}

void mvdm_softpc_patch_clear_instance_data(void)
{
    clear_touch(1);
}

void mvdm_softpc_patch_set_physical_records(uint32_t host_identity,
    uint32_t intel_address, uint32_t byte_count)
{
    uintptr_t native_value;

    (void)intel_address;
    (void)byte_count;
    /* DIVERGENCE: the NT4 patch treated HostAddress as a truncated host
     * pointer.  It is an opaque 32-bit session identity here.  Resolving it
     * proves ownership without ever exposing the native value to MVDM.  The
     * PhysicalPageREC machine-table update remains owned by the selected
     * SoftPC machine composition, not by this host-facing patch bridge. */
    (void)mvdm_host_identity_resolve(host_identity, &native_value);
}

void mvdm_softpc_patch_fast_bop(uint32_t immediate)
{
    session *owner = session_thread_current();

    (void)immediate;
    /* A fast BOP needs a copied CPU frame before selector routing can occur.
     * This source-shaped hook owns no CPU frame and therefore cannot invent a
     * selector route.  A bound session terminates in a typed, controlled way
     * rather than retaining the NTVDMx64 int 3 process breakpoint. */
    if (owner != NULL)
        (void)session_request_cancellation(owner, SESSION_CANCELLATION_REQUESTED);
}

uint32_t mvdm_softpc_patch_virtualise_instruction(uint32_t eip_in_rom,
    uint32_t byte_count, uint32_t linear_address_or_port, uint32_t data_in)
{
    (void)eip_in_rom;
    (void)byte_count;
    (void)linear_address_or_port;
    (void)data_in;
    /* Original zero means "not virtualised".  No host address, pointer or
     * handle is represented by these four numeric CPU inputs. */
    return 0u;
}

uint8_t *mvdm_softpc_patch_touch(uint32_t address, uint32_t byte_count)
{
    session *owner = session_thread_current();
    mapping_manager *mappings;
    uint8_t *bytes = NULL;

    clear_touch(1);
    if (owner == NULL || byte_count == 0u) return NULL;
    if (!session_guest_memory_acquire(owner, address, byte_count,
        GUEST_MEMORY_ACCESS_READ | GUEST_MEMORY_ACCESS_WRITE,
        &current_touch.lease, &bytes)) return NULL;
    mappings = session_guest_memory_mappings(owner);
    if (mappings == NULL || !mapping_manager_publish(mappings,
        (uintptr_t)bytes, &current_touch.native_identity)) {
        (void)session_guest_memory_release(owner, current_touch.lease, 0);
        current_touch.lease = NULL;
        return NULL;
    }
    current_touch.owner = owner;
    return bytes;
}
#endif
