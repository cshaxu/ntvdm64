# debugger family

Owns the historical BDE, DBG, VDMDBG and VDMEXTS debugger-product boundary.
The full `mvdm-host/dbg/dbg.c` debugger event/guest-frame product is not
selected: it requires NT4 VDM debug-event transport.  The single original
startup call `DBGInit` has a same-shaped adapter because its body only probes
the process debug port and always returns `TRUE`.

## Registered divergences

| ID | Original purpose | Reason | Implementation | Files |
| --- | --- | --- | --- | --- |
| ADAPTER-MVDM-DEBUGGER-001 | `DBGInit(argc, argv)` queries `ProcessDebugPort`, stores whether this VDM is debugged, and returns `TRUE` before normal SoftPC startup continues. | The NT4-linked `NtQueryInformationProcess` declaration is not a stable public application contract; selecting all of `dbg.c` would also import private VDM debug-event transport. | Preserve the original entrypoint, argument/result shape and status branch. Resolve the same NTDLL export at the adapter boundary; an unavailable export retains `fDebugged = FALSE` and `TRUE`. | `source/dbg_init.c`; original reference `../../mvdm-host/dbg/dbg.c` |
| ADAPTER-MVDM-DEBUGGER-002 | Preserve reached `DBGDispatch`, `ModuleLoad`, `ModuleFree`, `ModuleSegmentMove` and `DbgPrompt` call shapes from original `dbg.c`. | Their active bodies require the private NT4 VDM debugger/CSR event transport, which is absent from the ordinary non-debug profile. | An actual reached debugger operation records `BACKEND_UNAVAILABLE` and terminates only the bound session with `ERROR_CALL_NOT_IMPLEMENTED`; it never returns false success, creates a substitute debugger event, or terminates the application process. | `source/dbg_unavailable.c`; original reference `../../mvdm-host/dbg/dbg.c` |
