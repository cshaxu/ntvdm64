# M0 T364 — COMMAND command delivery and built-in dispatch

## Purpose

T363's one fixed CPU40 workload passed the original conventional-device
cohort, completed the first original `SVC_CMDGETNEXTCMD` (`54:01`) return, and
then stopped at a later original `54:01` continuation. This task recovers the
complete original COMMAND command-delivery and built-in-dispatch owner package
before attempting DOS EXEC or native-child execution.

## Source-first boundary

The primary source bodies are the mirrored original OpenNT COMMAND package:

- `mvdm-host/dos/command/{cmddisp.c,cmdmisc.c,cmddata.c,cmd.c,cmdexec.c}`;
- original BaseClient-shaped `GetNextVDMCommand` and `VDMINFO` interface;
- the existing `app` launch declaration and session-owned base-VDM provider;
- required original DEM/DOS continuations only where the COMMAND body reaches
  them.

The task retains the original `CMDINFO`, `VDMINFO`, `IsFirstCall`,
`IsRepeatCall`, error, environment and current-directory ordering. A modern
replacement is allowed only at an already-audited historical BaseSrv/CSR
transport boundary, and only through the existing named adapter/session
contract. No command parser, BOP result, or DOS child lifecycle may be
invented in an adapter.

## S plan

1. **S1 — Original command-delivery owner ledger.** Trace all first/repeat
   `cmdGetNextCmd` branches, `VDMINFO` state transitions, app declaration
   binding, BaseClient capture/copy behavior, built-in handoff and source
   failure exits. Classify the exact second-`54:01` predecessor without a
   product change or another runtime observation.
2. **S2 — Bounded original command-source recovery.** Recover the whole
   source-selected command queue/declaration/built-in cohort. Preserve original
   `CMDINFO`/`VDMINFO` layout and failure direction; use the shared mapping
   manager only at original guest-address seams. Focused checks cover first,
   repeat, absent/consumed command and source-defined error outcomes.
3. **S3 — Formal COMMAND delivery closure.** Link the selected original
   COMMAND, BaseClient-shaped adapter, app and session units into the CPU40/x86
   product. Verify no old self-authored command path remains selected.
4. **S4 — One frozen command-delivery observation.** Run the fixed package
   once. It may prove command/built-in continuation or transfer exactly one
   earlier original owner. It may not create a service-by-service repair list.

## Exclusions

No guest media mutation, virtual command source, DOS `.COM`/MZ `EXEC`,
cmdExec32/native child, BaseSrv/CSRSS recreation, CPU/device change, CPU30,
Bochs, x64 runtime claim, WOW, Redirector, physical printer behavior or
observation retry enters this task.

## Completion

T364 closes only after the complete original COMMAND delivery cohort has a
source/disposition ledger, source-shaped recovery or exact terminal,
formal CPU40/x86 link proof, and one frozen workload result. A first `54:01`
return alone is not command-delivery closure.
