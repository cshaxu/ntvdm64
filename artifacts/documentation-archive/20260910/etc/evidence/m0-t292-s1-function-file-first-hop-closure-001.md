# M0 T292 S1 — function and file-level first-hop closure

## Question

Does every source-aware direct-call symbol from the selected MVDM host union
now have a function-level disposition, and do all non-public first-hop source
definition candidates have a hash-pinned file record?

## Inputs and method

- The 1,379-symbol semantic first-hop ledger and the 1,689-row selected MVDM
  file ledger.
- Both approved local OpenNT trees, used only as original-source evidence.
- `tools/governance/export-t292-s1-function-file-closure.mjs`.

The generator scans C/C++ definition candidates after masking comments,
strings and preprocessor directives. It records every candidate function body
as an exact original path, line and SHA-256. Public Win32/CRT calls retain their
historical declaration/definition evidence per function but deliberately do
not turn same-named historical implementations into fake package edges.

## Results

- All 1,379 semantic symbols have exactly one function-level disposition; the
  function and semantic ledgers have identical cardinality and zero resolution
  errors.
- 858 symbols terminate at a modern public Win32/CRT binding; 420 occur only
  in files whose final disposition excludes them from the host runtime; and 13
  are macro/function-pointer contracts rather than functions needing import.
- The remaining private, native, CSR, kernel-VDM, Redirector, Console, WOW,
  VDD/GDI and named non-MVDM cases have a source-shaped boundary owner.
- 54 exact, hash-pinned non-public definition-candidate file rows remain.
  These are the real one-degree file frontier: for example NTDLL CSR/currdir,
  NT kernel event/VDM contracts, Base VDM, Console, Redirector and WOW source
  files. They are evidence only; none is imported, linked or enabled.
- Of the two named non-MVDM generic package frontier symbols, one has an exact
  C/C++ provider candidate and one remains a declaration-only boundary. Both
  are explicit, not unresolved.

## Interpretation and limit

This is a completed **one-degree function/file closure**, not a package BFS.
The file ledger intentionally records only non-public/private boundary files;
recording every historical CRT or USER implementation sharing a public symbol
would fabricate hundreds of false package dependencies. A later task may
expand any of the 54 hash-pinned records one package at a time, preserving the
same owner/disposition standard.

## Verification

The generator output was checked for 1,379 function rows, matching semantic
cardinality, zero `stage-one-resolution-error` rows, and a non-empty SHA-256
on every file row. Documentation governance and `git diff --check` are run
before closure.
