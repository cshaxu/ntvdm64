# M0 T270 S3 P1 — Opennt-host threshold audit

## Result

The initial identity scan confirms six exact selected originals and four
locally marked modified originals.  It also finds that three previously
accepted true subsets violate the current mirror-component 50-percent rule:

- `softpc.new/host/src/nt_error.c`: +23/-1125 against 1190 original lines;
  96.5 percent normalized delta.
- `vdmredir/vrinit.c`: +7/-675 against 748 original lines; 91.2 percent
  normalized delta.
- `vdmredir/vrnmpipe.c`: +103/-2496 against 3412 original lines; 76.2
  percent normalized delta.
- `vdmredir/vrputil.c`: +12/-16 against 194 original lines; 14.4 percent
  normalized delta and therefore remains an eligible same-shaped mirror
  modification.

The exact retained originals are `inc/oemuni.h`, `inc/vrnmpipe.h`,
`softpc.new/host/inc/{error.h,nt_pif.h,nt_uis.h}`, and `vdmredir/vrputil.h`.
All modified source files contain local `DIVERGENCE(...)` markers and have
matching `opennt-host/README.md` entries.

## Boundary result

`opennt-host-overlay` has three registered binding fragments. A source scan
found no include of that overlay from outside `opennt-host` or its overlay.
The formal manifest compiles the four host source units into `opennt-host`.
The already fresh formal graph's focused executable passed:

```text
build/M0-T270-S1/r002/bin/t263-s8-opennt-host-mirror-fixture.exe
T263 S9 OpenNT host mirror: utility and synchronous pipe paths pass
exit 0
```

This verifies the currently admitted synchronous pipe/utility behavior only;
it does not waive the source-layout threshold.

## Required S3 repair direction

The three over-threshold bodies cannot remain as truncated sources below
`opennt-host`. Their retained implementations must move to corresponding
private `opennt-host-overlay` sources. `opennt-host` may retain only a
registered, same-signature minimal entry boundary to the matching overlay;
that boundary must preserve existing exported symbol names, parameters and
observable result. The overlay remains private: adapters and BOP sources may
consume the `opennt-host` library ABI but may not include/link an overlay
source or header directly.

No source move is made by this audit checkpoint. The next S3 step must prove
the build-list and internal linkage arrangement before relocating any body.
