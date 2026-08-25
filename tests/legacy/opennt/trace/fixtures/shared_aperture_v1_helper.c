#include <stdint.h>
#include <stdlib.h>
#include <windows.h>

#include "shared_aperture_v1.h"

static uint32_t read_u32(const uint8_t *bytes, size_t offset)
{
    return (uint32_t)bytes[offset] | ((uint32_t)bytes[offset + 1u] << 8) |
        ((uint32_t)bytes[offset + 2u] << 16) | ((uint32_t)bytes[offset + 3u] << 24);
}

static void write_u32(uint8_t *bytes, size_t offset, uint32_t value)
{
    bytes[offset] = (uint8_t)value;
    bytes[offset + 1u] = (uint8_t)(value >> 8);
    bytes[offset + 2u] = (uint8_t)(value >> 16);
    bytes[offset + 3u] = (uint8_t)(value >> 24);
}

int wmain(int argc, wchar_t **argv)
{
    HANDLE mapping;
    uint8_t *view;
    unsigned long aperture_bytes;
    int result = 1;

    if (argc != 3) return 2;
    aperture_bytes = wcstoul(argv[2], NULL, 10);
    if (aperture_bytes < RUNNER_SHARED_APERTURE_V1_MINIMUM_BYTES) return 3;
    mapping = OpenFileMappingW(FILE_MAP_READ | FILE_MAP_WRITE, FALSE, argv[1]);
    if (mapping == NULL) return 4;
    view = MapViewOfFile(mapping, FILE_MAP_READ | FILE_MAP_WRITE, 0u, 0u, aperture_bytes);
    if (view == NULL) {
        CloseHandle(mapping);
        return 5;
    }
    if (read_u32(view, RUNNER_SHARED_APERTURE_V1_OFFSET_MAGIC) !=
            RUNNER_SHARED_APERTURE_V1_MAGIC ||
        read_u32(view, RUNNER_SHARED_APERTURE_V1_OFFSET_VERSION) !=
            RUNNER_SHARED_APERTURE_V1_VERSION ||
        read_u32(view, RUNNER_SHARED_APERTURE_V1_OFFSET_BYTES) != aperture_bytes ||
        read_u32(view, RUNNER_SHARED_APERTURE_V1_OFFSET_HOST_READY) != 1u ||
        read_u32(view, RUNNER_SHARED_APERTURE_V1_OFFSET_HOST_PROBE) != 0x13579bdfu) {
        goto cleanup;
    }
    write_u32(view, RUNNER_SHARED_APERTURE_V1_OFFSET_HELPER_PROBE, 0x2468ace0u);
    write_u32(view, RUNNER_SHARED_APERTURE_V1_OFFSET_HELPER_READY, 1u);
    result = 0;

cleanup:
    UnmapViewOfFile(view);
    CloseHandle(mapping);
    return result;
}
