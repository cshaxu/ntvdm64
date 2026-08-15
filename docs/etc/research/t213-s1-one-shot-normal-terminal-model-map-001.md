# T213 S1: One-Shot Normal-Terminal Model Map

Packet: M0 T213 S1, Ordinary Mode  
Date: 2026-08-15  
Status: source/ABI/failure mapping; no behavior change

## Question

Does the selected one-shot OpenNT DOS profile possess a source-backed path to
ordinary host completion, or is its reached final BOP correctly a controlled
DEM terminal?  What owner must be recovered before a future CLI can publish a
normal completion?

## Source path

| Step | Original/current owner | Fact and disposition |
| --- | --- | --- |
| COMMAND return | OpenNT `cmdsvc.h`, `cmddisp.c`; current COMMAND lifecycle provider | `54:0B` is `cmdReturnExitCode`. T206 preserves its final-plan copied record only after an accepted final return; it is not a Bochs stop or CLI result. |
| COMMAND exit | OpenNT `SVC_CMDEXITVDM` / `cmdExitVDM` | The historical command-exit path is separate from `54:0B`; the original host implementation ultimately delegates termination to `TerminateVDM`. |
| DOS/COMMAND fatal/root exit | OpenNT `command/init.asm` and `sysinit1.asm` | Both emit `SVC_DEMEXITVDM` (`50:3D`); the latter source comment says it kills the VDM.  The selector is therefore a host lifecycle request, not a DOS service return. |
| Historical host end | OpenNT `v86/scaffold/i386/softpc.c` | `TerminateVDM` calls historical `ExitVDM(...)` and then `ExitProcess(0)`.  This is a historical product-shell teardown, not an in-process modern engine contract. |
| Current profile end | `bx_ntvdm_dem_cli_unavailable_provider_v1.c` | `50:3D` produces a typed controlled stop.  T207 observed this exact DEM stop before any accepted final `54:0B` resume. |
| Engine/CLI publication | mantle engine and `ntdos64_lifecycle_v1` | Engine maps controlled stop to `CONTROLLED_GUEST_TERMINAL`; ordinary completion is valid in the ABI/classifier but has no producer. |

## Conclusion

For the current one-shot profile, **ordinary completion is not presently
admissible**.  The observed `50:3D` is source-consistent with historical VDM
termination and is currently represented honestly as a controlled terminal.
The absence of a later `54:0B` is not a missing leaf BOP to patch; it follows
the distinct historical host teardown path.

A future ordinary completion needs a complete, source-derived **one-shot host
terminal package**, not a trace-specific handler.  It must define: the
authenticated terminal condition, COMMAND/DEM result precedence, copied exit
record ownership, engine cleanup-before-publication order, and CLI exit
presentation.  It may not make `50:3D` resume, call `ExitProcess`, or pass a
Windows handle into engine/mantle.

## Win32 availability disposition

`ExitProcess` exists to a normal user-mode process, but is rejected: the
engine shares the CLI process and must return a typed result after cleanup.
The historical `ExitVDM`/BaseSrv/CSR broker is not an available ordinary
Win32 CLI API and cannot be a runtime dependency.  This is an architecture
and composition boundary, not a missing modern Win32 API that blocks the
audit.

## Next owner package

T213 S2 may map the complete historical COMMAND/DEM terminal state and design
one copied host-terminal record.  It must include source/failure evidence for
both `54:0B` and `50:3D` and may not implement a provider until that package
map proves an unambiguous terminal condition.
