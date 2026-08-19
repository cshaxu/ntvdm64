#include "bx_ntvdm_dem_path_v1.h"

#include <string.h>
#include <wctype.h>
#include <windows.h>

static int component(const wchar_t *source, size_t length, wchar_t out[13])
{
    size_t index, base = 0u, extension = 0u;
    int dot = 0;
    if (source == 0 || out == 0 || length == 0u || length >= 13u) return 0;
    for (index = 0u; index < length; ++index) {
        wchar_t value = towupper((wint_t)source[index]);
        int allowed;
        if (value == L'.') {
            if (dot || base == 0u) return 0;
            dot = 1; out[index] = value; continue;
        }
        allowed = (value >= L'A' && value <= L'Z') ||
            (value >= L'0' && value <= L'9') || value == L'$' || value == L'%' ||
            value == L'\'' || value == L'-' || value == L'_' || value == L'@' ||
            value == L'~' || value == L'!' || value == L'#' || value == L'(' ||
            value == L')' || value == L'{' || value == L'}' || value == L'^';
        if (!allowed) return 0;
        if (dot) ++extension; else ++base;
        if (base > 8u || extension > 3u) return 0;
        out[index] = value;
    }
    if (base == 0u || (dot && extension == 0u)) return 0;
    out[length] = L'\0';
    return 1;
}

static int append_component(wchar_t output[BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE],
    const wchar_t *source, size_t length)
{
    wchar_t canonical[13];
    size_t used = wcslen(output);
    if (!component(source, length, canonical) ||
        used + (used != 0u ? 1u : 0u) + wcslen(canonical) >=
            BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE) return 0;
    if (used != 0u) output[used++] = L'\\';
    wcscpy_s(output + used, BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE - used, canonical);
    return 1;
}

int bx_ntvdm_dem_path_v1_resolve(const char *oem_path,
    const bx_ntvdm_dem_cwd_context_v1 *cwd, uint8_t *drive_index_out,
    wchar_t relative_out[BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE])
{
    wchar_t converted[BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE];
    wchar_t cwd_relative[BX_NTVDM_DEM_CWD_CONTEXT_V1_MAX_RELATIVE];
    const wchar_t *cursor;
    size_t input_bytes, written;
    uint8_t drive;
    int rooted = 0;
    if (drive_index_out != 0) *drive_index_out = 0u;
    if (relative_out == 0 || oem_path == 0 || cwd == 0 ||
        !bx_ntvdm_dem_cwd_context_v1_valid(cwd)) return BX_NTVDM_DEM_PATH_V1_REJECTED;
    input_bytes = strnlen_s(oem_path, BX_NTVDM_DEM_PATH_V1_MAX_OEM_BYTES + 1u);
    if (input_bytes == 0u || input_bytes > BX_NTVDM_DEM_PATH_V1_MAX_OEM_BYTES)
        return BX_NTVDM_DEM_PATH_V1_REJECTED;
    written = (size_t)MultiByteToWideChar(CP_OEMCP, 0,
        oem_path, (int)input_bytes, converted,
        BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE - 1u);
    if (written == 0u) return BX_NTVDM_DEM_PATH_V1_ENCODING;
    converted[written] = L'\0';
    if (!bx_ntvdm_dem_cwd_context_v1_query_default_drive(cwd, &drive))
        return BX_NTVDM_DEM_PATH_V1_REJECTED;
    cursor = converted;
    if (((cursor[0] >= L'A' && cursor[0] <= L'Z') ||
         (cursor[0] >= L'a' && cursor[0] <= L'z')) && cursor[1] == L':') {
        drive = (uint8_t)(towupper((wint_t)cursor[0]) - L'A');
        cursor += 2;
    }
    if (*cursor == L'\\' || *cursor == L'/') {
        rooted = 1;
        while (*cursor == L'\\' || *cursor == L'/') ++cursor;
    }
    relative_out[0] = L'\0';
    if (!rooted) {
        if (bx_ntvdm_dem_cwd_context_v1_query(cwd, drive, cwd_relative,
                BX_NTVDM_DEM_CWD_CONTEXT_V1_MAX_RELATIVE) != BX_NTVDM_DEM_CWD_V1_OK)
            return BX_NTVDM_DEM_PATH_V1_REJECTED;
        if (wcslen(cwd_relative) >= BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE)
            return BX_NTVDM_DEM_PATH_V1_CAPACITY;
        wcscpy_s(relative_out, BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE, cwd_relative);
    }
    while (*cursor != L'\0') {
        const wchar_t *end = cursor;
        while (*end != L'\0' && *end != L'\\' && *end != L'/') ++end;
        if (!append_component(relative_out, cursor, (size_t)(end - cursor)))
            return BX_NTVDM_DEM_PATH_V1_REJECTED;
        cursor = end;
        while (*cursor == L'\\' || *cursor == L'/') ++cursor;
    }
    if (relative_out[0] == L'\0') return BX_NTVDM_DEM_PATH_V1_REJECTED;
    *drive_index_out = drive;
    return BX_NTVDM_DEM_PATH_V1_OK;
}
