# M0 T433 S3 — native frontend migration

One AMD64 NTCON, interoperating with x86 NTSRV/NTVDM/run16/NTMON and the
delivered AMD64 NTVWM/Hook64. No new frontend, worker, helper or wire revision.
APP0.0.433/RPC41/I/O25 remain; original OpenNT/MVDM bodies are unchanged.
S4 is not admitted by this work.

## Source and implementation ledger

| Boundary | Provenance and action | Ownership retained |
| --- | --- | --- |
| Bootstrap | Project main used32-bit wcstoul for four inherited resources. Private session_arguments checks pointer-width hex, overflow and invalid input; window identity stays uint64. | NTSRV authenticates actual process/resources; parsing is not authorization. |
| Window/library | Existing registered nxvm/softpc private frontend and native pointer callbacks, rebuilt AMD64 with existing C11 flags. No renderer/source duplication. | Worker-neutral NTCON rendering/input/title and zero/one authorized pipe. |
| RPC/common | Existing fixed copied contracts and architecture-local MIDL41/common archives. Finite five project client-binding members retained; original BaseClient/RTL bodies excluded. | NTSRV association, lifecycle, task and restoration authority. |
| Build/package | Reuse native generator's Worker/Frontend selection. Formal graph imports explicit AMD64 frontend; stage/verifier enforce selected machine and hashes. | No mixed objects or x86 product fallback. |
| Observer | Existing test-private Window prefix has a pointer. Use x64 observer; x86 NTVDM fault contexts use WOW64 APIs. | Read-only rendered-state evidence, no production hook/protocol/helper. |
| Console fixture | Inherited user font/DPI and previous canonical extent made prerequisites invalid. Set small raster font only in test-owned hidden Console, provision the threshold fixture buffer. | All original geometry, ordering, failures and cleanup assertions unchanged. |

## Inputs and commands

Baseline: S2 production14796f8ba56a51075f098a532d251531a9937db2, sealed
`build/M0-T433/S2/r010-runtime`, publication r018. Shared x86 cache
`build/M0-T427/S2/r001`; native worker `build/M0-T433/S2/r001`, Hook64
`build/M0-T433/S2/r002-hook64`, unchanged WOW32
`build/M0-T432/S6/r004-wow32`. All new outputs stay below `build/M0-T433/S3`.

- r001: New-NativeWorkerNinja.ps1 with Component Frontend, BuildRoot
  build/M0-T433/S3/r001 and ReferenceGraph build/M0-T427/S2/r001/build.ninja.
  Generated x64 run-ninja.cmd selects NTCON and eleven frontend fixtures.
  MSVC14.43/SDK22621/MT; pointer-narrowing diagnostics are errors. No global
  MVDM CPU flags, worker-base or original body is selected in production.
- Formal generator: Architecture x86, explicit NativeWorker and NativeFrontend;
  run-ninja-parallel.cmd frontend-session-arguments-test.exe ntcon.exe.
  Parser tests both report48 checks, zero failures (pointer bits32/64).
- r002-runtime: Stage-System32ProductPackage with explicit frontend, worker,
  both Hooks and unchanged original media/configuration.
- r005-observer: Generate-ObservationNinja with x64; startup observer,
  input-milestone-test and worker-window-snapshot. Milestone negatives pass.
- r007-focused: tests/observation/verify-native-frontend.ps1 with the above
  cache/baseline/WOW32/Hook64/worker inputs. Eleven AMD64 fixtures and two
  package-input negatives pass; results.json retains each result and log.
- r001 channel-x86-fixture/frame-x86-fixture: the same updated full Console
  channel/failure fixtures also pass as x86, with original assertions intact.
- r006-full: Invoke-ProductVerification Suite Full; r002 runtime, x86 cache,
  r005 startup/window observers, retained terminal observer/G7 fixture,
  explicit native frontend/worker/Hook64, S2/T431 WOW frontiers. Real global
  endpoint scenarios remain serial; existing finally releases Z.

## Failed experiments, not passes

Initial native recipe lost source-specific C11 settings, then forced main for
a wmain test. Correct flags and automatic test entry selection resolve these
build failures without changing the imported library. Initial formal import
rule followed its use; declaration placement corrected and reimport verified.

r003-full passes Console17/WOW but fails the first Window case: x86 observer
truncates AMD64 GWLP_USERDATA and cannot prove prompt consumption. Rebuilding
the observer at x64 corrects the evidence source, not production input or
assertions. NTVDM diagnostics remain x86 through WOW64 context APIs.

Initial channel fixtures fail error87 establishing80x30: largest canvas58x15
on the hidden private desktop. Unchanged x86 baseline fails the same line.
Changing a derived buffer font alone fails. Establishing the canonical font
before buffer creation fixes that prerequisite; one threshold fixture also
needs its own buffer extent instead of preceding test geometry. No assertion
is removed or changed to tolerate failure. Failed logs remain under r001.

## Verified delivery

r006-full exits0, all14 groups pass in399055ms: Console17 (59063ms),
Window17 (62162ms), independent WOW frontiers, RPC7, GUI5, version negatives5,
strict DIR, modern EDIT, cooked return, twelve native/DOS relaunch pairs,
twelve interactive relaunches, isolated sessions, retirement wiring and final
nested Window handoff. No old prompts, missing evidence, changed exit codes
or eliminated failure assertions are accepted as success.

r008-publication backs up the actual S2 images/notice and configuration,
publishes only the verified ten host images and MIT notice to
`O:/winnt/system32`, and checks every hash. Guest/configuration files are not
replaced. AMD64 images are NTCON, NTVWM and Hook64; seven other images remain
I386. NTCON SHA256 is
`61793C8E66D84467ADFDFB627939945F750F84D51AB0EDAF9D61DBCC1675EEBA`.
The package remains APP0.0.433/RPC41/I/O25.

r009-published-smoke with the same explicit short-history Console geometry
as the retained matrix passes real DOS MEM, actual32/64 CMD VER/echo,
direct result0 and final ten-image/notice hashes. Only allowed Z alias is
used and released in finally; cleanup terminates only pinned published
project processes. Explicit cleanup is not normal retirement proof.

Earlier smoke attempts remain failures: missing guest PATH/CWD prerequisite,
then default hidden-desktop53x15 initialization produces the original
environment-error dialog; Z alone does not resolve that condition. A
width-only comparison with the prior x86 NTCON also fails in that default
configuration and restores the native file in finally. This is not a complete
S2-package comparison. The three other relinked core images' .text hashes
match S2, while whole-image hashes differ; no equality of all image sections
is claimed. A proposed temporary ten-image comparison was denied by safety
review and was not executed; the permitted narrowed check replaces only
NTCON. A malformed observer argument quote was also corrected. None of these
attempts is a passing result or a diagnosed production fix. Final smoke uses
the established explicit geometry fixture, retaining output/exit assertions.
The default53x15 desktop initialization boundary needs separate causal
investigation and is not claimed solved by native-width migration.

Source review confirms no original mirror body diff or NTCON dependency on
worker-base. Documentation governance, relative links and Git diff checks pass.
S3 delivery retains these explicit boundaries; T433 stays open and S4 waits
for owner admission.

Production P1 `245d3ac120cc060b04189bfa5118e08b34fd2a76` is committed and
pushed to main. Documentation-only P2 records bounded S3 closure; no later S
is admitted and no further product change/build is part of that P2.

No owner hands-on acceptance or physical RDP/focus/clipping observation is
claimed. WINMINE visible startup and SOL/WRITE retained frontiers are not WOW
gameplay/functionality proof. Known long-path limitation remains untouched.
