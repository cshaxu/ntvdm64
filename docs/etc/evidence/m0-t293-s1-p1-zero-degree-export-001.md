# M0 T293 S1 P1 — complete zero-degree definition export

## Question

Can the complete current `mvdm-host` component be represented as distinct
original-source function definitions, with every direct non-zero call recorded
without inspecting the callee body?

## Inputs and method

- The live MVDM file ledger provides selected original paths, hashes and final
  file dispositions.
- `src/mvdm-host` supplies path membership only; its bytes are not parsed.
- `export-t293-s1-zero-degree-function-ledgers.mjs` masks comments, literals
  and directives; accounts for mutually exclusive preprocessor branches while
  locating bodies; preserves same-spelling definitions by path/hash/line; and
  records direct named calls only.
- Macro spellings are collected from both approved original OpenNT trees so
  forms such as `NT_SUCCESS` and `RtlCopyMemory` do not become fictitious
  first-degree functions.

## Result

- All 522 component C/C++ files have a source-coverage row: 342 contain one
  or more definitions and 180 contain none.
- The zero-degree ledger has 4,499 distinct definition identities. There are
  no duplicate IDs or path/hash/line/symbol identities; 94 definitions are
  file-static and remain distinct from same-spelling definitions elsewhere.
- Every definition has a leaf status and a function-level final disposition,
  inherited explicitly from its already audited original file: 1,278
  adapter-backed, 1,248 overlay-required, 1,498 not-host-runtime and 475
  tool-only. No runtime behavior changes.
- There are 2,706 first-degree call-site candidates. 1,065 are already known
  public-modern-API leaf candidates; 1,641 require the next degree's source
  audit. 353 retain an explicitly ambiguous selected-definition spelling
  rather than being wrongly merged by name.

## Interpretation and limit

This P completes the structural zero-degree export, not the full S closure.
Macro/function-pointer forms make the containing function conservatively
non-leaf until their target or non-call status has source evidence. T293 does
not inspect any candidate function body, package, header provider or adapter.
The next P reviews the function-level disposition/leaf projection and parser
edge cases before the S can close.

## Verification

The export assertions prove coverage cardinality 522, definition cardinality
4,499, candidate cardinality 2,706, no missing function disposition/leaf,
no leaked already-resolved zero-degree target, and no duplicate candidate ID.
