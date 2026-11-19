#ifndef COMPAT_COMMAND_GUEST_DIRECTORY_H
#define COMPAT_COMMAND_GUEST_DIRECTORY_H
#include "command_services.h"

/* DOS supplies a numeric location; the product owns registration lifetime. */
#define mvdm_command_guest_state_clear_doswow compat_command_current_services()->clear_doswow
#define mvdm_command_guest_state_register_doswow compat_command_current_services()->register_doswow
#endif
