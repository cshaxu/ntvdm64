#include "dos_file_service.h"

#include "dos_namespace.h"

void nvtdm_dos_files_init(nvtdm_dos_files *files)
{
    size_t index;

    for (index = 0u; index < NVDTM_DOS_MAX_HANDLES; ++index) {
        files->handles[index] = INVALID_HANDLE_VALUE;
    }
}

void nvtdm_dos_files_destroy(nvtdm_dos_files *files)
{
    size_t index;

    for (index = NVDTM_DOS_FIRST_USER_HANDLE; index < NVDTM_DOS_MAX_HANDLES;
        ++index) {
        if (files->handles[index] != INVALID_HANDLE_VALUE) {
            CloseHandle(files->handles[index]);
            files->handles[index] = INVALID_HANDLE_VALUE;
        }
    }
}

int nvtdm_dos_open_readonly(nvtdm_dos_files *files, const wchar_t *root,
    const wchar_t *dos_path, uint16_t *dos_handle)
{
    wchar_t resolved[MAX_PATH];
    HANDLE file;
    size_t index;

    file = nvtdm_open_rooted_readonly(root, dos_path, resolved);
    if (file == INVALID_HANDLE_VALUE) return 0;
    for (index = NVDTM_DOS_FIRST_USER_HANDLE; index < NVDTM_DOS_MAX_HANDLES;
        ++index) {
        if (files->handles[index] == INVALID_HANDLE_VALUE) {
            files->handles[index] = file;
            *dos_handle = (uint16_t)index;
            return 1;
        }
    }
    CloseHandle(file);
    return 0;
}

int nvtdm_dos_read(nvtdm_dos_files *files, uint16_t dos_handle, void *buffer,
    DWORD requested, DWORD *read)
{
    if (dos_handle < NVDTM_DOS_FIRST_USER_HANDLE ||
        dos_handle >= NVDTM_DOS_MAX_HANDLES ||
        files->handles[dos_handle] == INVALID_HANDLE_VALUE) return 0;
    return ReadFile(files->handles[dos_handle], buffer, requested, read, NULL);
}

int nvtdm_dos_close(nvtdm_dos_files *files, uint16_t dos_handle)
{
    if (dos_handle < NVDTM_DOS_FIRST_USER_HANDLE ||
        dos_handle >= NVDTM_DOS_MAX_HANDLES ||
        files->handles[dos_handle] == INVALID_HANDLE_VALUE) return 0;
    if (!CloseHandle(files->handles[dos_handle])) return 0;
    files->handles[dos_handle] = INVALID_HANDLE_VALUE;
    return 1;
}
