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

## Remaining S15 work

The old native engine still installs `bop-v1/bx_ntvdm_composition_runtime_v1`
for startup and other unported package state.  Consequently this record does
not claim that the old engine image has been recomposed, nor that retained
v1 DEM files can already be deleted.  S15 remains active until that engine
path is bound to the v2 entry and every retained DEM v1 member is unreachable
or removed; S16 then owns the bounded native observation.
