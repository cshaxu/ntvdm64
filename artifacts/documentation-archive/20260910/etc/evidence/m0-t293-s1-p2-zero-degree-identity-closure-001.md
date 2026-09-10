# M0 T293 S1 P2 — zero/first-degree identity closure

## Correction

P1 deliberately retained 353 direct calls with same-spelling selected-source
definitions as first-degree candidates while the source-form resolver was
still being refined. That was conservative, but it did not yet meet the
owner's stronger rule: a call to a definition already inside the selected
`mvdm-host` corpus must remain zero degree, even when historical build
alternatives supply more than one mutually exclusive body.

P2 does not edit P1. It replaces the live generated ledgers using an explicit
source-context rule:

- unique source/linkage/package/directory targets bind to one zero-degree
  definition identity;
- 91 calls with historical conditional/product alternatives bind to an
  enumerated set of zero-degree conditional-variant targets;
- only a spelling with **no** selected zero-degree definition is emitted as a
  first-degree candidate.

The 91 conditional rows name all candidate path/line/definition identities.
They do not choose a runtime configuration, inspect an external body, or
silently merge variants. Examples are `TerminateVDM` (`nt_reset`, `nt_term`
and scaffold forms), `getIntelRegistersPointer` (monitor versus non-monitor
stub), and `LocalLock16`/`LocalUnlock16` (`WOWFASTEDIT` alternatives).

## Closed output

- 522 selected `mvdm-host` C/C++ source paths have exactly one coverage row:
  342 with definitions and 180 with no C/C++ function definition.
- 4,499 source-path/SHA-256/line/linkage definition identities have exactly
  one zero-degree row; 94 are file-static and are never merged by spelling.
- 2,353 direct external call-site candidates form the frozen first-degree
  input: 1,065 public modern API/CRT leaf candidates and 1,288 source-audit
  candidates.
- 91 explicit conditional-variant bindings remain in the zero degree and do
  not appear in the first-degree ledger.
- Every zero-degree definition has a leaf basis and an audited final recovery
  disposition inherited from its selected original file: 1,278 adapter-backed,
  1,248 overlay-required, 1,498 not-host-runtime and 475 tool-only.

## Verification

`export-t293-s1-zero-degree-function-ledgers.mjs` regenerated all four
ledgers from provenance-selected original source bytes. Assertions passed for
522 coverage rows, 4,499 zero-degree identities, complete leaf/disposition
fields, first-degree rows whose internal resolution is only
`no-selected-definition`, and conditional rows with non-empty enumerated zero
targets. No source import, adapter, runtime route, build input or `src.old`
input was introduced.

## Result

S1 is closed at the zero-degree boundary. T294 alone may inspect the bodies
of the frozen first-degree candidates; it must not reclassify a recorded
zero-degree conditional binding as a new external dependency.
