# M0 T306 S1 P6 — MVDM rebase and historical SoftPC machine cohort

## Question

Do the 87 MVDM-source and 76 historical SoftPC-source signature seeds require
new external package imports, or should they return to the MVDM closure and the
already-declared Bochs machine boundary respectively?

## Inputs

- The 163 physical rows labelled `mvdm-internal-rebase` or
  `mvdm-softpc-second-executor` in
  [the signature-seed package ledger](../operations/m0-t306-s1-signature-seed-package-ledger.tsv).
- Existing selected MVDM mirrors in `mvdm-host` and `mvdm-support`.
- SHA-256 comparisons for `oemuni/file.c`, `oemuni/process.c`,
  `suballoc/suballoc.c`, `inc/wowfax.h`, `vdmredir/vrremote.c`, and the
  registered `dpmi32/dpmiselr.c` mirror divergence.
- The pinned architecture rule that Bochs is the only active machine executor.

## Observations

- All 87 rebase rows resolve to six selected MVDM source paths already rooted
  in this repository's MVDM components. Five carriers are byte-exact mirrors;
  `dpmiselr.c` is a separately registered mirror divergence. These are not
  external package dependencies and must not be assigned adapters merely
  because source components were re-rooted.
- The 76 SoftPC rows span serial, EMM, keyboard/video, timer, floppy/disk,
  console, sound, detection and low-level memory/device forms. Linking them
  would activate a second historical machine/device executor beside the
  declared Bochs machine model.

## Decision

Return all 87 internal rows to zero-degree MVDM closure. Their real outward
dependencies are audited only when the owner source body reaches them.

Classify all 76 SoftPC rows as named machine-boundary evidence. Their original
source remains mirrored for provenance and source-shaped interface recovery,
but none enters the active host runtime as a SoftPC executor. The only allowed
future route is `adapter-mvdm-host-out/softpc` preserving a proved historical
interface shape over `adapter-bochs`; it must not import a parallel PIC,
device, timer or CPU loop.

The file-group result is in the companion
[machine cohort ledger](../operations/m0-t306-s1-mvdm-machine-cohort-ledger.tsv).

## Follow-up

This reduces the current non-MVDM adoption frontier by 163 physical seed rows.
Continue with public CRT and source-absent signatures; do not treat any
historical SoftPC definition as an automatic modern capability.
