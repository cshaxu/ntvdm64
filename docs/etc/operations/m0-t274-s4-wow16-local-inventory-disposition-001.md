# M0 T274 S4 WOW16 local-inventory disposition

## Observed local inventory

- `src.old/opennt-guest/wow16`: 44 files, 282,617 bytes; 42 `.fon` files and
  two `.inc` files.
- `src.old/opennt-guest/bin86`: 10 files, 340,995 bytes; binary/product and
  support forms only.
- `build/output/wow16`: 87 files, 2,299,306 bytes; retained compiled
  executables, DLLs, drivers, fonts and related guest products.

## Selected-union comparison

The S1 ledger has 1,273 `opennt-guest-wow16` rows. Only 12 have the
`carry-forward-local-guest-mirror` disposition; 1,261 are
`external-guest-source-outside-current-carried-inventory`. The pinned external
WOW roots contain 1,201 and 1,254 files respectively, confirming that this is
not a naming or enumeration error.

## Superseded initial interpretation

The initial pause incorrectly treated source-cardinality completeness as a
prerequisite for the straightforward carry-forward task. The owner clarified
that S4 must directly recover the files already present and retain the existing
outputs; it does not compile, supplement or claim an external source mirror.

## Active disposition

S4 copies the 54 local WOW16/bin86 files into `opennt-guest-wow16`, hashes the
87 existing output products in place and keeps all 1,261 external-only source
paths outside this task. The carried source count is reported accurately, but
does not block the defined load-only guest inventory.
