#ifndef RUNTIME_DEMFILE_CREATE_OBSERVATION_V1_H
#define RUNTIME_DEMFILE_CREATE_OBSERVATION_V1_H

#include <stdint.h>
#include <windows.h>

#define RUNTIME_DEMFILE_CREATE_OBSERVATION_V1_MAGIC UINT32_C(0x42584446)
#define RUNTIME_DEMFILE_CREATE_OBSERVATION_V1_VERSION UINT32_C(1)
#define RUNTIME_DEMFILE_CREATE_OBSERVATION_V1_PATH_CHARS 261u

struct runtime_demfile_create_observation_v1 {
    uint32_t magic, abi_version, struct_bytes;
    uint32_t enabled, call_count, conversion_error, first_error, retry_error;
    uint32_t access, share, creation, flags;
    uint16_t path[RUNTIME_DEMFILE_CREATE_OBSERVATION_V1_PATH_CHARS];
};

#ifdef __cplusplus
extern "C" {
#endif
void runtime_demfile_create_observation_v1_enable(uint32_t enabled);
void runtime_demfile_create_observation_v1_record(const wchar_t *path,
    DWORD access, DWORD share, DWORD creation, DWORD flags, DWORD conversion_error,
    DWORD first_error, DWORD retry_error);
int runtime_demfile_create_observation_v1_copy(
    struct runtime_demfile_create_observation_v1 *value);
#ifdef __cplusplus
}
#endif

#endif
