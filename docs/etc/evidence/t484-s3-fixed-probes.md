# T484 S3 P187: shared fixed-profile guest probes

Five direct NASM call sites are replaced by the existing native fixture graph.
The shared WOW caller now accepts only its three named probes; the arbitrary
source argument and its three callers' redundant paths are removed. Six fixed
images share guest-api. DLC reuses the existing guest-redir image instead of
introducing another source recipe. All sources are independently authored.

| Retained probe | Actual regression contract |
| --- | --- |
| EMBTEST | Embedding enumeration return, registry output and explicit historical stall comparison |
| KERNTEST | Kerning requested/returned count, guest error record and normal exit |
| PALTEST | Palette requested/returned count, guest error record and normal exit |
| FCBTEST | Three unique names, end/missing status and direct completion receipt |
| MSTATE | INT33 state save/restore and invalid checksum/cookie rejection |
| CURSOR | Register readback and three ordered unchanged-text frames |
| VDMDLC | Original provider-unavailable status07 with explicit no-provider prerequisite |

These remain focused regressions, not evidence that all are invoked by Full.
Only production of the authored images changes; scenario assertions, timeout,
cleanup and runtime preparation are unchanged. The shared staging dependency
remains open, including obsolete default build paths in some focused callers.
This batch does not claim to have fixed or reaccepted those runtime defaults.

## Verification

Owned run `build/t484-s3-fixed-probes/run.ps1` records seven original-command
byte comparisons, native no-op cache checks and all eight edited PS callers'
parse/direct-compiler checks. `images.json` records actual compared hashes.
The original NASM invocation is only the equivalence oracle. No fresh guest
runtime acceptance is inferred from these preparation checks.

`delivery-result.json` in the same directory records cached full compilation
and11 raw/22 published host hash checks. Unchanged products retain their existing
publication. Final documentation/ledger checks are in `closure-result.json`.

Fixed census316/355 disposed,39 pending is unchanged. P183's18 additional NASM
scripts now have nine handled producer sites and nine pending reconciliation.
There are ten supplemental caller chains with native producers but pending
shared runtime preparation; the independently retired observer builder is not
counted as one of those ten. S3 remains active.
