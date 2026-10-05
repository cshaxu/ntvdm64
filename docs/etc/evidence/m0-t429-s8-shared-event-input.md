# T429 S8 shared event-driven input

## Question and admitted boundary

Owner approves common input thread machinery in worker-base, keeping native
20ms sampling only for hidden Console output. Baseline is Pf484f6193, S7
r001/runtime and retained r003/r004/r005/r007 proof. Wire and original mirrors
remain unchanged. Other-session proposal edits are preserved and excluded.

## Provenance and source decisions

| Logic | Source/provenance | Disposition |
| --- | --- | --- |
| Readable-source pin, infinite wait, stable local wake, read/peek rearm, close/stop | console_client.c::console_input_watch; project-added adapter | Extract worker-base/input_watch with explicit instance/owned source copy, borrowed cancellation, owner callbacks and stop/join. Both workers use it. |
| Original event loop, suspend priority, keyboard hardware/history and DOS mouse processing | mvdm/softpc.new/host/src/nt_event.c and original consumers | Remain byte-unchanged; NTVDM returns common stable wake to the original wait owner. |
| Native input decode, mouse coordinate adaptation, hidden Console input write | ntvwm-exe/presentation.c | Remain native backend operation; invoked on input readiness instead of capture timeout. |
| Native begin/end, broker release and execution lock | ntvwm-exe/main.c; project-owned | Preserve authority/final-drain/input-return; add explicit admission wake and serialize readiness consumer against actual ownership changes. |
| Frontend input queue/ready notification | ntcon-exe/console_channel.c, frontend_session.c; existing project mechanism | Reuse unchanged. No new wire/event producer or frontend lifecycle policy. |
| Native Console snapshot publication | presentation.c and worker-base/publication | Preserve50Hz copied publisher;20ms timeout only runs capture while presenting. |

Original input processing is available and stays composed. This reuses an
existing project-owned adapter thread rather than extracting mirror behavior
or inventing a scheduler. Output-change notifications remain out of scope.

## Verification and progress

Implementation is connected to both production workers. The private DOS
watcher and the native timer input call are removed. Common callbacks never
hold the common lock: owner execution/channel locks may acquire the common
lock, not vice versa. Source replacement pins a SYNCHRONIZE duplicate before
waiting; old transport handles may close without invalidating that wait.
Acknowledgement follows actual read/peek, not delivery or mere wake. Native
release disables readiness under its execution lock before final paint/input
return; teardown signals cancellation before joining outside that lock.

Native begin/resume wakes the output owner with route_changed; users becoming
active also wake it to switch idle INFINITE waiting to20ms acquisition. There
is no timer admission retry. Unexpected failure retains the worker-fault
contract rather than autonomously rediscovering a frontend. Native input and
capture are separate threads but still serialize mutable channel/ownership
state; this does not promise lock-free input while a capture is in progress.

Resources and fixture outputs live under build/M0-T429/S8; Z: is the sole
allowed temporary alias. Early r001/r003 and r004/r005 prove progressively
integrated candidates; final r008/runtime is separately sealed after the
complete six-EXE formal relink. Do not present earlier hashes as final ones.

| Evidence | Actual result |
| --- | --- |
| build log and build2.log | Affected x86 workers and transport/presentation tests link; VdmTib ownership passes. |
| build4.log, shared-linked-test-final2.txt | Complete six-EXE x86 build; fixture directly links production worker-base.lib;627 assertions cover stable wake/callback, rearm, pinned source replacement, invalid arguments, stop/close priority, failure callback and50-cycle exact handle-count return. |
| r002/fixtures.log | All7 existing private-Console fixtures pass; native presentation501 and input-return689 assertions retained. |
| close-fixtures.log, r010 | Original-handler callback result, bounded hung callback and broken-pipe contracts pass with extracted watcher. |
| r003/product/timings.json | Preliminary coherent package Console17/Window17/WOW passes,199939ms. |
| r005/results.json | Preliminary final-code package all8 native/nested Console/nested Window/EDIT/cooked return/close/worker loss/two-session cases pass. |
| r011/published-manifest.json | Final r008 eight-file package published to O:/winnt/system32; every source/published hash matches; old package preserved in r011/recovery/system32. |
| r009/product/timings.json | Final r008 package: Console17/Window17/WOW pass;203832ms total (Console61813, Window69011, WOW67454ms). |
| focused.log, r007 | Final r008 Console4/Window4 and real EDIT200 pass; mouse report records200 posted movements and input-sink-acknowledged=yes. |
| r012/results.json | Final r008 all8 actual native/nested Console/nested Window/modern EDIT/cooked return/management close/worker loss/two-session cases pass. |
| r013/published-smoke.log and verified-published-manifest.json | Actual O:/winnt Console4/Window4 pass; final all-eight published hashes unchanged. Logs in O:/winnt/Logs2/t429-s8-r013-*. |

Final-package integration and published smoke pass; bounded S8 closure is
reached. Actual ntvdm/ntvwm link maps place worker_base_input_watch_create in
worker-base:input_watch.obj. Governance, relative links, ownership and diff
checks pass. Delivery is committed/pushed with this record; T429 remains
open for owner acceptance, with no next S admitted. No original mvdm/opennt-host source, NTCON
frontend policy, protocol version, helper or process changes. Native output
sampling and existing final-capture retry remain; no event-driven Console
output detection or measured RDP latency improvement is claimed.

Retained unsuccessful setup evidence: first graph invocation lacked the
explicit Node path; build3 rejected a fixture link rule referenced before its
definition (fixed to the existing earlier broker_test_link). No failed graph
was published. Candidate copy attempts aborted before tests on a shell syntax
error and absent cache WOW32.DLL; WOW32 uses the verified S7 runtime provider.
Final package/gates explicitly verify all eight files. The side-session
proposal hash remains AE9DB32FD31A87EFC07FF895DF66CB3FFDFD17ECBED6B1FBE4391152A2ADF9AB,
untouched and excluded from delivery.
