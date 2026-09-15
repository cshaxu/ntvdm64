/* OPENNT-HOST-031: function-body fragment, included only by srvvdm.c.
 * The existing wait-query implementation is relocated without semantic edits.
 * CheckDOS readiness alone does not prove a pending GetNextVDMCommand wait.
 * The original record and critical section remain the sole state owners. */
    NTSTATUS Status;
    PCONSOLERECORD pConsoleRecord;
    EVENT_BASIC_INFORMATION EventInformation;
    BOOL Pending = FALSE;

    if (!ConsoleRecordExists)
        return FALSE;
    *ConsoleRecordExists = FALSE;

    Status = RtlEnterCriticalSection(&BaseSrvDOSCriticalSection);
    ASSERT(NT_SUCCESS(Status));
    Status = BaseSrvGetConsoleRecord(ConsoleHandle,&pConsoleRecord);
    if (NT_SUCCESS(Status)) {
        *ConsoleRecordExists = TRUE;
        if (pConsoleRecord->hWaitForVDMDup) {
            Status = NtQueryEvent(pConsoleRecord->hWaitForVDMDup,
                                  EventBasicInformation,
                                  &EventInformation,
                                  sizeof(EventInformation),
                                  NULL);
            Pending = NT_SUCCESS(Status) && !EventInformation.EventState;
        }
    }
    RtlLeaveCriticalSection(&BaseSrvDOSCriticalSection);
    return Pending;
