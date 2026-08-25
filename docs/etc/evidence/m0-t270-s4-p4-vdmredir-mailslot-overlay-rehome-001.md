# M0 T270 S4 P4 — VDMREDIR mailslot overlay rehome

## Finding and repair

`opennt-bop/vdmredir/vrmslot.c` is the source-derived modernization of the
original VDMREDIR mailslot module.  It preserves the historical service entry
names and record/list lifecycle, but replaces raw CCPU/SAS pointers and the
private 16-bit handle bitmap with bounded mapping-manager operations.  Its
measured 113.3-percent delta exceeds the mirror-component limit.

The complete body moved with `git mv` to
`opennt-bop-overlay/vdmredir/vrmslot.c`.  The matching mirror translation unit
is a single marked private include boundary.  This is intentionally not a
collection of per-entry wrappers: `VrMakeMailslot`, read/write/delete, process
cleanup and the private list helpers share one static list instance.  Splitting
them would either duplicate that state or add a public overlay ABI.

## Verification

The external formal graph rebuilt the moved mirror object and all dependent
archives/fixtures, then reported a clean no-work second pass.  The focused
Redirector fixture passed:

```text
ninja -C build/M0-T270-S1/r002 -j 8
[1/80] CC obj/opennt-bop/src_opennt_bop_vdmredir_vrmslot_c.obj
...
[80/80] LINK bin/ntvdm64.exe

t251-s3-redir-ingress-fixture.exe
T263 S9 Redirector: unavailable lifecycle and synchronous pipe helper paths pass
exit 0

ninja -C build/M0-T270-S1/r002 -j 8
ninja: no work to do.
```

This is a source-layout correction only.  It neither enables a Redirector
lifecycle nor changes any BOP selection, result or failure disposition.
