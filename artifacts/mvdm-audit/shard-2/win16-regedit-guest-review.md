# Shard 2: Win16 REGEDIT guest source and image roles

Three shard entries were checked against both original trees, original build references and package paths. The observer and JSON capture reproducible identities; no guest was executed or rebuilt.

| Entry | Evidence and bounded recommendation |
| --- | --- |
| `reg1632.h` | Matches both originals. Original `regporte.c` includes it and calls registry functions; the original makefile compiles `regporte.obj` as a common object in REGEDT16 and REGLOAD links. Candidate exclusion as immutable guest build input only. Actual registry semantics and lifecycle remain separate. |
| `sdkreged.def` | Matches both originals and declares an SDKREGED module, but the inspected REGEDT16/REGLOAD makefile links different `.def` files and no actual consumer of this definition was established. Remains pending; do not infer non-use in other variants. |
| `debug/REGEDT16.EXE` | Current-only and absent from both original catalogs; the makefile describes debug output but does not establish this file's provenance. No ZIP member named REGEDT16.EXE or direct `O:/winnt/system32/REGEDT16.EXE` was observed. Retain as supplemental pending; other paths and renamed images were not ruled out. |

The original build also includes conditional `LANG`, debug and separate REGLOAD branches, so the file roles above are tied to the observed recipe. Source references do not prove a particular loaded binary derives from those inputs or that its registry facade behaves correctly. The previously recorded `../shard-3/guest-support-owner-review.md` covers other REGEDIT source files independently.

Replay `python artifacts/mvdm-audit/shard-2/win16-regedit-guest-observer.py` and compare its JSON. No product, guest, installation, shared ledger or build tree was changed.
