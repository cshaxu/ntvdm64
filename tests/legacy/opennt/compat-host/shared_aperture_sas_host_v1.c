#include "shared_aperture_sas_host_v1.h"

#include <windows.h>

#include "shared_aperture_v1.h"

static HANDLE runner_shared_aperture_mapping;
static uint8_t *runner_shared_aperture_view;
static uint32_t runner_shared_aperture_bytes;

static uint32_t runner_shared_aperture_read_u32(size_t offset)
{
    const uint8_t *bytes = runner_shared_aperture_view;

    return (uint32_t)bytes[offset] | ((uint32_t)bytes[offset + 1u] << 8) |
        ((uint32_t)bytes[offset + 2u] << 16) | ((uint32_t)bytes[offset + 3u] << 24);
}

static void runner_shared_aperture_release(void)
{
    if (runner_shared_aperture_view != NULL) {
        UnmapViewOfFile(runner_shared_aperture_view);
        runner_shared_aperture_view = NULL;
    }
    if (runner_shared_aperture_mapping != NULL) {
        CloseHandle(runner_shared_aperture_mapping);
        runner_shared_aperture_mapping = NULL;
    }
    runner_shared_aperture_bytes = 0u;
}

int runner_shared_aperture_sas_v1_prepare(const wchar_t *mapping_name, uint32_t bytes)
{
    HANDLE mapping;
    uint8_t *view;

    if (mapping_name == NULL || bytes < RUNNER_SHARED_APERTURE_V1_MINIMUM_BYTES ||
        runner_shared_aperture_view != NULL) {
        return 0;
    }
    mapping = OpenFileMappingW(FILE_MAP_READ | FILE_MAP_WRITE, FALSE, mapping_name);
    if (mapping == NULL) return 0;
    view = MapViewOfFile(mapping, FILE_MAP_READ | FILE_MAP_WRITE, 0u, 0u, bytes);
    if (view == NULL) {
        CloseHandle(mapping);
        return 0;
    }
    runner_shared_aperture_mapping = mapping;
    runner_shared_aperture_view = view;
    runner_shared_aperture_bytes = bytes;
    if (runner_shared_aperture_read_u32(RUNNER_SHARED_APERTURE_V1_OFFSET_MAGIC) !=
            RUNNER_SHARED_APERTURE_V1_MAGIC ||
        runner_shared_aperture_read_u32(RUNNER_SHARED_APERTURE_V1_OFFSET_VERSION) !=
            RUNNER_SHARED_APERTURE_V1_VERSION ||
        runner_shared_aperture_read_u32(RUNNER_SHARED_APERTURE_V1_OFFSET_BYTES) != bytes ||
        runner_shared_aperture_read_u32(RUNNER_SHARED_APERTURE_V1_OFFSET_HOST_READY) != 1u) {
        runner_shared_aperture_release();
        return 0;
    }
    return 1;
}

uint8_t *runner_shared_aperture_sas_v1_view(void)
{
    return runner_shared_aperture_view;
}

void *host_sas_init(uint32_t size)
{
    if (runner_shared_aperture_view == NULL || size != runner_shared_aperture_bytes) {
        return NULL;
    }
    return runner_shared_aperture_view;
}

void *host_sas_term(void)
{
    /* NULL tells the original sas_term that this host allocator owns release. */
    runner_shared_aperture_release();
    return NULL;
}
