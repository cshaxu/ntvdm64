# M0 T433 S4 — native monitor migration

## Scope and source conclusion

The single `ntmon.exe` is now AMD64. Its project-owned458-line production
main is unchanged: same Console renderer, tree/selection, exact title/footer,
UP/DOWN/DEL/ESC and confirmation behavior, service-only snapshot/control and
existing750ms refresh. No new task registry, process enumeration, worker-base,
helper, renderer or original OpenNT/MVDM body enters this migration.

Native dependencies are the existing NTSRV-owned broker transport and common
RPC/management plus an architecture-local MIDL41 client. Actual map selection
contains no original BaseClient/RTL implementation. Source manifests record
the finite native closure, not merely archive names. Copied key/row layouts
remain24/608 bytes, with object/image offsets16/88; high64-bit key differences
remain distinct. Native process/binding handles stay local; authenticated
system_handle transfers interoperate with the x86 service.

APP0.0.433/RPC41/I/O25 and ten product filenames/system32 locations remain.
AMD64 images: NTCON, NTVWM, NTMON, Hook64. Six remaining images are I386.
Labels DOS/WIN16/WIN32/WIN64 describe tasks, not carrier width. NTMON itself
therefore appears as WIN64 when launched through the native worker.

## Implementation and provenance ledger

| Change | Owner and rationale | Verification |
| --- | --- | --- |
| Native monitor graph | Reuse finite native generator with Monitor selection; no font/firmware or machine body selection. | MSVC14.43/SDK22621/MT x64, strict pointer-narrowing diagnostics, native link/map and source manifest. |
| Formal import | Shared frontend/monitor consumer import recipe, explicit NativeMonitor; no x86 production fallback. | Import checks AMD64 EXE/CUI and copied hash; source closure stays declarative. |
| Staging/verifier | Explicit native monitor machine/hash and native management fixture selection. | Full coherent ten-image package; four input/import negatives. |
| Layout fixture | Same production renderer/dispatch, added fixed ABI and high-key assertions. | Both native/x86 compilation; native real Console layout/selection/error/control assertions. |
| RPC fixture | Existing production management client, native-width internal capability formatting and additional wrong-app rejection. | Actual x64 client/x86 service, version/absent-key errors, live close/projection paths. |
| Session fixture | New test-only controller starts actual NTMON/NTSRV in its owned hidden Console. | Disconnect survival, real connect/loss/restart, exact title/footer, ESC exit0; no mock renderer. |
| Retained integration scripts | Correct old flat-path assumptions to actual system32 layout, explicit target PE/kind expectation and identity-checked cleanup. | Real Console/Window NTMON and mixed DOS/native/WOW/GUI tree/isolation. |

No production business logic is rewritten. The original mirrors and guest
bytes have zero source diff. Console/font setup belongs only to test-owned
fixtures, not a new product fallback.

## Inputs, commands and results

Baseline: S3 production245d3ac12, closure1de70587b, sealed
`build/M0-T433/S3/r002-runtime` and publication r008. S4 output root is
`build/M0-T433/S4`; unchanged dependencies reuse the validated x86 cache
`build/M0-T427/S2/r001`, S2 native worker/Hook64 caches and S3 frontend/observer.

- r001: `New-NativeWorkerNinja.ps1 -Component Monitor -BuildRoot
  build/M0-T433/S4/r001 -ReferenceGraph build/M0-T427/S2/r001/build.ninja`;
  run-ninja.cmd ntmon.exe monitor-rpc-test.exe monitor-layout-test.exe
  monitor-session-test.exe. Final closure includes14 source edges, including
  fixtures. Production main hash remains unchanged.
- Formal generator uses Architecture x86 and explicit NativeWorker,
  NativeFrontend, NativeMonitor. Formal import and x86 layout/RPC builds pass;
  native source closure regenerates without a parallel x86 product.
- Native layout runs in an owned hidden Console; layout-abi.txt passes exact
  title/footer/colors/25 rows, scrolling, stable selection/refresh, readonly
  and stale confirmation behavior and production key dispatch. Structure/key
  assertions above are additional, not weakened existing checks.
- Native monitor-rpc-test --empty in staged system32 passes actual x86 NTSRV
  snapshot, protocol/application mismatch and nonexistent-close rejection.
- r002-runtime is the coherent staged package. r003-full uses
  Invoke-ProductVerification Suite Full with explicit native frontend/worker/
  monitor/Hook64 and MonitorRpc r001; x64 S3 startup/window observers and
  retained terminal/G7/WOW frontier inputs. All14 groups pass in539462ms:
  Console17/Window17, WOW frontiers, RPC7, GUI6 (including actual management
  close), version negatives5, strict DIR, modern EDIT, cooked return, rapid
  and interactive relaunch, session isolation, retirement and nested Window
  handoff. No complete matrices run concurrently.
- monitor-session-test runs actual staged binaries: initial disconnected UI,
  x86 NTSRV connection, server loss with NTMON still alive, restart/Ready,
  and real Console ESC/exit0 all pass. session.txt retains the result.
- r004-monitor-integration/run.ps1 serially runs verify-native-monitor with
  ExpectedKind WIN64 and verify-monitor-tree-integration using the AMD64
  management client. Actual Console/Window title/input/exit passes. Mixed
  tree includes frontend-bound DOS/native, independent WOW worker/tasks and
  detached GUI. Closing the DOS root prunes it and returns1067 while other
  identities survive; GUI close preserves responsive native output/exit23
  and WOW. Z is released in finally.
- r005-input-negatives: test-native-monitor-package-inputs.ps1 passes wrong
  I386 producer, omitted native selection, outside-build producer and invalid
  formal import before output. No fallback or image-machine auto-acceptance.

## Publication and review

r006-publication preserves the actual preceding S3 ten images, MIT notice and
configuration; publishes only verified host images/notice to O:/winnt/system32
and checks all hashes. Guest/configuration are not replaced. NTMON SHA256:
`503F80542E9A9B40370117170A07697D185BB2819E90C986F03D35B4597AED01`.

The session fixture also passes against actual O:/winnt/system32 binaries:
published-monitor.txt proves standalone disconnected survival, actual x86
broker restart, title/footer and ESC completion. r007-published-smoke passes
DOS MEM and actual32/64 CMD VER/echo/direct result0 with final ten-image/notice
hashes. Only Z is used and removed; exact fixture/publication-owned processes
are cleaned. Cleanup itself is not normal retirement evidence.

Failed attempts are retained: direct invocation of layout in an inherited
controller Console fails geometry initialization; the same unchanged fixture
passes in its own hidden Console. A first deployed product smoke reused S3's
old manifest, so its final hash check correctly fails on the newly replaced
NTMON despite three successful workloads. Fresh S4 smoke checks the S4
manifest and passes. Neither failed attempt is relabelled as success.

Physical desktop/focus/RDP clipping and default hidden-desktop53x15/arbitrary
large-environment limits remain the prior explicit boundaries, not repaired
or newly passed. WINMINE startup/WOW projection is not gameplay; SOL/WRITE
remain separate retained frontiers. Known long-path defect remains untouched.
T433 stays open; S5 requires owner admission. Commit/push identities are
recorded after delivery; no hands-on owner acceptance is claimed.
