# M0 T270 S4 P2 — Redirector dispatcher overlay rehome

## Finding and repair

`opennt-bop/vdmredir/vrdisp.c` differed from the selected OpenNT source by
113.5 percent.  It is a source-derived copied-frame rehost of the historical
CCPU-based dispatcher, not a retained mirror body.  The implementation moved
with `git mv` to `opennt-bop-overlay/vdmredir/vrdisp.c`.

The mirror file now exports only the original `VrDispatch` and existing
`runtime_vr_dispatch_with_frame` entry boundaries.  It privately macro-renames
the overlay body in its own translation unit.  The 0x00--0x31 table, copied
frame state and source-derived callbacks remain private overlay code; no other
component includes the overlay.

## Verification

The external incremental formal graph completed at 80/80 after the move, and
the focused Redirector ingress fixture passed:

```text
t251-s3-redir-ingress-fixture.exe
T263 S9 Redirector: unavailable lifecycle and synchronous pipe helper paths pass
exit 0
```

No Redirector lifecycle or service disposition changed; this is only the
source-owner correction required by the mirror rule.
