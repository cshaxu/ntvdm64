# M0 T391 S6 — Native-Child Profile Admission

## Question

Why is an app-declared initial native executable not valid evidence for original
COMMAND `54:07` (`cmdCheckBinary`) and `54:08` (`cmdExec`), despite those
original host bodies being formally linked?

## Inputs

- Current app declaration: `src/app/launch_declaration.c`.
- Original COMMAND PIF and binary-classification bodies:
  `mvdm-host/dos/command/cmdmisc.c`, `cmdpif.c`, and `cmdexec.c`.
- Original guest EXEC decision:
  `mvdm-guest/dos/v86/doskrnl/dos/msproc.asm`.
- Current fixed-root product observations, including the pure-DOS `MEM.EXE`
  and `COMMAND.COM` runs retained in `O:\ntvdm64`.

## Source chain

1. The app publishes a BaseVDM **DOS record**: its initial target resides in
   the original `AppName`/`CmdLine` carriers for PermCom to execute.
2. Original `cmdGetNextCmd` unconditionally writes `SCS_Is_Dos_Binary = 1`
   after it receives that record (`cmdmisc.c`).  The byte-exact upstream
   `cmdmisc.c` has the same assignment and comment: BaseVDM has already said
   this is a DOS executable, so NTDOS need not classify it again.
3. Original NTDOS `$Exec` consumes that flag before its unknown-type branch.
   A nonzero value bypasses `SVC_CMDCHECKBINARY`; therefore an app-declared
   initial target, whether it happens to name `MEM.EXE`, `COMMAND.COM`, or a
   host executable, cannot prove the guest-originated native-child path.
4. The original path under test instead begins when an already running DOS
   program invokes `INT 21h EXEC` for an unclassified target.  NTDOS then
   evaluates its SCS policy and may call `54:07`; only a non-DOS result can
   subsequently reach unchanged `54:08 cmdExec -> cmdExec32`.

## Observation

The fixed-root pure-DOS runs prove guest DOS startup and Console operation, but
their BOP reports do not contain `54:07` or `54:08`.  The direct reason is the
original initial-record `SCS_Is_Dos_Binary=1` contract.  The PIF's
`CREATE_FORCEDOS`/`SUBSYS_DOS` settings are a separate potential policy input;
this evidence does not yet claim that they disable later guest-originated
classification.

## Decision

`pure-dos.pif` remains the correct profile for the proven pure-DOS workloads
such as `MEM.EXE` and the guest `COMMAND.COM` Console path.  No second profile
is admitted by this evidence.

S6 must next drive a **guest-originated** unclassified `INT 21h EXEC` through
the original Console/COMMAND route, then observe whether the existing PIF and
SCS state permits `54:07`.  It may not alter guest `COMMAND.COM`, force a BOP,
parse/launch the child in app, or reinterpret an initial BaseVDM record as
native execution.  Only if that source-shaped run proves a forced-DOS policy
rejection may a separate PIF/profile decision be admitted.

## Confidence and follow-up

High confidence in the initial-record exclusion: the relevant assignment is
byte-identical OpenNT source and NTDOS checks it before classification.
Runtime proof of a guest-originated trigger remains open.  The next S6
delivery must validate the actual PIF/SCS state at that trigger and require
reached `54:07` before any `54:08`/worker-result test.
