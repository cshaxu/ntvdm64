# Win16 SYSTEM driver family: NE exports and build variants

This grouped update covers six pending shard-2 files, extending `win16-system-guest-review.md`. Five match both original trees; `system.lrf` is current-only. The source-tree, ZIP and installed SYSTEM.DRV are byte-identical. The new observer parses the shipped NE resident/nonresident name tables and confirms names and ordinals: InquireSystem 1, Create/Kill/Enable/DisableSystemTimers 2–5, GetSystemMsecCount 6, 80x87 operations 7–9, WEP 10 and A20_Proc 20. These are export metadata only, not evidence that a caller loads or executes the functions.

| File | Evidence and suggested restricted disposition |
| --- | --- |
| `romsys.def` | Historical alternate romsys recipe names it. Candidate **historical alternate guest build metadata** only; the installed module's variant and functional equivalence remain unproved. |
| `system` | Historical alternative build recipe explicitly links system/timer objects. Candidate **historical recipe** only; never execute its clean/build steps for this audit. |
| `system.lrf` | Current-only link response file; historical makefile contains a generation rule with system.obj/timer.obj, but exact provenance remains unknown. **Pending**. |
| `system.rcv` | Included by system.rc, which historical makefile compiles and applies to driver. Candidate **historical guest resource input**, not a resource-byte or build-provenance claim. |
| `vecsys.inc` | envect.asm includes it, and alternate hpsystem recipe compiles envect.obj. Candidate **historical alternate-profile guest build input**; Vectra behavior remains separate. |
| `windefs.inc` | The inspected system.inc line is commented; active exact consumer/profile remains unresolved. **Pending**. |

Published names/ordinals demonstrate the shape of the retained driver interface; they do not establish timer resolution, callbacks, race handling, teardown, actual guest load path, or original recipe-to-image linkage. Do not conflate SYSTEM.DRV with TIMER.DRV or exclude timer functionality from file-role classification.

Replay `PYTHONDONTWRITEBYTECODE=1 python artifacts/mvdm-audit/shard-2/system-ne-family-observer.py` and compare JSON; previous `win16-system-guest-observer.py` captures detailed original recipe references. No guest, product, shared ledger or build tree was changed or executed.
