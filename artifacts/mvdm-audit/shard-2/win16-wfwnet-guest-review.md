# Shard 2: Win16 WFWNET guest build ownership

The five pending files match both original trees and the frozen catalog. The observer and JSON retain hashes, build references and target-image identity. Recommendations below concern individual immutable guest build inputs only, not removal of network or print capabilities.

| Entry | Original role and bounded recommendation |
| --- | --- |
| `bseerr.h` | Original `wfwnet.c` includes `locals.h`, which includes this header; the historical makefile lists it as a dependency of linked WFWNET objects. Candidate guest build input; include search can change when external `INCLUDE` is supplied. |
| `libentry.asm` | Historical makefile links `libentry.obj`; the source entry invokes `LocalInit` when needed and calls `LibMain`. Candidate guest build source; behavior of the shipped driver is not inferred. |
| `wfwnet.c` | Original makefile compiles and links `wfwnet.obj`; source describes WNet entry points and holds function pointers. Candidate guest build source, not proof that providers resolve or calls work. |
| `wfwnet.rcv` | Original `wfwnet.rc` includes it, and original recipe compiles/applies `wfwnet.res`. Candidate guest resource input. |
| `WFWNET.RES` | Matches both originals and is named in the original driver link/resource recipe. Candidate historical guest resource artifact; this does not establish exact generation or image provenance. |

`src/mvdm/wow16/wfwnet/wfwnet.drv`, ZIP `WINNT/SYSTEM32/WFWNET.DRV`, and installed `O:/winnt/system32/WFWNET.DRV` match at SHA-256 `4C321D43511F845EC1A95518507133D1CF4CF3993BAE422037DBA95CE6258CCA`. A CMake source search found no direct listing of these guest build sources, but that search alone is not a proof of every generated dependency; coordinator should reconcile against the current selected build graph. Binary identity does not prove the original recipe produced it or that a running guest loaded it. Network-provider resolution, printing, error propagation and callback/handle teardown remain untested here.

Replay `python artifacts/mvdm-audit/shard-2/win16-wfwnet-guest-observer.py` and compare the JSON. No guest, product, installed file, shared ledger or build tree was changed or run.
