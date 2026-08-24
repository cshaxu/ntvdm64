#ifndef NTDOS64_SHARED_APERTURE_SAS_HOST_V1_H
#define NTDOS64_SHARED_APERTURE_SAS_HOST_V1_H

#include <stdint.h>
#include <wchar.h>

int ntdos64_shared_aperture_sas_v1_prepare(const wchar_t *mapping_name, uint32_t bytes);
uint8_t *ntdos64_shared_aperture_sas_v1_view(void);

#endif
