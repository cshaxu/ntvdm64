# T225 S7 Custom Cache Retirement

## Decision

The P1 custom object-cache executor has been retired. The implementation files
removed are exactly:

- `tools/build/Invoke-ModuleObjectCache.ps1`;
- `tools/probe/Invoke-T225S7IncrementalCacheProbe.ps1`.

They are replaced by the manifest-bound full Ninja graph recorded in
[t225-s7-full-ninja-module-graph-result-001.md](t225-s7-full-ninja-module-graph-result-001.md).

## Preservation

No historical P1/P2 research record was deleted. Earlier commands and results
remain indexed evidence of the transition, not executable build recipes. New
builds must be generated with `tools/build/New-T225S7FullNinjaGraph.ps1` and
run through Ninja in a disposable `build/` root.

## Scope

This deletes no guest artifact, Bochs/OpenNT source, fixture, profile,
prebuilt output, or documentation evidence. It adds no runtime or BOP
semantics.