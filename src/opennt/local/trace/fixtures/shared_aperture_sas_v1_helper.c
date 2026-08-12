#include <stdint.h>
#include <stdlib.h>
#include <windows.h>

#include "shared_aperture_sas_host_v1.h"
#include "shared_aperture_v1.h"

extern void sas_init(uint32_t size);
extern void sas_term(void);
extern uint32_t c_sas_memory_size(void);
extern uint8_t *ntdos64_ccpu_sm0_ram(void);

static void write_u32(uint8_t *bytes, size_t offset, uint32_t value)
{
    bytes[offset] = (uint8_t)value;
    bytes[offset + 1u] = (uint8_t)(value >> 8);
    bytes[offset + 2u] = (uint8_t)(value >> 16);
    bytes[offset + 3u] = (uint8_t)(value >> 24);
}

int wmain(int argc, wchar_t **argv)
{
    uint32_t bytes;
    uint8_t *view;
    int result = 1;

    if (argc != 3) return 2;
    bytes = (uint32_t)wcstoul(argv[2], NULL, 10);
    if (!ntdos64_shared_aperture_sas_v1_prepare(argv[1], bytes)) return 3;
    view = ntdos64_shared_aperture_sas_v1_view();
    if (view == NULL) return 4;
    sas_init(bytes);
    if (ntdos64_ccpu_sm0_ram() != view || c_sas_memory_size() != bytes) goto cleanup;
    view[NTDOS64_SHARED_APERTURE_V1_OFFSET_SAS_PROBE] = 0x5au;
    write_u32(view, NTDOS64_SHARED_APERTURE_V1_OFFSET_SAS_READY, 1u);
    sas_term();
    if (ntdos64_ccpu_sm0_ram() != NULL || ntdos64_shared_aperture_sas_v1_view() != NULL) {
        return 5;
    }
    result = 0;

cleanup:
    return result;
}
