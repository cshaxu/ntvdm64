#include "startup_snapshot_evidence.h"

#include "byob_identity.h"

#include <windows.h>
#include <stdio.h>
#include <string.h>

static int append_text(char **cursor, size_t *remaining, const char *text)
{
    int written;
    if (*remaining == 0u) return 0;
    written = snprintf(*cursor, *remaining, "%s", text);
    if (written < 0 || (size_t)written >= *remaining) return 0;
    *cursor += written;
    *remaining -= (size_t)written;
    return 1;
}

static int append_u64(char **cursor, size_t *remaining, uint64_t value)
{
    int written;
    if (*remaining == 0u) return 0;
    written = snprintf(*cursor, *remaining, "%llu",
        (unsigned long long)value);
    if (written < 0 || (size_t)written >= *remaining) return 0;
    *cursor += written;
    *remaining -= (size_t)written;
    return 1;
}

int runtime_startup_snapshot_evidence_write(
    const wchar_t *byob_root, const wchar_t *file_name,
    const runtime_startup_snapshot_transaction *transaction,
    const uint8_t *output, uint64_t output_bytes, uint64_t digest)
{
    wchar_t path[MAX_PATH], temporary[MAX_PATH];
    char *record, *cursor;
    size_t capacity, remaining;
    uint64_t offset = 0u;
    uint32_t index;
    HANDLE file = INVALID_HANDLE_VALUE;
    DWORD written = 0u;
    int result = 0;

    if (byob_root == 0 || *byob_root == L'\0' ||
        !byob_component_name_safe(file_name) || transaction == 0 || output == 0 ||
        !runtime_startup_snapshot_transaction_preflight(transaction,
            UINT64_C(0x100000), output_bytes) ||
        wcslen(byob_root) + wcslen(file_name) + 6u > MAX_PATH) return 0;
    swprintf(path, MAX_PATH, L"%ls%ls%ls", byob_root,
        byob_root[wcslen(byob_root) - 1u] == L'\\' ? L"" : L"\\", file_name);
    swprintf(temporary, MAX_PATH, L"%ls.tmp", path);
    capacity = 512u + (size_t)output_bytes * 2u +
        (size_t)transaction->range_count * 96u;
    record = (char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, capacity);
    if (record == 0) return 0;
    cursor = record;
    remaining = capacity;
    if (!append_text(&cursor, &remaining,
        "{\"schema\":\"ntvdm64-startup-snapshot-evidence\",\"boundary\":{\"vector\":" ) ||
        !append_u64(&cursor, &remaining, transaction->boundary.vector) ||
        !append_text(&cursor, &remaining, ",\"fault_rip\":") ||
        !append_u64(&cursor, &remaining, transaction->boundary.fault_rip) ||
        !append_text(&cursor, &remaining, "},\"cpu\":{\"mode\":") ||
        !append_u64(&cursor, &remaining, transaction->cpu_before.execution_mode) ||
        !append_text(&cursor, &remaining, ",\"eip\":") ||
        !append_u64(&cursor, &remaining, transaction->cpu_before.eip) ||
        !append_text(&cursor, &remaining, ",\"cs\":") ||
        !append_u64(&cursor, &remaining, transaction->cpu_before.cs) ||
        !append_text(&cursor, &remaining, "},\"ranges\":[")) goto done;
    for (index = 0u; index < transaction->range_count; ++index) {
        const runtime_startup_snapshot_range *range = &transaction->ranges[index];
        uint64_t byte_index;
        if ((index != 0u && !append_text(&cursor, &remaining, ",")) ||
            !append_text(&cursor, &remaining, "{\"id\":") ||
            !append_u64(&cursor, &remaining, range->id) ||
            !append_text(&cursor, &remaining, ",\"address\":") ||
            !append_u64(&cursor, &remaining, range->guest_read.address) ||
            !append_text(&cursor, &remaining, ",\"bytes_hex\":\"") ) goto done;
        for (byte_index = 0u; byte_index < range->guest_read.length; ++byte_index) {
            int encoded;
            if (remaining < 3u) goto done;
            encoded = snprintf(cursor, remaining, "%02x", output[offset + byte_index]);
            if (encoded != 2) goto done;
            cursor += 2; remaining -= 2u;
        }
        if (!append_text(&cursor, &remaining, "\"}")) goto done;
        offset += range->guest_read.length;
    }
    if (offset != output_bytes || !append_text(&cursor, &remaining,
        "],\"digest\":\"")) goto done;
    {
        int final_written = snprintf(cursor, remaining, "%016llx\"}",
            (unsigned long long)digest);
        if (final_written < 0 || (size_t)final_written >= remaining) goto done;
    }
    file = CreateFileW(temporary, GENERIC_WRITE, 0, 0, CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, 0);
    if (file == INVALID_HANDLE_VALUE ||
        !WriteFile(file, record, (DWORD)strlen(record), &written, 0) ||
        written != strlen(record) || !FlushFileBuffers(file)) goto done;
    CloseHandle(file); file = INVALID_HANDLE_VALUE;
    if (!MoveFileExW(temporary, path, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
        goto done;
    result = 1;
done:
    if (file != INVALID_HANDLE_VALUE) CloseHandle(file);
    if (!result) DeleteFileW(temporary);
    HeapFree(GetProcessHeap(), 0, record);
    return result;
}
