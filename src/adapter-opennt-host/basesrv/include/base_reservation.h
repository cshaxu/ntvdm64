#ifndef OPENNT_BASE_RESERVATION_H
#define OPENNT_BASE_RESERVATION_H
#include <windows.h>
#include <stdint.h>

/* Private binding for the interval between original CheckVDM's no-worker
 * result and the worker's authenticated Connect.  IDs correlate a launcher
 * request; they are never authority.  Worker authority derives only from a
 * launcher-supplied OS process reference and the RPC-authenticated PID. */
typedef struct OPENNT_BASE_RESERVATIONS OPENNT_BASE_RESERVATIONS;

BOOL OpenNtBaseReservationsInitialize(OPENNT_BASE_RESERVATIONS **);
BOOL OpenNtBaseReservationsDestroy(OPENNT_BASE_RESERVATIONS *);
DWORD OpenNtBaseReservationCreate(OPENNT_BASE_RESERVATIONS *,DWORD launcher_pid,
    DWORD launcher_generation,ULONG task,HANDLE console,BOOL shared_wow,uint64_t *reservation);
DWORD OpenNtBaseReservationPrepareWorker(OPENNT_BASE_RESERVATIONS *,uint64_t reservation,
    DWORD launcher_pid,DWORD launcher_generation,HANDLE worker);
/* A worker cannot nominate a reservation.  The service finds the one whose
 * pre-registered live process identity matches this authenticated connection. */
DWORD OpenNtBaseReservationClaimWorker(OPENNT_BASE_RESERVATIONS *,DWORD worker_pid,
    DWORD worker_generation,uint64_t *reservation,ULONG *task,HANDLE *console);
/* Standard streams are copied by original BaseSrvUpdateVDMEntry before the
 * worker connects.  Retain typed broker resources under that finite
 * reservation until original GetNextVDMCommand consumes them. */
DWORD OpenNtBaseReservationAcceptStream(OPENNT_BASE_RESERVATIONS *,uint64_t reservation,
    HANDLE stream,uint32_t *receipt);
DWORD OpenNtBaseReservationResolveStream(OPENNT_BASE_RESERVATIONS *,uint64_t reservation,
    uint32_t receipt,HANDLE *stream);
DWORD OpenNtBaseReservationRevokeStream(OPENNT_BASE_RESERVATIONS *,uint64_t reservation,
    uint32_t receipt);
/* Original Update can duplicate a standard stream directly into the suspended
 * worker before its first Get.  Its resulting numeric value is worker-local,
 * not a broker receipt; record that finite fact so Get does not re-resolve it. */
DWORD OpenNtBaseReservationMarkWorkerLocalStream(OPENNT_BASE_RESERVATIONS *,
    uint64_t reservation,HANDLE stream);
BOOL OpenNtBaseReservationIsWorkerLocalStream(OPENNT_BASE_RESERVATIONS *,
    uint64_t reservation,HANDLE stream);
/* The launcher may retain only the live worker it registered under its own
 * reservation.  This is the finite replacement for the original CSR client
 * process-handle namespace; command records still contain no OS handles. */
DWORD OpenNtBaseReservationRetainWorker(OPENNT_BASE_RESERVATIONS *,uint64_t reservation,
    DWORD launcher_pid,DWORD launcher_generation,HANDLE *worker);
DWORD OpenNtBaseReservationRelease(OPENNT_BASE_RESERVATIONS *,uint64_t reservation,
    DWORD launcher_pid,DWORD launcher_generation);
#endif
