# M0 T310 S8 P4 NT hard-error boundary disposition

## Scope

This review covers the two current C4311 diagnostics in
`mvdm-host/softpc.new/host/src/nt_error.c`, both in the original
`WOWpSysErrorBox` path.

## Source contract

The NT4 code converts ANSI messages to two stack `UNICODE_STRING` values,
stores their addresses in a four-`ULONG` parameter array, then invokes
`NtRaiseHardError`. That array layout is not a generic message-box interface:
it is an NT kernel/CSRSS hard-error marshalling contract whose 32-bit pointer
representation was part of the original product environment.

Modern public Win32 provides no equivalent caller-owned `ULONG` parameter
transport to the hard-error broker. Widening the entries would invent a new
private ABI and still could not make the old service available.

## Disposition

The two diagnostics are classified
`kernel-csrss-hard-boundary-not-host-runtime`. The original code remains
mirror evidence and diagnostics remain compiler-visible. A future single-
session public UI/controlled-stop route, if admitted, belongs to its own
adapter/session owner; it must not masquerade as `NtRaiseHardError` or alter
this historical source contract.
