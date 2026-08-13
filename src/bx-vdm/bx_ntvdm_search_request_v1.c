#include "bx_ntvdm_search_request_v1.h"

#include <string.h>
#include <wctype.h>

static uint32_t bx_ntvdm_search_request_u32(const uint8_t *input)
{
    return (uint32_t)input[0] | ((uint32_t)input[1] << 8) |
        ((uint32_t)input[2] << 16) | ((uint32_t)input[3] << 24);
}

static int bx_ntvdm_search_request_name_char(uint8_t value, int wildcard)
{
    return (value >= 'A' && value <= 'Z') || (value >= 'a' && value <= 'z') ||
        (value >= '0' && value <= '9') || value == '$' || value == '%' ||
        value == '\'' || value == '-' || value == '_' || value == '@' ||
        value == '~' || value == '!' || value == '#' || value == '(' ||
        value == ')' || value == '{' || value == '}' || value == '^' ||
        (wildcard && (value == '*' || value == '?'));
}

static int bx_ntvdm_search_request_component(const uint8_t *input,
    size_t length, int wildcard, wchar_t output[13])
{
    size_t index, base = 0u, extension = 0u;
    int dot = 0;
    if (input == 0 || output == 0 || length == 0u || length > 12u) return 0;
    for (index = 0u; index < length; ++index) {
        uint8_t value = input[index];
        if (value == '.') { if (dot) return 0; dot = 1; output[index] = L'.'; continue; }
        if (!bx_ntvdm_search_request_name_char(value, wildcard)) return 0;
        if (dot) { if (++extension > 3u) return 0; }
        else { if (++base > 8u) return 0; }
        output[index] = towupper((wint_t)value);
    }
    if (base == 0u || (dot && extension == 0u && !wildcard)) return 0;
    output[length] = L'\0';
    return 1;
}

static int bx_ntvdm_search_request_decode(const uint8_t path[128],
    uint16_t attributes, bx_ntvdm_search_query_v1 *query)
{
    size_t length = 0u, offset = 0u, directory_length = 0u;
    if (path == 0 || query == 0) return 0;
    while (length < BX_NTVDM_SEARCH_REQUEST_V1_PATH_BYTES && path[length] != 0u) {
        if (path[length] > 0x7fu) return 0;
        ++length;
    }
    if (length == BX_NTVDM_SEARCH_REQUEST_V1_PATH_BYTES || length < 4u ||
        !((path[0] >= 'A' && path[0] <= 'Z') || (path[0] >= 'a' && path[0] <= 'z')) ||
        path[1] != ':' || path[2] != '\\') return 0;
    memset(query, 0, sizeof(*query));
    query->drive_index = (uint8_t)(towupper((wint_t)path[0]) - L'A');
    query->attributes = attributes;
    offset = 3u;
    while (offset < length) {
        size_t end = offset;
        wchar_t component[13];
        while (end < length && path[end] != '\\') ++end;
        if (end == offset) return 0;
        if (end == length) {
            if (!bx_ntvdm_search_request_component(path + offset, end - offset,
                    1, query->pattern)) return 0;
        } else {
            if (!bx_ntvdm_search_request_component(path + offset, end - offset,
                    0, component) || (directory_length != 0u &&
                    directory_length + 1u >= BX_NTVDM_SEARCH_PLAN_V1_MAX_RELATIVE_DIRECTORY))
                return 0;
            if (directory_length != 0u) query->relative_directory[directory_length++] = L'\\';
            if (directory_length + wcslen(component) > BX_NTVDM_SEARCH_PLAN_V1_MAX_RELATIVE_DIRECTORY)
                return 0;
            {
                size_t component_length = wcslen(component);
                memcpy(query->relative_directory + directory_length, component,
                    (component_length + 1u) * sizeof(component[0]));
                directory_length += component_length;
            }
        }
        offset = end + 1u;
    }
    return query->pattern[0] != L'\0';
}

int bx_ntvdm_search_request_v1_decode_first_path(const uint8_t path[128],
    uint16_t attributes, bx_ntvdm_search_query_v1 *query)
{
    return bx_ntvdm_search_request_decode(path, attributes, query);
}

int bx_ntvdm_search_request_v1_decode_first_fcb(const uint8_t path[128],
    uint8_t extended_fcb, uint8_t attributes, bx_ntvdm_search_query_v1 *query)
{
    return bx_ntvdm_search_request_decode(path, extended_fcb ? attributes : 0u, query);
}

int bx_ntvdm_search_request_v1_decode_next_path(const uint8_t dta[43],
    bx_ntvdm_search_token_v1 *token)
{
    if (dta == 0 || token == 0) return 0;
    token->cookie = bx_ntvdm_search_request_u32(dta);
    token->id = bx_ntvdm_search_request_u32(dta + 4u);
    return token->cookie != 0u && token->id != 0u;
}

int bx_ntvdm_search_request_v1_decode_next_fcb(const uint8_t searchbuf[53],
    bx_ntvdm_search_token_v1 *token)
{
    if (searchbuf == 0 || token == 0) return 0;
    token->cookie = bx_ntvdm_search_request_u32(searchbuf + 32u);
    token->id = bx_ntvdm_search_request_u32(searchbuf + 36u);
    return token->cookie != 0u && token->id != 0u;
}
