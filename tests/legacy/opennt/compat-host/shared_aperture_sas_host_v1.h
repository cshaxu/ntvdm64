#ifndef RUNNER_SHARED_APERTURE_SAS_HOST_V1_H
#define RUNNER_SHARED_APERTURE_SAS_HOST_V1_H

#include <stdint.h>
#include <wchar.h>

int runner_shared_aperture_sas_v1_prepare(const wchar_t *mapping_name, uint32_t bytes);
uint8_t *runner_shared_aperture_sas_v1_view(void);

#endif
