# Shard 2: CTL3D guest reference files and package identity

Six pending entries match both original trees and the frozen catalogs. The companion observer records their SHA-256, relevant source edges and delivered-image identity. These are file-role recommendations only; no guest was rebuilt or executed.

| Entry | Evidence and bounded recommendation |
| --- | --- |
| `readme.txt` | Original describes the directory as debugging source and says the WOW DLL is binplaced from `bin86`. Candidate historical reference documentation. Its KnownDLL description is historical, not evidence of a particular current loader decision. |
| `ctl3d.doc` | Byte-identical to `new3d.doc`, already reviewed in shard 3 as historical CTL3D documentation. Candidate historical documentation; no document deletion is proposed. |
| `ctl3d.c` | Reference source describing 3D control behavior; the original readme says the actual DLL is separately maintained/binplaced. Candidate historical guest debugging/reference source, subject to coordinator checking the current product build graph; no conclusion about which source produced the DLL or the correctness of its controls. |
| `ctl3d.h`, `common.ver` | Explicit inputs of the retained `ctl3d.rc` reference resource; `ctl3d.h` is also included by `ctl3d.c`. Candidate reference/build-input roles, subject to actual consumer and configuration review; no assertion of deployed DLL resource generation. |
| `msdn.ico` | Identical to both originals, but an exact consumer/configuration was not established. Remains pending. |

The original resource and source have special WOW and other configuration branches; the readme's source description cannot prove which branch a particular binary used. `src/mvdm/bin86/ctl3dv2.dll` matches ZIP `WINNT/SYSTEM32/CTL3DV2.DLL` at `3387135A5075439B9238D6C486B047D4DBA8D9A6D1DAD6BB74050347C70616DB`; the direct installed `O:/winnt/system32/CTL3DV2.DLL` path was absent. Other module search paths remain possible. Actual control subclassing, drawing, input and teardown remain unverified. The preceding shard 3 `ctl3d-source-role-review.md` covers other inputs.

Replay `python artifacts/mvdm-audit/shard-2/win16-ctl3d-guest-observer.py` and compare its JSON. No guest, product, installation, shared ledger or build tree was modified.
