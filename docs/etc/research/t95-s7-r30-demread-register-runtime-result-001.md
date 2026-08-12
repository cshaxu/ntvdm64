# T95 S7 r30 DEM Read Register Runtime Result 001

## Question

Does the source-required AX/CF-only adapter result for reached `50:42`
`SVC_DEMFASTREAD` reach the native Bochs resume path, and does it remove the
retained post-close stack failure?

## Inputs and procedure

The fresh r30 root copied the admitted r24 closure with the sole adapter
change recorded in `t95-s7-demread-register-contract-001.md`.  MSVC/x86 built
`ntdos64-s7-runtime-trace.exe` once; its SHA-256 is
`DB041AB2F881784DC54844FF06E8FC7D48ED0C967B143A3409A7F2F4D347FB1C`.
`Invoke-T95S7RuntimeTraceObservation.ps1` then executed it once with the same
contained v2 profile, source-built guest inputs, ROMs and 15-second watchdog.

## Observation

The retained observation JSON reports 39 BOP identities and 10 committed
transactions before watchdog termination.  At the terminal contained read,
the copied input has `AX=4005`, `BP=4E53`, `CX=0020`; the zero-byte EOF result
logs an accepted resume with `delta=01`, preserving BP and every other GPR.
The following `50:02` close remains `delta=00` and resumes at `61A7`.

After that ordinary continuation, Bochs again reports three real-mode
`stackPrefetch(2)` accesses at `0xFFFF`, then the existing third exception
terminal state with `SS=00A7`, `ESP=0003`, and exception-vector `CS:EIP`
`0000:019D`.  No BOP identity occurs between the accepted close and this
native exception escalation.

## Interpretation

The source-derived read ABI correction is now proved in the source-built
Bochs path; it was necessary but is **not** the root cause of the later stack
underflow.  The evidence continues to classify the remaining issue as
ordinary guest control flow or a prior state writer.  It does not prove a
missing BOP, device, CPU semantic, FPU, PIC, BIOS or host capability.

## Follow-up

Do not add a BOP endpoint or a Bochs feature from this terminal state.  The
next investigation must correlate the normal NTDOS return path after `61A7`
with the source-built image and determine the first source-owned writer of the
eventual user-stack state using an existing same-configuration observation
facility or a separately proven closure.  The rejected r27/r28 CPU-loop
instrumentation remains unavailable.
