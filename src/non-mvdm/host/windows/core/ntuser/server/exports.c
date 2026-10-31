/* Copyright 1985-91, Microsoft Corporation.
 * DIVERGENCE(OPENNT-HOST-030): true subset of original winsrv exports.c.
 * Only the BaseSrv-reached token query helper is retained; precomp.h and the
 * remaining winsrv/USER product are excluded. The sole private predicate is
 * bound to the admitted standalone interactive logon scope. */
#include <base_interactive.h>

NTSTATUS
_UserTestTokenForInteractive(
    HANDLE Token,
    PLUID pluidCaller
    )
{
    PTOKEN_STATISTICS pStats;
    ULONG BytesRequired;
    NTSTATUS Status;

    /*
     * Get the session id of the caller.
     */
    Status = NtQueryInformationToken(
                 Token,                     // Handle
                 TokenStatistics,           // TokenInformationClass
                 NULL,                      // TokenInformation
                 0,                         // TokenInformationLength
                 &BytesRequired             // ReturnLength
                 );

    if (Status != STATUS_BUFFER_TOO_SMALL) {
        return Status;
        }

    //
    // Allocate space for the user info
    //

    pStats = (PTOKEN_STATISTICS)LocalAlloc(LPTR, BytesRequired);
    if (pStats == NULL) {
        return Status;
        }

    //
    // Read in the user info
    //

    Status = NtQueryInformationToken(
                 Token,             // Handle
                 TokenStatistics,       // TokenInformationClass
                 pStats,                // TokenInformation
                 BytesRequired,         // TokenInformationLength
                 &BytesRequired         // ReturnLength
                 );

    if (NT_SUCCESS(Status)) {
        if (pluidCaller != NULL)
             *pluidCaller = pStats->AuthenticationId;

        /*
         * A valid session id has been returned.  Compare it
         * with the id of the logged on user.
         */
        Status = NtUserTestForInteractiveUser(&pStats->AuthenticationId);
#ifdef LATER
        if (pStats->AuthenticationId.QuadPart == pwinsta->luidUser.QuadPart)
            Status = STATUS_SUCCESS;
        else
            Status = STATUS_ACCESS_DENIED;
#endif
    }

    LocalFree(pStats);

    return Status;
}
