# M0 T270 S3 P2 — Opennt-host over-threshold overlay rehome

## Repair

The three over-threshold true subsets identified in P1 were moved with
`git mv` to their corresponding private overlay paths:

- `opennt-host-overlay/softpc.new/host/src/nt_error.c`
- `opennt-host-overlay/vdmredir/vrinit.c`
- `opennt-host-overlay/vdmredir/vrnmpipe.c`

Their former mirror paths now contain only registered same-signature entry
boundaries.  Each boundary macro-renames the private body during its own
translation unit, then exports the original public function name, parameters
and return/result behavior through a direct call.  The overlay sources are not
separate manifest modules and expose no include path to adapters, BOP, session
or app; they compile only as the corresponding `opennt-host` mirror unit.

This retains the established ABI while ensuring no 76--96 percent cropped
body remains below the mirror root.  The newly short mirror boundaries contain
no service selection, replacement algorithm or new host capability.

## Verification

An external incremental formal Ninja build completed successfully after the
move:

```text
ninja -C build/M0-T270-S1/r002 -j 8
[1/81] CC ... opennt-host/vdmredir/vrnmpipe.c
[2/81] LIB lib/opennt-host.lib
...
[81/81] LINK bin/t243-s2-spckbd-handoff-fixture.exe
exit 0
```

The focused host-mirror fixture also passed:

```text
build/M0-T270-S1/r002/bin/t263-s8-opennt-host-mirror-fixture.exe
T263 S9 OpenNT host mirror: utility and synchronous pipe paths pass
exit 0
```

The allowed mirror-to-matching-overlay includes are the sole overlay
consumers.  No source outside these two component roots includes an
`opennt-host-overlay` path.
