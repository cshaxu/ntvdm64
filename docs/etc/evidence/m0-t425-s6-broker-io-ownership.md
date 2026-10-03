# T425 S6 broker-owned I/O handoff — release evidence

## Question and authority

The owner requires NTCON and each worker to retain only their current physical
I/O transport, not their logical associations or acquisition policy. NTSRV
owns those associations and grants, including both-end disconnect confirmation.
S5 8936db484, protocol/RPC36 and I/O24, remains the recovery baseline.
The verified r015 S6 eight-file set is published and postpublication smoke
passes. The containing reviewed S6 P delivers the repair. T425 remains open.

## Source and disposition

All changes below are project-added adaptation; no original MVDM/OpenNT-host
mirror, guest or imported-library change is selected.

| Owner and implementation | Selected mechanism and boundary |
| --- | --- |
| NTSRV frontend_registry.c | One current route, admitted execution/resume grant, explicit frontend close instruction, two endpoint acknowledgements before another grant. Logical multi-worker associations remain service-private. |
| worker-base connection.c | Shared authenticated acquisition/release RPC sequence and locally owned pipe/peer/ready handles. Peer handles authenticate transport; they do not authorize lifecycle decisions. |
| NTCON session_service.c / console_channel.c | Receive broker connect/close instructions; create the pipe and export the worker endpoint through NTSRV; stop/join/dispose before close acknowledgement. No EOF-based selection of the next owner. |
| NTCON native_console_frontend.c | Sole borrowed channel identity protects rendering/input. Removed binding_waiter list and wait_ready API; a conflicting bind immediately fails. Frame transactions and snapshot locks remain. |
| NTVWM main.c / execution.c | Native execution/Console handling remains local. Broker admission/release signals govern transport; a suspended parent cannot independently reacquire. Approved 30ms capture remains. |
| NTVDM win32/console_client.c | Original block/resume hooks use the shared broker sequence. Local input watcher uses stable readiness references. Original execution and completion are not extracted or replaced. |
| NTSRV base_service.c / lifecycle.c | Authorize the original no-command DOS resume result without changing it; distinguish an idle retained association from pending I/O work. |
| common service.acf | WorkerIoTransition and WaitFrontend explicitly use context_handle_noserialize alongside resident GetNext; service state remains guarded by the existing service lock. |
| common console/client.c | Both workers share non-poisoning unavailable-transport handling after normal disposal. No sequence/video serial mutation or persistent failure is created by an unbound exchange/publication. Existing bound-channel failures and parameter validation remain. |

RPC37 and I/O25 are candidate contracts. Temporary service/native I/O trace
helpers used to locate startup deadlock are removed; raw diagnostic reports
remain under build. Existing frontend bootstrap diagnostics remain unchanged.

## Procedure and observed results

Declared runs: build/M0-T425/S6/r001 through r015. Incremental cache:
build/M0-T424/S2/r001, MSVC Win32/x86 /MT CCPU40. Reports are preserved by run
identifier; older failing reports do not certify later artifacts.

- r002 native startup initially failed with 1460. Instrumentation showed the
  I/O RPC could not enter while resident GetNext held default context
  serialization. After the ACF repair, actual CMD input worked but return
  waited indefinitely because an idle association counted as pending work.
  Fixing that predicate produced real exit=23.
- r002 Window nested return then exposed missing DOS resume authorization.
  The original no-command RETURN_ON_NO_COMMAND branch returns STATUS_NO_MEMORY;
  its caller clears command size and resumes. The project grant now covers
  that original result without changing the return or original caller.
- `tests/observation/verify-broker-io-handoff.ps1 -Case nested-window` r002 r011
  and cleaned r003 r001 pass actual CMD -> COMMAND -> VER/MEM -> CMD output,
  executed parent marker, CAF visibility and final exit=23. The script records
  all eight runtime hashes and verifies they remain unchanged during each run.
  This is runtime identity, not yet full source/build-input closure proof.
- Production-linked `basesrv-service-reservation-test.exe` r002 r012 passes
  sixteen cases: io-authority; all five reentry orderings; frontend-delegated,
  frontend-wait, worker/root/request loss, frontend-rundown; collected and
  uncollected completed launcher rundown; completed-worker-loss and unfinished
  worker exit. Original reentry/result assertions remain. Fixtures now model
  authenticated root identity and broker grant before typed endpoint validation.
- The latest io-authority fixture additionally proves native release-event
  rejection before binding and for stale identity; exported event rights are
  wait-only. Actual production service owns the transition and acknowledgement.
- Cleaned r003 real `console-channel-lifetime-test` and
  `console-frame-failure-test`, via observer on unswitched private desktops,
  both exit zero. Full logs assert immediate conflicting-bind rejection,
  ordered input, stale channel isolation, publication rollback/terminal failure,
  joined real channels and exact post-warm-up handle equality (441).
  Their broker attachment is substituted: these supplement, not replace, the
  real nested probe and RPC authentication gates.

Frontend-local pending/cancel/wait tests are replaced because that mechanism
has been removed. Broker cancellation, death and both-close barrier tests remain
production-linked; endpoint/input/handle tests remain frontend-local. Do not
claim the old multi-pending frontend behavior as the new acceptance contract.

## Open completion gates

- r003 Console17 passes. Its uninstrumented Window nested-MEM reproducibly
  times out. r004 with temporary failure diagnostics passes Window17, but that
  timing-dependent diagnostic run does not certify the uninstrumented package.
- Presentation/input-return fixtures pass 441/688 checks after replacing the
  obsolete pipe-activation expectation with rejection of every activation
  command. Existing frame/input-order assertions remain. Common control passes
  198 checks and the link-ownership verifier passes.
- r006 channel/frame-failure fixtures pass with stable handles (448/441).
  Channel stop no longer calls CancelIoEx through a borrowed pipe handle which
  the channel thread may already have closed. Timeout-only observer capture now
  follows test descendants and records the actual NTVDM stack.
- r006 failure is in original nt_graphics_tick's text-publication error path.
  Project console_client now rejects a queued publication on the normally
  released, disposed transport with ERROR_NOT_READY rather than forwarding a
  null handle into the transfer mechanism. Invalid payload arguments still
  return ERROR_INVALID_PARAMETER. No original mirror is changed.
- r007 production-linked Console client fixture passes, including unavailable
  released publication, unchanged frontend serial, real invalid-argument
  rejection and successful reacquisition. Uninstrumented Window nested-MEM
  still fails: the captured stack is now cmdGetNextCmd -> RcErrorDialogBox,
  with the environment-setup error rather than the graphics-tick error.
  This is progress in failure localization, not a full runtime pass. r008 is
  an isolated failure-only GetNext diagnostic package, never a release; it
  passes nested-MEM and captures only applied original retry statuses
  STATUS_INVALID_PARAMETER/STATUS_NO_MEMORY, not a failing RPC. That does not
  explain r007's environment error. The temporary client diagnostic is removed
  and the worker rebuilt for r009's uninstrumented repeat. A successful traced
  run must not replace repeatable ordinary-product verification.
- r009's first uninstrumented repeat passes; the second again fails in the
  formal GetNext call in cmdGetNextCmd (object disassembly confirms the dialog
  call at function offset 0x6ab). r010 diagnostic repeats pass three times;
  r011 excludes ordinary probe statuses and again fails. These diagnostics
  are removed, not retained as production logging or passing final evidence.
- The production-linked released-query regression fails before the common
  repair (r011: fixture exit=1, error=87). A late state query on a disposed
  client previously latched that transfer error; a new transport did not clear
  the newly poisoned instance. The shared client now returns NOT_READY before
  transfer or sequence mutation. It preserves existing fatal bound-channel
  errors and invalid argument errors. r012's same fixture passes, as do three
  uninstrumented Window nested-MEM repeats with their original input timing.
- The temporary NTVDM-only video guard is removed in favor of the common
  exchange/video mechanism. r013 rebuilds that final worker and reruns the
  production client fixture (expected close callback exit=0x49) and all seven
  worker-neutral input/presentation fixtures. All pass; presentation and input
  return retain 441/688 checks with zero failures. Its Console17/Window17
  Console17 and Window17 both pass in full, including nested-MEM and EDIT exit.
  The final io-authority fixture also exits zero;
  common control again passes 198 checks with zero handle delta.
- Final r013 real channel-lifetime and frame-failure observers both exit zero.
  Retained modern EDIT return passes actual screen detection, Ctrl+Q, native
  CMD echo, DOS MEM and launcher completion. Retained relaunch also passes.
  The first two-session management attempt fails with RPC1717 because the
  retained monitor-rpc-test executable still has the prior interface. Rebuilding
  that fixture against RPC37 and rerunning with fresh reports passes real close
  acknowledgement and independent-session input/exit=23. All four broker
  retirement cases pass: NTVDM/NTVWM worker loss and frontend loss, failed direct
  receipt, broker-ordered peer retirement and empty-service retirement.
- r014 rebuilds the affected VDMREDIR link closure after the shared-client
  changes. The separate WOW graph has an explicit ntvdm.lib dependency and
  reports no pending work. A fresh candidate copies all eight graph-selected
  products, checks their x86 machine fields and records source/runtime hashes;
  earlier six-EXE plus retained-DLL runtime results remain predecessor evidence.
  RPC fixtures are rebuilt before final control/GUI verification to avoid a
  stale-interface test-tool mixture. r014 full-package gates are still open.
- r014 RPC r001 passes the first nine cases, then native-command fails at its
  unregistered-route take. The fixture now admits its real frontend association
  before delivery; no production authorization is loosened. Its first relink
  is blocked by a failed fixture child still holding the EXE; r002 therefore
  still exercises the old image and fails. Exact-path test-child cleanup and
  successful relink precede r003: native-command passes, but native-registry
  still expects connection delivery before execution admission/acquisition.
  That fixture now asserts denial before admission, then uses the real take
  and ACQUIRE before checking the original endpoint/attachment assertions.
  Both final r004 native-command and native-registry cases pass. All five GUI
  cases also pass: startup-only, explicit wait=37, GUI-created fresh text,
  carrier retirement without killing GUI, and text->GUI->text exit=19.
  These fixture updates change no runtime product hashes. r014 freezes 284
  source/generated/build-graph inputs and verifies 47 imported library inputs
  unchanged. Link-ownership leakage negatives and documentation governance pass.
  Final r014 Console17/Window17, all five actual version negatives and strict
  repeated DIR pass. Its WOW observations do not pass: all three guests exit
  1067 before the retained visible frontier. The aggregate script's terminal
  success only proves it collected those observations, not WOW non-regression.
- An isolated r014 WINMINE repeat again exits 1067. With the same current
  observer, retained S5 instead reaches the visible WINMINE window and its
  historical bounded observation. This proves a candidate regression rather
  than an unavailable desktop or observer-only difference. Worker startup
  probes character I/O before selecting WOW's original native-window route;
  the existing standalone caller tolerates ERROR_NOT_SUPPORTED, not an
  authentication failure. WorkerIoTransition had rejected authenticated WOW
  with ERROR_ACCESS_DENIED before reaching TakeFrontend's original unsupported
  disposition. The service now authenticates first, then preserves that
  unsupported disposition. No WOW/frontend association or original mirror
  change is introduced. The production-linked fixture adds stale-peer and
  launcher denial plus authenticated WOW ACQUIRE=ERROR_NOT_SUPPORTED assertions.
  Affected products and fixture are rebuilt; r015 stages a fresh coherent
  eight-file package. Its strict three-guest comparisons against S2, S3 and
  immediate S5 all pass, including last-sample worker liveness and exact
  visible window/error-text signatures. These are retained observation
  frontiers, not gameplay acceptance. Separate rebuilt --wow-start-late-query
  and default archive fixtures exit zero with all new fallback/authentication
  assertions. r015 Console17 and Window17 both pass in full with all eight
  product hashes unchanged. All sixteen production-archive authority,
  cancellation, reentry, rundown and WOW completion cases also pass after the
  fix, including the new fallback assertions. Final retained RPC/GUI/version,
  EDIT/relaunch/isolation/retirement, DIR and actual nested-window handoff
  verification is running against this fresh complete package.
  Final r015 RPC11, GUI5, five actual version negatives and modern EDIT return
  pass. The remaining driver then stops at its exact-path broker guard: it
  permits the physical runtime/cache paths but not the tests' Z: alias. The
  broker has already retired by the subsequent read-only CIM check, so no
  unrelated process is stopped and this is not claimed as a production fault
  or a completed retained gate. A fresh retained-r002 driver accepts only the
  physical candidate path or its owned Z: alias with identical product hash,
  records rejected identities explicitly and keeps unrelated-session denial.
  It reruns EDIT then proceeds through relaunch/isolation/retirement/DIR and
  actual Window nested handoff, without overwriting the earlier reports.
  All 284 source/generated inputs are freshly sealed and all 47 imported
  library inputs remain unchanged. r014's sealed inputs are retained as its
  predecessor rather than rewritten to describe the repair.

## Whole-objective verification map

| Requirement | Actual source boundary and proof | Current disposition |
| --- | --- | --- |
| Logical frontend/multi-worker associations only in NTSRV | Service-private frontend_routes and frontend_io_route; NTCON session_service owns one channel, no association list. Actual diff/source sweep removes binding_waiter and wait_ready; --io-authority tests two admitted associations and one grant. | Source/focused and r015 real Window nested handoff pass. |
| NTCON cannot arbitrate or queue an incoming worker | Conflicting local bind rejects immediately; pipe activation control is rejected. console-channel-lifetime-test and NTVWM presentation/input-return tests retain data/input/transaction checks and reject activation. | Final changed frontend implementation passed r013; shared input/render code unchanged in r015. |
| Workers acquire/release only via authenticated broker | worker-base io_open/io_close; actual service admission precedes ACQUIRE, a competing ACQUIRE cannot force release. --io-authority and RPC native-registry tests assert denial before admission. | Final r015 archive and real RPC11 pass. |
| Both actual pipe ends close before next grant | NTCON stops/joins/disposes before FrontendIoDisconnected; worker closes before RELEASED; service holds route until both acknowledgements. --io-authority blocks both next acquire and release completion until frontend ACK; real channel fixture joins/cancels/drains and checks stable handles. | Archive/channel proof and r015 actual Window nested handoff pass. |
| Original DOS execution/reentry and native Windows completion remain local | No MVDM/OpenNT-host diff. The project adapter grants after original GetNext, including original RETURN_ON_NO_COMMAND status; native execution still waits actual target and reports its result. Five reentry orderings and completion/rundown cases remain asserted. | All sixteen r015 archive cases, Console17/Window17, EDIT, both-session isolation and all four retirement cases pass. |
| Normal disconnection is not task failure or idle reacquisition | Expected disposed-client queries/publications return NOT_READY without poisoning sequence; resident association persists, admission required to reconnect. Strict before/after client fixture, --io-authority and final nested-MEM both directions. | Focused and r015 full matrix proof pass. |
| No new process/component/ticket, original WOW boundary retained | Existing authenticated RPC identity and one I/O pipe; no new executable or mirror/library change. WOW authenticated unsupported probe retained while stale/launcher peers denied. | New archive assertions and all three strict S2/S3/S5 WOW frontier comparisons pass. |
| Coherent delivery and original side-test baseline retained | r015 eight x86 product hashes, 284 frozen source/generated inputs, 47 unchanged library inputs and 57 unchanged guest/config files verified. S5 recovered eight-file backup verified before publication. | S6 published, post-smoke passes; containing reviewed P delivers S6. |

## Final package and publication

All r015 prepublication gates pass: RPC11/GUI5, Console17/Window17, sixteen
archive cases, five actual version negatives, retained WOW comparisons,
modern EDIT return, cooked outer-CMD relaunch, independent Console isolation,
four broker-ordered fault/retirement cases, strict repeated DIR and actual
Window CMD -> DOS VER/MEM -> CMD parent output/input and direct exit=23.
The first retained driver rejection is preserved; retained-r002 accepts its
owned alias only after checking product identity and completes all gates.
The publication DIR repeat additionally captures its strict observer stdout
(`final=0 entered-dos=1 dirty-prompt=0`) for mechanical release verification.

Commands use r005 observer and the retained private-desktop fixtures:
`build/M0-T425/S6/r015/verify-package.ps1`, `remaining-gates.ps1`
(RPC/GUI/version/EDIT passes, then alias-guard stop), `retained-r002.ps1`
(complete retained/nested gates), and `publish.ps1`. Compiler selection is
VS2022 BuildTools MSVC14.43.34808, Windows SDK22621, Win32/x86 /MT CCPU40,
dependency-driven Ninja cache build/M0-T424/S2/r001. The WOW graph explicitly
depends on ntvdm.lib; it remains current. Product/source/build identity is in
r015 candidate-manifest.json and release-source-manifest.json, not inferred
from the existence of cached binaries. No mirror/library/guest changes occur.

`publish.ps1` verifies test reports and sealed inputs, checks all eight prior
published hashes, saves/validates a coherent S5 backup under
r015/accepted-s5-recovery, then replaces only the selected eight product files.
Its error path restores and verifies all eight S5 files. Current publication
passes; guest/configuration and user data are not overwritten.

| File | Published S6 SHA-256 | Retained S5 recovery SHA-256 |
| --- | --- | --- |
| run16.exe | 69716A12204E82E8AAA91A411400E2ADB47467F071B5DE222CD7471BE8435320 | 9BF75F5B634550503417242AB2A515292E03E60FC5C3AF860F0C755DEDB8647B |
| ntsrv.exe | 87FE2C77B1224CD14C0EE632E8DCD6128C5B3E0A011D16808C6F70FF77C548B1 | 8ACA4D82B3D9CAE1E04BE7CF937ADE263DB0B2BF5B293C0D7BA16EE88DC1DB7E |
| ntvdm.exe | 7161022689556F3D3E6D7633A72F335EA08F446E8EA2D0C82021A02D55373E5C | F29B70759B94843C78AF83C4F277BAD72FFF13FF77C40BEFFDF1CEC575E8E0B7 |
| ntvwm.exe | 8C1E20876361C9B8E6C8F0F8DF91E96A4F38749E6FD04C26B5A4062BD6AD2351 | 9AD76C4D824B774D8C0F0B026BF00788586CF8FB06123AF4270602A748B646A1 |
| ntcon.exe | DAAFB3E4A6D88EF90A1CCD1D1EA22D893C718726503EF548BA165EAE366D87BF | CCA29825256F4CC83D67DEFEF3C57BF9A2D3253E029928444F6AAC54C7C58C5C |
| ntmon.exe | 9FDC95C114DBE2473D46415043AD5193CCB66DFA4B97D56A09B6351D49757EF2 | 9B861F15171C13785A1E9274C354062B073AA5B24DD7570BCDF20318B1B0E6B5 |
| WOW32.DLL | BEB688FE832921D788DB297787B2D1062D7FDD095AF1A304824F31DE7F19D0C1 | BEB688FE832921D788DB297787B2D1062D7FDD095AF1A304824F31DE7F19D0C1 |
| VDMREDIR.DLL | 8AB0FD23232B899FF0454CD78FC3CCAA48923851CDF74B349C0C2A0855FCC955 | 19228039788CFD099553A090B30E463B564101468160EFC31948C8A1651C307A |

Postpublication-smoke.ps1 passes 12 rapid native/DOS pairs, 12 interactive
native CMD relaunches, actual cooked outer-CMD return/exit=19 and GUI
startup/wait=0/37. All eight published hashes remain identical. The final
diff/source review checks each row above against actual production callers,
not merely declarations; logical association and arbitration are service-only,
both endpoints own/close their local transport, and no original execution,
completion or lifecycle algorithm is extracted. Source/guest/library identity,
documentation governance, relative links and diff checks pass. Other-session
Queue/proposal edits are preserved outside the containing reviewed S6 P.

Physical desktop/RDP observation is owner-waived, not passed. Three WOW checks retain
their previous bounded window/error frontier, not new gameplay capability.
Approved native 30ms capture remains; full NTVWM reentry/NTMON tree work is not
silently included. T425 remains open for owner verification.
