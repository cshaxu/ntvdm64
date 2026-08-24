#include "opennt_xms_compat.h"

#include "adapter-softpc/softpc_int15_watch_shim.h"

#include <string.h>

static ULONG g_configured_memory_kib;
static int g_xms_initialized;

/* Direct softpc source ownership remains in opennt-bop/softpc.  The opaque
 * watch storage and checked reads remain adapter-softpc mechanics. */
int bx_ntvdm_softpc_int15_watch_source_begin(void);
int bx_ntvdm_softpc_int15_watch_source_end(void);
void bx_ntvdm_softpc_int15_watch_source_reset(void);

static USHORT low16(uint32_t value)
{
    return (USHORT)(value & 0xffffu);
}

int bx_ntvdm_xms_call_valid(const bx_ntvdm_xms_call *call)
{
    return call != NULL && call->magic == BX_NTVDM_XMS_CALL_MAGIC &&
        call->abi_version == BX_NTVDM_XMS_CALL_VERSION &&
        call->struct_bytes == sizeof(*call) && call->service < XMS_LASTSVC &&
        call->boundary != NULL && bx_ntvdm_exception_event_v1_valid(call->boundary) &&
        call->cpu != NULL && bx_ntvdm_cpu_state_v1_valid(call->cpu) &&
        call->cpu->execution_mode == BX_NTVDM_CPU_EXECUTION_REAL &&
        call->result != NULL && call->guest_read != NULL && call->guest_write != NULL;
}

int bx_ntvdm_xms_configure_memory_kib(ULONG kib)
{
    /* OpenNT's XMS package has no destruction entry.  A later session bind
     * may only reaffirm capacity; it cannot recreate source-owned state. */
    if (kib < 1024u || (g_xms_initialized && kib != g_configured_memory_kib))
        return 0;
    if (g_xms_initialized) return 1;
    g_configured_memory_kib = kib;
    xmsMemorySize = kib;
    return 1;
}

void bx_ntvdm_xms_reset(void)
{
    /* Test/process teardown only: imported suballoc has no live-session
     * destruction contract. */
    g_configured_memory_kib = 0u;
    g_xms_initialized = 0;
    xmsMemorySize = 0u;
    ExtMemSA = NULL;
    bx_ntvdm_xms_softpc_reset();
    bx_ntvdm_softpc_int15_watch_source_reset();
}

int bx_ntvdm_xms_initialize(void)
{
    if (g_xms_initialized) return 1;
    if (g_configured_memory_kib == 0u || !XMSInit(0, NULL)) return 0;
    g_xms_initialized = 1;
    return 1;
}

int bx_ntvdm_xms_invoke(bx_ntvdm_xms_call *call)
{
    bx_ntvdm_xms_softpc_context context;
    int invoked;
    if (!bx_ntvdm_xms_call_valid(call) || call->boundary->fault_rip > UINT64_MAX - 4u)
        return 0;
    /* These two imported source paths lack a service-level failure result;
     * decline at the route if their required mechanical capability is absent. */
    if ((call->service == 0u || call->service == 6u) &&
        !bx_ntvdm_xms_a20_available()) return 0;
    if (!g_xms_initialized) return 0;
    memset(&context, 0, sizeof(context));
    context.cpu = call->cpu;
    context.result = call->result;
    context.guest_state = call->guest_state;
    context.guest_read = call->guest_read;
    context.guest_write = call->guest_write;
    bx_ntvdm_cpu_result_v2_pass_through(call->result);
    if (!bx_ntvdm_cpu_result_v2_resume(call->result, call->boundary->fault_rip + 4u) ||
        !bx_ntvdm_xms_softpc_context_begin(&context)) return 0;
    if (call->service == 6u && !bx_ntvdm_xms_bind_himem_a20_state(
            low16(call->cpu->eax), low16(call->cpu->ebx))) {
        bx_ntvdm_xms_softpc_context_end();
        return 0;
    }
    if (call->service == 9u && !bx_ntvdm_softpc_int15_watch_source_begin()) {
        bx_ntvdm_xms_softpc_context_end();
        return 0;
    }
    invoked = XMSDispatch(call->service);
    if (call->service == 9u && !bx_ntvdm_softpc_int15_watch_source_end())
        invoked = 0;
    bx_ntvdm_xms_softpc_context_end();
    return invoked && bx_ntvdm_cpu_result_v2_valid(call->result);
}
