# Part 1 MVDM: BDE debugger-extension family

Ownership: 11 `bde/*` keys in part 1, as defined by `nine-way-ownership.csv`; no cross-part disposition. `bde-evidence.json` contains the full current and three reference hashes, normalized VALID dependencies across four Ninja trees, and bounded ZIP/system32 name checks; replay `PYTHONDONTWRITEBYTECODE=1 python -B artifacts/mvdm-audit/nine-way/part-1/bde-observer.py`. Every file equals both primary OpenNT editions and third reference by hash. All have zero exact VALID deps in these graphs; this alone cannot establish feature irrelevance, runtime absence, or alternate build selection. `assets/release/bde.dll` and exact O:/winnt/system32/bde.dll were absent when checked.

Original `bde/sources:41-53,83-90` defines a separate NTSD debugger extension DLL with the eight named C sources. `bde.def` exports debugger commands. This is an external debugger-invoked role, distinct from the product's worker CPU loop. The existing shard-0 `bde-debugger-extension-audit.md`, shard-3 `bde-source-role-review.md` and shard-1 `debugger-extension-trace.md` contain earlier original interface and ownership analysis; no claim of equivalent modern debugging is inferred from a separate project debugger.

| Assigned file | Concrete role; suggested status and remaining obligation |
| --- | --- |
| bde.def | Export contract for k/kb/r/heap/symbol/memory/unassembly commands; pending explicit extension-delivery/ABI decision and invocation evidence. |
| bde.h | Shared extension declarations; pending same closure and context/target ABI. |
| bdecmds.c | Debugger entrypoints obtain WINDBG_EXTENSION_APIS and dispatch helpers; pending command behavior and lifecycle. |
| bdeheap.c | Global heap base/entry lookup and dump functions, invoked by extension commands; pending valid guest heap identity, pointer safety and failed reads. |
| bdemisc.c | Target memory dumps and context/selector helpers; pending partial/error reads and target identity. |
| bdestack.c | 16-bit guest stack walking; pending current execution-context/guest frame contract and malformed stack handling. |
| bdesym.c | Debugger symbol lookup/evaluation helpers; pending modern symbol provider and failure/handle lifetime. |
| bdeutil.c | Debugger target reads, context and stack helpers; pending source-of-truth and failed access behavior. |
| data.c | Storage for debugger-supplied API function pointers and global extension state; pending initialization, concurrency and teardown. |
| disasm.c | x86 16:16 code unassembly for extension `u` command; pending opcode coverage, bounds and target reads. |
| precomp.h | Historical BDE precompiled include bundle; pending original dependency closure; not an executable behavior. |

All recommendations remain pending until the project's external debugger extension contract and bounded functionality are adjudicated. No runtime debugger, target process, guest, compiler or provider was exercised. Original source equality is not behavioral proof; product absence is limited to tested graphs and exact package paths. No source, guest or shared ledger was changed.
