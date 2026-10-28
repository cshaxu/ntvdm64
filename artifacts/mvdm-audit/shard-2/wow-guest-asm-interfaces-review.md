# Shard 2: WOW guest assembly interface includes

Four pending entries match both original trees. The companion observer records original guest references, file hashes and a normalized `wowtbl.obj` Ninja dependency snapshot; no guest was rebuilt or run.

| Entry | Evidence and proposed disposition |
| --- | --- |
| `inc/WOW.INC` | Original GDI, USER, KERNEL, driver and other guest assembly sources reference it; the original include makefile describes dependencies on `wow.h`, `mvdm.h` and `bop.h`. Candidate historical guest build input **only after** the coordinator checks the full current build closure and other configurations. Its absence from `wowtbl.obj` dependencies is not a whole-graph exclusion. |
| `inc/WOWMMCB.INC` | Original MMSYSTEM `stack.asm` and `isr.asm` include it; the original makefile names their objects. Candidate historical guest build input subject to selected-profile and complete current-graph verification; callback ABI, ordering and lifetimes remain separate. |
| `inc/WOWTH.INC` | Original TOOLHELP `ththunks.asm` includes it; its historical makefile lists the include on the object dependency. Candidate historical guest build input subject to full current-graph verification. The original ClassFirst/ClassNext host table entries are unimplemented; this does not establish complete ToolHelp behavior. |
| `inc/WOWNT32.INC` | Original include makefile has an `h2inc` recipe, but no exact active consumer was established in this review. Retain pending. |

The four files were not dependencies of the one captured host object; the observer resolves Ninja's relative paths rather than treating text mismatch as absence. This limited result cannot exclude other compiled users. Original generation rules do not prove that the retained `.INC` bytes were generated from the currently checked headers, that the deployed guest uses those bytes, or that guest and host ABI layouts/ordinals match. Runtime behavior, failure and teardown remain unverified.

Replay `python artifacts/mvdm-audit/shard-2/wow-guest-asm-interfaces-observer.py` and compare the JSON. No guest, product, installation, shared ledger or build tree was modified.
