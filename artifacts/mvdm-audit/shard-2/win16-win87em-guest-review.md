# Shard 2: Win87em historical guest build profiles

The two pending entries match both original trees and frozen catalogs; the observer and JSON retain source and image hashes. No guest was executed or rebuilt.

| Entry | Evidence and bounded disposition |
| --- | --- |
| `emds.asm` | Original makefile explicitly assembles `emds.obj` and includes it in `emmt.lib` and `87mt.lib`; it is also listed among emulator source dependencies. Candidate historical guest-build source for those specific library profiles only. The DLL recipe lists `win87em.obj` and `emoemwin.obj`; it does not name `emds.obj` directly, so the `EMASM` dependency list alone does not prove direct DLL linkage or execution. |
| `emoemwin.t1` | Matches both originals and contains assembly-like source text, but an actual consumer of this exact `.t1` file was not established. The original DLL recipe builds `emoemwin.obj` from `emoemwin.asm`, a different file. Retain pending. |

`src/mvdm/bin86/i386/win87em.dll` matches ZIP `WINNT/SYSTEM32/WIN87EM.DLL` at `30D6B965D45EDDF306268C9526B01494DF4841B8B530BAB2654C379E1CF49DB2`. Direct `O:/winnt/system32/WIN87EM.DLL` was absent at inspection; other search locations remain possible. Image identity does not prove source build provenance, actual guest loading or floating-point instruction, exception and state behavior. These capabilities remain separately scoped.

Replay `python artifacts/mvdm-audit/shard-2/win16-win87em-guest-observer.py` and compare the JSON. No guest, product, installation, shared ledger or build tree was modified.
