#include "bx_ntvdm_startup_configuration_provider_v1.h"

#include <string.h>

static int text_equal(const uint8_t *actual, uint32_t bytes, const char *expected)
{
    size_t expected_bytes = strlen(expected);
    return bytes == expected_bytes && memcmp(actual, expected, expected_bytes) == 0;
}

static int prepare_input(bx_ntvdm_startup_configuration_input_v1 *input,
    uint32_t mode, const char *config, const char *autoexec)
{
    bx_ntvdm_mutation_profile_v1 profile;
    bx_ntvdm_mutation_owner_registration_v1 registration;
    size_t config_bytes = strlen(config), autoexec_bytes = strlen(autoexec);
    if (config_bytes == 0u || config_bytes > BX_NTVDM_STARTUP_CONFIGURATION_INPUT_V1_BYTES ||
        autoexec_bytes > BX_NTVDM_STARTUP_CONFIGURATION_INPUT_V1_BYTES) return 0;
    bx_ntvdm_mutation_profile_v1_initialize(&profile, mode);
    registration.owner_id = BX_NTVDM_MUTATION_OWNER_V1_COMMAND;
    registration.mutation_class = BX_NTVDM_MUTATION_CLASS_V1_SESSION_CONTEXT;
    registration.permitted_mode_mask = 0x0fu;
    registration.reserved0 = 0u;
    if (!bx_ntvdm_mutation_profile_v1_register_owner(&profile, &registration)) return 0;
    memset(input, 0, sizeof(*input));
    input->magic = BX_NTVDM_STARTUP_CONFIGURATION_PROVIDER_V1_MAGIC;
    input->abi_version = BX_NTVDM_STARTUP_CONFIGURATION_PROVIDER_V1_VERSION;
    input->struct_bytes = (uint32_t)sizeof(*input);
    if (!bx_ntvdm_startup_configuration_policy_v1_initialize(&input->policy, &profile,
            BX_NTVDM_STARTUP_CONFIGURATION_SOURCE_V1_CONTAINED_FIXTURE)) return 0;
    memcpy(input->system_root, "C:\\Windows", 11u);
    input->system_root_bytes = 10u;
    input->country_id = 1u;
    input->oem_code_page = 437u;
    memcpy(input->config, config, config_bytes);
    memcpy(input->autoexec, autoexec, autoexec_bytes);
    input->config_bytes = (uint32_t)config_bytes;
    input->autoexec_bytes = (uint32_t)autoexec_bytes;
    return bx_ntvdm_startup_configuration_input_v1_valid(input);
}

int main(void)
{
    bx_ntvdm_startup_configuration_input_v1 input;
    bx_ntvdm_startup_configuration_provider_v1 provider;
    const char *config = " country =044,850,x\r\nshell=%SystemRoot%\\System32\\command.com /e:512\r\nfiles=20\r\n";
    const char *autoexec = "prompt=$p$g\r\nPATH = C:\\BIN\r\nset Name = Value\r\nrem %SystemRoot%\r\n";
    const char *expected_config = "files=20\r\ncountry=001,437,C:\\Windows\\system32\\country.sys\r\nshell=C:\\Windows\\System32\\command.com /p C:\\Windows\\system32 /e:512\r\n";
    const char *expected_autoexec = "prompt=$p$g\r\nPATH = C:\\BIN\r\nset Name = Value\r\nrem C:\\Windows\r\n";
    const char expected_merge[] = "PROMPT\0$p$g\0PATH\0C:\\BIN\0Name \0 Value\0\0";

    if (!prepare_input(&input, BX_NTVDM_MUTATION_MODE_V1_DIRECT, config, autoexec) ||
        !bx_ntvdm_startup_configuration_provider_v1_build(&provider, &input) ||
        !bx_ntvdm_startup_configuration_provider_v1_valid(&provider) ||
        provider.result != BX_NTVDM_STARTUP_CONFIGURATION_RESULT_V1_READY ||
        !text_equal(provider.config_image, provider.config_image_bytes, expected_config) ||
        !text_equal(provider.autoexec_image, provider.autoexec_image_bytes, expected_autoexec) ||
        provider.merge_bytes != sizeof(expected_merge) ||
        memcmp(provider.merge, expected_merge, sizeof(expected_merge)) != 0) return 1;

    if (!prepare_input(&input, BX_NTVDM_MUTATION_MODE_V1_READONLY,
            "shell=C:\\OTHER.COM\r\n", "") ||
        !bx_ntvdm_startup_configuration_provider_v1_build(&provider, &input) ||
        provider.result != BX_NTVDM_STARTUP_CONFIGURATION_RESULT_V1_CONSOLE_DEFERRED ||
        !bx_ntvdm_startup_configuration_provider_v1_valid(&provider)) return 2;

    if (!prepare_input(&input, BX_NTVDM_MUTATION_MODE_V1_OVERLAY, "x", "") ) return 3;
    input.config_bytes = 0u;
    if (!bx_ntvdm_startup_configuration_provider_v1_build(&provider, &input) ||
        provider.result != BX_NTVDM_STARTUP_CONFIGURATION_RESULT_V1_SOURCE_INVALID ||
        !bx_ntvdm_startup_configuration_provider_v1_valid(&provider)) return 4;

    if (!prepare_input(&input, BX_NTVDM_MUTATION_MODE_V1_VIRTUAL, "x", "") ||
        !bx_ntvdm_startup_configuration_provider_v1_build(&provider, &input) ||
        provider.result != BX_NTVDM_STARTUP_CONFIGURATION_RESULT_V1_READY) return 5;

    if (!prepare_input(&input, BX_NTVDM_MUTATION_MODE_V1_DIRECT, "x", "")) return 6;
    memset(input.system_root, 'R', sizeof(input.system_root));
    input.system_root[sizeof(input.system_root) - 1u] = 0u;
    input.system_root_bytes = sizeof(input.system_root) - 1u;
    memset(input.config, 0, sizeof(input.config));
    for (input.config_bytes = 0u;
         input.config_bytes + 12u <= sizeof(input.config);
         input.config_bytes += 12u)
        memcpy(input.config + input.config_bytes, "%SystemRoot%", 12u);
    if (!bx_ntvdm_startup_configuration_input_v1_valid(&input) ||
        !bx_ntvdm_startup_configuration_provider_v1_build(&provider, &input) ||
        provider.result != BX_NTVDM_STARTUP_CONFIGURATION_RESULT_V1_BOUNDS ||
        !bx_ntvdm_startup_configuration_provider_v1_valid(&provider)) return 7;
    bx_ntvdm_startup_configuration_provider_v1_clear(&provider);
    if (bx_ntvdm_startup_configuration_provider_v1_valid(&provider)) return 8;
    return 0;
}
