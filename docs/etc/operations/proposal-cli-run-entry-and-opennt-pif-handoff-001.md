# OpenNT COMMAND Launch, PIF And Child-Lifecycle Recovery Proposal

## Decision Requested

Admit one future owner package that restores the public CLI entry contract and
the complete reachable OpenNT COMMAND launch plane:

```text
ntdos64-run [global-options] <file> [args...]
```

The CLI owns only parsing, immutable launch-input admission, and engine
handoff.  It must not parse PIF contents, interpret BAT syntax, emulate DOS,
or select a guest execution policy.  Those behaviors remain with the imported
OpenNT COMMAND owner.  The same package recovers the original COMMAND PIF
startup selection and the complete child-process/redirection lifecycle needed
when the resolved target leaves DOS execution.

This is an unnumbered candidate until selected and admitted by `STATUS.md`.

## Current Evidence

`src/cli/ntdos64_run.c` currently treats the first non-option as the target;
there is no `run` verb.  Its image classifier accepts DOS MZ executables and
`.com`, but leaves `.bat` and `.pif` unknown.  Its BYOB target admission and
`byob_launch_plan_v2` additionally accept only declared `TARGET.COM` or
`TARGET.EXE`; that fixed plan cannot carry a PIF initial input.

The directly imported OpenNT COMMAND path is the preferred recovery route:

- `base/mvdm/dos/command/cmdmisc.c:cmdGetNextCmd` receives an initial
  `VDMINFO` through `GetNextVDMCommand` and calls `cmdCheckForPIF` before
  binary handling.
- `base/mvdm/dos/command/cmdpif.c:cmdCheckForPIF` identifies `.pif`, invokes
  the original `nt_pif.c` parser, and updates the same `VDMINFO` with target,
  command tail, current directory, title, and DOS-binary disposition.
- The same source validates that a PIF-resolved target is `.EXE`, `.COM`, or
  `.BAT`; BAT therefore belongs to the original COMMAND input path rather
  than a CLI batch interpreter.

The current v2 COMMAND mirror already compiles the original `cmdGetNextCmd`,
`cmdCheckForPIF`, and `nt_pif.c` parser.  Its PIF fixture verifies title,
tail, start directory, target, and close-on-exit extraction.  The mirror also
retains the original `cmdconf.c`, `cmdexec.c`, `cmdredir.c`, `cmdmisc.c`, and
their COMMAND service table, including known guarded or substituted historical
host-composition bodies.  This candidate closes those related launch-plane
gaps as owner packages, rather than leaving them as unrelated adapter work.

## Source-Recovery Ledger

| Rung | Disposition |
| --- | --- |
| Original source reuse | Reuse OpenNT `cmdGetNextCmd` and `cmdCheckForPIF` unchanged in ownership and ordering. |
| Smallest seam | Replace only historical BaseSrv `GetNextVDMCommand` input with a versioned session-owned copied initial-command record; use existing bounded guest-memory/CPU seams. |
| External intrusion | Rejected: no Bochs or OpenNT external-source intrusion is required. |
| New behavior | The existing modern CLI option parser and target-path admission are project-owned input mechanics. They carry no PIF, BAT, DOS, or COMMAND semantic. |

## Proposed Sequential Deliveries After Admission

### P1 — Target Grammar And Format Admission

Retain the target-first grammar: the first non-option argument is the target
path. Classify `.com`, DOS MZ `.exe`, `.bat`, and `.pif` as COMMAND-initial-
input candidates. Preserve current PE32/PE64 direct host launch and NE
explicit WOW-unavailable behavior.

**Acceptance:** option parsing cannot consume or reinterpret the first target;
malformed invocations fail deterministically; the format matrix has focused
CLI tests.

### P2 — Versioned Initial-COMMAND Input Contract

Replace the COM/EXE-only launch-plan restriction with a versioned,
session-owned copied record that carries the admitted initial pathname, tail,
and input kind through the engine to `GetNextVDMCommand`.  Do not use raw host
pointers or let the CLI write guest memory.

**Acceptance:** the engine receives a bounded copied pathname and tail; legacy
COM/EXE profile launches retain an explicit compatibility path or a migrated,
tested equivalent.

### P3 — COM, DOS EXE And BAT Original COMMAND Handoff

Feed those three forms into `VDMINFO.AppName` and `VDMINFO.CmdLine` through the
existing session provider.  Do not add a CLI COM loader, EXE loader, or BAT
parser.

**Acceptance:** each form reaches the original `cmdGetNextCmd` input boundary
with the expected copied path/tail and source-shaped failure behavior.

### P4 — PIF Original-Owner Handoff

Feed the PIF pathname unchanged into the same input provider.  Let the already
imported `cmdCheckForPIF` and `nt_pif.c` produce final target, tail and start
directory.  Invalid PIF data remains on its original COMMAND failure path.

**Acceptance:** one end-to-end fixture proves CLI input -> copied initial
`VDMINFO` -> original PIF parser -> final COM/EXE/BAT COMMAND input, including
CLI-tail precedence and PIF-tail fallback.

### P5 — PIF CONFIG/AUTOEXEC Startup-Lifecycle Recovery

Before implementation, source-map the original PIF/BASE timing: identify how
the historical `GetPIFConfigFiles` capability obtains PIF-selected CONFIG and
AUTOEXEC inputs before `54:0C`/`54:0D`, and how that ordering relates to the
later `cmdGetNextCmd -> cmdCheckForPIF` call.  Then reuse `cmdconf.c` and the
original PIF data/provider path through the smallest session-owned seam.

**Acceptance:** one PIF-selected CONFIG/AUTOEXEC pair is chosen before its
respective original COMMAND service executes; ordinary CLI-configured inputs
retain their original precedence; no environment-variable shortcut or CLI PIF
parser substitutes for the original owner order.

### P6 — Complete COMMAND Child And Redirection Lifecycle

Recover `54:06`, `54:08`, `54:0A`, and `54:0B` as one component package:
standard-handle acquisition, pipe/temporary stream ownership, child launch,
VDM re-entry, exit-code return, redirection completion, and cleanup.  The
currently excluded OpenNT `cmdCreateProcess`/`cmdExec32` worker composition
must be assessed against the source-recovery ladder as one unit; a leaf
`CreateProcess` implementation is not closure.

**Acceptance:** Direct child launch and return preserve the original service
ordering and source-shaped failures; no host handle enters guest state; pipe
and non-pipe paths have whole-lifecycle regression; child completion cannot
leave session-owned streams or re-entry state behind.

### P7 — PIF Resolved-Target Classification

Use the original `cmdCheckForPIF` and `cmdCheckBinary` ownership to classify a
PIF-resolved target.  COM, DOS MZ EXE, and BAT remain guest COMMAND inputs.
A PE target proceeds only through the completed COMMAND child lifecycle;
until then it receives an explicit source-shaped unavailable/failure result,
never a hidden CLI `CreateProcess` bypass.

**Acceptance:** one PIF fixture covers a DOS target and one PE-target
disposition; the observed owner is COMMAND in both cases.

### P8 — WOW And Diagnostic Boundary Disposition

Keep the original `GetWowKernelCmdLine` and `GetWOWShortCutInfo` outside this
non-WOW launch profile and record their transfer to the WOW16 owner package.
Classify `54:03 cmdSaveWorld`, whose historical body is itself protected by
`CHECK_IT_LATER`, as a retained diagnostic no-op unless a separate owner need
is admitted.

**Acceptance:** `VDMForWOW` and NE inputs produce an explicit WOW-unavailable
outcome without terminating through an accidental shim path; `cmdSaveWorld`
has a documented intentional disposition rather than an unlabelled no-op.

### P9 — Boundary Matrix And Migration Markers

Exercise all admitted entry formats and explicitly mark non-OpenNT routes:

- PE32/PE64 remain the retained project CLI `CreateProcessW` host-launch
  feature only when invoked as a direct CLI host launch.  The COMMAND/PIF
  path uses P6/P7 and receives no CLI bypass.  Mark this distinction at the
  entry boundary.
- NE remains explicit WOW16 unavailable and transfers through P8.
- PIF CONFIG/AUTOEXEC selection is verified through P5, never implemented in
  the CLI.

**Acceptance:** no format silently changes owner; every unsupported form has a
named diagnostic; the source-recovery ledger, focused tests, formal build and
governance checks pass.

## Non-Goals

- Implementing WOW16/NE execution itself; this package only establishes its
  explicit transfer/unavailable boundary.
- Replacing OpenNT COMMAND binary classification, batch semantics, or PIF
  parsing with CLI code.
- Changing Bochs, bx-mantle, BOP routing, guest memory, or host mutation
  policy.

## Risks And Stop Conditions

Pause and re-admit if the initial-input transport cannot carry the original
pathname/tail without exposing a host pointer, if extending BYOB identity
admission weakens the declared-component integrity rule, or if a PIF-resolved
target requires CLI interpretation before `cmdCheckForPIF` runs.  In those
cases, preserve the original COMMAND owner and design a narrower typed input
record rather than adding a CLI fallback.
