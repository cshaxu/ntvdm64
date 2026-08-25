#include "byob_launch_declaration.h"

#include <string.h>

#define BYOB_LAUNCH_DECLARATION_VERSION 1u

static int append_ascii(uint8_t *out, uint32_t *used, const wchar_t *text)
{
    while (*text != L'\0') {
        if (*text < 0x20 || *text > 0x7eu || *text == L'=' ||
            *used >= BYOB_LAUNCH_DECLARATION_TAIL_BYTES) return 0;
        out[(*used)++] = (uint8_t)*text++;
    }
    return 1;
}

static int kind_from_file_name(const wchar_t *file_name, uint32_t *out_kind)
{
    if (file_name == 0 || out_kind == 0) return 0;
    const wchar_t *extension = wcsrchr(file_name, L'.');
    if (extension == NULL) return 0;
    if (_wcsicmp(extension, L".COM") == 0) { *out_kind = BYOB_LAUNCH_TARGET_KIND_COM; return 1; }
    if (_wcsicmp(extension, L".EXE") == 0) { *out_kind = BYOB_LAUNCH_TARGET_KIND_EXE; return 1; }
    if (_wcsicmp(extension, L".BAT") == 0) { *out_kind = BYOB_LAUNCH_TARGET_KIND_BAT; return 1; }
    if (_wcsicmp(extension, L".PIF") == 0) { *out_kind = BYOB_LAUNCH_TARGET_KIND_PIF; return 1; }
    return 0;
}

int byob_launch_declaration_from_arguments(
    byob_launch_declaration *out_value, const wchar_t *target_file_name,
    int argument_count, wchar_t *const *arguments)
{
    int index;
    uint32_t used = 0u;
    if (out_value == 0 || argument_count < 0 || (argument_count != 0 && arguments == 0) ||
        !kind_from_file_name(target_file_name, &out_value->target_kind)) return 0;
    memset(out_value, 0, sizeof(*out_value));
    out_value->version = BYOB_LAUNCH_DECLARATION_VERSION;
    if (!kind_from_file_name(target_file_name, &out_value->target_kind)) return 0;
    for (index = 0; index < argument_count; ++index) {
        if (arguments[index] == 0 || (index != 0 &&
            (used >= BYOB_LAUNCH_DECLARATION_TAIL_BYTES ||
             ((out_value->tail[used++] = ' '), 0))) ||
            !append_ascii(out_value->tail, &used, arguments[index])) return 0;
    }
    out_value->tail_bytes = used;
    return 1;
}

int byob_launch_declaration_to_environment(
    const byob_launch_declaration *value, wchar_t *kind, uint32_t kind_capacity,
    wchar_t *tail, uint32_t tail_capacity)
{
    uint32_t index;
    const wchar_t *kind_value;
    if (value == 0 || kind == 0 || tail == 0 || value->version != BYOB_LAUNCH_DECLARATION_VERSION ||
        value->tail_bytes > BYOB_LAUNCH_DECLARATION_TAIL_BYTES ||
        tail_capacity <= value->tail_bytes) return 0;
    kind_value = value->target_kind == BYOB_LAUNCH_TARGET_KIND_COM ? L"com" :
        value->target_kind == BYOB_LAUNCH_TARGET_KIND_EXE ? L"exe" :
        value->target_kind == BYOB_LAUNCH_TARGET_KIND_BAT ? L"bat" :
        value->target_kind == BYOB_LAUNCH_TARGET_KIND_PIF ? L"pif" : 0;
    if (kind_value == 0 || kind_capacity < 4u) return 0;
    wcscpy(kind, kind_value);
    for (index = 0u; index < value->tail_bytes; ++index) {
        if (value->tail[index] < 0x20u || value->tail[index] > 0x7eu || value->tail[index] == '=') return 0;
        tail[index] = (wchar_t)value->tail[index];
    }
    tail[value->tail_bytes] = L'\0';
    return 1;
}

int byob_launch_declaration_from_environment(
    byob_launch_declaration *out_value, const wchar_t *kind, const wchar_t *tail)
{
    uint32_t used = 0u;
    if (out_value == 0 || kind == 0 || tail == 0) return 0;
    memset(out_value, 0, sizeof(*out_value));
    out_value->version = BYOB_LAUNCH_DECLARATION_VERSION;
    if (wcscmp(kind, L"com") == 0) out_value->target_kind = BYOB_LAUNCH_TARGET_KIND_COM;
    else if (wcscmp(kind, L"exe") == 0) out_value->target_kind = BYOB_LAUNCH_TARGET_KIND_EXE;
    else if (wcscmp(kind, L"bat") == 0) out_value->target_kind = BYOB_LAUNCH_TARGET_KIND_BAT;
    else if (wcscmp(kind, L"pif") == 0) out_value->target_kind = BYOB_LAUNCH_TARGET_KIND_PIF;
    else return 0;
    if (!append_ascii(out_value->tail, &used, tail)) return 0;
    out_value->tail_bytes = used;
    return 1;
}
