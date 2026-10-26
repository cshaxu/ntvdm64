# M0 T470 S1 — WOW ToolHelp/debugger owner matrix

## Question and method

This source-only audit maps the admitted ToolHelp/debugger slice through the
immutable Win16 ToolHelp DLL, the original WOW32 thunk table and task manager,
and the already-accepted non-WOW debugger package.  It distinguishes a guest
API that remains guest-owned from host lifecycle management.  It does not
claim a new debugger provider, a replacement scheduler, or runtime acceptance.

## ToolHelp ownership

The original WOW `toolhelp/makefile` builds the standard query, task, module,
memory, notification, signal and termination sources into `TOOLHELP.DLL`.
Those sources own their guest structures and callbacks: for example,
`notify1.c` owns registration/unregistration and death-signal state, while
`terminat.asm` uses the original task-switch/fatal-exit route.  The guest DLL
and its image are immutable in this project.  T447's service-mediated
individual-task deletion is a separate management operation; it must not
replace or be called by ToolHelp `TerminateApp`.

| Surface | Actual owner | Host/WOW boundary | Normal, failure and retirement disposition |
| --- | --- | --- | --- |
| Global/local/module/task/stack/system query exports | Original Win16 `TOOLHELP.DLL` sources and guest data | Original guest export/selector ABI | Guest validates and fills the caller buffer; task/module retirement remains the original guest owner. No host mirror is admitted. |
| Notify, interrupt, timer, signal and `TerminateApp` | Original `notify1.c`, `notify2.asm`, `int*`, `timer.asm`, `signal.c`, `terminat.asm` | Original guest callback/task-switch ABI | Guest registration and task-switch code owns unregister/death/failure. A host lifecycle request does not become a ToolHelp callback. |
| `ClassFirst` / `ClassNext` | No implementation in the selected WOW build | `ththunks.asm` routes both calls to `wow32/wthtbl2.h` | The WOW table deliberately maps both rows to `UNIMPLEMENTEDAPI`. The makefile explicitly excludes `class1.c`/`class2.asm` for WOW. This exact disposition is byte-identical in the inspected OpenNT/NT4 source references; no correction source exists. It is an immutable original WOW limitation, not an adapter candidate. |

`wthman.c` contains no corrective body.  Consequently S2 may test the retained
query rows and their guest-visible failures, but may not invent class
enumeration from modern USER windows or private task data.

## Debugger ownership

| Surface | Actual owner | Boundary and identity rule | Retirement disposition |
| --- | --- | --- | --- |
| Debug events, module/segment/fault notifications and continuation | Byte-identical original `mvdm/dbg/dbg.c` | Original public `vdmdbg.h` / `STATUS_VDM_EVENT`; it is a Win32 debugger exception, not a BaseSrv or private-CSR protocol | The debuggee and standard debugger own continue/exit semantics. T420 S43 already accepted the non-WOW event transport. |
| Standalone-worker inputs unavailable to the original body | `ntvdm-exe/debugger/dbg_registers.c` and `dbg_prompt.c` only | Checked call-scoped six-DWORD register copy; dynamic `DbgPrompt` export lookup | No guest pointer or second dispatcher persists. A missing `DbgPrompt` is explicit failure. |
| WOW task startup and debugger enablement | Original `wow32/wkman.c::W32Thread` and `WK32WOWNotifyWOW32` | `W32Thread` calls `DBGNotifyNewTask`; `WK32WOWNotifyWOW32` receives the original WOW debug/kernel addresses and calls `IsDebuggerAttached` | Original WOW task teardown keeps ownership. S3 must prove selected WOW notifications cannot survive task/module retirement. |
| Debugger callback into 16-bit ToolHelp | Original `W32RemoteThread` and `WK32KillRemoteTask` | Dedicated debugger callback TD/context (`vcRemote`, `vpRemoteBlock`), not an application task or generic worker thread | Remote task cleanup restores the original callback context or removes its dedicated task record. It must not be reused for lifecycle/service control. |
| External VDMDBG client | Original `mvdm/vdmdbg/vdmdbg.c` reference client | Direct shared mapping/process/remote-thread machinery from the historical debugger product | Not a current runtime dependency and not an allowed replacement for the NTSRV-controlled management RPC. |

## Build and scope result

The formal Ninja graph selects `src/mvdm/dbg/dbg.c`; the two documented
standalone bindings are the only project-owned debugger seams.  The selected
WOW producer code is original `wkman.c`, not a project-created notification
engine.  The finite S2 question is therefore whether retained ToolHelp query
exports observe correct original task/module data in a real WOW worker.  S3
owns selected WOW debug notification, stale-identity and task-loss proof.

No production source, guest media, ABI, release asset or installed binary is
changed by this audit.  The explicit `ClassFirst`/`ClassNext` limitation is
retained as original guest/WOW behavior rather than moved to a host TODO.
