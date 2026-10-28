# Shard 2: Win16 multimedia guest resource and task ownership

Five entries match both original source trees and the frozen catalog; the observer and JSON record the hashes and original build edges. No runtime, guest build or installation change was performed.

| Entries | Evidence and bounded recommendation |
| --- | --- |
| `messages/usa/mci.rc`, `mmsystem.rc` | Original main makefile selects `messages/usa` and copies `mci.rc`; `mmsystem.rc` includes that resource. Candidate exclusions as immutable guest resource build inputs, specifically for this original recipe. This does not establish the effective language in a deployed run. |
| `messages/usa/mmtask.rc`, `mmtask/mmtask.asm` | The main makefile calls the MMTASK sub-build; its default non-`LANG` branch copies the USA resource and compiles the task object. Candidate exclusions as historical guest build inputs only; alternate international branches are separate. |
| `timea.asm` | Identical to the originals, but the inspected main object list names `time.obj`, not `timea.obj`; no exact selected consumer was established. Remains pending. |

The original resource identifies `mmtask.tsk` and the main build lists it as a prerequisite and binplace output. In the captured package, the direct `WINNT/SYSTEM32/MMSYSTEM.DLL` member matches the source-tree DLL; direct `O:/winnt/system32/MMSYSTEM.DLL` was absent. No direct `WINNT/SYSTEM32/MMTASK.TSK` ZIP member or `O:/winnt/system32/MMTASK.TSK` was present. These are exact-path observations only; they do not establish all possible loader or package paths, actual task launch, or that the source built the binaries. Multimedia callbacks, timing, task lifecycle and available devices remain unverified. The previous `guest-loader-location-audit.md` discusses loader search limitations.

Replay `python artifacts/mvdm-audit/shard-2/win16-mmsystem-guest-observer.py` and compare the JSON. Classification concerns individual historical build files, never removal of guest capability. No product, guest, installation, shared ledger or build tree was modified.
