# M0 T276 S25: debugger-tool package final integration disposition

## Decision

The selected `bde`, `dbg`, `vdmdbg` and `vdmexts` roots are all final
**tool-only** products. They are independently built NTSD/WinDbg extensions,
a static debug-support library, or a debugging-support DLL; they are not a
library dependency of the main MVDM host runtime.

The source bodies are retained byte-exact for a separately admitted debugger
tool recovery. Their old external-debugger operations—remote process memory,
thread contexts, symbol expressions, `STATUS_VDM_EVENT` and fixed NTVDM image
addresses—must not be linked into the single-session host path.

## Future boundary

A future `adapter-debugger` is permitted only as a whole-tool decision. It
must preserve the selected original tool's observable contract through copied
diagnostic snapshots or an explicit broker; it cannot expose guest pointers or
host HANDLEs through a main-runtime ABI.

## Verification

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File tools/governance/Set-T276S25DebuggerToolsFinalDisposition.ps1 -RepositoryRoot O:\repos.hobby\ntvdm64
powershell.exe -NoProfile -ExecutionPolicy Bypass -File tools/governance/Verify-T276S25DebuggerToolsFinalDisposition.ps1 -RepositoryRoot O:\repos.hobby\ntvdm64
```
