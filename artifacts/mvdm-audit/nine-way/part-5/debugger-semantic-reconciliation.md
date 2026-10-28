# Part 5: debugger client, extension and guest-event boundary

Seventeen owned debugger paths and source hashes are in `debugger-family-review.md`. Three original tools are distinct: `softpc.new/debugger/vdmdebug.c` is the SoftPC debugging program, `vdmdbg/vdmdbg.c` supplies external VDMDBG APIs, and `vdmexts` is an NTSD extension DLL built by its own `sources` file. The captured product worker/WOW32 graphs do not select these paths. This is not evidence that the product has no guest debugger support.

Original `vdmdbg.def` exports selector/context, task/module enumeration and task operations. Its implementation accesses debuggee memory via ReadProcessMemory (e.g. `vdmdbg.c:281-294`); those are client APIs and cannot be replaced by an NTMON display or blindly interpreted as current NT private layouts. `vdmdbg.txt` describes guest INT interception → worker DBG BOP → four-argument `STATUS_VDM_EVENT` exception → attached debugger continuation. This is original historical protocol, not proof the present external client can safely consume a live worker.

`m0-t420-s43-debugger-capability-ledger.md` records a historical candidate that compiled original `dbg.c`, reached the original BOP/dispatch boundary, and used a native debugger/child fixture to inspect a four-slot event and continue a child. `m0-t473-s1-unselected-host-source-disposition.md` separately calls the current dbg selection fixture-only with an S2 package decision outstanding. These concern different times/selections; reconcile against the current build graph before claiming production support. The native fixture is not a real Win16 debugger attach/teardown or VDMDBG API acceptance.

| Owned file | Original role and recommended bounded classification |
| --- | --- |
| `softpc.new/debugger/vdmdebug.c` | Separate SoftPC interactive debugger application; not current worker code. |
| `vdmdbg/vdmdbg.c` | Original external debug client APIs and process-memory access; compatibility with selected CPU/context and task owner remains unproved. |
| `vdmdbg/vdmdbg.def` | Original VDMDBG public export list, including task operations. |
| `vdmdbg/vdmdbg.rc` | Original DLL resource input in its recipe; not a runtime event handler. |
| `vdmdbg/vdmdbg.txt` | Original protocol documentation; preserve as evidence, not a test pass. |
| `vdmexts/data.c` | NTSD extension shared debugger callback/state carrier; separate process/tool context. |
| `vdmexts/debug.c` | Extension breakpoint commands using debugger target expressions; not worker breakpoint ownership. |
| `vdmexts/dib.c` | Original debugger graphics/data inspection command; target layout dependence requires verification. |
| `vdmexts/disasm.c` | Separate extension disassembly commands; not CPU executor. |
| `vdmexts/heap.c` | Separate guest heap inspection; requires actual target memory/layout identity. |
| `vdmexts/help.c` | Extension command help. |
| `vdmexts/i386/profile.c` | Original x86-specific debugging/profiling commands; not general x86/x64 provider. |
| `vdmexts/i386/regs.c` | Original x86 register display code; requires matching live register representation. |
| `vdmexts/i386/vdmtib.c` | Original x86 private VDM TIB inspection; do not interpret a modern opaque object as this old layout. |
| `vdmexts/misc.c` | Separate extension helper commands. |
| `vdmexts/platform.c` | Extension architecture/provider boundary. |
| `vdmexts/precomp.h` | Original extension precompiled-header input, not runtime functionality. |

S17 should first determine admitted client/tool ABI and actual current dbg provider, then use immutable guest events with a controlled debugger: attached/unattached, continue/decline, exception/register restore, task and worker exit, invalid target memory and detach. Treat VDMDBG and NTSD extension completeness as separately scoped; S11 owns worker execution recovery and S19 exact release evidence. Do not restore private NT4 layouts as host pointers, fabricate successful debugger replies or conflate NTMON task termination with the historical debugging API. No tests or product/guest/shared-ledger changes were made.

Replay `rg -n "VDMEnumTaskWOW|VDMTerminateTaskWOW|VDMGetThreadContext|ReadProcessMemory" src/mvdm/vdmdbg`, inspect `vdmexts/sources`, the part-5 observer and the T420/T473 evidence cited above.
