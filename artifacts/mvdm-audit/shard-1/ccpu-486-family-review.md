# S10 shard 1: CCPU 486 declaration and build-carrier family

Six pending `softpc.new/base/ccpu386` files were checked together. All match both reference editions by SHA-256. Full identities, exact direct textual include sites and normalized current worker VALID dependencies are in `ccpu-486-family-evidence.json`; replay `PYTHONDONTWRITEBYTECODE=1 python -B artifacts/mvdm-audit/shard-1/ccpu-486-family-observer.py`. Current build command was inspected with `ninja -C build/.ninja/msvc-x86-release/worker -t commands obj/ccpu/invd.obj` and contains CPU_40_STYLE and SPC386 but no SPC486. Dependency logs are not an exhaustive original or alternate build graph.

| File | Source/build role and disposition boundary |
| --- | --- |
| bswap.h | BSWAP declaration. Two textual include sites; exact current VALID dep is bswap.obj. Current bswap.obj has no `_BSWAP` symbol; header inclusion and source compilation do not establish enabled 486 instruction. |
| c-files | Original CCPU filename list; includes bswap.c and invd.c. It is not a compilation input in the observed dependency log and list membership alone does not prove executable selection. |
| ccpudefs.inc | Original build macro carrier used by ccpu386/sources; defines CPU_30_STYLE, CPU_40_STYLE, CCPU, SPC386, etc., but not SPC486. Current Ninja command is separate evidence of active macro set; do not infer 486 from CPU_40_STYLE. |
| gdpvar.h | Generated workspace/offset macro carrier, 15 basename include sites and one exact VALID dep sasCdef.obj. Other profiles have same-named variants and must be resolved by actual include path, not name alone. Runtime memory identity and bounds need separate validation. |
| invd.c | Original INVD provider, compiled by a direct Ninja source edge to invd.obj and included in archive. Body is guarded by SPC486 and intentionally empty when enabled if no simulated cache; current invd.obj has no `_INVD` symbol. Object compilation alone is not enabled semantics. |
| invd.h | INVD declaration, two textual include sites, exact VALID dep invd.obj. Current preprocessor profile excludes implementation; header selection does not establish executable code. |

Original c_main.c:1298-1308 selects INVD under SPC486, otherwise raises Int6; BSWAP has its own SPC486 gate. Prior `shard-3/cpu486-profile-boundary.md` and `ccpu-remaining-header-source-review.md` set out finite paired profile/dispatch/provider/fault work. No new CPU test, installed EXE identity binding, 486 support or full runtime semantics established here. The current worker build and installed NTVDM.EXE hashes differ; do not conflate their binaries. No guest, product, shared ledger or build tree modified.
