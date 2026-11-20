#ifndef MVDM_COMMAND_GUEST_DIRECTORY_H
#define MVDM_COMMAND_GUEST_DIRECTORY_H
#include <stdint.h>

void mvdm_command_guest_state_clear_doswow(void);
int mvdm_command_guest_state_register_doswow(uint16_t segment, uint16_t offset);
int mvdm_command_guest_state_directory_instance(uint32_t *linear_out,
    uint32_t *bytes_out);
#endif
