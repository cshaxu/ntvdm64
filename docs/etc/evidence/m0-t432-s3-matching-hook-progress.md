# T432 S3 matching Hook progress

Implementation checkpoint,2026-10-05. S3 remains active; not a closure,
production P, acceptance or publication. T431 files in O:/winnt/system32
are unchanged. Other-session proposal/TODO edits remain preserved.

## Implementation and source boundary

Shared native profiles link Hook32/64 from the same search, classifier,
interception, context and installer sources. Formal product targets delegate
both workers and Hooks to the matching profile. Each architecture's nested
Ninja has exclusive pool ownership of its output/dependency database.

Context version2 retains the64-byte header, replacing its unused span with
Hook64's path. Both paths are copied; machine and numeric handle bounds are
recipient-specific. Contiguous bounded terminated paths, reserved fields,
flags and capability pairing remain validated. Installation queries actual
child machine; matching native children receive matching DLLs. Only the exact
pinned x86 launcher receives context-only installation from either width.
Ordinary opposite-width interception fails closed until S4. Installer borrows
suspended handles, never resumes/closes them; creators preserve caller
suspension and abort only unpublished children on failure.

Hook64 selects unchanged original classifier and RTL error bodies plus the
existing finite path adapter. Historical _X86_ selection is confined to the
classifier slice, not the worker/global architecture. No OpenNT/MVDM mirror
function body or Detours source changed.

## Actual evidence

Build root: build/M0-T432/S3/r001; formal cache: build/M0-T427/S2/r001.
Architecture-local native profiles use MSVC14.43 /MT.

| Evidence | Result and boundary |
| --- | --- |
| formal-build.log | Formal NTVWM32/64, Hook32/64, x86 Run16 and RPC fixture link. Native sub-build serialization corrects an observed overlapping-output build failure. |
| hook32.log / hook64.log |113 /114 assertions pass: actual DLL load, same-width immediate descendants/CMD, ANSI, suspension, true exit codes, handle list, Unicode environment/CWD/streams, GUI/CUI authority stripping, malformed fields/spans and startup failures. |
| Context-only case |64 parent copies version2 into actual32 fixture; private duplicated events work, no interceptor loads. Fixture is explicitly pinned as launcher. Not actual Run16 DOS/WOW execution proof. |
| rpc-dual-direct.log | Real authenticated NTSRV creates both workers; READY/WIN32/WIN64, same-PID reuse and width-change rejection. Each executes two direct GUI fixtures with matching Hook, actual exit0 and service receipt. Not text-I/O proof. |
| Unchanged search fixture at both widths | CWD/PATH COM/EXE/BAT/PIF precedence, explicit/drive/empty/Unicode/capacity pass. Owned search32/search64 directories under build. |
| Unchanged image classification fixture at both widths |227 checks each, failures0, handle-delta0; cross-machine GUI/CUI metadata, DOS rejection and invalid arguments. Not legacy guest execution proof. |

Reproduce with architecture run-ninja.cmd targets nthook-install-test.exe,
application-search-test.exe and image-classification-test.exe. Hook64 test
requires --launcher-fixture pointing to actual x86 fixture. Real RPC test uses
worker-identity-version-test.exe, this run's rpc-runtime/system32/ntsrv.exe,
retained T431 Run16, --native-workers and both GUI fixture paths. Cleanup
terminates only verified fixture-owned worker identities.

## Remaining gates

The real64 legacy recognition/return gate now has the integration evidence
below; it is no longer inferred from context copying. S4 ordinary
opposite-width installation/helper failure boundaries are untouched. Full
Console/Window/WOW tests, final eleven-image package and publication remain
required. No mixed candidate is deployed and no S3 P is delivered.

## Real product integration, 2026-10-05

All product-programs rebuilt successfully (`r001/product-build.log`). The
integration-only eleven-image package `S3/r002-runtime` uses the formal current
images and unchanged sealed baseline WOW32/guest inputs, with its staging
manifest. APP0.0.428/RPC39/I/O25. This package is not published or accepted.

The common observation cleanup now recognizes both width-specific workers
using exact package paths and process creation identity. The existing Hook
chain script takes `-NativeMachine x86|x64`; its search, output, ordering,
actual exit and window assertions are unchanged. An x86 observer uses Sysnative
for actual native64 CMD. No production protocol or runtime diagnostics added.

Reproduction: set `MVDM_OBSERVER_SHORT_HISTORY=1`, run
`tests/observation/verify-native-hook-chain.ps1` with RuntimeRoot
`build/M0-T432/S3/r002-runtime`, Observer
`build/M0-T432/S3/r001/console-startup-observer.exe`, NativeMachine x64,
WindowObserver `build/M0-T432/S3/r001/worker-window-snapshot.exe` and fresh
LogRoot `build/M0-T432/S3/r004-hook64-chain`. The script creates/removes only
Z: and explicitly cleans its owned process identities.

All ten actual64 CMD chains pass: absolute/bare/stem COMMAND, bare/stem/absolute
MEM, ordered MEM-parent return, interactive MEM-parent return, COMMAND-parent
return and explicit x86 launcher. Actual Windows exit0 and current Console
output are required. Actual64 CMD redirecting WINMINE also passes startup-only
receipt plus the visible Mines window frontier; no gameplay claim.

The same eleven cases also pass with NativeMachine x86 on the same candidate,
recorded independently in `S3/r005-hook32-chain`. This retains the accepted
Hook32 legacy search, parent return and visible WINMINE frontier, rather than
assuming shared source means unchanged behavior.

`r003-integration/s3-native64-short-*` additionally passes explicit actual64
CMD -> x86 Run16 MEM and repeated two-MEM return (two output markers and exit0).
Use Verify-CommandExitStatus.ps1 Cases native-cmd-dos,native-cmd-dos-repeat,
NativeApplication `<host Windows directory>/Sysnative/cmd.exe` and OrdinaryFrontend with the
same short-history fixture. Submission/input delivery alone is never a pass.

Default private-desktop tests retain a failure: both candidate actual32 and64
CMD -> Run16 -> NTVDM reach the guest environment-error dialog and observer
timeout0x53504354, not a guest exit result. Adding only80 buffer columns also
fails. The unchanged accepted T431/r027-runtime produces the same dialog in
the same actual32/default fixture (`r003-integration/baseline-native32-*`).
This agrees with the baseline's documented prepare-text87/default53x15
limitation; the current runs do not independently prove the exact error87
return. The short-history fixture is the previously accepted Product geometry,
not a new assertion waiver. No general arbitrary-geometry repair is claimed.
