# M0 T276 S3 original textual interface-declaration evidence

## Scope and method

The extractor scans all byte-exact selected header/inc/gi mirrors for a bounded lexical `extern ... ;` form. It records each source occurrence with original file and line evidence. It neither preprocesses conditionals/macros nor resolves a declaration to a caller, library, adapter or available modern implementation.

## Counts

- Selected declaration files: 857
- Existing unexpanded interface-family rows retained: 12
- Lexical `extern` declaration candidates: 4803
- Candidates without an unambiguous lexical name: 0

### Candidates by declaration package

- `bde`: 13
- `dos`: 68
- `dpmi32`: 53
- `fax`: 1
- `ieuvddex`: 9
- `inc`: 309
- `softpc.new`: 4172
- `v86`: 23
- `vdmexts`: 11
- `vdmredir`: 7
- `vdmutils`: 1
- `wow32`: 134
- `xms.486`: 2

## Limits

- An `extern` declaration is not evidence that any selected translation unit reaches it.
- Macro-expanded declarations, static definitions, assembly exports, import libraries and call-site calling conventions require later package/caller analysis.
- The candidate ledger preserves raw declaration text to make S4 resolution auditable rather than silently guessing an adapter owner.
- No final composition classification, adapter requirement, x86/x64 build decision or enabled package follows from this extraction.
