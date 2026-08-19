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
pair.  `bx_ntvdm_engine_run_v1` binds it after the v2 startup composition has
validated its inputs but before `bx_ntvdm_machine_stage_v1_begin`; it resets
the pair on every subsequent rejection and after stage reset.  The adapter
session's only guest-memory callbacks are the selector-blind mantle checked-
RAM functions, so no access can occur while the stage is inactive.

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

## Startup replacement

`dem_v2_startup_composition.c` now replaces the five startup-composition
interfaces formerly called by `bx_ntvdm_engine_run_v1`: install, reset,
machine-stage request, stage entry and ordinary-terminal classification.  It
uses the copied BYOB descriptors, exact identity-checked guest images and the
existing selector-blind initial-state/startup-plan ABI; it does not include or
call `bop-v1`.

The only source-shaped DEM-specific composition is setting the imported
`dem.c` global `pszDefaultDOSDirectory` to the validated CLI BYOB root.  This
is the recorded replacement for OpenNT's installed-system-directory lookup.
The two frees in imported `demmisc.c` now null that global after preserving
the original free ordering: a reusable Direct engine session can otherwise
re-enter reset after an original `demLoadDos` (`50:11`) has released it.

A fresh formal Ninja graph at
`build/M0-T230-S15/v2-startup-r2` source-built the complete 221-edge graph and
linked `bin/ntdos64-native.exe`.  The route-precedence gate passed with no
`bop-v1` manifest input.  A no-argument executable invocation printed usage
and returned 2, proving the formal executable's normal CLI front door.

The hash-locked primary original-toolchain input set was staged only below the
disposable `build/M0-T230-S15/v2-startup-inputs-r1` and invoked with a 128-tick
budget.  It returned `terminal=2 detail=3`, i.e. rejected composition while
preparing the machine-stage request.  This proves neither successful guest
execution nor native DEM dispatch.  It is retained as a v1-free startup
replacement observation; the pre-stage rejection must be diagnosed before
claiming the S16 native observation.

## Remaining S15 work

The native engine no longer calls
`bop-v1/bx_ntvdm_composition_runtime_v1` for startup.  The audited 140 tracked
handwritten `bop-v1/bx_ntvdm_dem_*` implementation/header files are now
deleted.  The surviving `bop-v1` directory contains only non-DEM historical
families and their shared legacy scaffolding; the formal module manifest has
no `bop-v1` source.  Consequently this record does not claim the 73-slot v2
runtime/native closure: the pre-stage rejection and one bounded native Direct
observation remain open.

After deletion, the same formal graph built and ran all 14 T230 fixtures with
zero exit: checked RAM; Direct host/native session; `demhndl`, `demfile` /
`demlabel`, `demdir`, `demsrch` / `demfcb`, `demgset`, `demdasd` /
`demioctl`, `demerror` / `demlock`, and `demmisc`; the original `demdisp.c`
73-slot table; bounded ingress; and copied-UD composition.  The fixture
outputs explicitly report the original dispatcher and v1-free composition.
This is a complete post-deletion local regression of the formal T230 fixture
set, but it remains distinct from an executing guest's native observation.

## CLI link sweep

The two selector-blind BOP/#UD sequence observers were moved from `bop-v1` to
`bop/observation` and are invoked by the v2 composition entry.  The CLI-only
`dem-read-header` and `dem-namespace-identity` switches were removed because
they observed legacy provider-private state and would falsely describe the v2
Direct session.  Their removal does not remove a DEM service or failure path.

Fresh formal build root `build/M0-T230-S15/cli-v2-observation-r1` now compiles
the complete v2 `bx-vdm`, `bx-core`, `bx-mantle`, `cli` and `opennt-host`
archives.  The `ntdos64-native` link no longer reports legacy observers or
unlinked OpenNT host APIs.  The then-remaining five startup symbols were
subsequently replaced by `dem_v2_startup_composition.c`; the fresh
`v2-startup-r2` graph now links `ntdos64-native.exe` without the legacy
composition runtime.  This does not turn the pre-stage rejection into a
native DEM execution claim.
