#include "bx_ntvdm_command_initial_environment_v1.h"

#include <stdlib.h>
#include <string.h>
#include <windows.h>

static int begins_with_ci(const char *value, const char *expected)
{
    uint32_t index;
    for (index = 0u; expected[index] != '\0'; ++index) {
        char character;
        character = value[index];
        if (character == '\0') return 0;
        if (character >= 'a' && character <= 'z')
            character = (char)(character - 'a' + 'A');
        if (character != expected[index]) return 0;
    }
    return 1;
}

int bx_ntvdm_command_initial_environment_v1_capture(
    bx_ntvdm_command_host_context_v1 *context)
{
    char *ansi, *entry;
    uint8_t *environment;
    uint32_t used = 0u;
    int has_prompt = 0, found_comspec = 0, found_windir = 0;
    int result = 0;

    if (context == 0) return 0;
    environment = (uint8_t *)malloc(BX_NTVDM_COMMAND_HOST_CONTEXT_V1_ENVIRONMENT_BYTES);
    if (environment == 0) return 0;
    /* OpenNT cmdGetInitEnvironment snapshots ANSI process data and converts
     * every retained entry to OEM.  This seam copies that result at CLI
     * admission and retains no host-owned environment storage. */
    ansi = GetEnvironmentStringsA();
    if (ansi == 0) { free(environment); return 0; }
    for (entry = ansi; *entry != '\0'; entry += strlen(entry) + 1u) {
        uint32_t bytes = (uint32_t)strlen(entry) + 1u;
        uint32_t index, name_bytes = 0u;
        if (entry[0] == '=') continue;
        /* Preserve the original prefix test and first-match state. Later
         * duplicates and malformed nonempty entries are retained. */
        if (!found_comspec && begins_with_ci(entry, "COMSPEC=")) {
            found_comspec = 1;
            continue;
        }
        if (!found_windir && begins_with_ci(entry, "WINDIR")) {
            found_windir = 1;
            continue;
        }
        /* The original grows an intermediate block as needed.  This fixed
         * seam reserves the maximum original COMSPEC record and therefore
         * fails only at the actual single-transaction 16-bit boundary. */
        if (bytes < 2u || bytes >
                BX_NTVDM_COMMAND_HOST_CONTEXT_V1_ENVIRONMENT_BYTES - used - 1u ||
            !CharToOemBuffA(entry, (char *)environment + used, bytes)) goto done;
        for (index = 0u; index < bytes - 1u; ++index) {
            if (environment[used + index] == '=') { name_bytes = index; break; }
        }
        if (name_bytes != 0u) {
            for (index = 0u; index < name_bytes; ++index)
                if (environment[used + index] >= 'a' && environment[used + index] <= 'z')
                    environment[used + index] = (uint8_t)(environment[used + index] - 'a' + 'A');
        }
        if (name_bytes == 6u && begins_with_ci((const char *)environment + used, "PROMPT"))
            has_prompt = 1;
        used += bytes;
    }
    if (!has_prompt) {
        static const uint8_t prompt[] = "PROMPT=$P$G";
        if (used > BX_NTVDM_COMMAND_HOST_CONTEXT_V1_ENVIRONMENT_BYTES -
                sizeof(prompt) - 1u) goto done;
        memcpy(environment + used, prompt, sizeof(prompt));
        used += (uint32_t)sizeof(prompt);
    }
    if (used >= BX_NTVDM_COMMAND_HOST_CONTEXT_V1_ENVIRONMENT_BYTES) goto done;
    environment[used++] = 0u;
    result = bx_ntvdm_command_host_context_v1_set_environment(context, environment, used);
done:
    FreeEnvironmentStringsA(ansi);
    free(environment);
    return result;
}
