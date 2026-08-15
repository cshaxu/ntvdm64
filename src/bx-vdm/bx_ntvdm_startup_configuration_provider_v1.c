#include "bx_ntvdm_startup_configuration_provider_v1.h"

#include <stdio.h>
#include <string.h>

static int graph_byte(uint8_t value)
{
    return value >= 33u && value <= 126u;
}

static uint8_t upper_byte(uint8_t value)
{
    return value >= (uint8_t)'a' && value <= (uint8_t)'z' ?
        (uint8_t)(value - ((uint8_t)'a' - (uint8_t)'A')) : value;
}

static int starts_ci(const uint8_t *value, uint32_t bytes, const char *text)
{
    uint32_t index;
    for (index = 0u; text[index] != '\0'; ++index) {
        if (index >= bytes || upper_byte(value[index]) != (uint8_t)text[index]) return 0;
    }
    return 1;
}

static int append(uint8_t *destination, uint32_t capacity, uint32_t *used,
    const uint8_t *source, uint32_t bytes)
{
    if (*used > capacity || bytes > capacity - *used) return 0;
    if (bytes != 0u) memcpy(destination + *used, source, bytes);
    *used += bytes;
    return 1;
}

static int append_text(uint8_t *destination, uint32_t capacity, uint32_t *used,
    const char *text)
{
    return append(destination, capacity, used, (const uint8_t *)text,
        (uint32_t)strlen(text));
}

static int append_expanded(uint8_t *destination, uint32_t capacity, uint32_t *used,
    const uint8_t *source, uint32_t bytes, const uint8_t *root, uint32_t root_bytes)
{
    uint32_t index = 0u;
    while (index < bytes) {
        if (source[index] == '%' && bytes - index >= 12u &&
            starts_ci(source + index, bytes - index, "%SYSTEMROOT%")) {
            if (!append(destination, capacity, used, root, root_bytes)) return 0;
            index += 12u;
        } else if (!append(destination, capacity, used, source + index, 1u)) {
            return 0;
        } else {
            ++index;
        }
    }
    return 1;
}

static uint32_t trim_start(const uint8_t *source, uint32_t start, uint32_t end)
{
    while (start < end && !graph_byte(source[start])) ++start;
    return start;
}

static int config_command(const uint8_t *line, uint32_t bytes, const char *name,
    uint32_t *value_start)
{
    uint32_t index = (uint32_t)strlen(name);
    if (!starts_ci(line, bytes, name)) return 0;
    while (index < bytes && !graph_byte(line[index])) ++index;
    if (index >= bytes || line[index] != '=') return 0;
    *value_start = index + 1u;
    return 1;
}

static int append_tuple(bx_ntvdm_startup_configuration_provider_v1 *provider,
    const uint8_t *name, uint32_t name_bytes, const uint8_t *value, uint32_t value_bytes)
{
    uint32_t *used = &provider->merge_bytes;
    return append(provider->merge, sizeof(provider->merge), used, name, name_bytes) &&
        append(provider->merge, sizeof(provider->merge), used, (const uint8_t *)"", 1u) &&
        append(provider->merge, sizeof(provider->merge), used, value, value_bytes) &&
        append(provider->merge, sizeof(provider->merge), used, (const uint8_t *)"", 1u);
}

static int capture_autoexec_tuple(bx_ntvdm_startup_configuration_provider_v1 *provider,
    const uint8_t *line, uint32_t bytes)
{
    uint32_t index;
    if (starts_ci(line, bytes, "PROMPT") || starts_ci(line, bytes, "PATH")) {
        const char *name = starts_ci(line, bytes, "PROMPT") ? "PROMPT" : "PATH";
        uint32_t name_bytes = (uint32_t)strlen(name);
        index = trim_start(line, name_bytes, bytes);
        if (index < bytes && line[index] == '=') index = trim_start(line, index + 1u, bytes);
        return append_tuple(provider, (const uint8_t *)name, name_bytes, line + index, bytes - index);
    }
    if (!starts_ci(line, bytes, "SET")) return 1;
    index = trim_start(line, 3u, bytes);
    {
        uint32_t name_start = index;
        while (index < bytes && line[index] != '=') ++index;
        if (index == bytes) return 1;
        return append_tuple(provider, line + name_start, index - name_start,
            line + index + 1u, bytes - index - 1u);
    }
}

static int append_config_shell(bx_ntvdm_startup_configuration_provider_v1 *provider,
    const bx_ntvdm_startup_configuration_input_v1 *input, const uint8_t *environment_option,
    uint32_t environment_option_bytes)
{
    char country[384];
    uint32_t used = provider->config_image_bytes;
    int count = snprintf(country, sizeof(country), "country=%3.3u,%3.3u,%.*s\\system32\\country.sys\r\n",
        input->country_id, input->oem_code_page, (int)input->system_root_bytes, input->system_root);
    if (count < 0 || (uint32_t)count >= sizeof(country) ||
        !append(provider->config_image, sizeof(provider->config_image), &used,
            (const uint8_t *)country, (uint32_t)count) ||
        !append_text(provider->config_image, sizeof(provider->config_image), &used, "shell=") ||
        !append(provider->config_image, sizeof(provider->config_image), &used,
            input->system_root, input->system_root_bytes) ||
        !append_text(provider->config_image, sizeof(provider->config_image), &used,
            "\\System32\\command.com /p ") ||
        !append(provider->config_image, sizeof(provider->config_image), &used,
            input->system_root, input->system_root_bytes) ||
        !append_text(provider->config_image, sizeof(provider->config_image), &used, "\\system32")) return 0;
    if (environment_option_bytes != 0u &&
        (!append_text(provider->config_image, sizeof(provider->config_image), &used, " ") ||
         !append(provider->config_image, sizeof(provider->config_image), &used,
             environment_option, environment_option_bytes))) return 0;
    if (!append_text(provider->config_image, sizeof(provider->config_image), &used, "\r\n")) return 0;
    provider->config_image_bytes = used;
    return 1;
}

void bx_ntvdm_startup_configuration_provider_v1_clear(
    bx_ntvdm_startup_configuration_provider_v1 *provider)
{
    if (provider != 0) memset(provider, 0, sizeof(*provider));
}

int bx_ntvdm_startup_configuration_input_v1_valid(
    const bx_ntvdm_startup_configuration_input_v1 *input)
{
    uint32_t index;
    return input != 0 && input->magic == BX_NTVDM_STARTUP_CONFIGURATION_PROVIDER_V1_MAGIC &&
        input->abi_version == BX_NTVDM_STARTUP_CONFIGURATION_PROVIDER_V1_VERSION &&
        input->struct_bytes == sizeof(*input) &&
        bx_ntvdm_startup_configuration_policy_v1_valid(&input->policy) &&
        input->system_root_bytes != 0u &&
        input->system_root_bytes < sizeof(input->system_root) &&
        input->config_bytes != 0u && input->config_bytes <= sizeof(input->config) &&
        input->autoexec_bytes <= sizeof(input->autoexec) && input->country_id != 0u &&
        input->oem_code_page != 0u &&
        (index = input->system_root_bytes, input->system_root[index] == 0u);
}

int bx_ntvdm_startup_configuration_provider_v1_valid(
    const bx_ntvdm_startup_configuration_provider_v1 *provider)
{
    if (provider == 0 || provider->magic != BX_NTVDM_STARTUP_CONFIGURATION_PROVIDER_V1_MAGIC ||
        provider->abi_version != BX_NTVDM_STARTUP_CONFIGURATION_PROVIDER_V1_VERSION ||
        provider->struct_bytes != sizeof(*provider) || provider->reserved0 != 0u ||
        provider->result < BX_NTVDM_STARTUP_CONFIGURATION_RESULT_V1_READY ||
        provider->result > BX_NTVDM_STARTUP_CONFIGURATION_RESULT_V1_CONSOLE_DEFERRED)
        return 0;
    if (provider->result != BX_NTVDM_STARTUP_CONFIGURATION_RESULT_V1_READY)
        return provider->config_image_bytes == 0u && provider->autoexec_image_bytes == 0u &&
            provider->merge_bytes == 0u;
    return provider->config_image_bytes <= sizeof(provider->config_image) &&
        provider->autoexec_image_bytes <= sizeof(provider->autoexec_image) &&
        provider->merge_bytes <= sizeof(provider->merge) &&
        (provider->merge_bytes == 0u || provider->merge[provider->merge_bytes - 1u] == 0u);
}

int bx_ntvdm_startup_configuration_provider_v1_build(
    bx_ntvdm_startup_configuration_provider_v1 *provider,
    const bx_ntvdm_startup_configuration_input_v1 *input)
{
    uint32_t offset, line_start, line_end, shell_value = 0u;
    const uint8_t *environment_option = 0;
    uint32_t environment_option_bytes = 0u;
    if (provider == 0) return 0;
    bx_ntvdm_startup_configuration_provider_v1_clear(provider);
    provider->magic = BX_NTVDM_STARTUP_CONFIGURATION_PROVIDER_V1_MAGIC;
    provider->abi_version = BX_NTVDM_STARTUP_CONFIGURATION_PROVIDER_V1_VERSION;
    provider->struct_bytes = (uint32_t)sizeof(*provider);
    if (!bx_ntvdm_startup_configuration_input_v1_valid(input)) {
        provider->result = BX_NTVDM_STARTUP_CONFIGURATION_RESULT_V1_SOURCE_INVALID;
        return 1;
    }
    offset = 0u;
    while (offset < input->config_bytes) {
        line_start = trim_start(input->config, offset, input->config_bytes);
        line_end = line_start;
        while (line_end < input->config_bytes && input->config[line_end] != '\r' && input->config[line_end] != '\n') ++line_end;
        offset = line_end;
        while (offset < input->config_bytes && (input->config[offset] == '\r' || input->config[offset] == '\n')) ++offset;
        if (line_start == line_end) continue;
        if (config_command(input->config + line_start, line_end - line_start, "COUNTRY", &shell_value)) continue;
        if (config_command(input->config + line_start, line_end - line_start, "SHELL", &shell_value)) {
            uint32_t value_end = trim_start(input->config + line_start, shell_value, line_end - line_start);
            if (!starts_ci(input->config + line_start + value_end, line_end - line_start - value_end,
                "%SYSTEMROOT%\\SYSTEM32\\COMMAND.COM") &&
                !starts_ci(input->config + line_start + value_end, line_end - line_start - value_end,
                "C:\\WINDOWS\\SYSTEM32\\COMMAND.COM")) {
                provider->result = BX_NTVDM_STARTUP_CONFIGURATION_RESULT_V1_CONSOLE_DEFERRED;
                return 1;
            }
            while (value_end < line_end - line_start) {
                if (input->config[line_start + value_end] == '/' &&
                    starts_ci(input->config + line_start + value_end,
                        line_end - line_start - value_end, "/E:")) {
                    uint32_t end = value_end;
                    while (end < line_end - line_start && graph_byte(input->config[line_start + end])) ++end;
                    environment_option = input->config + line_start + value_end;
                    environment_option_bytes = end - value_end;
                    break;
                }
                ++value_end;
            }
            continue;
        }
        if (!append_expanded(provider->config_image, sizeof(provider->config_image),
                &provider->config_image_bytes, input->config + line_start, line_end - line_start,
                input->system_root, input->system_root_bytes) ||
            !append_text(provider->config_image, sizeof(provider->config_image),
                &provider->config_image_bytes, "\r\n")) goto bounds;
    }
    if (!append_config_shell(provider, input, environment_option, environment_option_bytes)) goto bounds;
    offset = 0u;
    while (offset < input->autoexec_bytes) {
        line_start = trim_start(input->autoexec, offset, input->autoexec_bytes);
        line_end = line_start;
        while (line_end < input->autoexec_bytes && input->autoexec[line_end] != '\r' && input->autoexec[line_end] != '\n') ++line_end;
        offset = line_end;
        while (offset < input->autoexec_bytes && (input->autoexec[offset] == '\r' || input->autoexec[offset] == '\n')) ++offset;
        if (line_start == line_end) continue;
        if (!capture_autoexec_tuple(provider, input->autoexec + line_start, line_end - line_start) ||
            !append_expanded(provider->autoexec_image, sizeof(provider->autoexec_image),
                &provider->autoexec_image_bytes, input->autoexec + line_start, line_end - line_start,
                input->system_root, input->system_root_bytes) ||
            !append_text(provider->autoexec_image, sizeof(provider->autoexec_image),
                &provider->autoexec_image_bytes, "\r\n")) goto bounds;
    }
    if (provider->merge_bytes != 0u && !append(provider->merge, sizeof(provider->merge),
            &provider->merge_bytes, (const uint8_t *)"", 1u)) goto bounds;
    provider->result = BX_NTVDM_STARTUP_CONFIGURATION_RESULT_V1_READY;
    return bx_ntvdm_startup_configuration_provider_v1_valid(provider);
bounds:
    bx_ntvdm_startup_configuration_provider_v1_clear(provider);
    provider->magic = BX_NTVDM_STARTUP_CONFIGURATION_PROVIDER_V1_MAGIC;
    provider->abi_version = BX_NTVDM_STARTUP_CONFIGURATION_PROVIDER_V1_VERSION;
    provider->struct_bytes = (uint32_t)sizeof(*provider);
    provider->result = BX_NTVDM_STARTUP_CONFIGURATION_RESULT_V1_BOUNDS;
    return 1;
}
