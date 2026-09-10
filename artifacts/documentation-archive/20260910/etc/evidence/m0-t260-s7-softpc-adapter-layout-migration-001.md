# M0 T260 S7 — adapter-softpc layout migration

`adapter-softpc` is now the sole product home for the same-shaped
SoftPC/CCPU/SAS-to-Bochs mechanical facade.  This packet moved, without
source-body semantic edits:

| Source role | Destination |
| --- | --- |
| Guest-pointer, host-handle and session-data mapping manager (one implementation, three session instances) | `src/adapter-softpc/` |
| DPMI descriptor/startup/xmem mechanical seams | `src/adapter-softpc/` |
| SoftPC EMM, INT15, memory-size, mouse, printer, tape and keyboard mechanics | `src/adapter-softpc/` |
| CPU frame/result, checked RAM, engine, transaction and observation records | `src/adapter-softpc/` |
| CLI/image/preentry/startup composition records | `src/app/` |
| Bochs-core patch record | `src/bx-core/patches/` |

All remaining product files were removed from the transitional `src/bx-vdm`
tree.  The directory is no longer a component and must not be recreated.
References remaining in tests, governance scripts, import scripts and the
formal Ninja manifest are explicit S8 graph/test migration work, not product
source ownership.

S7 does not claim a build: source and build-graph path repair is deliberately
completed together in S8.
