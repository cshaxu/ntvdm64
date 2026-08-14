# T199 S37 DEM Native Integration Observation 001

## Purpose

This is the one bounded native integration observation admitted after the DEM
pre-trace ledger.  It validates the current composition object closure and
records reached behavior; it does not select future BOP implementation work.

## Build closure repair

The first r88 preparation exposed a stale fixture reference: GSET and
error/lock are now members of `composition.dem`, not direct composition
members.  The fixture was corrected accordingly.  r89/r90 then showed that
the old probe repeatedly invoked `VsDevCmd` for every translation unit and
timed out before link/run.  `Invoke-T198S94SourceBuiltNormalReturnNativeProbe.ps1`
now writes one batch that enters the x64 MSVC environment once and compiles
the same manifest sources sequentially.

r92/r93 reached link and exposed the actual current-manifest closure: the
checked `demCheckPath` provider, mantle A20/extended-memory capabilities, and
the current `bx-core/memory/misc_mem.cc` backing-RAM methods were not yet
members.  They were added without changing their selector-blind ownership or
any BOP semantics.

## r94 procedure and result

`artifacts/build/t199-s37-dem-integration-native-r94/` was built from the
frozen CPU5/mantle baseline plus the current composition manifest under MSVC
x64 `/MT`.  It contains exactly one guest execution, as recorded in
`t198-s97-post-bootstrap-native-probe.json` (`runs=1`).  The current object
compile, link and bounded run complete in about 52 seconds.

The fixture reports a controlled stop (process exit 2), not a DOS or CLI
completion.  Before that stop it records completed source-derived DEM
transactions including `50:11`, `50:3B`, DTA registration, hard-error
registration, drive state, and sixteen open/fast-read/close lifecycle
entries.  The first canonical BOP observation is `50:11`; no raw DEM
pass-through is reported.

The stopped next identity is `54:01`.  It belongs to the already planned
COMMAND package rather than DEM.  This observation therefore creates no
endpoint task and makes no claim about COMMAND completion, normal DOS return,
or continuous guest execution.

## Boundary

The result is integration evidence for the completed DEM package and current
composition-link closure only.  It does not authorize trace-led repair, new
Bochs devices, host filesystem access, or a change to any service provider
disposition.
