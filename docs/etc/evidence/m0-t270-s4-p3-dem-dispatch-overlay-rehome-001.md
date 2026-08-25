# M0 T270 S4 P3 — DEM dispatcher overlay rehome

## Finding and repair

`opennt-bop/dem/demdisp.c` is a reached source subset whose retained body
differs from the selected OpenNT original by 99.2 percent.  It owns the
original DEM dispatcher table and shared process-wide dispatcher state:
`fShowSVCMsg`, `CurrentISVC`, and `apfnSVC`.  A wrapper which renamed or copied
those globals would create distinct state and alter all DEM provider behavior.

The body therefore moved with `git mv` to
`opennt-bop-overlay/dem/demdisp.c`.  The same-named mirror translation unit is
the smallest marked private include boundary.  It compiles the overlay body in
the original linkage context, so the original global names, table instance,
dispatch order, ordinary service bodies, and provider-visible state remain
single-instance and unchanged.

## Verification

The formal external Ninja graph was already current and reported no work; the
focused imported-dispatch fixture then passed:

```text
ninja -C build/M0-T270-S1/r002 -j 8
ninja: no work to do.

t230-s10-demdisp-direct-import-fixture.exe
T230 S10 direct OpenNT demdisp table: 73 slots and all ordinary/no-op bodies verified
exit 0
```

No selector, service dispatch, provider result, or failure disposition changed.
The repair only moves the over-threshold retained implementation behind the
matching mirror's private boundary.
