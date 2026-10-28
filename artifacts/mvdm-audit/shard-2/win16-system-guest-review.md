# Shard 2: Win16 SYSTEM guest build variants

Six shard entries are captured in the accompanying observer and JSON. Five match both original trees; `system.lrf` is absent from both originals and from both frozen catalogs. No guest, production binary, shared ledger or build tree was changed or executed.

| Entries | Evidence and limited recommendation |
| --- | --- |
| `romsys.def` | Original `romsys` alternate recipe names this module definition. Candidate exclusion as a historical guest build input for that variant; it is not proof the deployed image uses the ROM variant. |
| `system` | Original alternate recipe names `system.def`, `system.lnk`, `system.obj`, and `timer.obj`, linking `system.drv`. Candidate exclusion as a historical guest build recipe only. |
| `system.rcv` | Original `system.rc` includes it; the original makefile uses the compiled resource in the SYSTEM driver. Candidate exclusion as a guest resource build input only. |
| `vecsys.inc` | Original `envect.asm` includes it, and alternate `hpsystem` recipe names that object/input. Candidate exclusion as a historical guest build input for that variant only. |
| `windefs.inc` | Matches both originals, but the inspected `system.inc` reference is commented out; the active source consumer and include search resolution were not established. Remains pending. |
| `system.lrf` | The original makefile has a recipe generating this link response file, but the current file has no counterpart in either original tree or frozen catalogs. Its exact generation and supplemental-file disposition remain pending. |

The source-tree `system.drv`, `assets/winnt.zip` member `WINNT/SYSTEM32/SYSTEM.DRV`, and `O:/winnt/system32/SYSTEM.DRV` have identical SHA-256. This demonstrates image identity only, not that the inspected sources or a specific recipe produced it, nor the loaded path and behavior in a running guest. SYSTEM timer, callback and teardown behavior remain unproven. Guest build input classifications do not eliminate any runtime capability.

Replay `python artifacts/mvdm-audit/shard-2/win16-system-guest-observer.py` and compare with the accompanying JSON. No historical build or cleanup recipe was invoked.
