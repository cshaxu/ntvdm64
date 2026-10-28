# Win16 multimedia and MCI/OLE family: selected sources and package

Eight shard-2 pending entries match both original editions. The prior `win16-mciole-guest-review.md` and `win16-mmsystem-guest-review.md` capture historical recipe/source relationships. The companion observer adds a targeted check on timer implementation selection and scans all `assets/winnt.zip` members by size and SHA-256 for the retained MMTASK.TSK, including renamed files.

| Entry | Evidence and limited recommendation |
| --- | --- |
| `wow16/mciole/libinit.asm` | Historical MCIOLE makefile selects libinit.obj. Candidate **historical guest build source**, not OLE behavior exclusion. |
| `wow16/mciole/mciole16.rcv` | Included by the historical resource script. Candidate **historical guest build resource**, not OLE resource/runtime proof. |
| `wow16/mciole/MCIOLE16.DLL` | Source-tree image matches both originals and ZIP member. Candidate **partial package identity**; the direct installed System32 path was absent at capture, so real loader resolution and OLE lifetime remain unverified. |
| `wow16/mmsystem/messages/usa/mci.rc` | Main recipe selects/copies USA resource in default branch. Candidate **historical guest resource input** only; alternate LANG configuration and actual language behavior remain open. |
| `wow16/mmsystem/messages/usa/mmtask.rc` | Default MMTASK sub-build copies USA resource. Candidate **historical guest resource input** only. |
| `wow16/mmsystem/mmsystem.rc` | Includes MCI resource and names MMTASK. Candidate **historical guest resource input**; no task launch or live resource behavior inferred. |
| `wow16/mmsystem/mmtask/mmtask.asm` | MMTASK sub-build selects this task source. Candidate **historical guest build source**; not evidence that the task was started. |
| `wow16/mmsystem/timea.asm` | Defines real timer APIs, but observed main recipe explicitly compiles `time.c` as `time.obj`, and retained map has `time.obj(time.c)` lines and timer entry names. Candidate **unselected alternate source in that particular recipe**, preserving timer behavior and alternate profiles; map-to-released-image provenance and exact semantic equivalence are unproved. |

Current source-tree MMSYSTEM.DLL matches ZIP member; MCIOLE16.DLL likewise matches ZIP. Both direct installed System32 files were absent in this capture, which does not rule out other loader paths. Two retained MMTASK.TSK copies match each other; no ZIP member is byte-identical, including under another name, and no entry has that basename. This narrows an earlier filename-only observation for this archive but does not prove the task cannot be provided elsewhere or that the application loads it. No firmware or guest was rebuilt or executed, no shared ledger/build/product changed.

Replay `PYTHONDONTWRITEBYTECODE=1 python artifacts/mvdm-audit/shard-2/multimedia-family-owner-observer.py` and compare JSON; replay both previous family observers for detailed historical source/recipe checks.
