/* Bounded user-mode fallback for DEM's unavailable indexed directory query. */
#ifndef DEM_RESTART_HINT_H
#define DEM_RESTART_HINT_H

typedef struct _DEM_RESTART_HINT {
    ULONG Index;
    ULONG Length;
    WCHAR Name[MAXIMUM_FILENAME_LENGTH + 1];
} DEM_RESTART_HINT;

static void DemCaptureRestartHint(DEM_RESTART_HINT *hint, PFILE_BOTH_DIR_INFORMATION next)
{
    hint->Length = 0;
    if (next && next->FileNameLength && !(next->FileNameLength & 1) &&
        next->FileNameLength <= MAXIMUM_FILENAME_LENGTH * sizeof(WCHAR)) {
        hint->Index = next->FileIndex;
        hint->Length = next->FileNameLength;
        RtlCopyMemory(hint->Name, next->FileName, hint->Length);
    }
}

static NTSTATUS DemFindRestartHint(const DEM_RESTART_HINT *hint, HANDLE directory,
                                  PVOID buffer, ULONG size, PVOID *next)
{
    IO_STATUS_BLOCK io;
    PFILE_BOTH_DIR_INFORMATION entry;
    UNICODE_STRING expected, actual;
    NTSTATUS status;
    BOOLEAN restart = TRUE;

    expected.Length = expected.MaximumLength = (USHORT)hint->Length;
    expected.Buffer = (PWCHAR)hint->Name;
    for (;;) {
        status = NtQueryDirectoryFile(directory, NULL, NULL, NULL, &io, buffer,
                                      size, FileBothDirectoryInformation, FALSE,
                                      NULL, restart);
        restart = FALSE;
        if (!NT_SUCCESS(status)) return status;
        entry = buffer;
        for (;;) {
            if (entry->FileIndex == hint->Index && entry->FileNameLength == hint->Length) {
                actual.Length = actual.MaximumLength = (USHORT)entry->FileNameLength;
                actual.Buffer = entry->FileName;
                if (!RtlCompareUnicodeString(&expected, &actual, TRUE)) {
                    *next = entry;
                    return STATUS_SUCCESS;
                }
            }
            if (!entry->NextEntryOffset) break;
            entry = (PVOID)((PUCHAR)entry + entry->NextEntryOffset);
        }
    }
}
#endif
