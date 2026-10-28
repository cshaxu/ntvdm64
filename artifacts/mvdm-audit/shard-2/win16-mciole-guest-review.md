# Shard 2: Win16 MCIOLE guest ownership

Three pending files match both original trees and the frozen catalog. The companion observer records source and package identities and can be independently replayed without running or rebuilding guest code.

| Entry | Evidence and limited recommendation |
| --- | --- |
| `libinit.asm` | The original makefile assembles `libinit.obj` and links it with `mciole.obj` for MCIOLE16.DLL. Candidate exclusion as a historical immutable guest build-source file only. |
| `mciole16.rcv` | Original `mciole16.rc` includes this version resource; the original makefile applies the corresponding compiled resource. Candidate exclusion as a historical guest build input only. |
| `MCIOLE16.DLL` | Byte-identical to both original trees and archive `WINNT/SYSTEM32/MCIOLE16.DLL`. Candidate partial on identity; direct `O:/winnt/system32/MCIOLE16.DLL` was absent at inspection. This alone does not establish all possible guest loader search locations. |

The original module definition describes an OLE handler for MCI objects, exporting creation/load entry points and importing OLECLI defaults and an MMSYSTEM symbol. This identifies the intended guest link surface, not a proven live caller, resolved binary or successful OLE/media functionality. Nor do makefile edges establish that this exact prebuilt image was produced from the reviewed source. The previously audited local `olecli.lib` link input must remain distinct from the similarly named WRITE input. Guest-source exclusions preserve all such behavior and do not assert it is complete.

Replay `python artifacts/mvdm-audit/shard-2/win16-mciole-guest-observer.py` and compare with its JSON. No installed files, product, guest, build tree or shared ledger were modified.
