# T448 S2 implementation and verification ledger

Status: in progress, not acceptance or publication. Source baseline c838c332c;
published product remains 4d4ef4d47. This record does not supersede the
[approved plan](../operations/t448-lifecycle-plan.md).

## Implemented candidate

- Run16 no longer retains NTCON/worker processes to decide task completion or
  submission retry. It submits once and waits on NTSRV-owned receipts plus the
  authenticated service process. Bootstrap has one ten-second deadline.
- NTSRV owns native-worker readiness, parent reacquisition after DOS completion,
  and explicit frontend parking. Management draining excludes normal parking.
- NTCON executes the park instruction rather than observing the launcher and
  querying the worker census. Its competing transport hard-kill watcher is removed.
- NTVDM lifecycle reception is being separated from character I/O, so WOW can
  receive worker-close without a frontend. The original CntrlHandler and
  TerminateVDM remain distinct executable-owned entries.
- The candidate protocol is 48, application version 0.0.448. It requires a
  coherent thirteen-image publication, which has not happened yet.

## Actual runs

Temporary owner: build/t448-s2, under the budget in CURRENT. All jobs hidden.

| Run | Evidence and result |
| --- | --- |
| Full components, first candidate | PID 22232, 2026-10-21 02:38:48-02:42:12 UTC. All thirteen images built; second pass preserved 1376 intermediates and output hashes. build-result.json and build/result.json. Later edits invalidate this as final acceptance. |
| Fixture build | PID 22396, 02:48:09-02:48:23 UTC, exit 0. Earlier runner attempts did not execute correctly and are not passes. |
| Focused fixtures | Parent PID 32916. frontend-scope-lifetime-test (PID 544), basesrv-idle-policy-test (32596), basesrv-reservation-test (25976) passed. |
| Service matrix, first candidate | Failed frontend-authority at assertion 812: old expectation was no action after creator disconnect; candidate emits explicit PARK. Assertions updated to require PARK, not merely accept arbitrary nonzero status. Matrix must be rerun. |
| Full components, revision 2 | PID 31448, failed: source-shaped client header lacked STATUS_PORT_DISCONNECTED declaration. Added the original numeric status to the project-owned client declaration; no mirror or guest change. |
| Full components, revision 3 | PID 24264; completed 03:00:34 UTC. Thirteen images, 1380 unchanged intermediates in second pass. Later separate-WOW/input-watch edits require another build. |
| Fixture build, revision 2 | PID 32404, 03:01:15-03:01:35 UTC, exit 0. Includes x86 input-watch and Console-client fixtures. |
| Focused/service revision 2 | PID 14000, 03:02:04-03:02:18 UTC, exit 0. Three focused fixtures and all 33 service cases pass. |
| Input-watch revision 2 | PID 31992; 631 assertions pass, including lifecycle-only reception and 50-cycle handle count. |
| Console-client full graphics fixture | Parent PID 2668; callback case stopped before close at wait_video_serial assertion 882, not a palette-value assertion. Not a close pass. Exact owned suspended child PID 12668 was verified against fixture path/parent and terminated. Revision 2 likewise lacked the fixture's explicit asynchronous publication grant normally issued by the SoftPC video tick. Added that grant at initial/recreated surface activation; no production publication change or weakened assertion. |
| Full components, revision 4 | PID 13164, 03:07:20-03:10:27 UTC, pass thirteen images and 1389 unchanged intermediates. Separate-WOW startup receipt and original-TD launch identity added without mirror edits. |
| Focused/service revision 3 | Parent PID 18820, pass three focused fixtures and all 33 service cases, including authenticated separate-WOW startup before/after command delivery. This is service-fixture evidence, not actual separate-WOW application proof. |
| Console fixtures, revision 3 | Pass lifecycle-console, lifecycle-wow, complete callback, hung-callback fallback and pipe-error. Fixture build PID 14716, 03:14:46-03:14:59 UTC. Logs console-fixtures-r3.out and console-r3-* retain the individual reports. |
| Isolated package | PID 25648 staged all thirteen revision-4 images with SHA-256 comparison; immutable guest inputs expanded from assets/winnt.zip. build/t448-s2/package is a candidate, not formal publication. |
| Actual WINMINE explicit wait | mine-off/result.txt and probe.txt: pass flag/counter, F2 reset, open cell, WM_CLOSE and launcher exit 0. Worker PID 24804, observer PID 2792; native frame, private desktop. |
| Actual WINMINE default startup | Runner PID 3124, observer and probe reports in mine-startup/. Added -StartupOnly to the retained gameplay test: launcher exit 0 is required before any gameplay/close input, then the surviving game must pass flag/reset/open/close. Passed with worker PID 2004. Proves shared WOW default startup return, not separate-WOW propagation. |
| Full components, revision 5 | PID 23936, 03:20:56-03:23:58 UTC, pass thirteen images and unchanged incrementals. Local WOW-startup timeout now latches the client failure before finalization; nested parent reacquisition shares the service finalization deadline rather than starting a second ten-second budget. |
| Actual separate WOW | First native witness incorrectly treated returned startup-only Run16 handles as worker-lifetime handles and failed its overlap assertion. Corrected the witness to require successful launcher return while both independently pinned workers remain alive, then verify worker exit/surviving-peer callbacks. Runner PID 1896, separate-workers-r2/result.txt passes both real workers, idle callbacks, independent retirement and startup-only returns. No assertion of worker survival was removed. |
| Full components, revision 6 | PID 21028, 03:25:23-03:27:08 UTC, pass thirteen images and second incrementals. Includes launch-style failure propagation and removal of the unused Run16 parent-resume API path. Isolated package-r6 hash-verified; not formally published. |
| Real service death | Runner PID 34764 staged package-r6 and terminated its exact NTSRV PID 34596; Run16 returned 6 in 87ms. Test cleanup failed because the orphan native CMD temporarily pinned nthook64.dll. No worker cleanup pass. After its exit, removed only verified O:/tmp/n19d stage, retaining package-r6. Runner PID 27624 repeats with explicit pinned-native-task test containment: return 6 in 74ms, test cleanup complete. service-loss-r6b contains identities/report. |
| Latest service matrix | PID 26944, 03:32:46-03:33:00 UTC, passes all three focused tests and 33 service cases against revision-6 inputs. |
| Deadline test setup | First attempted old reservation-parent setup failed its unauthorized root registration, before timeout testing. The product correctly rejected it. Replacement uses real StartFrontend and pauses only that pinned test frontend; service remains live. No deadline pass claimed yet. |
| Fixture builds | PID 29908 failed a target placed in the wrong architecture graph; PID 26984 then exposed the old removed Run16 resume API in its fixture. Removed the obsolete launcher-resume mock/test, retaining service parent-resume-origin coverage and adding NULL-start rejection. PID 30628 rebuilt successfully. |
| Full components, revision 7 | PID 27704, 03:34:56-03:36:40 UTC, pass full/incremental. Atomic first failure and normalization of remaining Run16-used control RPC exits. Supervisor PID 28812 staged package-r7 and built fixtures. |
| Live-service finalization deadline | client-deadline-r7 observer Console output: status 1460 at 10000ms; same service process alive. Fixture pauses only service-created NTCON, verifies repeated finalization and disconnect take less than 1s together, then resumes it. Test containment is not normal frontend-cleanup acceptance. No guest change, task replay or relaxed authentication. |
| Final return priority | Code review found a prior nonzero task result could conceal a later latched disconnect/timeout. Run16 now queries the independent transport latch; it never passes a task exit code to the transport classifier. Full/incremental build PID 33024 passed 03:37:31-03:39:15 UTC. |
| Final candidate actual regression | Supervisor PID 16012, package-r8: native output/exit 37, shared WOW startup-only, explicit --wait, separate-WOW workers and SOL deal/stock/close pass. WRITE retains illegal-instruction startup failure. DOS17 initially rejected the fixture path before executing; rerun PID 30484 used the existing required task/S path and stopped on COMMAND's first-prompt timeout. Remaining sixteen routes were not executed or passed. |
| Additional DOS comparison | Candidate PID 28272: direct MEM, independent DOS exit-7 probe and COMMAND /c all time out before output; native exit 7 passes. Baseline PID 33076: fresh complete thirteen-image assets/release set, same guest/observer/fixture gives exactly the same four outcomes. See baseline-r8-inputs.json and baseline-focused-results.json. This establishes matching observed failure, not root-cause attribution or DOS lifecycle success. |
| Final candidate native GUI | PID 29532: x86 and x64 GUI startup-only, --wait exit 37, GUI-spawned text child, nonresident carrier/target survival, and text-GUI-text exit 19 all pass. gui-r8-results.json and per-architecture assertion logs. |
| Final candidate service loss | PID 30592, package-r8: exact test service killed, Run16 returns 6 in 71ms. Test owns/cleans its pinned native CMD; no claim that production worker loss cleanup is complete. |
| Launcher request fixture | PID 8948: copied authority stripping, real native exit 37, final I/O result/error barriers, no NULL-start/parent-resume API and broker error clearing pass. |
| Governance | PID 22768: documentation governance and diff --check pass. Subsequent evidence edits need final recheck before delivery. |

## S2 bounded conclusion and remaining delivery gates

Update during S3: [controlled-environment results](m0-t448-s3-close-implementation.md)
supersede the DOS startup blocker below for the fixed lifecycle matrix: all
seventeen DOS/EDIT cases pass on package-r4 with a declared short test PATH.
Inherited long build-controller PATH remains a separate unproved compatibility
boundary. WRITE still fails its startup frontier. Publication remains deferred;
the historical package-r8 failures below are retained, not rewritten as passes.

The final package-r8 runs above complete the planned normal-chain verification
attempts, but do not pass every gate. Native CUI/GUI, shared/separate WOW, SOL,
real service death and ten-second finalization have positive evidence. DOS
normal-return acceptance is blocked before its target executes: the retained
timeout-live report contains a visible illegal-instruction dialog (guest-seven:
CS 03f4, IP 23a9, bytes 65 63 69 66 69), not merely a silent Console wait.
The complete published baseline with identical guest inputs also fails. WRITE
retains its separately recorded illegal-instruction frontier. No cause beyond
these observations is claimed and no guest modification is authorized.

S2 P publication/commit/push is deferred, not accepted. The existing execution
rule permits sequential S reopening at this bounded conclusion with the deferral
explicitly retained. S3 now implements only its previously approved close/fault
group. All S2 changes and exact test packages remain intact. The following
checklist remains the delivery review, not a new implementation expansion:

1. Re-run startup-only shared/separate WOW and affected runtime gates on the
   final candidate. Earlier revision-4 actual tests pass; later transport edits
   are not covered by those earlier hashes. No guest changes are allowed.
2. Verify basic admitted-session loss and timeout returns, including no cleanup
   RPC extending an exhausted deadline, without normalizing real task exit codes.
   Review task-result versus finalization-failure precedence: a prior nonzero
   task result must not conceal a subsequently latched service-loss return.
3. Verify independent lifecycle-only reception and its teardown; input-watch
   optional sources must not create duplicate wait handles or competing close owners.
4. Re-run the complete service matrix and affected production-linked fixtures,
   then real DOS/native/WOW startup and return regression against one candidate.
5. Review authority ownership and exception/diff inventory, publish all thirteen
   images coherently, hash-verify both destinations, commit and push.

Full management concurrency, spontaneous worker exit permission and role-specific
service-loss cleanup remain the already assigned S3 work, not newly deferred
S2 defects. End-to-end CAF acceptance remains S4. No current candidate result
establishes that the inherited visible-input defect is fixed.

No OpenNT mirror or guest files have been changed in this candidate so far.

## Independent review pass and inherited acceptance boundary

The single-person reviewer re-read the production diff, not just the run logs:
service receipt/reservation authentication, separate-WOW delivery latch, optional
input-watch handles, normal PARK versus management draining, transport-error
versus application-code separation, and original close callback ownership.
The review removed the obsolete Run16 resume entry, joined nested finalization
deadlines, propagated remaining launcher RPC failures, made first-failure capture
atomic, and corrected final-result precedence. Those changes were rebuilt.

The complete DOS/EDIT matrix is still not proven. Matching published-baseline
failures are inherited from the explicitly transferred T447 frontier, not waived,
repaired or attributed to immutable guest merely by association. No S2 or T
functional acceptance follows from these partial passes. Final pre-closure
acceptance must retain the actual DOS/WRITE gaps and their independent evidence.
S3 still owns the already planned worker/frontend service-loss cleanup,
whole-worker exit permission and management concurrency; source watcher hard
termination remains an explicitly unfinished implementation, not a new solution.
