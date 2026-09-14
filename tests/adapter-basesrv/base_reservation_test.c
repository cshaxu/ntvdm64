#include <base_reservation.h>
#include <stdio.h>
#define CHECK(value) do { if (!(value)) { fprintf(stderr,"FAIL %d\\n",__LINE__);return 1; } } while (0)
int main(void)
{
    OPENNT_BASE_RESERVATIONS *state=NULL;
    HANDLE self=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION|SYNCHRONIZE,FALSE,GetCurrentProcessId());
    uint64_t first=0,second=0,claimed=0;ULONG task=0;HANDLE console=NULL;
    CHECK(self && OpenNtBaseReservationsInitialize(&state));
    CHECK(OpenNtBaseReservationCreate(state,101,7,41,(HANDLE)0x1234,&first)==ERROR_SUCCESS);
    CHECK(OpenNtBaseReservationCreate(state,102,8,42,(HANDLE)0x1235,&second)==ERROR_SUCCESS && second>first);
    CHECK(OpenNtBaseReservationPrepareWorker(state,first,102,7,self)==ERROR_ACCESS_DENIED);
    CHECK(OpenNtBaseReservationPrepareWorker(state,first,101,7,self)==ERROR_SUCCESS);
    CHECK(OpenNtBaseReservationPrepareWorker(state,first,101,7,self)==ERROR_ALREADY_EXISTS);
    CHECK(OpenNtBaseReservationClaimWorker(state,GetCurrentProcessId(),9,&claimed,&task,&console)==ERROR_SUCCESS);
    CHECK(claimed==first && task==41 && console==(HANDLE)0x1234);
    CHECK(OpenNtBaseReservationClaimWorker(state,GetCurrentProcessId(),10,&claimed,&task,&console)==ERROR_ALREADY_EXISTS);
    CHECK(OpenNtBaseReservationRelease(state,first,102,7)==ERROR_ACCESS_DENIED);
    CHECK(OpenNtBaseReservationRelease(state,first,101,7)==ERROR_SUCCESS);
    CHECK(OpenNtBaseReservationRelease(state,second,102,8)==ERROR_SUCCESS);
    CHECK(OpenNtBaseReservationsDestroy(state));CloseHandle(self);
    puts("PASS: launcher-owned worker reservation, authenticated claim, generation and release");
    return 0;
}
