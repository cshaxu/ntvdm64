# T484 S3 P165 existing Win16 native chains

Audit five remaining inventory entries together: discovery, PrintDlg cancel,
PrintDlg default, raw spool profile and ToolHelp. All already delegate through
Invoke-Win16ProbeBuild.ps1 to build-fixtures.cmd, win16-probes.cmake and native
NASM/MSVC. Discovery also uses the native wow-discovery-peer target. No embedded
compiler, original guest rebuild or new duplicate fixture is required. These
are retained focused integration tests, not a claim that Full invokes them all.

The first real run (PID31156, build/t484-s3-win16-existing) inherited the agent
process environment. All five failed before successful completion; discovery
peer returned4 waiting for the guest READY file. Keep that evidence. It does
not prove a defect in the CMD producer or identify which inherited variable
caused the failures.

A controlled second run changed only the supervisor's environment to the
existing native CreateEnvironmentBlock current-token result. Launcher PID26692,
child PID21060, build/t484-s3-win16-logon. No environment values were logged.
Same test sources/assertions and same host/guest package: all five passed.
Results prove FindWindow/enumeration/property cleanup, PrintDlg cancellation,
default conversion/returned handles, explicit no-profile raw-spool failure,
and the previously registered ToolHelp-unavailable negative contract. ToolHelp
passing does not mean its unsupported query exports work. The immutable DLL
used is candidate-final/system32/TOOLHELP.DLL, SHA256
DDEECCC103F7E3C105602A6F791792BAA7778A796709BE63983A8A921DF80CFE.

Both cases.json and per-case result/guest-report/input/producer records are
retained. The second run also passed cached full and the 11 raw input / 22
published identity checks. No source or product image changes and no new
publication. This is validation of existing producer migrations, not a new
runtime repair. Arbitrary inherited agent environments are not accepted by
this evidence. S5 must use an explicit normal user environment when invoking
these integration runners; do not silently rerun or omit the first failures.

Disposable package copies from both runs were removed after termination;
existing scoped cleanup removed their short runtime stages. Closure evidence
is build/t484-s3-win16-logon/closure-result.json. S3 remains active.
