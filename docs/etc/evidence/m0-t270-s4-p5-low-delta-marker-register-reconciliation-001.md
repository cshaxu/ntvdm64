# M0 T270 S4 P5 — Low-delta mirror marker/register reconciliation

## Question

Do the remaining directly retained OpenNT BOP bodies satisfy the local marker
and README-register portion of the mirror standard after the over-threshold
bodies have moved to the private overlay?

## Procedure and observations

The selected original comparison set covers 34 current directly retained C
translation units: 1 is exact (`softpc.new/host/src/nt_pif.c`) and 33 have a
measured delta below 50 percent.  The largest retained direct deltas are
`demmsg.c` at 47.9 percent, `demdata.c` at 45.8 percent and `dpmi32.c` at 40.4
percent.  The previously over-threshold bodies are now private overlay
implementations (`mem_size.c`, `tape_io.c`, `emm_fncs.c`, `vrdisp.c`,
`demdisp.c`, and `vrmslot.c`).

The audit found nine low-delta direct bodies whose existing prose used an
unindexed `Divergence` spelling.  They now use registered local markers:

- `BOP-DIV-092`: historical include-closure replacement in `cmdkeyb.c` and
  eight DEM bodies;
- `BOP-DIV-093`: `cmdkeyb.c` modern public-header and width-stable casts;
- `BOP-DIV-094`: `demmisc.c` width-safe loader progression and reusable-session
  reset; and
- `BOP-DIV-095`: `demmsg.c`'s explicit original debug-condition projection.

No source algorithm, BOP route, provider selection, result, or failure rule
changed; these are registration and comment-normalization edits only.

## Verification

The external formal graph rebuilt the nine affected BOP objects, linked all
dependent targets, and then reached a no-work pass.  Focused direct fixtures
passed:

```text
t231-s4-command-console-keyboard-direct-fixture.exe
T231 S4 direct OpenNT console, keyboard fallback/success, and standard-handle token ABI verified

dem-runtime-session-fixture.exe
T230 v2 runtime session owns DEM and declines without legacy fallback

ninja -C build/M0-T270-S1/r002 -j 8
ninja: no work to do.
```

The component still contains project-owned BOP ingress and observation
composition files.  They are not claimed as OpenNT mirror bodies; the final
S4 ledger must separately record their source-derived owner/disposition rather
than treating them as unregistered source deltas.
