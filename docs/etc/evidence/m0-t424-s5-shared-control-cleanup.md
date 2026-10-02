# T424 S5 shared-control cleanup

## Question and baseline

Remove the five owner-approved project-code duplications without changing
original DOS/WOW execution or the published S4 contracts. Baseline is
`e4fbaed21`, admitted by `ce7033379`, protocol/RPC 30. All five cleanup rows
are implemented and pass the delivery gates below. Disposable output belongs under
`build/M0-T424/S5/r001`; the valid S2/r001 x86 /MT CCPU40 cache is reused.

## Provenance and disposition

| Logic | Source/current location | Shared decision/target | Independent boundary |
| --- | --- | --- | --- |
| Completed-first byte transfer | Project additions: run16-exe/native_request_io.c and ntw32-exe/channel_io.c, identical bodies after symbol/include normalization | Reuse the existing frontend-client transport object in native production and fixtures; remove channel_io.c and io.h, not a forwarding wrapper | worker-base/console_client.c has strict-peer-first semantics and must not replace this contract. NTSRV/NTKVM consumers cannot become worker-base consumers. |
| Launch packet and resource primitive | Project additions in run16-exe/native_launch_packet.c and native_launch.c, linked by multiple executable owners | Retain the finite owned-client implementation model; audit one object/provider per executable, declaration-only interface | Target creation and hidden Console execution remain native-worker-local; no generic common root. |
| Native completion/return | Project additions in run16 frontend_scope/main and NTSRV native result path | Launcher closes the startup diagnostic target handle immediately; broker receipt and authenticated target_completed result determine completion/Console return | Real target handle/wait stays NTW32; original DOS completion remains original. |
| NTKVM session plumbing | Project-added session_service.c/h | capability/restored/borrowed/admitted stored fields have no read sites; borrowed argument still selects Console anchor at startup. Remove only dead storage and redundant internal parameters | Notification, creator, retire, Console anchor, restoration report and channel joins remain. |
| Direct bootstrap fixture | Project tests/app/frontend_bootstrap_test.c | Retire the dead direct-bootstrap fixture and redundant broker-test executable alias; see assertion mapping below | Current authentication, restoration, reuse and broker-loss assertions stay in the retained fixtures. |

The two workers already share connection/watch-broker and Console protocol
clients through worker-base. Their original/backend-specific execution is not
an extraction candidate. Consumer-side policies remain in their executable
owner and use common paths, rather than expanding worker-base into a generic
library.

Worker-only reuse is already production-wired: connection/watch-broker,
authenticated frontend capability resolution and shutdown-event acquisition
are implemented in worker-base/connection.c. Both workers embed the explicit
ntkvm_worker_client state from interface/worker_console_client.h and use
worker-base/console_client.c for request sequencing/validation, input batch
encoding, frame transactions and title publication. Callers keep their endpoint
locks; the client owns its overlapped event, not borrowed pipe/peer/cancel
handles. Backend activation/retry and original DOS API partial-result semantics
remain caller-owned. This is the shared mechanism boundary, not a second
task registry or common scheduler. Consumers do not link NTKVM private rendering.

## Bootstrap assertion transfer

The S4 graph already compiled broker_frontend_bootstrap_test.c for both
executable aliases. The obsolete file was not a selected build input and used
removed APIs. Removing it does not remove the actual broker fixture.

| Old assertion | Retained coverage/current meaning |
| --- | --- |
| Cannot inject caller handles | Broker fixture rejected_peer DuplicateHandle denial |
| Wrong version/application/status, partial reply, unregistered peer, no handle leak | Broker fixture --startup-rejections |
| Finite silent-live timeout and cleared resources | Broker fixture --startup-timeout |
| Real owner identity and forged capability denial | Broker normal path parent_pid, RetainFrontendRoot and fake-capability checks |
| Non-root management denied, retired capabilities cannot rejoin | Retained service RPC frontend-root/authority cases; broker fixture denies uncreated root and forged restoration. Old fake-root registration is no longer a valid success under S4. |
| Local creator references do not own root lifetime | Broker normal path root remains alive after return; scope-lifetime inner ownership and parked-root assertions |
| Independent root identities | Retained verify-ntw32-management -TwoSessions isolation gate |
| Broker loss closes frontend | Broker normal path death wait and authenticated process watcher |
| Restore before caller returns | Broker normal path ReturnFrontendConsole/restored/WaitFrontendConsoleRestored, scope channel joins |

These actual broker/service/isolation gates passed in S4 and remain required
for S5. Mapping is not a fresh S5 integration-pass claim.

## Verification state

Affected x86 NTW32/NTKVM and focused fixtures compile/link from the reused
cache. control-transfer-test has 72 checks / zero failures: mock/unit ordering
evidence, not a real kernel test. Actual request/execution fixtures pass:
NTW32 request-lifetime reports 586 checks / zero failures / zero remaining
handles, including cancellation and target survival. Frontend-request-client
retains final-presentation error/EOF, partial reply, version/rejection and dead
worker checks. Frontend-scope-lifetime passes stream roles, inner admission,
parked lease, shutdown priority, channel reclamation and final join.

Commands: existing run-ninja-parallel.cmd builds control-transfer-test.exe,
frontend-request-client-test.exe, ntw32-execution-lifetime-test.exe,
frontend-scope-lifetime-test.exe, ntw32.exe and ntkvm.exe. Each fixture's report
is below build/M0-T424/S5/r001. Generator uses the installed Node executable
explicitly; the first invocation without that argument failed its prerequisite
check and is not a successful build.

Removed duplicate transport: 28-line body and 6-line private header. NTKVM
removes four write-only fields, redundant capability/restored parameters and
the duplicate lease-start wrapper. Borrowed Console anchor selection and
authenticated restoration remain. Single transfer source links once per
consumer; NTSRV's binding archive selects the same source, not another body.
No mirror, guest or shared-library source changes.

## Broker completion contract

Application protocol and RPC major are both 31, with regenerated MIDL and
full x86 production/supplemental linking. FinishNativeRequest returns an
authenticated target_completed flag separately from final-I/O status. A real
exit remains completed even if the final acknowledgement fails; an unfinished
worker failure, forged request or replay does not grant Console return.
SubmitNativeRequest verifies the exported process PID against the bound record
and compares the exported receipt object before exposing the request. NTW32
remains the only target waiter; NTSRV does not add a second process wait.

Scope lifetime tests deliberately return an unsignaled diagnostic event as the
startup handle. Production launch closes it before waiting and uses the broker
receipt alone. Success and final-I/O failure preserve exit 37 and completion;
unfinished worker failure returns 1067 without completion. Handle delta is zero.

Actual private-desktop broker tests pass normal startup, startup rejection,
startup timeout, unfinished worker loss and completed-target worker loss.
The completed-target test initially failed an incorrect expectation that loss
must discard an already buffered acknowledgement. Diagnostic evidence showed
status 0, exit 37, completed 1. The corrected assertion retains actual exit and
one-time receipt consumption, permitting a valid buffered acknowledgement or
an explicit final-I/O failure. Both failed reports are retained, not called passes.

Fresh reports: control-transfer.txt (72/0), frontend-request-client.txt,
frontend-scope-lifetime.txt, ntw32-execution-lifetime-protocol31.txt (586/0,
zero remaining handles), and broker-*-final.txt under the S5 build root.
An attempted request-lifetime rerun using an existing exclusive-create log
failed; its fresh protocol31 log passes and is the cited new result.

The full product gate and coherent publication are verified below. Git delivery
is recorded in CURRENT after the reviewed production P is created.

Console17 and Window17 both pass 17/17 from the frozen protocol31 runtime
through Z:. Actual service reservation/original Check/Update/Get/ExitVDM and
management RPC authentication/version fixtures pass. A relaunch probe invoked
with the 52-character physical build path failed before entering DOS; MEM was
then executed by outer CMD as an incompatible native image (exit 216).
This is retained as failed evidence, not a product pass or a new long-path fix.
The owner-approved Z: path is used for the required DOS/native integration.

Relaunch through Z: passes DOS MEM/EXIT -> native VER -> DOS MEM/EXIT ->
outer cooked CMD exit 19. Actual modern EDIT renders its File/Edit/View/Help
screen, accepts Ctrl+Q, returns to native CMD echo and DOS MEM, then completes
with the retained baseline exit 1. Independent-session management passes:
the selected worker closes, the other Console remains live, accepts input and
returns 23. Protocol30's real RPC client is rejected by the listening protocol31
service with ERROR_REVISION_MISMATCH 1306. The first mismatch probe ran after
Z: removal and reports not-started; only the fixed-path retry is a valid negative.

The preceding passes use verify-frontend-relaunch.ps1,
verify-command-native-edit-return.ps1 and verify-ntw32-management.ps1
-TwoSessions. Reports use relaunch-z-final, edit-return-z-final,
t424-s5-isolation and rpc30-client-rejected-by31-final stems in the build root.

## Final delivery

verify-broker-retirement.ps1 passes all four NTVDM/NTW32 worker/frontend-loss
cases: failed direct receipt, broker-ordered peer retirement and empty-service
retirement. WOW observations preserve WINMINE's visible guest main class
00C900A800C000D7, SOL's known memory dialog and WRITE's known memory dialog.
These are retained frontiers, not three usability passes. Physical foreground,
RDP pointer/clipping and gameplay are not revalidated or claimed passing.

Full x86 /MT CCPU40 production/MIDL31 and supplemental WOW links pass. All
eight PE machine fields are x86; release-source-manifest.json matches the
frozen source/test/build inputs. release-candidate-manifest.json and
published-manifest.json match every O:/winnt product hash. Accepted S4 eight
files and existing configuration are recoverable in accepted-s4-recovery with
its manifest. Guest media, SYSTEM.INI and NTVDM.REG are not overwritten.
Published-relaunch.txt passes actual DOS/native/DOS, cooked outer CMD exit 19
and normal service retirement. No Z: mapping remains.

Removal accounting: the 28-line duplicate byte-transfer body and six-line
private header are gone; four write-only frontend fields and the redundant
lease-start wrapper are gone. The obsolete 183-line direct-bootstrap fixture
and its duplicate build alias are removed after mapping assertions. The
launcher drops the target-HANDLE completion parameter/status check rather than
keeping a forwarding compatibility path. Protocol result extension adds one
authenticated completion flag, not a second process waiter or task registry.

Documentation governance, relative links and git diff --check pass. Mirror,
guest and shared-lib source diffs are zero. Approved parallel planning changes
for ordered S6 naming/S7 common separation/S8 GUI/S9 frontend naming/S10 audit
are included as planning, not implemented capability. GUI explicit --wait
remains a separate launcher-owned path until S8; broader common/service
reorganization and Console-list consolidation are S7, not secretly done here.
T424 remains open for owner acceptance.
