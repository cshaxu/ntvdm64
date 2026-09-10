# M0 T261 S4 P5 — DEM search/FCB owner split

## Change

- Moved `demsrch`/`demfcb` source-body service selection by Git rename to
  `opennt-bop/dem/opennt_demsrch_fcb_composition.c`.
- Moved `FindFirstFileOem`, `FindNextFileOem`, `RtlProcessHeap` and the
  historical `NtVdmControl` spelling to `adapter-win32`.
- Public OEM enumeration and process-heap behavior are directly recovered.
  The removed invasive `NtVdmControl` deliberately returns
  `STATUS_NOT_IMPLEMENTED`; the imported search source then retains its
  original slow-reset fallback instead of receiving a fabricated success.
- The unavailable VDD user-hook termination remains at its original call
  boundary as `BOP-DIV-041`; it is no longer misclassified as an
  `opennt-host` BOP composition.

## Verification

- Fresh formal MSVC x64 `/MT` root: `build/M0-T261-S4/r013`.
- Full graph completed 461 Ninja edges and then reported `ninja: no work to
  do.`
- `t230-s5-demsrch-fcb-direct-import-fixture.exe` returned zero, verifying
  slow-reset search lifecycle, tokenized FCB operations and PDB cleanup.
- `git diff --check` and documentation governance verification passed.

## Result

OpenNT search/FCB provider semantics remain in their imported bodies.
`opennt-host` no longer owns their BOP selection or same-shaped Win32 facade;
the sole unavailable VDD behavior is explicitly registered rather than hidden.
