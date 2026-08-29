# M0 T310 S8 P4 — COMMAND RTL environment binding closure

## Source finding

The original COMMAND sources `cmdenv.c` and `cmdexec.c` preserve the NT4 RTL
names and call ordering, but the formal T310 graph omitted the already-owned
same-shaped RTL adapter. As a result, `RtlCreateEnvironment`,
`RtlSetEnvironmentVariable`, `RtlDestroyEnvironment`, and reached string RTL
calls compiled by implicit declaration.

## Disposition

The source-facing `ntrtl.h` true subset now carries the original three
environment declarations. `adapter-mvdm-host-out/win32` supplies host-private
mutable UTF-16 MULTI_SZ blocks through the same names: clone/empty creation,
ordered case-insensitive replacement/removal, and destruction. No COMMAND
mirror body changes, guest pointers, or mapping identities are introduced.

## Verification

The regenerated formal graph compiled the adapter and both original COMMAND
translation units on x86 and x64:

- `build/M0-T310/S8/p1-machine-source/x64/command-rtl-forced-x64.log`
- `build/M0-T310/S8/p1-machine-source/x86/command-rtl-forced-x86.log`

The former C4013 diagnostics are absent. Both formal forced-link closure DLLs
also exist without a duplicate-definition diagnostic:

- `build/M0-T310/S8/p1-machine-source/x64/command-rtl-forced-link-x64.log`
- `build/M0-T310/S8/p1-machine-source/x86/command-rtl-forced-link-x86.log`

This closes the source ABI/binding gap only. It does not claim completion of
the remaining COMMAND guest-pointer, handle, or child-lifecycle semantics.
