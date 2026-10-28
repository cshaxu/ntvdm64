# Original external debugging tools and guest helper family

The eight shard-2 pending files match both original editions. None of their exact paths appears in the captured worker or WOW32 Ninja graphs. Their original source recipes declare separate debugger tools rather than the core execution component. The captured ZIP has no files named BDE.DLL, VDMDBG.DLL, VDMEXTS.DLL, VDMDEBUG.EXE or WOWDEB.EXE; filename absence cannot rule out renamed binaries, external tools, installations or adapters.

| File | Actual original role and bounded recommendation |
| --- | --- |
| `bde/bde.def` | Exports for the independent BDE debugger extension described by bde/sources. Historical **external debugger metadata**; exact original link consumption and current replacement behavior unverified. |
| `bde/data.c` | bde/sources selects debugger API and process/thread context state. **Pending** debugger-tool scope/acceptance; not the guest CPU state provider. |
| `bde/disasm.c` | bde/sources selects disassembly implementation; original `bdecmds` calls it from debugger `u` command (prior review). **Pending** debugger-tool behavior; not proof CPU instruction decode is missing. |
| `softpc.new/debugger/vdmdebug.c` | Separate `vdmdebug` console program with debugger pipe/event source logic. **Pending** if debugger shell functionality is required; no protocol or runtime equivalence claim. |
| `vdmdbg/vdmdbg.def` | Exports original VDM debugging support APIs, including task/selector/context operations. **Pending** API/ABI owner and scope; product task management alone does not prove this DLL interface exists or is equivalent. |
| `vdmexts/i386/vdmtib.c` | Original vdmexts i386 source selected; platform command invokes VdmTibp, which looks up `ntvdm!VdmTib` and uses debugger APIs. **Pending** debugger command and symbol/runtime behavior. |
| `vdmexts/vdmexts.def` | Original vdmexts DLL extension export metadata. Historical **external debugger metadata**, exact linkage and replacement scope unproved. |
| `wow16/wowdeb/wowdeb.def` | Original Win16 debugging utility module definition imports KERNEL.511 WOWKILLREMOTETASK. Candidate **historical guest utility build metadata**, not a statement that NTMON implements its original UI, API, or task lifecycle. |

Keep debugger functions pending where their product scope has not been decided; these file roles alone must not move required capabilities out of the audit. The observed separate-tool identities do not prove their binaries are delivered, run, or supported; no interactive debugging, guest execution or test was performed. No product, guest, shared ledger or build tree changed.

Replay `PYTHONDONTWRITEBYTECODE=1 python artifacts/mvdm-audit/shard-2/debugger-tools-family-observer.py` and compare JSON; `control-thunk-bde-review.md` and `debugger-extension-review.md` contain additional source call details.
