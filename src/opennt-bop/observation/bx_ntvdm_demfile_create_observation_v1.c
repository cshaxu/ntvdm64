#include "bx_ntvdm_demfile_create_observation_v1.h"

#include <string.h>

static uint32_t enabled;
static struct bx_ntvdm_demfile_create_observation_v1 record;

static void clear(void)
{
    memset(&record, 0, sizeof(record));
    record.magic = BX_NTVDM_DEMFILE_CREATE_OBSERVATION_V1_MAGIC;
    record.abi_version = BX_NTVDM_DEMFILE_CREATE_OBSERVATION_V1_VERSION;
    record.struct_bytes = sizeof(record);
    record.enabled = enabled;
}

void bx_ntvdm_demfile_create_observation_v1_enable(uint32_t value)
{
    enabled = value == 1u;
    clear();
}

void bx_ntvdm_demfile_create_observation_v1_record(const wchar_t *path,
    DWORD access, DWORD share, DWORD creation, DWORD flags, DWORD conversion_error,
    DWORD first_error, DWORD retry_error)
{
    uint32_t index;
    if (!enabled) return;
    ++record.call_count;
    record.access = access;
    record.share = share;
    record.creation = creation;
    record.flags = flags;
    record.conversion_error = conversion_error;
    record.first_error = first_error;
    record.retry_error = retry_error;
    memset(record.path, 0, sizeof(record.path));
    if (path == NULL) return;
    for (index = 0u; index + 1u < BX_NTVDM_DEMFILE_CREATE_OBSERVATION_V1_PATH_CHARS &&
            path[index] != L'\0'; ++index)
        record.path[index] = (uint16_t)path[index];
}

int bx_ntvdm_demfile_create_observation_v1_copy(
    struct bx_ntvdm_demfile_create_observation_v1 *value)
{
    if (value == NULL || !enabled) return 0;
    *value = record;
    return 1;
}
