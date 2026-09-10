# M0 T284 S9 — unified MVDM guest root evidence

## Scope and result

The prior two load-only guest component roots were moved without source-body
changes into one component:

- `src/mvdm-guest/dos` replaces `src/mvdm-guest-dos`.
- `src/mvdm-guest/win16` replaces `src/mvdm-guest-win16`.

No host compile or link manifest selects either subtree.

## Identity checks

- All 676 paths below `src.old/opennt-guest/dos-v86` exist below
  `src/mvdm-guest/dos`; SHA-256 comparison produced `missing=0 mismatch=0`.
- All 44 paths below `src.old/opennt-guest/wow16` exist below
  `src/mvdm-guest/win16`; SHA-256 comparison produced `missing=0 mismatch=0`.
- All 10 paths below `src.old/opennt-guest/bin86` exist below
  `src/mvdm-guest/win16/bin86`; SHA-256 comparison produced
  `missing=0 mismatch=0`.
- The eight additional selected Win16 driver products are byte-identical to
  their source paths under `build/output/wow16`:
  `drivers/comm/comm.drv`, `drivers/display/vga.drv`,
  `drivers/keyboard/keyboard.drv`, `drivers/mouse/mouse.drv`,
  `drivers/sound/sound.drv`, `system/system.drv`, `timer/TIMER.DRV`, and
  `wfwnet/WFWNET.DRV`.

## Boundary checks

- The old roots are absent from the live production, build, test and current
  governance trees.
- The sole component README is `src/mvdm-guest/README.md`; inner directories
  contain original carried paths only.
- Guest binaries remain load-only assets. This packet makes no guest rebuild,
  host-link, BOP, or runtime claim.
