# T221 S1 P1: DEM current bound-route audit

## Question

Do all 73 historical DEM `50:00..48` identities retained by the T200/T219
inventory still have the bound route attributed to them after T220's
process-resource changes, and can that attribution be checked without
mistaking ingress recognition or a trace for a provider implementation?

## Inputs

- The 73 DEM rows in
  [T219's reconciliation](t219-s2-t200-bound-route-reconciliation-001.json).
- Current `bx_ntvdm_vdm_generic_ud_bridge_v1.c`,
  `bx_ntvdm_boot_namespace_composition_v1.c`,
  `bx_ntvdm_dem_package_session_v1.c`,
  `bx_ntvdm_dem_package_facade_v1.c`,
  `bx_ntvdm_dem_whole_provider_v1.c`, and `bx_ntvdm_dem_plane_v1.c`.
- The original dispatcher attribution already carried by the T219 rows:
  `src/opennt/base/mvdm/dos/dem/demdisp.c`.

## Procedure

Run:

```powershell
powershell.exe -ExecutionPolicy Bypass -File `
  tools/governance/Export-T221S1DemBoundRouteAudit.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -OutputPath O:\repos.hobby\ntdos64\docs\etc\research\t221-s1-dem-current-bound-route-audit-001.json
```

The exporter rejects a non-73-row or non-contiguous service inventory and a
duplicate service.  For each T219 route category it requires the present
generic bridge, composition-to-session call, and the session/facade/whole
provider symbols that establish that category.  It deliberately does not run
the guest or infer a provider from a mapped/deferred ingress result.

The small Windows PowerShell 5.1 flattening clause is intentional: its JSON
parser presents a top-level array differently from newer PowerShell releases.
The exporter normalizes that host-tool difference before checking the
inventory; it does not alter the audited input.

## Observation

The generated [73-row audit JSON](t221-s1-dem-current-bound-route-audit-001.json)
reports `serviceCount: 73` and `staticOnly: true`.  Every row retains its
original `demdisp.c` handler, current route, later shim, temporary fallback,
and shared-profile disposition from T219, plus current-source route evidence.

The verified static route partition is:

| Current bound category | Services |
| --- | ---: |
| GSET/CWD/drive/volume source-derived planes | 11 |
| misc helpers/no-op planes | 11 |
| facade to original-no-op provider | 7 |
| finite boot lifecycle provider | 1 |
| whole-provider FCB direct/I-O | 2 |
| whole-provider FCB path | 4 |
| whole-provider token handles | 6 |
| whole-provider merged search | 4 |
| whole-provider namespace | 15 |
| ordered special planes then boot/fallback | 12 |

The totals are 73.  T220 changes the owner association inside the direct
namespace path; it does not change these route families or turn `50:3C` into
a token teardown path.

## Interpretation and confidence

This is a high-confidence **static bound-route** reconciliation.  It confirms
that T219's route ledger is still an appropriate exhaustive current-state
baseline for the DEM owner package.  It does **not** prove that any route
implements its original handler's ABI, layout, ordering, profile behavior, or
failure path.  In particular, the 29 whole-provider memberships, finite
`50:3C` boot cleanup, original-no-op leaves, FASTREAD compatibility, and
`dem_cli_unavailable` fences retain their independently recorded limitations.

## Follow-up

T221 S1 P2 must add the missing per-owner-group source-recovery ledger:
original translation-unit composability, precise ABI/layout/order/failure
contract, host API availability, shared mutation-class/profile decision, and
retain/migrate/replace/delete disposition for every shim/fallback.  It must
use this 73-row audit as its coverage guard and still make no provider or
native-trace change.
