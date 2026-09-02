# M0 T370 — Original COMMAND native-child lifecycle

## Purpose

Recover and prove the smallest complete original OpenNT COMMAND owner package
for the native-child path: `54:08`, `54:0A`, and `54:0B`, including
`cmdExec32`, `cmdCreateProcess`, `cmdGetNextCmd`, and `cmdReturnExitCode`.
This package follows the closed original DOS EXEC/parent-return packet. It
does not treat a runtime trace as a source of individual BOP work.

## Source owner and boundary

The selected source owner is the mirrored OpenNT COMMAND package beneath
`src/mvdm-host/dos/command`, in particular `cmdexec.c`, `cmdmisc.c`,
`cmddisp.c`, `cmdenv.c`, and their original declarations. Its historical
dependencies divide into three bounded seams:

- Base VDM command/re-entry accounting (`GetNextVDMCommand`, `VDMINFO`) is
  provided through the existing source-shaped Base VDM binding.
- CCPU/SoftPC register, guest-address and event calls retain their historical
  spelling through `adapter-mvdm-host-out/softpc`, with checked synchronous
  mapping leases where an original `GetVDMAddr` call is reached.
- Public process/thread/console primitives use the bounded Win32 adapter. A
  per-child `CreateProcess` setup may not mutate the host process's standard
  handles, retain a raw host handle in guest state, or recreate CSR/CSRSS.

Historical CSR/BaseSrv multi-VDM broker paths, WOW-specific broker paths,
Redirector pipe completion and DOS EXEC/PSP parent recovery are explicit
non-goals unless the original package proves one is an immediate requirement
of the admitted local native-child path.

## Subtasks

1. **S1 — Original owner topology and current binding audit.** Read every
   reached original caller, record source ordering, data/handle ownership,
   failure result, worker/re-entry transitions and the current adapter route.
   Select a complete earliest recovery cohort; do not edit production code.
2. **S2 — Source-shaped local child lifecycle recovery.** Recover the chosen
   cohort as a whole, retaining original COMMAND control flow and modernizing
   only unavailable host mechanics behind same-shaped adapters. Use the
   session mapping manager for any guest-memory operation and preserve failure
   behavior where a historical private dependency remains unavailable.
3. **S3 — Formal link and local lifecycle proof.** Rebuild the selected
   CPU40/x86 product and execute focused source-owner tests for launch,
   invalid inputs, completion, cancellation/cleanup, re-entry and standard
   handle isolation.
4. **S4 — One fixed-container native-child observation.** Only after S3,
   make one bounded non-debug observation using the fixed runtime container;
   classify the first source-owned native-child result without creating a
   trace-selected repair.

## Exit criteria

The task closes only when every reached `54:08/0A/0B` local native-child edge
has an original owner/disposition, the selected source-shaped cohort formally
links and passes focused proof, and one fixed-container observation is
classified. It may close with an explicit earlier original terminal or a
documented unavailable historical product-shell boundary; it may not claim
WOW, Redirector, multi-VDM CSR broker, or DOS child/PSP completion.
