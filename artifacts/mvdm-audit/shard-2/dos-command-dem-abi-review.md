# Shard 2: DOS command, DEM and BOP interface declarations

Three pending declarations match both original trees. The accompanying observer records their hashes, original/current source edges and selected worker build evidence. Ninja dependency entries are resolved against the actual build directory before exact-path comparison.

| Entry | Evidence and disposition |
| --- | --- |
| `inc/cmdsvc.h` | Current `cmddisp.c` includes it, and normalized Ninja deps confirm the exact file for `obj/command/cmddisp.obj`. The worker map attributes `CmdDispatch` to that object; its source table selects the original command service handlers. Candidate **partial**; service-number identity does not establish every guest call, session, handle or error path. |
| `dos/dem/dosdef.h` | Current `dem.h` includes it; normalized deps confirm the exact file for `obj/dem/demfile.obj`. Candidate **partial** for selected declarations; DOS file attributes, sharing and failure semantics are not established merely by header selection. |
| `inc/BOP.INC` | Original/current `CMDSVC.INC` includes it and uses `BOP_CMD`, with value 54h also declared by `bop.h`. It is referenced by guest and other original assembly, including `wow32/i386/fastwow.asm`; an exhaustive selected-build/profile disposition was not established. Retain **pending**, rather than treating it as guest-only from one consumer. |

Prior `../shard-3/cmdsvc-abi-role-review.md` compares the command service constants and traces guest command request to the changed host BOP continuation. Current `cmddisp.c` places its service bounds check under `#if DBG`; a release request's upstream validation and reachability were not established here, so this is a boundary to investigate, not a demonstrated guest failure. DEM structure and error semantics likewise require independent behavior and lifecycle evidence. The current BOP continuation can have registered adaptations; these header identities do not establish whole-chain equivalence.

Replay `python artifacts/mvdm-audit/shard-2/dos-command-dem-abi-observer.py` and compare the JSON. Build/map hashes are a snapshot. No guest, product, installation, shared ledger or build tree was modified.
