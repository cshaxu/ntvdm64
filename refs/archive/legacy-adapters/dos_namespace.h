#ifndef NVDTM_DOS_NAMESPACE_H
#define NVDTM_DOS_NAMESPACE_H

#include <windows.h>

/* Resolves a DOS path below a caller-selected host root without file I/O. */
int nvtdm_resolve_dos_path(const wchar_t *root, const wchar_t *dos_path,
    wchar_t output[MAX_PATH]);

/* Opens a regular file only after every existing path component is checked. */
HANDLE nvtdm_open_rooted_readonly(const wchar_t *root, const wchar_t *dos_path,
    wchar_t resolved[MAX_PATH]);

#endif
