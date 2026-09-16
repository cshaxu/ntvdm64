#include <nt.h>

static DWORD ntwait_timeout_milliseconds(const PLARGE_INTEGER timeout)
{
    ULONGLONG ticks;
    ULONGLONG milliseconds;
    FILETIME file_time;
    ULARGE_INTEGER now;

    if (timeout == NULL) {
        return INFINITE;
    }

    if (timeout->QuadPart < 0) {
        ticks = (ULONGLONG)(-(timeout->QuadPart + 1)) + 1u;
    } else {
        GetSystemTimeAsFileTime(&file_time);
        now.LowPart = file_time.dwLowDateTime;
        now.HighPart = file_time.dwHighDateTime;
        if ((ULONGLONG)timeout->QuadPart <= now.QuadPart) {
            return 0;
        }
        ticks = (ULONGLONG)timeout->QuadPart - now.QuadPart;
    }

    milliseconds = (ticks + 9999u) / 10000u;
    return milliseconds >= (ULONGLONG)MAXDWORD ? MAXDWORD :
        (DWORD)milliseconds;
}

NTSTATUS NTAPI opennt_NtWaitForMultipleObjects(
    ULONG count, HANDLE *handles, ULONG wait_type, BOOLEAN alertable,
    PLARGE_INTEGER timeout)
{
    DWORD result;

    if (count == 0 || count > MAXIMUM_WAIT_OBJECTS || handles == NULL ||
        (wait_type != WaitAll && wait_type != WaitAny)) {
        return STATUS_INVALID_PARAMETER;
    }

    result = WaitForMultipleObjectsEx(
        count, handles, wait_type == WaitAll, ntwait_timeout_milliseconds(timeout),
        alertable);
    if (result >= WAIT_OBJECT_0 && result < WAIT_OBJECT_0 + count) {
        return (NTSTATUS)(result - WAIT_OBJECT_0);
    }
    if (result >= WAIT_ABANDONED_0 && result < WAIT_ABANDONED_0 + count) {
        return (NTSTATUS)(0x00000080L + result - WAIT_ABANDONED_0);
    }
    if (result == WAIT_TIMEOUT) {
        return STATUS_TIMEOUT;
    }
    if (result == WAIT_IO_COMPLETION) {
        return STATUS_USER_APC;
    }
    return (NTSTATUS)0xC0000001L; /* STATUS_UNSUCCESSFUL */
}
