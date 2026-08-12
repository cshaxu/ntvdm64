#ifndef NVDTM_DOS_FILE_SERVICE_H
#define NVDTM_DOS_FILE_SERVICE_H

#include <windows.h>
#include <stdint.h>

#define NVDTM_DOS_FIRST_USER_HANDLE 5u
#define NVDTM_DOS_MAX_HANDLES 32u

typedef struct nvtdm_dos_files {
    HANDLE handles[NVDTM_DOS_MAX_HANDLES];
} nvtdm_dos_files;

void nvtdm_dos_files_init(nvtdm_dos_files *files);
void nvtdm_dos_files_destroy(nvtdm_dos_files *files);
int nvtdm_dos_open_readonly(nvtdm_dos_files *files, const wchar_t *root,
    const wchar_t *dos_path, uint16_t *dos_handle);
int nvtdm_dos_read(nvtdm_dos_files *files, uint16_t dos_handle, void *buffer,
    DWORD requested, DWORD *read);
int nvtdm_dos_close(nvtdm_dos_files *files, uint16_t dos_handle);

#endif
