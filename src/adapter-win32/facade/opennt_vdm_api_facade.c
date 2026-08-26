#include "opennt_vdm_api_facade.h"

#include <limits.h>
#include <string.h>

static __declspec(thread) session_input *active_input;
static __declspec(thread) uint32_t active_input_depth;
static const char empty_multisz[2] = { '\0', '\0' };

int opennt_vdm_api_bind_input(session_input *input)
{
    if (!session_input_valid(input) ||
        (active_input != NULL && active_input != input)) return 0;
    active_input = input;
    ++active_input_depth;
    return active_input_depth != 0u;
}

void opennt_vdm_api_unbind_input(session_input *input)
{
    if (active_input != input || active_input_depth == 0u) return;
    if (--active_input_depth == 0u) active_input = NULL;
}

BOOL WINAPI GetNextVDMCommand(PVDMINFO vdm_info)
{
    session_input *input = active_input;
    size_t target_bytes, argument_bytes;
    const char *environment;
    uint32_t bytes;
    if (vdm_info == NULL || !session_input_valid(input)) return FALSE;
    /* OpenNT base/win32/client/vdm.c sends these two state transitions to
     * BaseSrv. The modern single-session facade preserves their ordered
     * result using only this scoped neutral record. */
    if (vdm_info->VDMState == INCREMENT_REENTER_COUNT) {
        ++input->reentry_count;
        if (input->reentry_count > input->reentry_peak)
            input->reentry_peak = input->reentry_count;
        return TRUE;
    }
    if (vdm_info->VDMState == DECREMENT_REENTER_COUNT) {
        if (input->reentry_count == 0u) return FALSE;
        --input->reentry_count;
        return TRUE;
    }
    if (input->ready == 0u) return FALSE;
    if ((vdm_info->VDMState & ASKING_FOR_ENVIRONMENT) != 0u) {
        environment = input->environment;
        bytes = input->environment_bytes;
        if (environment == NULL || bytes == 0u) {
            environment = empty_multisz;
            bytes = sizeof(empty_multisz);
        }
        if (vdm_info->Enviornment == NULL || vdm_info->EnviornmentSize < bytes) {
            vdm_info->EnviornmentSize = bytes;
            return FALSE;
        }
        memcpy(vdm_info->Enviornment, environment, bytes);
        vdm_info->EnviornmentSize = bytes;
        return TRUE;
    }
    if (input->delivered != 0u || vdm_info->AppName == NULL ||
        vdm_info->CmdLine == NULL) return FALSE;
    target_bytes = strlen(input->target) + 1u;
    argument_bytes = strlen(input->arguments);
    if (target_bytes > vdm_info->AppLen || argument_bytes + 2u > vdm_info->CmdSize ||
        target_bytes > UINT16_MAX || argument_bytes + 2u > UINT16_MAX) return FALSE;
    memcpy(vdm_info->AppName, input->target, target_bytes);
    memcpy(vdm_info->CmdLine, input->arguments, argument_bytes);
    ((char *)vdm_info->CmdLine)[argument_bytes] = '\r';
    ((char *)vdm_info->CmdLine)[argument_bytes + 1u] = '\n';
    ((char *)vdm_info->CmdLine)[argument_bytes + 2u] = '\0';
    vdm_info->AppLen = (USHORT)target_bytes;
    vdm_info->CmdSize = (USHORT)(argument_bytes + 2u);
    vdm_info->CurDrive = input->location;
    vdm_info->CodePage = input->text_code_page;
    input->delivered = 1u;
    return TRUE;
}

BOOL WINAPI SetVDMCurrentDirectories(ULONG bytes, LPSTR directories)
{
    return session_input_valid(active_input) &&
        session_input_set_published_directories(active_input, directories, bytes);
}
