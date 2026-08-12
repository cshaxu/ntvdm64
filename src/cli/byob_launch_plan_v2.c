#include "byob_launch_plan_v2.h"

#include <string.h>

#define BYOB_LAUNCH_PLAN_V2_VERSION 2u

static int valid(const byob_launch_plan_v2 *value)
{
    uint32_t i;
    if (!value || value->version != BYOB_LAUNCH_PLAN_V2_VERSION || value->slot_count != 2u ||
        value->first.version != 1u || value->first.tail_bytes > BYOB_LAUNCH_DECLARATION_V1_TAIL_BYTES ||
        (value->first.target_kind != BYOB_LAUNCH_TARGET_KIND_V1_COM &&
         value->first.target_kind != BYOB_LAUNCH_TARGET_KIND_V1_EXE)) return 0;
    for (i = 0u; i < value->first.tail_bytes; ++i)
        if (value->first.tail[i] < 0x20u || value->first.tail[i] > 0x7eu || value->first.tail[i] == '=') return 0;
    return 1;
}

int byob_launch_plan_v2_from_arguments(byob_launch_plan_v2 *out_value,
    const byob_profile_selection *selection, int argument_count, wchar_t *const *arguments)
{
    if (!out_value || !selection || selection->declared_target_count != 2u ||
        selection->declared_targets[0].terminal != 0u || selection->declared_targets[1].terminal != 1u ||
        wcscmp(selection->declared_targets[1].component.file_name, L"QUIT.COM") != 0 ||
        !byob_launch_declaration_v1_from_arguments(&out_value->first,
            selection->declared_targets[0].component.file_name, argument_count, arguments)) return 0;
    out_value->version = BYOB_LAUNCH_PLAN_V2_VERSION;
    out_value->slot_count = 2u;
    return valid(out_value);
}

int byob_launch_plan_v2_to_environment(const byob_launch_plan_v2 *value,
    wchar_t output[BYOB_LAUNCH_PLAN_V2_ENV_CHARS])
{
    static const wchar_t digits[] = L"0123456789abcdef";
    uint32_t i, at = 8u;
    if (!output || !valid(value)) return 0;
    output[0] = L'2'; output[1] = L','; output[2] = L'2'; output[3] = L',';
    output[4] = value->first.target_kind == BYOB_LAUNCH_TARGET_KIND_V1_COM ? L'c' : L'e';
    output[5] = L','; output[6] = digits[value->first.tail_bytes >> 4];
    output[7] = digits[value->first.tail_bytes & 15u];
    for (i = 0u; i < value->first.tail_bytes; ++i) {
        output[at++] = digits[value->first.tail[i] >> 4];
        output[at++] = digits[value->first.tail[i] & 15u];
    }
    output[at] = L'\0';
    return 1;
}

int byob_launch_plan_v2_from_environment(byob_launch_plan_v2 *out_value,
    const wchar_t *input)
{
    uint32_t i, bytes; wchar_t kind;
    if (!out_value || !input || wcsncmp(input, L"2,2,", 4u) != 0 ||
        (input[4] != L'c' && input[4] != L'e') || input[5] != L',' ||
        input[6] < L'0' || input[6] > L'7' ||
        !((input[7] >= L'0' && input[7] <= L'9') || (input[7] >= L'a' && input[7] <= L'f'))) return 0;
    kind = input[4]; bytes = (uint32_t)((input[6] <= L'9' ? input[6] - L'0' : input[6] - L'a' + 10) << 4) |
        (uint32_t)(input[7] <= L'9' ? input[7] - L'0' : input[7] - L'a' + 10);
    if (bytes > BYOB_LAUNCH_DECLARATION_V1_TAIL_BYTES || wcslen(input) != 8u + bytes * 2u) return 0;
    memset(out_value, 0, sizeof(*out_value)); out_value->version = BYOB_LAUNCH_PLAN_V2_VERSION;
    out_value->slot_count = 2u; out_value->first.version = 1u;
    out_value->first.target_kind = kind == L'c' ? BYOB_LAUNCH_TARGET_KIND_V1_COM : BYOB_LAUNCH_TARGET_KIND_V1_EXE;
    out_value->first.tail_bytes = bytes;
    for (i = 0u; i < bytes; ++i) {
        wchar_t a = input[8u + i * 2u], b = input[9u + i * 2u];
        if (!((a >= L'0' && a <= L'9') || (a >= L'a' && a <= L'f')) ||
            !((b >= L'0' && b <= L'9') || (b >= L'a' && b <= L'f'))) return 0;
        out_value->first.tail[i] = (uint8_t)(((a <= L'9' ? a - L'0' : a - L'a' + 10) << 4) |
            (b <= L'9' ? b - L'0' : b - L'a' + 10));
    }
    return valid(out_value);
}
