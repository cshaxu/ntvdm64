#include "bop-v1/bx_ntvdm_command_initial_environment_v1.h"

#include <stdio.h>
#include <string.h>
#include <windows.h>

typedef struct saved_value {
    const char *name;
    char value[32768];
    int present;
} saved_value;

static int save(saved_value *value)
{
    DWORD bytes = GetEnvironmentVariableA(value->name, value->value,
        (DWORD)sizeof(value->value));
    if (bytes == 0u) {
        value->present = GetLastError() != ERROR_ENVVAR_NOT_FOUND;
        return value->present ? 0 : 1;
    }
    if (bytes >= sizeof(value->value)) return 0;
    value->present = 1;
    return 1;
}

static int restore(const saved_value *value)
{
    return SetEnvironmentVariableA(value->name,
        value->present ? value->value : 0) != 0;
}

static int contains(const uint8_t *environment, uint32_t bytes, const char *entry)
{
    uint32_t offset = 0u;
    while (offset < bytes && environment[offset] != 0u) {
        size_t length = strlen((const char *)environment + offset);
        if (strcmp((const char *)environment + offset, entry) == 0) return 1;
        offset += (uint32_t)length + 1u;
    }
    return 0;
}

static int contains_name(const uint8_t *environment, uint32_t bytes, const char *name)
{
    uint32_t offset = 0u;
    size_t name_bytes = strlen(name);
    while (offset < bytes && environment[offset] != 0u) {
        if (strncmp((const char *)environment + offset, name, name_bytes) == 0 &&
            environment[offset + name_bytes] == '=') return 1;
        offset += (uint32_t)strlen((const char *)environment + offset) + 1u;
    }
    return 0;
}

static int named_value_has_bytes(const uint8_t *environment, uint32_t bytes,
    const char *name, size_t value_bytes)
{
    uint32_t offset = 0u;
    size_t name_bytes = strlen(name);
    while (offset < bytes && environment[offset] != 0u) {
        size_t entry_bytes = strlen((const char *)environment + offset);
        if (strncmp((const char *)environment + offset, name, name_bytes) == 0 &&
            environment[offset + name_bytes] == '=' &&
            entry_bytes == name_bytes + 1u + value_bytes) return 1;
        offset += (uint32_t)entry_bytes + 1u;
    }
    return 0;
}

int main(void)
{
    saved_value values[] = {
        { "NTDOS64_T217_LOWER", { 0 }, 0 },
        { "NTDOS64_T217_LONG", { 0 }, 0 },
        { "COMSPEC", { 0 }, 0 }, { "WINDIR", { 0 }, 0 }, { "PROMPT", { 0 }, 0 }
    };
    static char long_value[5001];
    bx_ntvdm_command_host_context_v1 context;
    uint8_t root[] = "C:\\";
    unsigned index;
    int passed = 1;

    memset(&context, 0, sizeof(context));
    memset(long_value, 'X', sizeof(long_value) - 1u);
    long_value[sizeof(long_value) - 1u] = '\0';
    for (index = 0u; index < sizeof(values) / sizeof(values[0]); ++index)
        if (!save(&values[index])) return 2;
    if (!SetEnvironmentVariableA("NTDOS64_T217_LOWER", "Value") ||
        !SetEnvironmentVariableA("NTDOS64_T217_LONG", long_value) ||
        !SetEnvironmentVariableA("COMSPEC", "C:\\UNWANTED.COM") ||
        !SetEnvironmentVariableA("WINDIR", "C:\\UNWANTED") ||
        !SetEnvironmentVariableA("PROMPT", 0) ||
        !bx_ntvdm_command_host_context_v1_initialize(&context, 2u, root, 3u) ||
        !bx_ntvdm_command_initial_environment_v1_capture(&context)) passed = 0;
    if (passed && (!contains(context.environment, context.environment_bytes,
            "NTDOS64_T217_LOWER=Value") || !contains(context.environment,
            context.environment_bytes, "PROMPT=$P$G") || contains_name(
            context.environment, context.environment_bytes, "COMSPEC") || contains_name(
            context.environment, context.environment_bytes, "WINDIR") ||
            context.environment_bytes <= 4023u || !named_value_has_bytes(
            context.environment, context.environment_bytes, "NTDOS64_T217_LONG",
            sizeof(long_value) - 1u))) passed = 0;
    /* OpenNT retains a nonempty malformed entry and later duplicates after
       its first-prefix filter. The copied ABI must not erase that source
       contract merely because normal Win32 APIs cannot create such a block. */
    {
        static const uint8_t source_permitted[] =
            "COMSPEC=first\0COMSPEC=second\0NO_EQUALS\0";
        if (passed && !bx_ntvdm_command_host_context_v1_set_environment(&context,
                source_permitted, (uint32_t)sizeof(source_permitted))) passed = 0;
    }
    for (index = 0u; index < sizeof(values) / sizeof(values[0]); ++index)
        if (!restore(&values[index])) passed = 0;
    if (!passed) return 1;
    puts("bx-ntvdm COMMAND initial environment: OpenNT ANSI/OEM filter contract verified");
    return 0;
}
