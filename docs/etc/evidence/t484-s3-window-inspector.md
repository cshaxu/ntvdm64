# T484 S3 P172 cached window inspector

The Win31/Win95 runtime matrix and observe-win31-enhanced.ps1 dot-source
inspect-session-windows.ps1. It compiled the same embedded C# class with Add-Type
into every run directory. Its Read method provides PID-filtered window text and
explicit test-desktop warning handling; it is not an obsolete probe.

Move that C# body byte-for-byte (LF) into window_inspector.cs. The existing CMD
fixture entry selects window-inspector; native CMake invokes the Windows .NET
Framework csc.exe compiler and tracks source/compiler dependencies. The resulting
AnyCPU DLL is a cached test artifact in fixtures-x64, not a production component
or release input. The PowerShell assertion runner loads it with Assembly.LoadFrom,
without dynamic source compilation. Existing callers and Read arguments remain
unchanged. No new window enumeration, callback or warning-dismissal algorithm.
The selected compiler is the existing Framework64/v4.0.30319/csc.exe; absence
fails this target instead of falling back to Add-Type or another interpreter.

First PID20360 failed compilation: the framework compiler did not accept the
forward-slash source path emitted by CMake. Retain build/t484-s3-window-inspector
as failure evidence. Convert source/output paths with TO_NATIVE_PATH; no algorithm
change. Final run is PID24484, build/t484-s3-window-inspector-final/run.ps1.

The final run verifies exact source extraction against the preceding revision,
real compile and CLR load, no windows for unselected PID0, missing-desktop error
propagation, unchanged DLL hash/mtime on a second CMD invocation, and rejection
of an unsupported x86 fixture selector. It then runs cached full host build and
11 raw/22 published identity checks. These are producer/load/boundary checks,
not a new Win31/Win95 boot or actual dialog-click acceptance run.

B0071 and its four matrix consumers remain open: dynamic probe fields and runtime
preparation still need complete-chain reconciliation. Removing this shared
compiler edge does not complete those entries;311/355 disposed,44 remain.

PID24484 completed successfully in66.3 seconds. All11 raw host inputs and22
published hashes remain unchanged; no redundant publication. The inspector was
an indirect dependency not independently counted in the original355 census.
Governance results are retained in closure-result.json beside the final run.
