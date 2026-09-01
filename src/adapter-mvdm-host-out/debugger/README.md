# debugger family

Owns the historical BDE, DBG, VDMDBG and VDMEXTS debugger-product boundary.
The full `mvdm-host/dbg/dbg.c` debugger event/guest-frame product is not
selected: it requires NT4 VDM debug-event transport.  The single original
startup call `DBGInit` has a same-shaped adapter because its body only probes
the process debug port and always returns `TRUE`.

## Registered divergences

| ID | Original purpose | Reason | Implementation | Files |
| --- | --- | --- | --- | --- |
| ADAPTER-MVDM-DEBUGGER-001 | `DBGInit(argc, argv)` queries `ProcessDebugPort`, stores whether this VDM is debugged, and returns `TRUE` before normal SoftPC startup continues. | The NT4-linked `NtQueryInformationProcess` declaration is not a stable public application contract; selecting all of `dbg.c` would also import private VDM debug-event transport. | Preserve the original entrypoint, argument/result shape and status branch. Resolve the same NTDLL export at the adapter boundary; an unavailable export retains `fDebugged = FALSE` and `TRUE`, in the shared source-shaped debugger state. | `source/dbg_init.c`, `source/dbg_state.c`; original reference `../../mvdm-host/dbg/dbg.c` |
| ADAPTER-MVDM-DEBUGGER-002 | Preserve the private debugger transport direction for debugged `ModuleLoad`, `ModuleFree`, `ModuleSegmentMove` and the separately imported `DbgPrompt` boundary. | Those operations require the private NT4 VDM debugger/CSR event transport, which is absent from the ordinary non-debug profile. | Non-debug module notifications retain their original no-event result. An actually debugged/private operation records `BACKEND_UNAVAILABLE`, records its original exported entry name only when the default-off termination report is requested, and terminates only the bound session with `ERROR_CALL_NOT_IMPLEMENTED`; it never returns false success, creates a substitute debugger event, or terminates the application process. | `source/dbg_unavailable.c`; original references `../../mvdm-host/{dbg/dbg.c,softpc.new/host/src/fprt.c}` |
| ADAPTER-MVDM-DEBUGGER-003 | `dbg.c::DBGDispatch` decodes a VDM-stack debugger mode; `DBG_TOOLHELP` unconditionally retains a 16:16 hook/f386 pair, while ordinary non-debug event branches are no-event/default-false paths. | NT4's `Sim32GetVDMPointer` gave the debugger a durable raw VDM alias, which is not safe for the selected session lifecycle. | Preserve mode ordering and fixed-width results. Resolve current CPU40 `SS:SP` to a guest-linear value and read only the required finite words through the existing session mapping-manager lease; a debugged branch remains the explicit ADAPTER-MVDM-DEBUGGER-002 unavailable transport. | `source/dbg_dispatch.c`, `source/dbg_state.c`; original reference `../../mvdm-host/dbg/dbg.c` |
