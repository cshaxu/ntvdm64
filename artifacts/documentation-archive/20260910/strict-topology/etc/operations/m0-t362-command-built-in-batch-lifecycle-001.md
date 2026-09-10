# M0 T362 — Original COMMAND built-in and batch lifecycle

## Purpose

Prove a bounded original `COMMAND.COM /C EXIT` lifecycle after the valid
CPU40/x86 root-layout observation. T358 proved the command declaration and
Base VDM record delivery, but its final valid observation stopped after
original `54:05` and before original `54:01`; it did not prove built-in
execution or return. T362 reopens no old task: it owns the remaining complete
COMMAND initialization/built-in package.

## S plan

1. **S1 — original initialization owner map.** Reconstruct the entire
   `54:05` through first `54:01` path across guest COMMAND, MVDM COMMAND,
   NTDOS, Base VDM and CPU40/SAS. Classify every source-defined input, state
   transfer, selected build unit and failure branch before changing code.
2. **S2 — whole source-shaped cohort recovery.** If S1 finds a real missing
   immediate original cohort, reconnect that entire cohort through original
   source plus the smallest same-shaped adapter binding; locally test and
   formally link it. A BOP leaf or guest change is never a substitute.
3. **S3 — frozen built-in observation.** Stage a fresh hash-verified package
   at the approved short root and run exactly one console-owning `/C EXIT`
   observation. Require original command delivery followed by the original
   `SVC_CMDEXITVDM`/controlled-session-stop marker, or transfer one exact
   earlier owner.
4. **S4 — batch disposition and closure.** Prove whether one immutable batch
   remains inside the recovered COMMAND package. If it reaches `$Exec`, record
   that exact original transfer to the DOS execution package rather than
   claiming batch completion.

## Boundaries

No DOS child, MZ loader, `cmdExec32`, CreateProcess, CSRSS/BaseSrv transport,
WOW, CPU/device semantics, guest-media rewrite, CPU30, Bochs or x64 runtime
work enters this package. All recovery remains source-first and preserves the
selected original mirror/adapter boundaries.

## Completion

T362 closes only with a source-owned `/C EXIT` terminal/return observation,
or with one exact predecessor transferred under the dependency-first rule.
It cannot claim DOS external program execution or batch execution that
requires original `$Exec`.
