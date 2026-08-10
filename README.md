# ntdos64-run

## Documentation Governance

Current policy and task state are organized under [docs/](docs/README.md). Start
with the [project goal](docs/design/GOAL.md), then read the
[architecture](docs/design/ARCHITECTURE.md), [source policy](docs/etc/operations/policy/source-policy.md),
and [current status](docs/STATUS.md). Existing research and build records are
retained through the [governance migration inventory](docs/etc/operations/governance-migration.md).

`ntdos64-run` is an owned, non-invasive Windows research CLI. Its default
build, runtime, and binary have no dependency on `ntvdm64`, Bochs, PCjs, or
any other external execution implementation. The active engineering objective
is to repair and compose the OpenNT SoftPC/CCPU path itself. Other codebases
may be read only as corroborating evidence for a caller-defined semantic;
they are never a runtime, link, build, or acceptance dependency.

## Commands

```text
ntdos64-run.exe target.exe [arguments...]
ntdos64-run.exe --engine C:\path\to\byob-engine.exe target.exe [arguments...]
```

The CLI reads only the executable headers. PE32 and PE32+ targets are started
through ordinary `CreateProcessW` host handoff. DOS MZ/COM and NE targets need
an explicitly supplied engine. The engine is invoked as:

```text
byob-engine.exe -- target.exe [arguments...]
```

The engine path is user-selected. The CLI never copies a runtime, edits the
registry, installs a driver, injects a process, changes a system directory, or
uses a Windows-private loader/console API.

## Bounded Instrumentation (Opt-In)

The former self-authored `nvtdm` startup, BOP, DEM, IVT/BDA, and DOS probes are
not a runtime and are not part of the default build or CLI path. They are built
only as source references when configuring with
`-DNTDOS64_ENABLE_TRACE_INSTRUMENTATION=ON`, through the separate
`ntvdm-startup-reference` target. The target produces no executable and no
library. These sources must not be offered as a BYOB engine or used to make a
DOS program advance.

```text
cmake -S . -B build-trace -DNTDOS64_ENABLE_TRACE_INSTRUMENTATION=ON
cmake --build build-trace --target ntvdm-startup-reference
```

The reference target does not execute its sources. Any future standalone trace
tool may read a local BYOB file, record a snapshot, or assert a bounded expected
transition only. It cannot change an instruction, BOP/SVC result, DOS return,
device response, or startup-success condition. The self-authored WHPX COM
engine and BOP observer are subject to the same source-only boundary.

## Current Boundary

The default build includes only the non-invasive dispatcher boundary and
supporting file-format utilities. It does not provide an NTDOS bootstrap,
profile service adapter, DOS namespace, a DOS engine, or a Win16 environment.
No historical runtime target may be added until its link acceptance proves the
original chain `CCPU -> BIOS[] -> MS_bop_0 -> DemDispatch -> DEM`, rather than
linking a self-authored replacement. The clean-room phase contract is in
`docs/NTDOS-BOOTSTRAP-CONTRACT.md`; the host API policy is in
`docs/WIN32-HOST-API-BOUNDARY.md`. An absent engine is a clear failure, not a
fallback to system hooks.

## Source Policy

The `upstream/` snapshots are read-only evidence. The separately manifested
historical research inputs in `src/opennt/` and `src/ntvdmx64/` are available
only through default-disabled CMake gates and are not linked into this CLI.
Their source roots, include paths, overlays, hashes, and research restrictions
are recorded in `docs/HISTORICAL-SOURCE-INPUT-GATES.md`. Public and
undocumented Windows APIs may be catalogued for historical compatibility
analysis, but the primary runtime path uses documented user-mode APIs only. A
private API cannot become a required dependency without a separate owner
decision and a new isolation analysis.

The latest clean MinGW-w64 build graph and dispatcher smoke evidence are
recorded in `docs/MODERN-RUNNER-ISOLATION-AUDIT.md`. Those results verify only
the owned PE handoff and DOS-without-engine rejection boundary, not DOS guest
execution.

## External Comparison Material

Some default-disabled evidence records and source-only probes mention external
machine implementations. They are retained solely to preserve the historical
research record. They are not part of the current roadmap, and must not be
configured, built, or cited as progress on the OpenNT SoftPC recovery route.
The binding rule is [SoftPC Reconstruction Governance](docs/SOFTPC-RECONSTRUCTION-GOVERNANCE.md).
