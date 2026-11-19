/* Directory restart state uses SDK and compat/ntioapi.h types. */
#ifndef DEM_RESTART_HINT_H
#define DEM_RESTART_HINT_H

typedef struct _DEM_RESTART_HINT {
    ULONG Index;
    ULONG Length;
    WCHAR Name[MAXIMUM_FILENAME_LENGTH + 1];
} DEM_RESTART_HINT;

void DemCaptureRestartHint(DEM_RESTART_HINT *hint, PFILE_BOTH_DIR_INFORMATION next);
NTSTATUS DemFindRestartHint(const DEM_RESTART_HINT *hint, HANDLE directory,
    PVOID buffer, ULONG size, PVOID *next);
#endif
