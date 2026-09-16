#ifndef MVDM_COMMAND_REDIRECTION_H
#define MVDM_COMMAND_REDIRECTION_H

#include <stdint.h>
#include <windows.h>

/* Same-shaped boundary for original COMMAND records and standard handles that
 * historically crossed the VDM register ABI as x86 process pointers. */
int mvdm_command_redirection_resolve(uint16_t high, uint16_t low,
    void **record_out);
int mvdm_command_redirection_publish(void *record, ULONG *identity_out);
void mvdm_command_redirection_retire(void *record);

#endif
