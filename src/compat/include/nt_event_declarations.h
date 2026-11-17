/* Shared original ntdef.h/ntexapi.h event ABI declarations.
 * Native event objects retain this ABI; no event policy is implemented. */
#ifndef OPENNT_EVENT_DECLARATIONS_H
#define OPENNT_EVENT_DECLARATIONS_H
typedef enum _EVENT_TYPE { NotificationEvent, SynchronizationEvent } EVENT_TYPE;
typedef enum _EVENT_INFORMATION_CLASS { EventBasicInformation } EVENT_INFORMATION_CLASS;
typedef struct _EVENT_BASIC_INFORMATION {
    EVENT_TYPE EventType;
    LONG EventState;
} EVENT_BASIC_INFORMATION, *PEVENT_BASIC_INFORMATION;
NTSTATUS NTAPI NtQueryEvent(HANDLE, EVENT_INFORMATION_CLASS, PVOID, ULONG, PULONG);
#endif
