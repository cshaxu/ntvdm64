#include "wow32-dll/include/wow_task_termination.h"
#include <stdio.h>

static const uint64_t first_ticket = UINT64_C(0x100000001);
static const uint64_t second_ticket = UINT64_C(0x200000002);
static const USHORT reused_task = 0x1234;
static HANDLE handler_started, handler_release, ticket_reopened;
static volatile LONG handler_result, calls;
static uint64_t reopened_ticket;

DWORD WINAPI OpenNtBaseClientFindWowTask(uint64_t identity, USHORT *htask)
{
    *htask = identity == first_ticket || identity == second_ticket ? reused_task : 0;
    return *htask ? ERROR_SUCCESS : ERROR_NOT_FOUND;
}

DWORD WINAPI OpenNtBaseClientFailWowTaskClose(uint64_t identity)
{
    reopened_ticket = identity;
    InterlockedIncrement(&calls);
    SetEvent(ticket_reopened);
    return ERROR_SUCCESS;
}

DWORD WINAPI W32HungAppNotifyThread(UINT htask)
{
    if (htask != reused_task || !SetEvent(handler_started) ||
        WaitForSingleObject(handler_release, 5000) != WAIT_OBJECT_0)
        return ERROR_INVALID_STATE;
    return (DWORD)InterlockedCompareExchange(&handler_result, 0, 0);
}

void WINAPI ExitVDM(BOOL wow, ULONG task)
{
    (void)wow;
    (void)task;
    ExitProcess(ERROR_INVALID_FUNCTION);
}

DWORD WINAPI NtvdmAuthorizeExit(DWORD code)
{
    ExitProcess(ERROR_INVALID_FUNCTION);
    return code;
}

int main(void)
{
    handler_started = CreateEventW(NULL, FALSE, FALSE, NULL);
    handler_release = CreateEventW(NULL, FALSE, FALSE, NULL);
    ticket_reopened = CreateEventW(NULL, FALSE, FALSE, NULL);
    if (!handler_started || !handler_release || !ticket_reopened) return 1;

    /* Both tickets resolve to the same guest identity. The starter must
     * reopen only its ticket, even if another close request is waiting. */
    InterlockedExchange(&handler_result, ERROR_CANCELLED);
    if (W32TerminateTaskTicket(3) != ERROR_NOT_FOUND ||
        W32TerminateTaskTicket(first_ticket) ||
        WaitForSingleObject(handler_started, 5000) != WAIT_OBJECT_0 ||
        W32TerminateTaskTicket(second_ticket) != ERROR_BUSY ||
        !SetEvent(handler_release) ||
        WaitForSingleObject(ticket_reopened, 5000) != WAIT_OBJECT_0 ||
        calls != 1 || reopened_ticket != first_ticket)
        return 2;

    InterlockedExchange(&handler_result, ERROR_SUCCESS);
    if (W32TerminateTaskTicket(second_ticket) ||
        WaitForSingleObject(handler_started, 5000) != WAIT_OBJECT_0 ||
        !SetEvent(handler_release) ||
        WaitForSingleObject(ticket_reopened, 500) != WAIT_TIMEOUT || calls != 1)
        return 3;

    CloseHandle(ticket_reopened);
    CloseHandle(handler_release);
    CloseHandle(handler_started);
    puts("WOW_TASK_CLOSE_RESULT cancel=original-ticket reused-htask=1 serialized=1 success-reopen=0");
    return 0;
}
