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

## Disposition

The owner instruction prohibits silently repeating an external source import,
and the source-supply proposal requires a complete carried guest mirror before
declaring S4 closed. Therefore S4 is not admitted and no external WOW source is
copied. The existing local source/products remain intact and `build/output/*`
remains in place.

## Required owner choice

Either declare the 54-file local WOW16/bin86 carry plus the retained output
products as the intended bounded guest inventory, or explicitly authorize a
package-scoped external WOW16 source recovery. Until then, a claim of complete
WOW16 source recovery would be false.
