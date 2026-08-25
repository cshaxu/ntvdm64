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

## Current Build Entrypoint

The only current in-process build route is the manifest-driven MSVC x64 `/MT`
Ninja graph. It owns the nine linkable runtime modules and their admitted
fixtures; the root CMake catalogue was retired because it described deleted
pre-component-layout paths. It has been removed; Git history and the T267 closure evidence preserve its provenance.

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools/build/New-T260S8FullNinjaGraph.ps1 `
  -RepositoryRoot (Get-Location).Path `
  -BuildRoot build/M0-T267-S1/r001
ninja -C build/M0-T267-S1/r001
```

Historical reconstruction probes—including the reconstructed monitor—are retained below
`tests/legacy/reconstruction/`. They are not formal Ninja inputs, runtime
providers, or a substitute for Bochs/OpenNT execution.
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

Retained historical research inputs are evidence only. They are not formal
Ninja inputs and are not linked into this CLI. Their provenance, source roots,
include paths, overlays, hashes, and research restrictions are recorded in
`docs/HISTORICAL-SOURCE-INPUT-GATES.md`. Public and
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
