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

void mvdm_command_redirection_retire(void *record)
{
    (void)record;
}
