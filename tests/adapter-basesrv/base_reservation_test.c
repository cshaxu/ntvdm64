#include <base_reservation.h>
#include <stdio.h>
#define CHECK(value) do { if (!(value)) { fprintf(stderr,"FAIL %d\\n",__LINE__);return 1; } } while (0)
int main(void)
{
    OPENNT_BASE_RESERVATIONS *state=NULL;
    HANDLE self=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION|SYNCHRONIZE,FALSE,GetCurrentProcessId());
    uint64_t first=0,second=0,third=0,claimed=0;ULONG task=0;HANDLE console=NULL,worker=NULL;BOOL shared_wow=FALSE;
    CHECK(self && OpenNtBaseReservationsInitialize(&state));
    CHECK(OpenNtBaseReservationCreate(state,101,7,41,(HANDLE)0x1234,FALSE,&first)==ERROR_SUCCESS);
    CHECK(OpenNtBaseReservationCreate(state,102,8,42,(HANDLE)0x1235,FALSE,&second)==ERROR_SUCCESS && second>first);
    CHECK(OpenNtBaseReservationCreate(state,103,9,43,NULL,FALSE,&third)==ERROR_INVALID_PARAMETER);
    CHECK(OpenNtBaseReservationCreate(state,103,9,43,NULL,TRUE,&third)==ERROR_SUCCESS && third>second);
    CHECK(OpenNtBaseReservationPrepareWorker(state,first,102,7,self)==ERROR_ACCESS_DENIED);
    CHECK(OpenNtBaseReservationPrepareWorker(state,first,101,7,self)==ERROR_SUCCESS);
    CHECK(OpenNtBaseReservationPrepareWorker(state,first,101,7,self)==ERROR_ALREADY_EXISTS);
    CHECK(!OpenNtBaseReservationIsWorkerLocalStream(state,first,(HANDLE)0x51));
    CHECK(OpenNtBaseReservationMarkWorkerLocalStream(state,first,(HANDLE)0x51)==ERROR_SUCCESS);
    CHECK(OpenNtBaseReservationMarkWorkerLocalStream(state,first,(HANDLE)0x52)==ERROR_SUCCESS);
    CHECK(OpenNtBaseReservationMarkWorkerLocalStream(state,first,(HANDLE)0x51)==ERROR_SUCCESS);
    CHECK(OpenNtBaseReservationIsWorkerLocalStream(state,first,(HANDLE)0x51));
    CHECK(!OpenNtBaseReservationIsWorkerLocalStream(state,first,(HANDLE)0x53));
    CHECK(OpenNtBaseReservationClaimWorker(state,GetCurrentProcessId(),9,&claimed,&task,&console,&shared_wow,&worker)==ERROR_SUCCESS);
    CHECK(claimed==first && task==41 && console==(HANDLE)0x1234 && !shared_wow);
    CHECK(worker!=NULL); CloseHandle(worker); worker=NULL;
    CHECK(OpenNtBaseReservationAbandon(state,first)); /* Claimed self must survive. */
    CHECK(OpenNtBaseReservationClaimWorker(state,GetCurrentProcessId(),10,&claimed,&task,&console,&shared_wow,&worker)==ERROR_ALREADY_EXISTS);
    CHECK(OpenNtBaseReservationRelease(state,first,102,7)==ERROR_ACCESS_DENIED);
    CHECK(OpenNtBaseReservationRelease(state,first,101,7)==ERROR_SUCCESS);
    CHECK(OpenNtBaseReservationPrepareWorker(state,third,103,9,self)==ERROR_SUCCESS);
    CHECK(OpenNtBaseReservationClaimWorker(state,GetCurrentProcessId(),11,&claimed,&task,&console,&shared_wow,&worker)==ERROR_SUCCESS);
    CHECK(claimed==third && task==43 && console==NULL && shared_wow);
    CHECK(worker!=NULL); CloseHandle(worker); worker=NULL;
    {
        WCHAR image[MAX_PATH];
        STARTUPINFOW startup={sizeof(startup)};
        PROCESS_INFORMATION child={0};
        CHECK(GetSystemDirectoryW(image,MAX_PATH));
        CHECK(lstrlenW(image)+9<MAX_PATH);lstrcatW(image,L"\\cmd.exe");
        CHECK(CreateProcessW(image,NULL,NULL,NULL,FALSE,CREATE_SUSPENDED|CREATE_NO_WINDOW,
            NULL,NULL,&startup,&child));
        CHECK(OpenNtBaseReservationPrepareWorker(state,second,102,8,child.hProcess)==ERROR_SUCCESS);
        CHECK(!OpenNtBaseReservationAbandon(state,second));
        CHECK(WaitForSingleObject(child.hProcess,5000)==WAIT_OBJECT_0);
        CloseHandle(child.hThread);CloseHandle(child.hProcess);
    }
    CHECK(OpenNtBaseReservationRelease(state,second,102,8)==ERROR_SUCCESS);
    CHECK(OpenNtBaseReservationRelease(state,third,103,9)==ERROR_SUCCESS);
    CHECK(OpenNtBaseReservationsDestroy(state));CloseHandle(self);
    puts("PASS: launcher-owned worker reservation, authenticated claim, generation and release");
    return 0;
}
