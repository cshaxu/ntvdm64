# Shard 2: WOW32 dispatch declaration headers

Four files were selected from shard-2 pending status and match both original trees. The observer/JSON record source hashes, current Ninja compile edges and the captured WOW32 map; no build or product execution was performed.

| Entry | Evidence and bounded disposition |
| --- | --- |
| `wowtbl.h` | Current `precomp.h` includes it; it declares `aw32WOW`, `TABLEOFFSETS` and initialization. Current `wowtbl.c` defines the unified table and initializes offsets, while `wow32.c` copies that structure to guest memory. Captured object/map evidence includes `_aw32WOW`, `_InitThunkTableOffsets@0`, and `_W32GetTableOffsets@4`. Candidate partial; guest ABI, offset order and complete dispatch semantics are not proved. |
| `wkbtbl.h` | Current `wow32.c` includes it; it declares the legacy separate `aw32Keyboard` table. The inspected unified `wowtbl.c` instead incorporates `wkbdtbl2.h`. Header inclusion is not evidence a separate table is linked or used; retain pending until all relevant build configurations and references are resolved. |
| `wthtbl.h` | Current `wow32.c` includes it and declares `aw32ToolHelp`; unified `wowtbl.c` incorporates `wthtbl2.h`. As above, retain pending; the ToolHelp image review separately records limitations of ClassFirst/ClassNext. |
| `witbl.h` | Declares `aw32Internal`, but no current source include or consumer was established in the inspected WOW32 tree. Retain pending; absence in this bounded search is not proof of exclusion across all configurations. |

The captured Ninja graph names `wow32.c` and `wowtbl.c`, and the map shows associated symbols. That is build/link evidence for those objects, not proof that every declaration is referenced, every conditional branch is active, the guest's layout matches the host, or a particular thunk executes. `kernel31/kdata.asm` has the corresponding table ordering context, but the supplied guest binary was not rebuilt or ABI-verified here. Lifecycle, bounds and failure paths remain independently open. No product, guest, installation, shared ledger or build tree was changed.

Replay `python artifacts/mvdm-audit/shard-2/wow32-dispatch-header-observer.py` and compare its JSON. As the observer reads the current build graph and map, a later legitimate rebuild can change those captured hashes; interpret the receipt as the recorded snapshot rather than immutable release identity.
