# T484 S3 P215: retire inline C# compilation

## Scope and decisions

A whole-tree Add-Type review found four inline/source C# compiler entrypoints
outside the frozen355 inventory. Remove three expired standalone samplers:

- test/integration/ntvdm64-386/verify-title-native.ps1 has no live caller and
  only emits title observations. The native console_title_native_test.c already
  covers capacities0/1/4/32, empty title and child exit behavior through the
  current worker target. Its old AB sample is not claimed as a separate retained
  assertion or a newly executed test.
- test/observation/optical_native_toc.ps1 has no live caller and dumps one
  physical drive's TOC without a regression assertion. Retain the current
  original MSCDEX optical integration case, no-media contract, repeated reads
  and optional ExpectedOpticalFrames assertion. Removing the host sampler is
  not a new optical pass or a claim of equivalent physical-device coverage.
- test/observation/verify-logon-token-lifetime.ps1 tests the historical external
  C# helper's WindowsIdentity/GC ownership failure. That helper is no longer
  present in the current production chain. The native logon_environment.c
  explicitly owns its token through CreateEnvironmentBlock, then closes it
  synchronously; no managed finalizer is involved. Historical T447 evidence
  remains unchanged, not relabeled as native lifetime verification.

The live verify-command-native-edit-return.ps1 optional login-environment path
now uses existing Read-TestLogonEnvironment and CMD target logon-environment.
Replace its external LogonHelper source-file option with LogonEnvironment;
there are no current callers passing the old option. The formal product gate
selects the complete Userenv environment explicitly. No environment-value
allowlist, guest patch or input/assertion change. Remove the already-unused
LogonHelper parameter from the DDEML runner as well.

Whole-tree searches for Add-Type, CSharpCodeProvider, CompileAssembly,
Reflection.Emit, csc/mcs and dotnet build/publish found only three remaining
Add-Type -AssemblyName System.Drawing uses. Those load an existing assembly;
they do not compile source. This is a bounded compiler-form review, not proof
that every staging/publication dependency is migrated.

## Verification and limits

Owned modern-pwsh run build/t484-s3-dynamic-compiler-retirement/run.ps1,
PID28860, starts2026-11-24T14:36:31.1800376Z and ends14:37:48.0873821Z.
The selected native logon case passes real EDIT screen, Ctrl+Q, CMD echo,
DOS MEM, expected return code and launcher completion. The inherited developer
environment case times out before the EDIT milestone; aggregate run exits1.

Owned compare.ps1 reruns the pre-change41b0d8e05 script with the same package
and inherited environment. Only its script-root location is rebound to the
original source directory. It fails at the identical missing EDIT milestone.
This comparison excludes a regression from the new optional environment path;
it does not locate or repair the inherited-environment failure. The existing
immutable-COMMAND environment debt is not silently marked fixed.

The comparison run also performs cached full host compilation and verifies11
raw host images and22 dual-published identities. Actual terminal status is in
comparison-result.json: PID14516,2026-11-24T14:38:49.5127861Z through14:40:26.8096825Z, exit0. No fresh Full suite or optical/title runtime pass is
claimed. Documentation/tracker/parser/diff gates precede commit and push.

Original census remains319/355 disposed,36pending. Three supplemental tools
are retired; the newly discovered EDIT caller adds one explicit S4 preparation
obligation, so supplemental preparation chains are now24 rather than23.
No preparation chain was silently dropped to close S3. No original guest,
product source, winnt.zip or publication contents are modified.
