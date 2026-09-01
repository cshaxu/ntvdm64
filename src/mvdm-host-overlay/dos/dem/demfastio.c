#include "dem.h"
#include "mvdm_softpc_guest_memory.h"

/*
 * DIVERGENCE(MVDM-HOST-DIV-188): NT4 dispatches SVC_DEMFASTREAD to the
 * kernel-only NTFastDOSIO trap handler.  This user-mode mirror overlay keeps
 * that handler's observable split: it completes a safe synchronous disk-file
 * read, and otherwise sets CF so unchanged NTDOS `$READ` retries SVC_DEMREAD.
 *
 * The original kernel handler used a PVDM_TIB guest alias.  The replacement
 * deliberately obtains one bounded session mapping lease instead; neither a
 * host HANDLE nor a guest-memory pointer crosses the service boundary.
 */

static BOOL
demFastReadIsStandardHandle(HANDLE handle)
{
    HANDLE standard;

    standard = GetStdHandle(STD_INPUT_HANDLE);
    if (standard != NULL && standard != INVALID_HANDLE_VALUE && handle == standard) {
        return TRUE;
    }

    standard = GetStdHandle(STD_OUTPUT_HANDLE);
    if (standard != NULL && standard != INVALID_HANDLE_VALUE && handle == standard) {
        return TRUE;
    }

    standard = GetStdHandle(STD_ERROR_HANDLE);
    return standard != NULL && standard != INVALID_HANDLE_VALUE && handle == standard;
}

/* See NTFastDOSIO in base/ntos/vdm/x86/rdwr.c. */
VOID
demFastRead(VOID)
{
    HANDLE hFile;
    DWORD bytesRead;
    DWORD linearAddress;
    LONG fileOffset;
    LONG zero;
    guest_memory_lease *lease;
    uint8_t *buffer;

    hFile = GETHANDLE(getAX(), getBP());
    if (hFile == NULL || hFile == INVALID_HANDLE_VALUE ||
        demFastReadIsStandardHandle(hFile) || GetFileType(hFile) != FILE_TYPE_DISK) {
        setCF(1);
        return;
    }

    if (!getZF()) {
        zero = 0;
        fileOffset = (LONG)((((DWORD)getBX()) << 16) | getSI());
        if (SetFilePointer(hFile, fileOffset, &zero, FILE_BEGIN) == -1L &&
            GetLastError() != NO_ERROR) {
            setCF(1);
            return;
        }
    }

    linearAddress = ((DWORD)getDS() << 4) + getDX();
    lease = NULL;
    buffer = NULL;
    if (!mvdm_softpc_guest_memory_acquire(linearAddress, getCX(),
            GUEST_MEMORY_ACCESS_WRITE, &lease, &buffer)) {
        setCF(1);
        return;
    }

    if (!ReadFile(hFile, buffer, (DWORD)getCX(), &bytesRead, NULL)) {
        (void)mvdm_softpc_guest_memory_release(lease, FALSE);
        setCF(1);
        return;
    }

    if (!mvdm_softpc_guest_memory_release(lease, TRUE)) {
        setCF(1);
        return;
    }

    setAX((USHORT)bytesRead);
    setCF(0);
}
