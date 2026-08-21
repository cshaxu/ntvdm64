#include "ntdos64_config.h"
#include "byob_identity.h"

#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

typedef struct config_component {
    wchar_t file[64];
    wchar_t hash[65];
    uint64_t bytes;
} config_component;

static int ascii_wide(const char *value, wchar_t *out, size_t capacity)
{
    size_t index;
    if (!value || !out) return 0;
    for (index = 0u; value[index] != '\0'; ++index) {
        if ((unsigned char)value[index] > 0x7fu || index + 1u >= capacity) return 0;
        out[index] = (wchar_t)(unsigned char)value[index];
    }
    out[index] = L'\0';
    return 1;
}

static int scalar(char *line, const char *key, char **value)
{
    size_t length = strlen(key);
    char *at = line + length;
    if (strncmp(line, key, length) != 0 || *at++ != ':') return -1;
    while (*at == ' ') ++at;
    if (*at == '\0' || strchr(at, '#') != NULL || strchr(at, '\t') != NULL) return -1;
    *value = at;
    return 1;
}

static int set_component_field(char *line, const char *prefix, config_component *component)
{
    char key[32], *value;
    int matched;
    (void)snprintf(key, sizeof(key), "%s_file", prefix);
    matched = scalar(line, key, &value);
    if (matched >= 0) return matched == 0 ? 0 : ascii_wide(value, component->file, 64u);
    (void)snprintf(key, sizeof(key), "%s_sha256", prefix);
    matched = scalar(line, key, &value);
    if (matched >= 0) return matched == 0 ? 0 : ascii_wide(value, component->hash, 65u);
    (void)snprintf(key, sizeof(key), "%s_bytes", prefix);
    matched = scalar(line, key, &value);
    if (matched >= 0) {
        char *end = NULL;
        unsigned long long parsed = strtoull(value, &end, 10);
        if (matched == 0 || end == value || *end != '\0' || parsed == 0u) return 0;
        component->bytes = (uint64_t)parsed;
        return 1;
    }
    return -1;
}

static uint32_t key_bit(const char *line)
{
    static const char *const keys[] = {
        "ntio_file", "ntio_sha256", "ntio_bytes", "ntdos_file", "ntdos_sha256",
        "ntdos_bytes", "command_file", "command_sha256", "command_bytes",
        "target_file", "target_sha256", "target_bytes", "config_file", "autoexec_file"
    };
    uint32_t index;
    const char *colon = strchr(line, ':');
    if (colon == NULL) return 0u;
    for (index = 0u; index < sizeof(keys) / sizeof(keys[0]); ++index)
        if (strlen(keys[index]) == (size_t)(colon - line) &&
            strncmp(line, keys[index], (size_t)(colon - line)) == 0) return 1u << index;
    return 0u;
}

static int valid_component(const wchar_t *root, const config_component *component)
{
    size_t index;
    if (!component || component->file[0] == L'\0' || component->bytes == 0u ||
        wcslen(component->hash) != 64u || !byob_component_name_safe(component->file)) return 0;
    for (index = 0u; index < 64u; ++index)
        if (!((component->hash[index] >= L'0' && component->hash[index] <= L'9') ||
            (component->hash[index] >= L'a' && component->hash[index] <= L'f'))) return 0;
    return byob_verify_component(root, component->file, component->bytes,
        component->hash) == BYOB_IDENTITY_OK;
}

int ntdos64_config_load_sibling(wchar_t config_path[MAX_PATH], wchar_t root[MAX_PATH],
    wchar_t config_source[MAX_PATH], wchar_t autoexec_source[MAX_PATH],
    byob_profile_selection *selection)
{
    char bytes[8193] = {0}, *line, *next;
    DWORD read = 0u, path_length;
    HANDLE file;
    config_component ntio = {0}, ntdos = {0}, command = {0}, target = {0};
    wchar_t config_name[64] = {0}, autoexec_name[64] = {0};
    char *value;
    uint32_t seen = 0u;
    if (!config_path || !root || !config_source || !autoexec_source || !selection) return 0;
    memset(selection, 0, sizeof(*selection));
    path_length = GetModuleFileNameW(NULL, config_path, MAX_PATH);
    if (path_length == 0u || path_length >= MAX_PATH) return 0;
    { wchar_t *slash = wcsrchr(config_path, L'\\');
      if (!slash) return 0;
      *slash = L'\0';
      if (wcslen(config_path) + 14u >= MAX_PATH) return 0;
      wcscpy(root, config_path);
      wcscat(config_path, L"\\ntvdmcfg.yaml"); }
    file = CreateFileW(config_path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE || !ReadFile(file, bytes, sizeof(bytes) - 1u, &read, NULL) ||
        read == sizeof(bytes) - 1u) { if (file != INVALID_HANDLE_VALUE) CloseHandle(file); return 0; }
    CloseHandle(file); bytes[read] = '\0'; line = bytes;
    while (line && *line) {
        next = strpbrk(line, "\r\n"); if (next) { *next = '\0'; do { ++next; } while (*next == '\r' || *next == '\n'); }
        if (*line && *line != '#') {
            uint32_t bit = key_bit(line);
            if (bit == 0u || (seen & bit) != 0u) return 0;
            seen |= bit;
            int result = set_component_field(line, "ntio", &ntio);
            if (result < 0) result = set_component_field(line, "ntdos", &ntdos);
            if (result < 0) result = set_component_field(line, "command", &command);
            if (result < 0) result = set_component_field(line, "target", &target);
            if (result < 0) { result = scalar(line, "config_file", &value); if (result > 0) result = ascii_wide(value, config_name, 64u); }
            if (result < 0) { result = scalar(line, "autoexec_file", &value); if (result > 0) result = ascii_wide(value, autoexec_name, 64u); }
            if (result <= 0) return 0;
        }
        line = next;
    }
    if (!valid_component(root, &ntio) || !valid_component(root, &ntdos) ||
        !valid_component(root, &command) || !valid_component(root, &target) ||
        !byob_component_name_safe(config_name) || !byob_component_name_safe(autoexec_name)) return 0;
    selection->ntio.bytes = ntio.bytes; wcscpy(selection->ntio.file_name, ntio.file); wcscpy(selection->ntio.sha256, ntio.hash);
    selection->ntdos.bytes = ntdos.bytes; wcscpy(selection->ntdos.file_name, ntdos.file); wcscpy(selection->ntdos.sha256, ntdos.hash);
    selection->command.bytes = command.bytes; wcscpy(selection->command.file_name, command.file); wcscpy(selection->command.sha256, command.hash);
    selection->target.bytes = target.bytes; wcscpy(selection->target.file_name, target.file); wcscpy(selection->target.sha256, target.hash);
    selection->has_target_placement = 1u; selection->target_placement.drive_index = 2u;
    (void)swprintf(selection->target_placement.path, BYOB_PROFILE_GUEST_PATH_MAX_CHARS, L"\\%ls", target.file);
    selection->declared_target_count = 1u; selection->declared_targets[0].component = selection->target;
    selection->declared_targets[0].placement = selection->target_placement;
    if (swprintf(config_source, MAX_PATH, L"%ls\\%ls", root, config_name) < 0 ||
        swprintf(autoexec_source, MAX_PATH, L"%ls\\%ls", root, autoexec_name) < 0) return 0;
    return 1;
}
