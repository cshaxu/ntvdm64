# M0 T299 S4 — first-degree translation-unit closure

## Rule closed

First degree is the reachable closure over physical original source files
entered by zero-degree calls. A definition reached in the caller's file, or
an externally-linkable definition in another already selected first-degree
file, stays first-degree. A `static` definition never crosses its own file.
Only an edge leaving that selected file set is second-degree.

## Reproducible result

Run:

```powershell
& O:\.nvm\versions\node\v22.22.1\bin\node.exe tools\governance\export-t299-s1-first-degree-original-ledgers.mjs
```

The generated ledgers record:

- 2,821 raw T298 candidates, of which 2,819 remain external first-degree
  calls after T298's two zero-degree reclassifications.
- 2,819 mapped external call identities: 358 concrete original mappings,
  1,443 individual `missing-original-definition` rows, and 1,157 public
  system-provider rows.
- 152 physical first-degree definitions across 25 physical original files.
  Provenance is cumulative: 76 direct zero-degree entries, 72 same-file
  helper entries and 75 calls to another selected first-degree file.
- 762 cross-file second-degree candidates. The generator asserts that none
  resolves in its caller file or to an externally-linkable definition in any
  selected first-degree file.

Same spelling, signature or bytes never collapses two source identities. A
direct call whose original body was not found remains a per-call missing row;
it is not merged with another similarly named call or promoted to second
degree.

## Inputs and exclusions

Only the two original OpenNT source roots and the frozen T298 ledgers are
inputs. Current mirrors, adapters, `src.old`, build inputs and runtime routes
remain excluded.
