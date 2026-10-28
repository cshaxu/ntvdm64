# Part 4: MVDM original files and selected-build boundary

The per-key `mvdm-evidence.json` records 120 assigned paths, SHA-256 against three source trees, direct graph edges and normalized VALID dependencies from four captured Ninja graphs. All 120 match the two primary original trees; identity is not evidence that a particular behavior runs. Reproduce by running `python -B artifacts/mvdm-audit/nine-way/part-4/mvdm-observer.py`, with `PYTHONDONTWRITEBYTECODE=1`; capture stdout separately. Per-key disposition remains pending until its semantics and runtime ownership are established.

## Functional groups

* CVIDC declarations and `sascdef.c`: no exact-path source build edge in captured graphs. The selected CCPU implementation is a different path; x86 PROD uses generated `sas4gen.h`, so absent original CVIDC edge is not proof that the original vector contract is irrelevant. See `shard-3/sas4gen-vector-audit.md` for the actual selection, relocation and unverified vector indices. No alternate configuration or runtime vector coverage is implied.
* Debug: several exact sources have compilation edges, but selected source differs from executable behavior. The separately reviewed `dasm.c` and `ega_dump.c` objects have no executable section under captured PROD; `profile.c` contains fallback code but no map owners, whereas `trace.c` retains data symbols in the x86 release map. See `shard-2/base-debug-objects-review.md`. Other debug edges require their own guard, archive extraction and runtime verification.
* Disk GFI: `gfi_test.c` compiles under PROD with its debug helper bodies disabled. The genuine provider dispatch is separately registered via `nt_rflop.c`; diagnostic success stubs are not disk implementation. See `shard-3/gfi-debug-shim-audit.md`. Captured dependency and source evidence does not establish real floppy operation.
* Base headers: exact VALID dependencies in the captured graphs establish inclusion in those objects only, not link inclusion or behavior. Notably `j_c_lang.h` is observed in a fixture rather than a production object. For individual paths and object owners see JSON; no graph hit does not prove another configuration is unused. Existing header cross-check: `shard-1/softpc-header-family-review.md`.

## Unresolved boundary

These are source identity and bounded build observations, not full semantic restoration. Release map binding, callable behavior, failure/lifetime paths, and other configurations remain unproven unless separately covered by the cited evidence. This audit has not changed guest, product or shared ledger.
