# M0 T310 S8 P4 — video non-width warning disposition

## Reviewed records

The remaining selected video-controller call records are:

- `T310-S8-P4-CLASS-019834` and `...019835` in `ega_read.c`;
- `...019861` and `...019862` in `ega_vide.c`;
- `...020048` in `video.c`.

## Finding

None is an x86/x64 representation difference:

- The `ega_read.c` C-video declarations omit the explicit `IU32` result and
  are therefore seen as historic C `int`; both are 32-bit scalar returns in
  the supported MSVC x86 and x64 ABIs.
- `ega_vide.c` passes the planar `EGA_planes` storage to `memset4`, whose
  historical declaration uses `unsigned int *`; the source cast uses
  `ULONG *`. Both are 32-bit storage elements on the supported Windows ABI.
- `video.c` assigns a `short` dirty-count address to the historical `word *`
  carrier. This is a 16-bit signedness/source-contract discrepancy, not a
  native pointer-width transition.

## Disposition

These records remain visible as original non-width source warnings. They are
not suppressed, cast away, or changed in the mirror during the x86/x64 ABI
closure. A future source-correctness task may audit them with the required
controller behavior tests; this S8 P4 pass must not use them to inflate the
modern-width diff.
