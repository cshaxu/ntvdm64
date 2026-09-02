# M0 T359 — CPU40 COMMAND-initialization control-flow recovery

## Purpose

Recover the exact selected CPU40 runtime control-transfer cohort that the
fixed T358 observation proved reaches immutable `COMMAND.COM` help text at
`03F4:2171` before original `54:01`.  Static source mapping establishes that
the CPU transfer files are selected and unchanged; the owner is the original
COMMAND resident-to-transient lifecycle and its NTDOS prerequisites.  This is
a predecessor of T358, not a COMMAND BOP implementation task.

## Sequence

1. **S1 — source and image control-transfer map.** Map original CCPU40
   instruction/interrupt/return handling, SAS image layout, `.COM` loading
   convention and the reached `illegal_op_int` path.  Classify the first bad
   transfer from original source rather than from the timeout.
2. **S2 — complete original COMMAND transient lifecycle recovery.** Recover
   as one owner package `EndInit`, `LodCom/LodCom1`, normal DOS allocation and
   `50:12/00/16/02` file lifecycle bindings, checksum/reload, `TranVars`,
   `HeadCall` and `TJmp`.  Restore only source-shaped same-interface bindings;
   do not alter CPU transfer code, COMMAND bytes, DEM results or command input.
3. **S3 — local conformance and formal link.** Exercise the repaired
   allocation/file/transient handoff contract locally and formally link the
   affected original owner cohort into the selected product.
4. **S4 — frozen resumed observation.** Use one new capacity-valid immutable
   stage with the same `--command EXIT` workload.  Require progress through
   original `54:01`, or record the next exact source-owned boundary.
   The capacity check is against the complete generated `shell=` value, not
   merely the `mvdm` root. A stage that violates the original 63-byte `commnd` input contract is
   invalid evidence and must be closed without interpretation; one separately
   admitted replacement S may use an already hash-verified short-root
   format-3 media container, replacing its explicitly mutable product only.

## Exclusions

No COMMAND provider rewrite, BOP leaf implementation, DOS EXEC, batch file,
guest/firmware mutation, CPU30, Bochs, x64 runtime, CSRSS/kernel VDM, WOW,
Redirector, graphics or broad CPU rewrite.

## Completion

T359 closes only when the selected original COMMAND/NTDOS cohort no longer
transfers into immutable COMMAND data and the resumed fixed observation reaches
the original `54:01` boundary (or proves a narrower source-owned predecessor).
T358 then resumes from its unchanged `/C EXIT` workload contract.
