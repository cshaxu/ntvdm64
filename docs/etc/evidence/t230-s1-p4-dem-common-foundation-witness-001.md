# T230 S1 P4 — DEM Common Foundation Witness

## Result

T230 S1 establishes the first complete structural foundation for the OpenNT
DEM parallel mirror.  This is a foundation result only.  It does **not** claim
that the 73 DEM BOPs, or even the full `demhndl.c` / `demfile.c` packages, are
runtime-complete.

## Source-shaped layout

The active v2 namespace is now:

```text
src/bx-vdm/bop/opennt/dem/
    dem.c demdata.c demmsg.c demdisp.c
    demhndl.c demfile.c
src/bx-vdm/bop/shim/
    bx_ntvdm_dem_direct_context.{c,h}
```

`demhndl.*` and `demfile.*` were moved with `git mv` from the former mixed
`src/bx-vdm/bop/` location.  Their former v1 whole-provider dependency was
replaced by the explicitly versioned, selector-neutral Direct context ABI.
The legacy-context producer lives in `src/bx-vdm/bop-v1/`, so the new mirror
and neutral shim do not include or name v1 provider, namespace, file-session,
or mutation-policy types.

## Original-file coverage in S1

| Original DEM file | S1 result |
| --- | --- |
| `dem.c` | Source-shaped `DemInit` ownership and direct host-composition divergence comments. |
| `demdata.c` | Source-shaped shared DEM state declarations/definitions. |
| `demmsg.c` | Original diagnostic table and `demPrintMsg` shape, with a named CLI diagnostic seam. |
| `demdisp.c` | All 73 original service identities retained in original order as a service-to-owner catalog.  Only original no-op/set-V86 helper behavior is exposed here; full dispatch awaits the owning mirror packages. |
| existing `50:00`–`50:02` v2 algorithms | Retained under `opennt/dem`: `demhndl.c` owns `50:00`/`50:02`; `demfile.c` owns `50:01`. |

The catalog is deliberately not a temporary generic dispatcher: it records
the exact original owner for every service and prevents a fabricated success
or no-op route before that owner's source file is mirrored.

## Verification

Formal Ninja target:

```text
build/M0-T230-S1/s1-foundation-r1/bin/t230-s1-dem-direct-mirror-fixture.exe
```

The fixture source-builds `bx-vdm.lib`, including all foundation files and
the legacy-only adapter needed by still-unmigrated v1 callers.  It then
verifies:

1. the neutral Direct context validity contract;
2. the 73-entry `demdisp.c` owner catalog and `50:00 -> demhndl.c` mapping;
3. source-shaped typed results for `50:00`, `50:01`, and `50:02` through a
   fake host handle/attribute backend; and
4. that close consumes/releases the fake host handle.

The executable returned zero on 2026-08-19.

Independent source-boundary scan returned no matches for
`bop-v1`, `whole_provider`, `file_session`, `host_namespace_v1`, `Readonly`,
`Overlay`, or `Virtual` below `src/bx-vdm/bop/opennt` and
`src/bx-vdm/bop/shim`.

## Remaining S1 boundary

S1 is the common-source and ownership foundation.  `demhndl.c` and
`demfile.c` retain only the already-migrated `50:00`–`50:02` entry points;
their remaining original functions and services belong to S2 and S3
respectively.  This records a package boundary, not a transfer of callable
DEM endpoints out of T230.
