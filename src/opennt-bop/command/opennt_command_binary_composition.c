#include "opennt_command_composition.h"

BOOL DontCheckDosBinaryType;
BOOL IsFirstWOWCheckBinary = TRUE;

uint32_t runtime_command_binary_scs_address(uint32_t offset)
{
    runtime_command_misc_session *session = runtime_command_misc_active_session();
    if (!runtime_command_misc_session_valid(session) ||
        offset > sizeof(session->scs_info) ||
        session->scs_info_address > 0x100000u - offset) return 0u;
    return session->scs_info_address + offset;
}
