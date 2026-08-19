# T230 S15 P4 v2 DEM composition entry

## Result

`src/bx-vdm/bop/dem_v2_composition_bridge.c` now provides the one external
generic-UD composition symbol required by the formal CPU5 mantle graph.  It
recognizes a valid `C4 C4 50 xx` window through the v2 bridge and dispatches
only to the bound Direct OpenNT DEM session.  It has no include, symbol or
call path into `bop-v1`.

For a valid DEM encoding, an absent/unbound session returns decline to the
CPU exception path.  It is deliberately not forwarded to the retained legacy
composition.  Non-DEM selectors also decline pending their own package
migration; this DEM change does not revive a generic legacy fallback.

## Formal local witness

The fresh formal Ninja graph was generated at the disposable path
`build/M0-T230-S15/dem-v2-composition-r3` from
`tools/build/t225-s7-full-module-manifest.json`.  That manifest contains the
full imported DEM mirror, neutral shims and the new composition source; it
contains no `bop-v1` source.

The command:

```text
ninja -C build/M0-T230-S15/dem-v2-composition-r3 \
  bin/t230-s10-dem-native-session-direct-fixture.exe
```

compiled `bx-vdm.lib`, including all imported DEM files and
`dem_v2_composition_bridge.c`, then linked the fixture with `bx-mantle` and
`bx-core`.  Executing the fixture exited zero and printed:

```text
T230 Direct DEM v2 composition: copied #UD reaches original dispatcher without v1 fallback
```

The fixture proves: bound `50:1f` reaches imported `DemDispatch`; `54:1f`
declines; and unbound `50:1f` declines.  It is a local composition witness,
not the S15 completion claim.

## Engine binding

`dem_v2_runtime_session.c` owns the process-local Direct host/native-session
pair.  `bx_ntvdm_engine_run_v1` binds it after the retained startup composition
has validated its inputs but before `bx_ntvdm_machine_stage_v1_begin`; it
resets the pair on every subsequent rejection and after stage reset.  The
adapter session's only guest-memory callbacks are the selector-blind mantle
checked-RAM functions, so no access can occur while the stage is inactive.

The fresh `dem-v2-runtime-r2` graph compiled the modified mantle archive and
the full v2 `bx-vdm` archive.  Its
`t230-s15-v2-runtime-session-fixture` linked against both and exited zero:

```text
T230 v2 runtime session owns DEM and declines without legacy fallback
```

`tools/governance/Test-T230S15V2DemRoutePrecedence.ps1` is the repeatable
source-membership check for this boundary.  It verifies that the formal Ninja
manifest has no `bop-v1` input, the v2 composition source has no `bop-v1`
include, the v2 entry recognizes and dispatches DEM, and the engine binds and
resets the v2 session.  It intentionally reports the historical v1 bridge as
retained outside the formal graph rather than hiding that remaining cleanup.

## Remaining S15 work

The old native engine still installs `bop-v1/bx_ntvdm_composition_runtime_v1`
for startup and other unported package state.  The new binding makes its DEM
provider state non-authoritative only when the v2 composition entry is the
linked external generic-UD symbol.  Consequently this record does not claim
that every historical derivative has been recomposed, nor that retained v1
DEM files can already be deleted.  S15 remains active until route scans prove
each product/derivative selects the v2 entry and every retained DEM v1 member
is unreachable or removed; S16 then owns the bounded native observation.

## CLI link sweep

The two selector-blind BOP/#UD sequence observers were moved from `bop-v1` to
`bop/observation` and are invoked by the v2 composition entry.  The CLI-only
`dem-read-header` and `dem-namespace-identity` switches were removed because
they observed legacy provider-private state and would falsely describe the v2
Direct session.  Their removal does not remove a DEM service or failure path.

Fresh formal build root `build/M0-T230-S15/cli-v2-observation-r1` now compiles
the complete v2 `bx-vdm`, `bx-core`, `bx-mantle`, `cli` and `opennt-host`
archives.  The `ntdos64-native` link no longer reports legacy observers or
unlinked OpenNT host APIs.  Its only remaining unresolved symbols are the five
startup functions of `bx_ntvdm_composition_runtime_v1`.  This precisely
isolates the next removal prerequisite: a v2 startup/composition owner, not a
remaining DEM handler, shim, observer, or Win32 API issue.
