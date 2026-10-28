# Shard 1: pending WOW C/assembly constants and actual dependencies

The observer verifies these four entries are `待核验` in the current shard ledger and match both frozen OpenNT editions. It scans VALID Ninja dependency records from four existing x86 worker/WOW32 and x64 frontend/service trees, resolving relative paths against the respective build directory. The JSON records each exact dependency hit, object, source hash and ledger hash.

| File | Role and current evidence |
| --- | --- |
| `inc/WOWCMPAT.INC` | Original 16-bit assembly compatibility-flag constants; literal include sites include Win16 kernel `3ginterf.asm`, `3galloc.asm`, `winexec.asm`, `lstring.asm`, and `context.asm`. No exact VALID hit in the four **host C** graphs does not prove current guest build or runtime selection. |
| `inc/wowcmpat.h` | Matching host C compatibility-flag definitions included by WOW32 and the original command path. Normalized records identify 89 VALID dependency records across the four checked trees, including worker `obj/command/cmdexec.obj`; do not classify this as unused. |
| `inc/WOWUSR.INC` | Original 16-bit assembly USER thunk ordinals and structures; literal include sites include USER and SYSTEM source. No exact VALID hit in the four host C graphs does not imply guest inactivity. |
| `inc/wowusr.h` | Matching host C USER identifiers and structures, included by WOW32 `precomp.h`. Normalized records identify 88 VALID dependencies in the checked graphs; it is an actual host compiler input. |

The observer compares all **52 unique numeric WOWCF/WOWCFEX definitions** and all **455 unique numeric FUN definitions** across each corresponding ASM/C pair: the names and literal values agree. Two FUN names occur twice with the same value in both original files. This is numeric-definition evidence only; it does not validate structure sizes, packing, every usage, selected guest binary or successful API calls.

The original USER table has 539 rows. Comparing those 455 names/ordinals to table display labels yields 16 differing labels, including aliases, blank entries and names used in contexts other than direct table dispatch. For example `FUN_DEFDLGPROCTHUNK` shares 534 with `FUN_WOWGETIDFROMDIRECTORY`, while original `walias.c` uses the former as a class procedure ordinal; name difference alone is not a proven dispatch conflict. The JSON records all 16 for targeted review; source-to-guest reachability and any actual mismatch remain unproved. The released WOW32 executable sections match the map-producing cached image, but this audit does not bind each table slot or claim API restoration.

These four remain pending for runtime/ABI classification. Replay `python artifacts/mvdm-audit/shard-1/wow-cross-language-constants-observer.py`. No guest, product, compiler, network or provider was run, and no guest, product, build tree or shared ledger was modified.
