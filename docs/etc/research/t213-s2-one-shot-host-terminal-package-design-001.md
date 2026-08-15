# T213 S2: One-Shot Host-Terminal Package Design

Packet: M0 T213 S2, Ordinary Mode  
Date: 2026-08-15  
Status: source/ABI/failure design; no provider implementation

## Design question

Can one copied host-terminal record distinguish a normal one-shot DOS exit
from fatal VDM teardown while preserving OpenNT's COMMAND/DEM ownership and
without moving source identity into Bochs?

## Required historical states

| State | Original source evidence | Current route | Safe current result |
| --- | --- | --- | --- |
| Final child return | COMMAND `54:0B` / `cmdReturnExitCode` | copied COMMAND record only after declared-plan exhaustion | package-local fact |
| Top-level COMMAND exit | `tcmd2b.asm` emits `CMDEXITVDM`, marked “Never returns” | COMMAND lifecycle typed stop | controlled terminal |
| COMMAND initialization failure | `command/init.asm` emits `DEMEXITVDM` when COMMAND is its own parent after version failure | DEM typed stop | controlled terminal |
| DOS boot/COMMAND launch error | `sysinit1.asm` `stall` emits `DEMEXITVDM`, explicitly “Will Kill The VDM” | DEM typed stop | controlled terminal |
| Historical host teardown | `TerminateVDM` → `ExitVDM` → `ExitProcess` | absent by design | no in-process equivalent |

The same DEM selector (`50:3D`) represents at least a root COMMAND failure
and a DOS initialization failure; the current admitted instruction window
contains selector/service but not a source-proven caller identity or a
historical host broker result.  Therefore selector, register contents, or
trace order alone cannot authoritatively classify it as normal.

## Candidate record and admission decision

A later provider would need a copied record with at least:

```text
terminal origin (COMMAND exit / DEM exit)
authenticated terminal class (normal / controlled failure)
optional DOS exit code + explicit validity bit
preceding final COMMAND-return observation + explicit validity bit
provider-owned reason/error code
```

The present composition can fill only the origin and controlled-stop fact.
It cannot authenticate the terminal class or a DOS exit code for DEM exit.
The existing `54:0B` record is insufficient because it is absent in the
observed `50:3D` path and because its presence would not prove that a later
DEM exit was normal rather than an independent failure.

Consequently **no host-terminal record implementation is admitted for the
current profile**.  The required package must first recover a source-backed
terminal provenance mechanism through the original COMMAND/DEM lifecycle, or
select a distinct profile whose normal terminal contract is independently
observable.  Bochs remains selector-blind; bx-vdm may retain its typed stop
and observers but may not infer provenance from the next trace event.

## Ownership and API disposition

- COMMAND retains its final-return record and `cmdExitVDM` ownership.
- DEM retains `DEMEXITVDM` lifecycle/failure ownership.
- Engine retains reset and only publishes terminal data after cleanup.
- CLI retains host exit presentation.
- `ExitProcess` is available but forbidden because it bypasses engine cleanup;
  `ExitVDM` and the BaseSrv/CSR broker are unavailable historical composition,
  recorded as an external dependency rather than a missing normal Win32 API.

## Follow-up

T213 closes with controlled termination retained.  A future terminal-recovery
task requires a whole-source provenance contract before it can modify engine
or CLI terminal results.  Interactive COMMAND remains gated by a separately
proven normal one-shot profile; it is not unblocked by this design.
