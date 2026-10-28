# Shard 2: DOS current-directory shared declarations

Two pending entries match both original trees. The observer and JSON capture their hashes, original guest references and current host dependency evidence. Ninja dependency entries were resolved from each actual worker/WOW32 build directory before exact-path comparison.

| Entry | Evidence and bounded disposition |
| --- | --- |
| `inc/curdir.h` | Normalized dependencies of current `obj/dem/demfile.obj` and `obj/wdos.obj` both point to this file. `wdos.obj` has mapped `GetCurrentDir` in the captured WOW32 image. Candidate **partial**: selection does not prove current-directory synchronization, per-task identity, guest memory validity or failure behavior. |
| `inc/CURDIR.INC` | Original DOS `mscode.asm` and `path.asm` include it, and their objects appear in the original DOS kernel response; original include makefile describes generation from `curdir.h` and `bop.h`. Candidate historical guest build input **subject to complete current build/profile consumer review**. Its exact generation provenance and deployed guest linkage are not proved. |

The C and assembly declarations contain corresponding current-directory field and flag names, but this review did not prove identical offsets, packing, numeric values in all configurations or a running guest/host synchronization. Source-level resemblance cannot substitute for an ABI test. The assembly include has other original guest consumers; a single Ninja dependency scan cannot prove it absent from all product build profiles. Existing source-to-image lineage and current directory ownership remain separate questions.

Replay `python artifacts/mvdm-audit/shard-2/curdir-abi-observer.py` and compare its JSON. Captured graph/map hashes can change after a build. No guest, product, installation, shared ledger or build tree was modified.
