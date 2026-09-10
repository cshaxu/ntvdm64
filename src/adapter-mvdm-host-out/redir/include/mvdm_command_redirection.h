#ifndef MVDM_COMMAND_REDIRECTION_H
#define MVDM_COMMAND_REDIRECTION_H

#include <stdint.h>
#include <windows.h>

/* Same-shaped boundary for original COMMAND records and standard handles that
 * historically crossed the VDM register ABI as x86 process pointers. */
int mvdm_command_redirection_resolve(uint16_t high, uint16_t low,
    void **record_out);
int mvdm_command_redirection_publish(void *record, ULONG *identity_out);
int mvdm_command_redirection_publish_handle(uintptr_t native_handle,
    uint16_t *high_out, uint16_t *low_out);
int mvdm_command_redirection_resolve_handle(uint32_t identity,
    uintptr_t *native_handle_out);
void mvdm_command_redirection_retire(void *record);

#endif
