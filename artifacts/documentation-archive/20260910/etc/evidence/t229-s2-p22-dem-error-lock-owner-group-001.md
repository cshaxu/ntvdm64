# T229 S2 P22 — DEM Error/Lock Owner Group

## Question

Does the complete DEM error/lock group retain the OpenNT source contracts in
both Direct and Readonly profiles without inventing a CLI hard-error UI or
silently taking host locks?

## Source and selected seams

- `src/opennt/base/mvdm/dos/dem/demerror.c`: `demSetHardErrorInfo` records
  `DS:DX` and `DS:BX`; `demRetry` restores saved state and, when no retryable
  original service exists, returns `AX=00FF`, `CF=1`.
- `src/opennt/base/mvdm/dos/dem/demlock.c`: failed lock/unlock calls
  `demClientError`.
- `src/bx-vdm/bx_ntvdm_dem_hard_error_service.c` and
  `src/bx-vdm/bx_ntvdm_dem_error_lock_plane_v1.c`: smallest source-derived
  x64 CLI seams. The original CCPU/SAS addressors and hard-error UI are not
  composable here.

## Direct and Readonly disposition

| BOP | Disposition | Result |
| --- | --- | --- |
| `50:32` | source-derived locator registration | bounded real-mode locators are retained; no host I/O |
| `50:33` | original no-retry terminal | `AX=00FF`, `CF=1`; no hidden retry |
| `50:3F` | source-derived unavailable failure | `AX=0005`, `CF=1`; no host handle or lock operation |

Both selected profiles deliberately share these outcomes. Direct does not
expand the historical contract into arbitrary host locking; a future admitted
file-handle owner must preserve `demClientError` semantics before that changes.

## Reproduction

```powershell
ninja -C build/M0-T229-S2/formal-direct-readonly-20260818b `
  bin/t226-s2-dem-error-lock-conformance-fixture.exe
& build/M0-T229-S2/formal-direct-readonly-20260818b/bin/t226-s2-dem-error-lock-conformance-fixture.exe
```

The fixture printed `t226-s2 DEM error/lock source-derived conformance
verified` and exited zero on 2026-08-18.

## Boundary

This closes the three-service owner group only. It does not implement an INT
24 UI, an arbitrary retry loop, or host-file locking outside a later
source-backed file-handle composition.