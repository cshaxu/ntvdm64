# nthook-dll

The same source component builds nthook32.dll for x86 and nthook64.dll for x64.
Each architecture has its own export definition file.
NTVWM and both DLLs select the same suspended-child installer;
run16 selects the copied-context reader without loading the interception DLL.
Native GUI propagation does not grant character-frontend authority.

Run16 and the Hook deliberately have separate discovery contracts. Run16 owns
one user CLI field: CWD/PATH COM/EXE/BAT/PIF discovery, including compact
`command/c` normalization. The Hook owns the original two-field
CreateProcess request: an explicit application is authoritative; without one,
the Hook uses bounded documented command-line candidates only to decide
whether a DOS/Win16 redirect applies. A redirect carries that selected image
and the OpenNT `BaseCheckVDM` tail shape to Run16. Native calls retain their
original A/W application, command, attributes and parameter tail.

The Hook uses the shared pure-C `lib-win32/process-hook`: startup-time MinHook
trampolines intercept CreateProcess, same-width suspended-child staging inserts
the Hook import, and the opposite-width helper uses Windows rundll32. Run16's
context-only payload is written directly without loading a Hook DLL or helper.
The bundled MIT and MinHook notices accompany the local release. The copied
library's helper has a documented local cleanup correction; original source
hashes and the difference are recorded in the S5 evidence.

The copied payload is private bootstrap data, not RPC or authorization.
Existing NTSRV object authentication remains authoritative. No broker call,
wait, process creation or session lookup is permitted in DllMain. The ordinal 1 helper entry is accepted only for the bounded system rundll32
handoff and validates its packet before staging the target.
