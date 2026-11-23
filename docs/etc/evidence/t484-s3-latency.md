# T484 S3 P58 latency and logon-environment producers

## Finding and change

B0063 generated CL commands from PowerShell and compiled an inline C# environment
reader. Its old CL source paths used the integration directory although the
actual sources are in observation. Native CMake now owns all eight supported
page/observer/callback/workload combinations plus two graph-page variants and
the x86 sampler. The compiler comparison uses the actual observation sources;
it does not claim that the old broken path could execute as written.

The current-token environment reader is a small MSVC test utility using the
same CreateEnvironmentBlock(token,FALSE) and DestroyEnvironmentBlock contract.
It returns the exact UTF-16 double-NUL block over an anonymous stdout pipe.
The runner consumes it only in memory and logs counts/process identity, never
variable values. No original guest or product source changed.

An initial precompiled C# proposal was rejected by automatic approval because
it introduced csc.exe as a build dependency. It was not applied. The delivered
alternative uses the existing MSVC toolchain; no C# compiler is required.

B0060 no longer copies/rewrites the latency runner or compiles an overlay from
PowerShell. It invokes the canonical runner with an explicit GraphCost switch.
Both native graphs use one checked header projection targeting WOW32 and
rejecting counter writer overlap. Diagnostic input hash checks are retained.

## Verification

Owned native build and ten original-command reference profiles pass. All ten
latency EXEs plus the sampler differ only in two PE timestamp bytes each.
The graph references use the previously delivered projected header. Invalid
arguments return64 for all ten latency variants and2 for sampler/environment;
x64 selection returns2. Both affected PowerShell runners parse successfully.

The environment reader test confirms double-NUL structure, normal SystemRoot,
well-formed entries and exclusion of a caller-only sentinel. It reports45
entries/3349 UTF-16 characters on the final x64 path without storing values. This is API/transport
verification, not a full GUI latency or normal guest-exit acceptance run.

Concise results live in artifacts/build-migration/t484/s3-latency; commands,
owned process records and raw logs live in build/t484-s3-latency.

## Remaining work

B0063 and B0060 remain pending: interpreted runtime staging/package identity
helpers and diagnostic package assembly are not yet migrated. Real timing,
callback costs and paired guest workloads were not remeasured by this batch.

The environment utility matches the runner process architecture. The initial
x86 check returned45 entries/3361 characters; the final x64 runner path
returned45/3349, so cross-bitness environment equivalence is not assumed.
Both structures and non-inheritance controls passed.

Cached full host build passed. The graph counter unit passes nested timing,
LastError, exception cleanup and concurrent-refusal controls. Both graph
header projections are byte-identical to the prior delivered header. No-op
latency builds preserve all11 witness hashes. All11 product input hashes and
22 published hashes remain unchanged; no release recopy was required. Script
parsing and documentation governance passed.
