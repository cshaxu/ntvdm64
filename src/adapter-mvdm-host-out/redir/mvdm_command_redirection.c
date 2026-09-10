#include "mvdm_command_redirection.h"

int mvdm_command_redirection_resolve(uint16_t high, uint16_t low,
    void **record_out)
{
    if (record_out != NULL) *record_out = NULL;
    if (high == 0u && low == 0u) return 0;
    if (record_out != NULL) *record_out = (void *)(uintptr_t)
        (((uint32_t)high << 16) | low);
    return 1;
}

int mvdm_command_redirection_publish(void *record, ULONG *identity_out)
{
    if (identity_out != NULL) *identity_out = 0u;
    if (record == NULL) return 0;
    if (identity_out != NULL) *identity_out = (ULONG)(uintptr_t)record;
    return 1;
}

int mvdm_command_redirection_publish_handle(uintptr_t native_handle,
    uint16_t *high_out, uint16_t *low_out)
{
    if (high_out != NULL) *high_out = 0u;
    if (low_out != NULL) *low_out = 0u;
    if (native_handle == (uintptr_t)0u) return 0;
    if (high_out != NULL) *high_out = (uint16_t)(native_handle >> 16);
    if (low_out != NULL) *low_out = (uint16_t)native_handle;
    return 1;
}

int mvdm_command_redirection_resolve_handle(uint32_t identity,
    uintptr_t *native_handle_out)
{
    if (native_handle_out != NULL) *native_handle_out = (uintptr_t)0u;
    if (identity == 0u) return 0;
    if (native_handle_out != NULL) *native_handle_out = (uintptr_t)identity;
    return 1;
}

void mvdm_command_redirection_retire(void *record)
{
    (void)record;
}
