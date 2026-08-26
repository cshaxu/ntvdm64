#include "opennt-bop/command/opennt_command_composition.h"

#include <string.h>

PCHAR lpszzcmdEnv16;

void runtime_command_config_set_inputs(runtime_command_misc_session *session,
    const CHAR *config_path, const CHAR *autoexec_path)
{
    if (!runtime_command_misc_session_valid(session)) return;
    strncpy(session->config_input_path, config_path == NULL ? "" : config_path,
        sizeof(session->config_input_path) - 1u);
    session->config_input_path[sizeof(session->config_input_path) - 1u] = '\0';
    strncpy(session->autoexec_input_path, autoexec_path == NULL ? "" : autoexec_path,
        sizeof(session->autoexec_input_path) - 1u);
    session->autoexec_input_path[sizeof(session->autoexec_input_path) - 1u] = '\0';
}

int runtime_command_config_set_bootstrap_command(
    runtime_command_misc_session *session, const CHAR *command_path)
{
    size_t bytes;
    if (!runtime_command_misc_session_valid(session) || command_path == NULL) return 0;
    bytes = strlen(command_path) + 1u;
    if (bytes <= 1u || bytes > sizeof(session->bootstrap_command_path) ||
        strchr(command_path, ' ') != NULL) return 0;
    memcpy(session->bootstrap_command_path, command_path, bytes);
    return 1;
}

const CHAR *runtime_command_config_bootstrap_command(void)
{
    runtime_command_misc_session *session = runtime_command_misc_active_session();
    return session != NULL && runtime_command_misc_session_valid(session) &&
        session->bootstrap_command_path[0] != '\0' ? session->bootstrap_command_path : NULL;
}

void GetPIFConfigFiles(BOOL bConfig, CHAR *file_name)
{
    runtime_command_misc_session *session = runtime_command_misc_active_session();
    const CHAR *configured = session == NULL ? NULL :
        (bConfig ? session->config_input_path : session->autoexec_input_path);
    DWORD length;
    if (configured == NULL || configured[0] == '\0') {
        length = GetSystemDirectoryA(file_name, MAX_PATH);
        if (length == 0u || file_name[length - 1u] != '\\') strcat(file_name, "\\");
        strcat(file_name, bConfig ? "config.nt" : "autoexec.nt");
    } else if (ExpandEnvironmentStringsA(configured, file_name, MAX_PATH + 12u) == 0u) {
        file_name[0] = '\0';
    }
}
