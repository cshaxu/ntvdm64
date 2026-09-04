# DOSX/DPMI/WOW bootstrap activation

## Purpose

Turn the already reached original `AUTOEXEC.NT -> DOSX.EXE` path into a proved
runtime capability. This is deliberately separate from the pure-DOS
first-shell profile and COMMAND `cmdExec32`: this original DOSX route reads
`SYSTEM.INI` and discovers `KRNL?86.EXE`, so it is a combined DPMI/WOW
bootstrap rather than a generic pure-DOS extender route.

## Scope

Recover the original-source path from the resident first shell through
`DOSX.EXE`, DPMI initialization, required `SYSTEM.INI`/`KRNL?86.EXE` discovery
and the first original WOW bootstrap boundary. Preserve original MVDM/DOSX/
DPMI/WOW owners; use existing adapters only for same-shaped public-Win32 or
machine bindings.

## Work sequence

### S1 — Reached DOSX dependency closure

Trace the reached `DOSX.EXE` startup path through its actual MVDM, DPMI, WOW
and host-file dependencies. Freeze the required stage media, configuration,
`SYSTEM.INI`/`KRNL?86.EXE` identities and the first selected bootstrap point.
Classify each missing symbol or asset by original owner; do not patch a
trace-selected BOP leaf.

### S2 — Original-owner binding recovery

Bring the complete immediate DPMI/DOSX/WOW bootstrap cohort into the formal
CPU40/x86 graph. Prefer original source bodies and existing same-shaped adapter
bindings; record any non-composable private NT boundary as an explicit failure
rather than inventing a parallel bootstrap.

### S3 — Formal link and focused lifecycle proof

Build the selected product and prove original `DOSX.EXE` startup, dependency
resolution, DPMI initialization and the selected WOW bootstrap entry.

### S4 — Fixed-container DOSX workload observation

In the stable console-owning runtime container, run the frozen bootstrap
profile. Retain product/media identities and prove its observable transition
to the next original owner boundary. A source-owned failure is a valid result
only when it identifies that whole owner cohort.

## Exclusions

No guest `COMMAND.COM` rewrite, no app-owned DOS parser, no synthetic DPMI,
no CPU workaround, no complete Win16/WOW runtime, and no native `cmdExec32`
child recovery. Those remain distinct owner packages.
