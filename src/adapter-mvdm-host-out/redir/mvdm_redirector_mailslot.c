#include "mvdm_redirector_mailslot.h"

#include <stdint.h>

#include "adapter-mvdm-host-out/softpc/include/mvdm_host_identity.h"

int mvdm_redirector_mailslot_publish(void *record, WORD *handle_out)
{
    uint32_t identity = 0u;
    if (handle_out != NULL) *handle_out = 0u;
    if (record == NULL || !mvdm_host_identity_publish((uintptr_t)record,
        &identity) || identity == 0u || identity > UINT16_MAX) {
        if (identity != 0u) (void)mvdm_host_identity_release(identity);
        return 0;
    }
    if (handle_out != NULL) *handle_out = (WORD)identity;
    return 1;
}

void *mvdm_redirector_mailslot_resolve(WORD handle)
{
    uintptr_t value;
    if (handle == 0u || !mvdm_host_identity_resolve((uint32_t)handle, &value))
        return NULL;
    return (void *)value;
}

int mvdm_redirector_mailslot_release(WORD handle)
{
    return handle != 0u && mvdm_host_identity_release((uint32_t)handle);
}
