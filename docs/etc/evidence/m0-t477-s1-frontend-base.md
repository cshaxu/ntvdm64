# T477 S1 frontend-base implementation evidence

## Scope and baseline

Question: can NTCON use a shared, per-instance frontend channel and service
without changing its native presentation, wire contract or NTSRV lifecycle?
The [design](../operations/m0-t477-s1-frontend-base-design.md) defines P1/P2.
Baseline is 869f001e6 plus admission/design commits through bc254f439.
Owner requires publication to this worktree assets/release and O:/winnt/system32,
local commits and no push. Main worktree source and its active task are separate.

## P1 channel extraction — delivered locally

The channel framing, authenticated attachment sequence, readiness notification,
serving thread and cancel/drain/join moved from NTCON console_channel.c to
common/frontend-base/channel.c. NTCON retains operation dispatch, native
Console handles, video/snapshot/publication state, input receipts and title
state. Common uses explicit adapter callbacks and no executable-private types.
The original protocol validation remains in the NTCON dispatcher; generation
and sequence do not acquire a second authority.

The NTCON channel fixture now uses base thread/readiness accessors. New tests
link the production base archive without NTCON, using real pipes and threads
to cover fragmented packets, two simultaneous instances, cancellation,
bind/attachment failure, malformed frames, worker death and reply ordering.
Local attachment callbacks are not evidence of authenticated service operation.

## Build and test setup observations

- Initial restricted compiler ABI probing stalled before product compilation.
  The owned configure process tree was identified and stopped. The normal
  authorized background compiler environment completed both ABI probes.
- The first generated graph exposed a test edge preceding console_test_link's
  declaration. Its ordering was corrected; no compiler or acceptance failure
  was reclassified as passing.
- The retained observer generator lacked the src include root after the NXVM
  library relocation. Adding that include root lets the actual observer build;
  no observation assertion was weakened.
- The existing frontend-scope fixture used GetParent without user32.lib in
  its x86 link. A dedicated scope-test rule adds user32, and the native graph
  maps that rule to its existing test linker. The x86 fixture and native product graph build.
- Current committed assets/winnt.zip has SHA-256
  92714715FC0FCE4CB33268F534E9E7D7D45AE612937249A2C2B2E2E013099947
  (archive commit b887c38b0). Its container hash differs from the older guest
  manifest header, but every listed guest file matches that manifest exactly.
  Baseline/candidate staging uses verified unchanged guest bytes. This task
  does not update guest media or silently rewrite the retained manifest.
- Baseline WOW observations use this worktree's released images and its own
  rebuilt observers, with isolated private desktops. They are startup/window
  frontier comparisons, not gameplay/editing acceptance.

The initial baseline observer inherited the full developer environment and
produced an illegal-instruction dialog instead of WINMINE's required window;
the Full gate correctly rejected it. Repeating the unchanged released package
with the candidate's controlled environment (minimal Windows variables, short
physical runtime root, Windows-only lookup and task-owned temporary paths)
restored WINMINE, SOL and WRITE application windows. Retained baseline is
build/t477-s1/baseline-wow-clean; baseline-wow is the rejected initial run.
This was a harness environment correction, with no guest or product patch.

## P1 observed verification

- Full ten-component build and second incremental stability check passed:
  build/t477-s1/p1-build-r3/result.json (10 images, 1180 intermediates).
  The final fixture-link correction was followed by successful x86 WOW/worker
  and x64 component incremental builds in p1-fixtures-and-build.stdout.log.
- test/common/frontend-base/verify-frontend-base.ps1 passed against both
  build/.ninja/msvc-x86-release/worker and
  build/.ninja/msvc-x64-release/frontend, Cases channel. Each image exited 0
  with the isolation/framing/cancellation/rollback/reply-ordering witness.
- Existing console-frontend-test, console-video-test,
  console-channel-lifetime-test, console-frame-failure-test and
  frontend-text-handoff-test passed on x64. frontend-scope-lifetime-test
  passed on x86. The two channel/frame fixtures used --private-full and an
  assertion report; text handoff reported zero failures. All were launched
  through console-startup-observer with MVDM_OBSERVER_PRIVATE_DESKTOP=1,
  observed exit 0, and retained assertion witnesses under
  build/t477-s1/p1-verification.
- Final APP_VERSION 0.0.477 package passed all of the above fixtures again,
  followed by all 16 Full gates; timings total 429693 ms.

## Task identity correction

The first environment-matched Full run passed WOW frontiers, both 17-case
Console/Window command matrices, RPC fixtures and native GUI routing, then
correctly failed the admitted-task version assertion: version.h still named
0.0.476. P1 advances APP_VERSION to 0.0.477 as required by T477 admission;
APP_PROTOCOL_VERSION remains 65, with no wire change. All affected product
components and fixtures are rebuilt, restaged and reverified before delivery.
The rejected candidate and logs remain under *.pre-version task paths.

## P1 delivery

Full runner: tools/audit/Invoke-ProductVerification.ps1 -Suite Full,
InputPolicy Observed, private desktops, own x64 observers and Terminal observer,
G7.COM from the retained command fixture, baseline-wow-clean. Candidate runtime
build/t477-s1/p1-package; control-cache holds the identical stamped images.
The final build/fixture/Full driver is build/t477-s1/retry-p1.ps1 and
verify-part.ps1; raw logs are p1-verification.stdout.log and
p1-verification/full/timings.json. All 16 gates and runner exit 0 passed:
guest media, 274 export ordinals, WOW frontiers, Console-17, Window-17, RPC,
native GUI, five version negatives, strict DIR, modern EDIT return, cooked
return, rapid and interactive relaunch, session isolation, retirement and
nested-window handoff. DOS4GW/NESticle remains owner-excluded; WOW frontiers
are startup comparison, not gameplay acceptance. Allocation/thread-creation
resource-exhaustion branches were source-reviewed, not fault-injected.

NTCON's link map resolves frontend_base_channel_start/stop from
common-frontend-base.lib. Common includes only its own header, common protocol,
lib-win32 pipe and platform headers; worker-base has no frontend-base edge.
Documentation governance and diff whitespace checks passed before delivery.

Package 79d9c8b8-c694-4f7c-91d1-031c12e8c2fe was copied and hash-verified at
this worktree assets/release and O:/winnt/system32. Only ten images plus the
existing hook license were copied; installed guest/configuration stayed intact.
Exact prior-file backups and rollback verification are retained in
build/t477-s1/p1-publication-backup; p1-publication.json records both destinations.
No installed project processes needed termination. Publication exited 0.

| Image | Published SHA-256 |
| --- | --- |
| run16.exe | 9B70E8030FD9F5615709B97AD86741A4182304C67EA2EBF1DFD8178424EDBE20 |
| ntsrv.exe | F424261F6076B66C26E0C861817873D17AA00AB1F762E3F6A15657663AB95C5F |
| ntcon.exe | 12F9FF2A318FDD841CDC7FDB9187E42ECE6E48D74999395A4C2829E55816849B |
| ntvdm.exe | DA9C102FEF38295FF6D5673DC9577F9351C567AB9B095D249000A9454D1BC90E |
| ntvwm.exe | 71E78F6B2FD725DAE7750581908F24CA6D58751B2D701146576681842D3BAE9D |
| ntmon.exe | 5CA8022F4C889329E805D34C4B8DE44C87363BD98A7E5B4604D3BF9940DC396A |
| WOW32.DLL | CCD4974EAE3EA55B857233D696AEB0BD4DD4C64A12CF86E4193ACB0733017BCE |
| VDMREDIR.DLL | 962632DD2B2EBA9FCDE0DB3DD397A3799E1133D13CD55C413A193DDC43F92A7B |
| nthook32.dll | 47A77826F5E1C1828343837C1085C788440B33B96BEE25F2B86DBA264B6B1B0D |
| nthook64.dll | 67E9600CC43B5B84EA111584DDF79DAAB03AC40FFF8760C39E9159F309C708C2 |

This evidence is committed with M0 T477 S1 P1. No push is authorized.
P2 service-pump extraction remains within the active S1 and is not yet delivered.
