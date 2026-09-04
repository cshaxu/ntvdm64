# DOSX/DPMI runtime activation

## Purpose

Turn the already reached original `AUTOEXEC.NT -> DOSX.EXE` path into a proved
runtime capability. This is deliberately separate from the pure-DOS
first-shell profile and from COMMAND `cmdExec32`: `DOSX.EXE` supplies the DOS
DPMI/extender environment needed by DOS workloads, whereas `cmdExec32` is the
later COMMAND-to-native-child lifecycle.

## Scope

Recover the original-source path from the resident first shell through
`DOSX.EXE`, DPMI initialization, required `SYSTEM.INI`/`KRNL386.EXE` discovery
and the selected DOS-extender workload. Preserve original MVDM/DOSX/DPMI
owners; use existing adapters only for same-shaped public-Win32 or machine
bindings. The package must prove actual execution of one DOS application under
the DOSX profile, with its original exit/parent-return behavior.

## Work sequence

### S1 — Reached DOSX dependency closure

Trace the reached `DOSX.EXE` startup path through its actual MVDM, DPMI and
host-file dependencies. Freeze the required stage media, configuration,
`SYSTEM.INI`/`KRNL386.EXE` identities and the first selected workload. Classify
each missing symbol or asset by original owner; do not patch a trace-selected
BOP leaf.

### S2 — Original-owner binding recovery

Bring the complete immediate DPMI/DOSX owner cohort into the formal CPU40/x86
graph. Prefer original source bodies and existing same-shaped adapter bindings;
record any non-composable private NT boundary as an explicit failure rather
than inventing a parallel extender.

### S3 — Formal link and focused lifecycle proof

Build the selected product and prove original `DOSX.EXE` startup, dependency
resolution, DPMI initialization and the selected DOS application entry/exit.

### S4 — Fixed-container DOSX workload observation

In the stable console-owning runtime container, run the frozen workload under
the DOSX profile. Retain product/media identities and prove its observable
output plus original parent/session return. A source-owned failure is a valid
result only when it identifies the next whole owner cohort.

## Exclusions

No guest `COMMAND.COM` rewrite, no app-owned DOS parser, no synthetic DPMI,
no CPU workaround, no Win16/WOW completion, and no native `cmdExec32` child
recovery. Those remain distinct owner packages.
