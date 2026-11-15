#ifndef OPENNT_HOST_NTOS_RTL_ENVIRONAPI_H
#define OPENNT_HOST_NTOS_RTL_ENVIRONAPI_H

/* Declarations for the linked original RTL environment owner and its snapshot ingress. */
#include <nt.h>
NTSTATUS NTAPI RtlCreateEnvironment(BOOLEAN CloneCurrentEnvironment, PVOID *Environment);
NTSTATUS NTAPI RtlDestroyEnvironment(PVOID Environment);
NTSTATUS NTAPI RtlSetCurrentEnvironment(PVOID Environment, PVOID *PreviousEnvironment);
NTSTATUS NTAPI RtlQueryEnvironmentVariable_U(PVOID Environment, PUNICODE_STRING Name, PUNICODE_STRING Value);
NTSTATUS NTAPI RtlSetEnvironmentVariable(PVOID *Environment, PCUNICODE_STRING Name, PCUNICODE_STRING Value);
BOOL OpenNtRtlRefreshEnvironmentSnapshot(VOID);

#endif
