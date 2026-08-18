# T228 S1 BOP code-complete reproducibility result

## Scope

This record rechecks the existing T226/T227 local code-complete baseline. It
is not a claim that a new all-fixture executable graph was rebuilt from the
current worktree, and it makes no native Guest DOS runtime claim.

## Current-source inventory check

The deterministic current-source command

```powershell
powershell -ExecutionPolicy Bypass -File tools/governance/Test-T226S1BopConformanceManifest.ps1 -RepositoryRoot (Get-Location).Path
```

passed on 2026-08-18 and exported/verified exactly 203 T226 S1 conformance
rows. The manifest binds each original BOP identity to one owner-package
fixture target.

## Retained formal-fixture execution

The retained formal root `build/M0-T226-S8/all-package-20260817a` contains all
40 manifest-registered `t226-*` fixture executables. Each executable was run
without rebuilding sources; all returned zero:

| Owner package | Fixtures |
| --- | ---: |
| DEM | 15 |
| COMMAND | 15 |
| XMS | 5 |
| DPMI | 1 |
| Redirector | 1 |
| Debugger/WOW/top-level | 3 |
| Total | 40 |

## Boundary

The retained-fixture run establishes executable reproducibility of the
previous formal T226 result. The manifest verifier establishes the current
203-row route/fixture inventory. The two facts do not substitute for a fresh
all-fixture rebuild after later mantle/CLI observation changes. That rebuild
is deliberately not claimed because the disposable current Ninja root had a
premature-end log recovery failure. No BOP implementation, provider
selection, profile behavior, or guest input changed during this recheck.