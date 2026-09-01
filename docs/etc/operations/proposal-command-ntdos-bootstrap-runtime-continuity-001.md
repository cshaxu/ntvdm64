# COMMAND/NTDOS bootstrap runtime continuity

## Purpose

Recover the complete original runtime owner package between the observed
COMMAND `SVC_CMDSETINFO` ingress and the first NTDOS bootstrap continuation.
This is not a `54:05` leaf-provider task. Its source span is the original
COMMAND dispatcher and scalar registration, the session mapping lease used by
that original body, the CPU40/SAS return path, and the NTDOS `msinit.asm`
consumer that performs the SCS/BIOS exchange and stack transition.

T350 established that this is a single coherent source cohort and that the
formal product links it. Its sole fixed observation reached original `54:05`
before its bounded timeout, so a graphics workload is not yet a reachable
integration target.

## Ordered subtasks

1. **S1 — original owner/ABI/failure admission.** Trace the complete original
   `MS_bop_4 -> CmdDispatch -> cmdSetInfo -> CPU40/SAS -> msinit.asm` chain,
   every mapping lease and session binding, and every original return/failure
   path. Compare the selected mirror/adapter code to the original source and
   name one complete recovery cohort.
2. **S2 — source-shaped bootstrap binding recovery.** Recover only the
   selected original bodies and their smallest same-shaped bindings. A
   divergence may correct a proven ABI/lifetime boundary, but no BOP-specific
   result, synthetic DOS state, guest rewrite or secondary mapper is allowed.
3. **S3 — local contract and formal closure.** Exercise the complete selected
   contract through focused owner tests and one fresh CPU40/Win32/x86 formal
   product link. Preserve original warnings; do not suppress them.
4. **S4 — one fixed-container observation.** Stage the formal product with
   unchanged original media and run exactly one non-debug console-owning
   observation. Classify the next source-defined boundary without retrying or
   selecting a leaf repair from the trace.

## Exclusions

No graphics workload, presentation controller change, COMMAND child/PSP
lifecycle, guest source rebuild, loader replacement, virtual media, new
mapping manager, CPU30, Bochs, x64 runtime, host installation, kernel VDM or
CSRSS/BaseSrv recreation.

## Completion

The original COMMAND-to-NTDOS bootstrap package is source, binding and formal
closed, and one fixed observation is accurately classified. Only after that
may the graphics-workload package attempt a selected DOS workload.
