/* Compile the actual selected original unit, not a copy of its private
 * structures or reset algorithm. Filesystem calls and NtVdmControl are real.
 * This is provider evidence; it is not a guest INT21 execution claim. */
#include "mvdm/dos/dem/demsrch.c"
#include <stdio.h>

static int assertions, failures;
static WCHAR directory[MAX_PATH];

static void check(const char *name, int condition)
{
    ++assertions;
    fprintf(stdout, "%s %s\n", condition ? "PASS" : "FAIL", name);
    if (!condition) ++failures;
}

static void path(WCHAR *out, const WCHAR *name)
{
    _snwprintf(out, MAX_PATH, L"%s\\%s", directory, name);
    out[MAX_PATH - 1] = 0;
}

static int create_file(const WCHAR *name)
{
    WCHAR full[MAX_PATH];
    HANDLE file;
    DWORD written;
    path(full, name);
    file = CreateFileW(full, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_DELETE,
        NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return 0;
    if (!WriteFile(file, "AB", 2, &written, NULL) || written != 2) {
        CloseHandle(file); return 0;
    }
    return CloseHandle(file) != 0;
}

static void dispose(FFINDLIST *entry)
{
    FileFindClose(entry);
    RtlFreeUnicodeString(&entry->PathName);
    RtlFreeUnicodeString(&entry->FileName);
    memset(entry, 0, sizeof(*entry));
}

/* GetHandleInformation can raise STATUS_INVALID_HANDLE for a closed handle.
 * Treat only that exception as the same closed-handle result. */
static int is_closed_handle(HANDLE handle)
{
    DWORD flags;
    __try {
        return !GetHandleInformation(handle, &flags) &&
            GetLastError() == ERROR_INVALID_HANDLE;
    } __except (GetExceptionCode() == 0xC0000008L ?
        EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
        return 1;
    }
}

static void scenario(int mutation)
{
    FFINDLIST entry;
    FFINDDOSDATA first, next;
    SRCHBUF fcb;
    WCHAR pattern[MAX_PATH], remembered[MAX_PATH], other[MAX_PATH], hint[MAX_PATH];
    HANDLE closed;
    NTSTATUS status;
    memset(&entry, 0, sizeof(entry));
    check("create AAA", create_file(L"AAA.TST"));
    check("create BBB", create_file(L"BBB.TST"));
    check("create CCC", create_file(L"CCC.TST"));
    path(pattern, L"????????.TST");
    status = FileFindOpen(pattern, &entry, 4096);
    check("real directory open", NT_SUCCESS(status));
    if (!NT_SUCCESS(status)) goto cleanup;
    status = FileFindNext(&first, &entry);
    check("real first matching file", NT_SUCCESS(status));
    if (!NT_SUCCESS(status)) goto cleanup;
    entry.DosData = first;
    check("first is owned 8.3 TST file", !strcmp(first.cFileName, "AAA.TST") ||
        !strcmp(first.cFileName, "BBB.TST") || !strcmp(first.cFileName, "CCC.TST"));
    memset(&fcb, 0, sizeof(fcb));
    FillFCBSrchBuf(&first, &fcb);
    check("original FCB formatter", !memcmp(fcb.FileExt, "TST", 3) &&
        fcb.ulFileSize == 2);
    closed = entry.DirectoryHandle;
    entry.LastQueryStatus = mutation == 0 ? STATUS_UNSUCCESSFUL :
        mutation == 1 ? STATUS_NO_SUCH_FILE : STATUS_NO_MORE_FILES;
    check("remaining buffered entries before reclamation", entry.FindBufferNext != NULL);
    FileFindClose(&entry);
    check("reclamation saves bounded successor", entry.RestartHint.Length > 0 &&
        entry.RestartHint.Length < sizeof(hint));
    memcpy(hint, entry.RestartHint.Name, entry.RestartHint.Length);
    hint[entry.RestartHint.Length / sizeof(WCHAR)] = 0;
    FileFindClose(&entry);
    check("repeated reclamation preserves successor hint", entry.RestartHint.Length > 0 &&
        !memcmp(entry.RestartHint.Name, hint, entry.RestartHint.Length));
    check("reclamation preserves errors and reopens normal end",
        entry.LastQueryStatus == (mutation == 0 ? STATUS_UNSUCCESSFUL : STATUS_SUCCESS));
    check("close releases actual handle", is_closed_handle(closed));
    check("close releases original buffer ownership", NumDirectoryHandle == 0 &&
        NumFindBuffer == 0 && !entry.FindBufferBase && !entry.FindBufferNext);
    if (mutation == 1) {
        path(other, !wcscmp(entry.DosData.FileName, L"CCC.TST") ? L"BBB.TST" : L"CCC.TST");
        path(remembered, L"DDD.TST");
        check("rename unrelated file", MoveFileW(other, remembered) != 0);
        check("create later file", create_file(L"EEE.TST"));
    } else if (mutation >= 2) {
        path(remembered, entry.DosData.FileName);
        check("delete remembered file", DeleteFileW(remembered) != 0);
        if (mutation == 3) {
            path(other, hint);
            check("delete successor hint", DeleteFileW(other) != 0);
        }
        if (mutation == 4) ++entry.RestartHint.Index;
        if (mutation == 5) entry.RestartHint.Length = 0;
    }
    entry.SupportReset = TRUE;
    status = FileFindOpen(NULL, &entry, 4096);
    check("real reopen", NT_SUCCESS(status));
    if (!NT_SUCCESS(status)) goto cleanup;
    status = FileFindReset(&entry);
    fprintf(stdout, "RESET mutation=%d status=%08lx index=%lu\n", mutation,
        (unsigned long)status, (unsigned long)entry.DosData.FileIndex);
    if (mutation >= 3) {
        check("missing or mismatched hint ends without guessed successor", status == STATUS_NO_MORE_FILES);
    } else {
        check("reset finds anchor or surviving hint", status == STATUS_SUCCESS);
        status = FileFindNext(&next, &entry);
        check("continuation returns different file", status == STATUS_SUCCESS &&
            wcscmp(next.FileName, entry.DosData.FileName) != 0);
        if (mutation == 2)
            check("deleted anchor returns saved hint unconsumed", !wcscmp(next.FileName, hint));
        while (NT_SUCCESS(status)) status = FileFindNext(&next, &entry);
        check("enumeration ends with original status", status == STATUS_NO_MORE_FILES);
    }
cleanup:
    dispose(&entry);
    check("no original handle/buffer leak", NumDirectoryHandle == 0 && NumFindBuffer == 0);
    {
        const WCHAR *names[] = {L"AAA.TST", L"BBB.TST", L"CCC.TST", L"DDD.TST", L"EEE.TST"};
        int i;
        for (i = 0; i < 5; ++i) { path(other, names[i]); DeleteFileW(other); }
    }
}

int main(int argc, char **argv)
{
    FFINDLIST entry;
    WCHAR pattern[MAX_PATH];
    NTSTATUS status;
    setvbuf(stdout, NULL, _IONBF, 0);
    if (argc != 2 || !MultiByteToWideChar(CP_ACP, 0, argv[1], -1, directory, MAX_PATH)) return 2;
    /* Runner supplies a new owned directory; never use a global TEMP wildcard. */
    if (!CreateDirectoryW(directory, NULL)) return 3;
    check("unbound real monitor fast path unavailable",
        NtVdmControl(VdmQueryDir, &entry) == STATUS_NOT_IMPLEMENTED);
    scenario(0); scenario(1); scenario(2); scenario(3); scenario(4); scenario(5);
    memset(&entry, 0, sizeof(entry));
    path(pattern, L"????????.TST");
    status = FileFindOpen(pattern, &entry, 1);
    check("undersized buffer original failure", status == STATUS_BUFFER_TOO_SMALL);
    check("open failure releases resources", !entry.DirectoryHandle &&
        !entry.FindBufferBase && NumDirectoryHandle == 0 && NumFindBuffer == 0);
    entry.LastQueryStatus = STATUS_NO_MORE_FILES;
    FileFindClose(&entry);
    check("empty buffer preserves final status", entry.LastQueryStatus == STATUS_NO_MORE_FILES);
    dispose(&entry);
    check("owned directory cleanup", RemoveDirectoryW(directory) != 0);
    fprintf(stdout, "assertions=%d failures=%d\n", assertions, failures);
    return failures ? 1 : 0;
}
