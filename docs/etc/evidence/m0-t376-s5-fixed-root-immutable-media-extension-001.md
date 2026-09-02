# M0 T376 S5 — Fixed-root immutable-media extension

## Result

The established `O:\ntvdm64` runtime package was extended through the staging
tool's explicit `--update` operation.  The operation verified each existing
destination against its source byte hash, retained matching bytes, copied only
the formerly absent original `FASTOPEN.EXE`, and rewrote the format-3 manifest
after successful verification.

The resulting identities are:

- product `original-softpc-process.exe`:
  `1b1fc86d15524bf269ff3a770e0f2ec85c85e33cf42ba21389dc33b105262c2f`;
- staged `FASTOPEN.EXE` and its source:
  `69dabbdb754b358ac4fe4b22de04c0e4c93076816f14bb0730caa9fd223996fc`;
- runtime manifest: `format=3`, `mediaAssets=18`, SHA-256
  `7bfc49d5e790f17dbac85b284a21078a9d5856204c3ec872b0402ac700dfaed6`.

## Positive operation

```text
node tools/build/Stage-OriginalSoftpcRuntime.mjs \
  --executable build/M0-T371/S2/formal-x86/original-softpc-process.exe \
  --output O:\ntvdm64 --update
```

## Refusal proof

The same operation with the observer executable substituted as the product
refused before writing:

```text
Error: refusing to replace different staged asset:
O:\ntvdm64\original-softpc-process.exe
```

This is the required no-overwrite negative case.  It neither changes the
product nor tests guest execution.

## Source boundary

`Stage-OriginalSoftpcRuntime.mjs` is app-owned package composition.  Its
`--update` mode has no guest or SoftPC semantics: it only permits a missing
source-identical file, rejects a differing destination, and preserves the
same format-3 product/media manifest contract.  The original MZ bytes remain
the selected guest artifact.  S6 owns the sole frozen MZ run.
