# M0 T348 S2 — Executable-relative staging reconciliation

Date: 2026-09-01

## Scope

This evidence verifies the existing product-stage contract only.  It does not
alter guest sources, guest binaries, firmware, or host runtime behavior.

## Fixed transaction

`tools/build/Stage-OriginalSoftpcRuntime.mjs` staged
`build/M0-T343/S3/record-x86/original-softpc-process.exe` into the fresh,
disposable root `build/M0-T348/S2/runtime-x86-r1`.

The generated `runtime-manifest.json` and the S1 ledger agree exactly:

- 16 selected media destinations, including the intentional dual placement of
  original `COMMAND.COM` at `mvdm/COMMAND.COM` and
  `mvdm/system32/COMMAND.COM`;
- 16 physical staged media files, with no extra media path;
- zero staged-media SHA-256 mismatches;
- a matching SHA-256 for the staged product executable.

## Refusal behavior

- Reusing the already-created output root failed with
  `refusing to overwrite runtime package`.
- A nonexistent product executable failed before creating the requested output
  root with `product executable does not exist`.
- The script contains the corresponding per-asset missing-source guard
  (`required source asset does not exist`).  No production asset was moved,
  renamed, deleted, or intentionally made missing to test that guard.

## Result

The executable-relative package contract is source-shaped and bounded.  S3
may now verify the whole ledger's final dispositions and its relationship to
the staging selection; it does not need a guest rebuild or a runtime trace.
