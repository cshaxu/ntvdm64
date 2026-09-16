# Final MVDM-root tail elimination

## Objective

Complete the owner-approved physical cleanup left after T414 without changing
runtime behavior: place the OpenNT font blobs at their real OpenNT path,
place the two original `v86/util` files under canonical `src/mvdm/`, and move
the one non-mirror printer carrier into the private MVDM overlay.  The old
`mvdm-guest`, `mvdm-tools`, and `mvdm-host` roots must then disappear.

## Boundaries

- Fonts are exact OpenNT `base/win32/winnls/fontsup/system` inputs.
- `v86/util` is exact OpenNT `base/mvdm/v86/util` input.
- `monitor_printer.c` is an extracted standalone carrier of functions from
  original `base/mvdm/v86/monitor/i386/monitor.c`; it stays private in
  `mvdm-overlay`, never masquerades as a complete original translation unit.
- No algorithm, ABI, link role, or runtime product behavior changes.

## Acceptance

All moved original blobs match their named OpenNT sources; the formal x86
three-program build and printer-containing worker link pass from the new
paths; current source-layout authorities name no retired root; documentation
governance and final diff review pass.  The worktree is committed/pushed
cleanly, without a runtime-package publication for this layout-only task.
