#ifndef NTDOS64_CONTAINED_DEM_FILE_TOKENS_H
#define NTDOS64_CONTAINED_DEM_FILE_TOKENS_H

#include <stdint.h>
#include <windows.h>

#define NTDOS64_DEM_FILE_MAX_SLOTS 64

typedef struct NTDOS64_DEM_FILE_SLOT {
    HANDLE handle;
    uint8_t generation;
    BOOL open;
} NTDOS64_DEM_FILE_SLOT;

typedef struct NTDOS64_DEM_FILE_SESSION {
    WCHAR root[32768];
    DWORD root_length;
    uint16_t session_id;
    NTDOS64_DEM_FILE_SLOT slots[NTDOS64_DEM_FILE_MAX_SLOTS];
} NTDOS64_DEM_FILE_SESSION;

BOOL ntdos64_dem_file_session_init(NTDOS64_DEM_FILE_SESSION *session, LPCWSTR root);
VOID ntdos64_dem_file_session_teardown(NTDOS64_DEM_FILE_SESSION *session);
BOOL ntdos64_dem_file_open_read(NTDOS64_DEM_FILE_SESSION *session, LPCWSTR relative_path,
    uint32_t *token, uint32_t *size);
BOOL ntdos64_dem_file_read(NTDOS64_DEM_FILE_SESSION *session, uint32_t token,
    uint64_t offset, BOOL use_current_offset, void *buffer, uint32_t requested,
    uint32_t *actual);
BOOL ntdos64_dem_file_seek(NTDOS64_DEM_FILE_SESSION *session, uint32_t token,
    int64_t offset, DWORD origin, uint32_t *actual);
BOOL ntdos64_dem_file_close(NTDOS64_DEM_FILE_SESSION *session, uint32_t token,
    BOOL set_final_offset, uint32_t final_offset);

#endif
