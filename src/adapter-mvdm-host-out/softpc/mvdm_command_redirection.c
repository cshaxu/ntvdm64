#include "mvdm_command_redirection.h"

#include "mvdm_host_identity.h"

int mvdm_command_redirection_resolve(uint16_t high, uint16_t low,
    void **record_out)
{
    uintptr_t native_value;
    uint32_t identity = ((uint32_t)high << 16) | low;
    if (record_out != NULL) *record_out = NULL;
    if (!mvdm_host_identity_resolve(identity, &native_value) ||
        native_value == (uintptr_t)0u) return 0;
    if (record_out != NULL) *record_out = (void *)native_value;
    return 1;
}

int mvdm_command_redirection_publish_handle(uintptr_t native_handle,
    uint16_t *high_out, uint16_t *low_out)
{
    uint32_t identity;
    if (high_out != NULL) *high_out = 0u;
    if (low_out != NULL) *low_out = 0u;
    if (!mvdm_host_identity_publish(native_handle, &identity)) return 0;
    if (high_out != NULL) *high_out = (uint16_t)(identity >> 16);
    if (low_out != NULL) *low_out = (uint16_t)identity;
    return 1;
}

void mvdm_command_redirection_retire(void *record)
{
    uint32_t identity;
    if (record != NULL && mvdm_host_identity_lookup((uintptr_t)record,
        &identity)) (void)mvdm_host_identity_release(identity);
}
