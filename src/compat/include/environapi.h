#ifndef OPENNT_HOST_NTOS_RTL_ENVIRONAPI_H
#define OPENNT_HOST_NTOS_RTL_ENVIRONAPI_H

/* DIVERGENCE(OPENNT-HOST-026): modern NTDLL exports the historical names.
 * Keep the imported original environ.c bodies in an explicit private symbol
 * domain, exposed only to their selected consumers. */
#include <nt.h>
#define RtlCreateEnvironment OpenNtRtlCreateEnvironment
#define RtlDestroyEnvironment OpenNtRtlDestroyEnvironment
#define RtlSetCurrentEnvironment OpenNtRtlSetCurrentEnvironment
#define RtlQueryEnvironmentVariable_U OpenNtRtlQueryEnvironmentVariable_U
#define RtlSetEnvironmentVariable OpenNtRtlSetEnvironmentVariable
NTSTATUS NTAPI RtlCreateEnvironment(BOOLEAN CloneCurrentEnvironment, PVOID *Environment);
NTSTATUS NTAPI RtlDestroyEnvironment(PVOID Environment);
NTSTATUS NTAPI RtlSetCurrentEnvironment(PVOID Environment, PVOID *PreviousEnvironment);
NTSTATUS NTAPI RtlQueryEnvironmentVariable_U(PVOID Environment, PUNICODE_STRING Name, PUNICODE_STRING Value);
NTSTATUS NTAPI RtlSetEnvironmentVariable(PVOID *Environment, PUNICODE_STRING Name, PUNICODE_STRING Value);
BOOL OpenNtRtlRefreshEnvironmentSnapshot(VOID);

#endif
