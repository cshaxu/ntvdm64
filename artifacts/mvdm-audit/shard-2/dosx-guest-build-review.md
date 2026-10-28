# Shard 2: DOSX guest build profiles and inputs

Twelve entries were selected from current shard-2 pending status. The companion observer/JSON capture original and current SHA-256, original link and build rules, and delivered image identity. Original makefile and response files are inspected without executing them.

| Entries | Evidence and bounded recommendation |
| --- | --- |
| `486/dxintr.asm`, `dxini.asm`, `dxmmgr.asm`, `dxnetbio.asm`, `dxoem.asm` | Original makefile explicitly assembles corresponding objects; both generic and x86 link response files name the objects. Candidate historical guest build sources only, not claims about original source-to-deployed-binary lineage or runtime semantics. |
| `ntnpxem.asm` | Its object is selected in the x86 conditional and named by `dosxi.lnk`; it is absent from the generic response. Candidate historical guest build source for the x86 profile only. |
| `usa/dxmsg.asm` | Original recipe defaults COUNTRY to `usa`, copies that path as `dxmsg.asm`, and links `dxmsg.obj`. Candidate historical localized guest build source for this default only; alternate country inputs remain separate. |
| `mk.cmd`, `migrate.bat` | Original historical build/migration scripts. Candidate historical scripts only; neither was executed, and file placement alone does not prove a deployed image was built by either script. |
| `dosx.def` | Original makefile lists it as a DOSX target prerequisite, but the inspected response-generation/link command does not explicitly identify its consumption. Remains pending. |
| `dxhpbios.asm` | An explicit object rule exists, but that object is absent from the inspected x86 link response; other profiles may differ. Remains pending. |
| `dxstrt.asm` | The observed makefile rule builds `dxstrt.obj` from `486/dxstrt.asm`, not the same-name root file. The root file's selected consumer remains pending. |

The source-tree `dpmi/486/dosx.exe`, ZIP `WINNT/SYSTEM32/DOSX.EXE`, and installed `O:/winnt/system32/DOSX.EXE` match SHA-256 `C5AF29A29ABF167B243DAABF877459E8278B8C9A339BF8E1E2576EAD5F6CEEFF`. This confirms image identity only. Historical selection uses generic `dosx.lnk` versus x86 `dosxi.lnk`; it does not prove the checked source generated that image. Real/protected mode transitions, exception handling, memory and NetBIOS paths, callbacks and teardown remain independently scoped; original guest build-source disposition never eliminates those capabilities.

Replay `python artifacts/mvdm-audit/shard-2/dosx-guest-build-observer.py` and compare its JSON. Coordinator must check the current generated build graph separately before merging role recommendations. No guest, product, shared ledger, installed file or build tree was modified.
