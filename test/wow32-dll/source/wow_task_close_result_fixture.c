#include "wow32-dll/include/wow_task_termination.h"
#include <stdio.h>

static const uint64_t first_ticket = UINT64_C(0x100000001);
static const uint64_t second_ticket = UINT64_C(0x200000002);
static const USHORT reused_task = 0x1234;
static HANDLE handler_started, handler_release, ticket_reopened;
static volatile LONG handler_result, calls;
static uint64_t reopened_ticket;

DWORD WINAPI OpenNtBaseClientBindWowTask(ULONG task, USHORT htask, uint64_t *identity)
{
    if (htask != reused_task || (task != 1 && task != 2)) return ERROR_INVALID_PARAMETER;
    *identity = task == 1 ? first_ticket : second_ticket;
    return ERROR_SUCCESS;
}

DWORD WINAPI OpenNtBaseClientRetireWowTask(uint64_t identity)
{
    (void)identity;
    return ERROR_SUCCESS;
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

    /* Bind the second ticket with the same guest identity while the first
     * handler is running. The starter must reopen the first, not the newest. */
    InterlockedExchange(&handler_result, ERROR_CANCELLED);
    if (wow_task_termination_bind(1, reused_task) ||
        W32TerminateTaskTicket(first_ticket) ||
        WaitForSingleObject(handler_started, 5000) != WAIT_OBJECT_0 ||
        wow_task_termination_bind(2, reused_task) ||
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
