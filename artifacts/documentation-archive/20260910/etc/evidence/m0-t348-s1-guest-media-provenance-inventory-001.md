# M0 T348 S1 — Guest media provenance inventory

## Inventory scope

The generated [path-level matrix](../operations/m0-t348-s1-guest-artifact-matrix.tsv)
covers every current selected guest/media file, excluding only the component
README that is not media:

- 2,596 paths under `src/mvdm-guest`;
- 28 source-built DOS output paths under `build/output/dos`;
- 87 source-built WOW16 output paths under `build/output/wow16`; and
- 8 immutable SoftPC ROM/profile/CMOS adjacency paths.

Total: **2,719** rows. Each records path, byte count, SHA-256, file class,
source series, final disposition, staged destination and provenance reference.

## Final dispositions

- 9 rows are current `runtime-media`; their 10 destinations include the two
  original `COMMAND.COM` locations.
- 6 rows are `runtime-firmware-adjacent`.
- 113 rows are `source-built-cross-check`; they are retained evidence, not a
  host link input or an automatic replacement for original media.
- 889 rows are selected guest mirror material outside the current runtime
  stage.
- 1,700 WOW16/font rows are `runtime-deferred-wow16`: selected mirror media
  retained for the future WOW16 stage, not silently omitted or currently
  staged.
- 2 remaining firmware rows are outside the current fixed runtime stage.

The 16 current staged media destinations exactly match the existing
`Stage-OriginalSoftpcRuntime.mjs` contract: DOS loader/kernel/command/config,
country/HIMEM, `REDIR.EXE`, `DOSX.EXE`, and six original firmware inputs.

## Verification

`Export-T348GuestArtifactMatrix.mjs` was run twice over unchanged inputs; both
runs produced 2,719 rows. The matrix contains no `src.old` reference. The
selected formal CPU40/x86 host build graph contains no `src/mvdm-guest` or
`build/output/{dos,wow16}` host compilation/link input; staging copies the
selected immutable files only after the executable is linked.

## S1 disposition

The original/source-built distinction is now explicit and no media item has an
implicit role. S2 may reconcile the current stage manifest against this matrix
and decide only missing/extra stage behavior. It may not rebuild, rewrite or
link guest code.
