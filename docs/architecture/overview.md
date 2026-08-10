# Pre-Governance Architecture Overview Record

> **Superseded as the current architecture authority on 2026-08-09.** Use
> [../design/ARCHITECTURE.md](../design/ARCHITECTURE.md). This detail remains
> supporting evidence during the controlled documentation migration.

## Intended Product

The target is a non-invasive, command-line invocable NT 4-era `ntvdm.exe`. It
is reconstructed from the OpenNT NT 4.0 MVDM and NTDOS base, with later source
trees and external emulators used only as comparative evidence. It must first
be useful as a standalone research executable; any future `ntvdm64`
integration is a separate downstream option, never an execution gate.

## Ownership Boundaries

| Area | Responsibility | Constraint |
| --- | --- | --- |
| CLI host | Parse invocation, establish process context, report failure | Must not invent DOS semantics |
| Historical MVDM path | CPU, BIOS/BOP, dispatcher, and DEM behavior | Preserve observed NT 4 ordering |
| NTDOS path | DOS-facing services and historical runtime behavior | Prefer original source and toolchain behavior |
| Compatibility layer | Isolate modern-host and toolchain differences | Keep outside historical source where possible |
| BYOB boundary | Validate and load user-supplied DOS/NE execution inputs | Missing engine support must fail explicitly |
| Research probes | Demonstrate file/API/ABI gaps and candidate repairs | Evidence before broad replacement |

## Historical Control Flow

The working model for the primary historical control path is `CCPU -> BIOS[] -> MS_bop_0 -> DemDispatch -> DEM -> NTDOS`. This is a behavioral and debugging guide, not permission to flatten layers into a new host implementation. A deviation needs evidence and focused validation.

## Integration Boundary

This repository may read `ntvdm64` to compare governance, contracts, or future integration requirements. It must not write to that directory. Any future integration is an explicit downstream activity with its own compatibility and provenance record.

## CPU And Machine Ownership Decision

`ntdos64` must not grow a replacement DOS runtime or a parallel BOP/SVC
dispatcher. It does, however, own the narrowly scoped reconstruction necessary
to make the historical OpenNT SoftPC/CCPU software-monitor path executable.
That work may complete missing monitor functions, SAS/segment behavior and
host seams only where their historical callers and contracts are evidenced.

Historical recovery is therefore the active implementation route, not a
preliminary oracle for another backend. A recovered function keeps the original
function boundary and data flow, fails closed outside its evidenced domain, and
is covered by a bounded caller fixture. It may not become a new general-purpose
machine API or a shortcut around `BIOS[] -> MS_bop_0 -> DemDispatch -> DEM`.

`ntvdm64`, Bochs, PCjs and similar projects are read-only comparison material.
They may corroborate a semantic reconstruction but cannot be linked, copied as
an execution backend, or used as an admission gate. The binding detail is in
`docs/SOFTPC-RECONSTRUCTION-GOVERNANCE.md`.

## Open Questions

Known API and ABI gaps are expected research inputs. Prefer high-ROI probes at the first missing boundary: caller, expected contract, available historical implementations, and a minimal test that confirms the conclusion.
