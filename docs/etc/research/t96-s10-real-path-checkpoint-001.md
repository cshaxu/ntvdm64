# T96 S10 Current BOP-Plane Real-Path Checkpoint

## Result

T96's architecture objective has its required runtime checkpoint: the current native runtime binary
contains the common BOP ingress, provider registry, all six T96 service-plane
classifiers, and the DEM/COMMAND legacy gate. A real NTIO/NTDOS path reaches
that composition. It does not establish continuous CLI execution.

The first unresolved runtime boundary is `BOP_DOS:SVC_DEMFASTREAD` (`50:42`).
OpenNT's own `DemDispatch` table routes that slot to `demNotYetImplemented`.
The current adapter correctly gives it no synthetic result. The guest remains
at `9346:6351` and repeatedly re-enters the generic `#UD` listener; the
15-second watchdog ends the observation. This is an explicit, source-backed
unavailable boundary, not a Bochs failure and not evidence to add a
trace-specific handler.

## Inputs And Procedure

- Generator: `tools/build/New-T95S7RuntimeTraceBuildRoot.ps1`, with the new `r49`
  root. Its closure list now includes the T96 ingress, registry, DEM,
  COMMAND, XMS/DPMI, system, and legacy-gate objects.
- Build: MSVC BuildTools x86, `nmake /f ntdos64-s7-runtime-trace.mak
  ntdos64-s7-runtime-trace.exe` in
  `artifacts/build/bochs-2.6-native-s7-native-post-handoff-msvc-r49`.
  It linked successfully; binary SHA-256:
  `D3332D7CDD5DC2BB395E0D7A6DB372DC52F13C2F4C891018BBF99D324A8CD5E6`.
- Observation: `tools/probe/Invoke-T95S7RuntimeTraceObservation.ps1` with the r49
  build root, deferred startup plan, `-BenchmarkMillions 1`, and
  `-WatchdogSeconds 15`. The immutable record and full log are retained in
  `artifacts/analysis/t96-s10-current-bop-plane-runtime-001/`.

No Bochs source, device selection, firmware, guest asset, or BOP provider was
changed for this checkpoint.

## Observed Path And Classification

The retained `observation.json` records 251,966 observed BOPs, three accepted
transactions, watchdog termination, and exit code 1. The early path includes
`50:11`, `50:3B`, `50:0F`, `50:1B`, `50:32`, `54:05`, `50:46`, `50:0D`,
`50:21`, `54:0C`, `50:12`, and `50:00`. These identities pass through the
current generic listener and retain their recorded plane dispositions.

The terminal repeated identity is `50:42` at `CS:EIP 9346:6351`, with the
same captured guest context on successive records. It is therefore not an
accidental selector collision or a missing compilation object.

`src/opennt/base/mvdm/inc/dossvc.h` defines `SVC_DEMFASTREAD` as `0x42`.
`src/opennt/base/mvdm/dos/dem/demdisp.c` places it in the original `apfnSVC`
table as `demNotYetImplemented` (line 165). S3 recorded that disposition; S8
removed the former adapter FASTREAD substitute. The source and runtime
evidence agree.

## Consequence

The next package is the already queued read-only DOS namespace/file/search
continuation. Its admission must establish the original service and guest ABI
that can legitimately replace the unavailable FASTREAD edge. It must not
revive the deleted ad-hoc helper, modify Bochs, or claim that an observation
loop is guest progress.
