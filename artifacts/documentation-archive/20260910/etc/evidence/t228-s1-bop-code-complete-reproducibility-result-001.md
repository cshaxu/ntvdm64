# T228 S1 BOP code-complete reproducibility result

## Scope

This record rechecks the existing T226/T227 local code-complete baseline from
the current worktree. It makes no native Guest DOS runtime claim.

## Current-source inventory check

The deterministic current-source command

```powershell
powershell -ExecutionPolicy Bypass -File tools/governance/Test-T226S1BopConformanceManifest.ps1 -RepositoryRoot (Get-Location).Path
```

passed on 2026-08-18 and exported/verified exactly 203 T226 S1 conformance
rows. The manifest binds each original BOP identity to one owner-package
fixture target.

## Fresh formal build and execution

A fresh isolated MSVC x64 `/MT` Ninja graph was generated at
`build/M0-T228-S1/bop-codecomplete-20260818a`. It built exactly the 40
manifest-registered `t226-*` BOP fixture targets in 391 graph edges, without
using the earlier premature-end work directory. Every freshly built executable
then returned zero:

| Owner package | Fixtures |
| --- | ---: |
| DEM | 15 |
| COMMAND | 15 |
| XMS | 5 |
| DPMI | 1 |
| Redirector | 1 |
| Debugger/WOW/top-level | 3 |
| Total | 40 |

## Retained-root cross-check

The previously retained formal root `build/M0-T226-S8/all-package-20260817a`
also contains the same 40 fixture executables, all of which independently
returned zero in a no-rebuild reproducibility run. This cross-check is
supporting evidence; the fresh current-source graph above is the operative
code-complete result.

## Boundary

Together, the current 203-row inventory verification and the fresh 40-fixture
build/execution establish the local BOP owner-package code-complete gate. They
do not prove native Guest DOS continuous execution, child/parent lifecycle,
or a complete host-product replacement. No BOP implementation, provider
selection, profile behavior, or guest input changed during this revalidation.