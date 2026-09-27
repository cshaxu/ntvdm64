# T423 S5 Hidden Backend Acceptance

## Question and baseline

Prove the owner-approved expanded hidden Console contract on the independent
frontend architecture, not merely that the transport links. S4 P1
9c27b5fd2d21bdf71a730c8d8fcfae10c613bf29 is pushed and its verified seven-file
package is at O:/winnt. The [S3/S4 ledger](m0-t423-s3-hidden-console-ledger.md)
pins binaries, configuration, runtime evidence and retained uncertainty.
Review existing owners and tests before authoring more implementation.

## Checklist

| Contract | Existing test to reuse | S5 evidence state |
| --- | --- | --- |
| Native output, cooked input, reuse, streams, EOF, Unicode environment, ordered input return | tests/app/native_console_host_test.c default | r3 component pass: real CMD 37/23, stream child 29, exact 604-record return before newer input |
| Raw/processed Ctrl+C, Ctrl+Alt+C, Ctrl+Break | native_console_host_test.c --control-input | r3 real hidden target results 42/43/44 pass |
| Visible frontend controls to hidden target, default exit, suspended DOS I/O | native_console_frontend_test.c --controls | r3 private-Console component pass; suspended DOS binding is a fixture, not a live guest |
| Completed result versus lost presentation | native_console_host_test.c --completion; native_console_frontend_test.c default | r3 preserves completed 37/0/259; live target survives error and later returns 41 |
| Cancel/unresponsive helper and bounded close | native_console_host_test.c --close-timeout; native_console_frontend_test.c default | r3 pass with deliberately stalled peer; bounded cancellation/EOF, not real guest acceptance |
| Unicode, palette/cursor, scrolling, viewport and resize | native_console_capture_test.c; native_console_host_test.c default | r3 real Console pass: 300 rows, 5001 columns, tiled data, resize retry and no duplicate notification |
| Actual users outlive direct parent, helper failure is not empty | native_console_members_test.c | r3 real helper membership 0/2/1/0 pass, failure leaves count untouched |
| Four-level DOS/native chains and I/O restoration | tools/audit/Verify-CommandExitStatus.ps1 frontend-chain-a/frontend-chain-b with OrdinaryFrontend | Published O:/winnt pass, final results 1/23, actual output/return markers and one independent frontend per chain |
| Mixed launcher/helper/frontend/worker faults and unrelated-session survival | verify-frontend-lifetime.ps1 -ExpandedFaults, frontend_loss_noio_test.c | Final five-case pass including actual native/DOS work and independent session; worker retirement asserted before cleanup |
| Regression and release | S4 DOS17/WOW/ownership baseline; S5 native seventeen, four-level pair, lifetime five; seven-file hashes | No production change: exact published and candidate seven hashes remain S4; tests rebuilt and run, governance/diff gates required before P2 |

## Boundaries and procedure

Native component fixtures use real Windows Console/helper resources, but do
not alone prove guest or broker integration. DOS tests select x86 CCPU40 and
unchanged original media. Intermediates stay under build/M0-T423/S5; runtime
reports stay under O:/winnt/logs. Use fresh identifiers, actual output
assertions and bounded observation. Never infer completion from helper death
or call forced cleanup natural retirement. Preserve failed evidence.

The historical unattributed startup timeout retains its S4 disposition/TODO.
Recurrence requires live worker/window capture before cleanup and exact
artifact/configuration identity, not a retry or fabricated success. Window,
display and guest modification are not admitted by S5.

## Results

S4 delivery and S5 admission are pushed as 9c27b5fd2 and d314d8173.
The existing transport runner gained ExpandedBackend, adding the host fixture's
default/control/completion/close modes, frontend control mode and actual
membership test. It retains the earlier eleven cases; component evidence is
not relabelled as guest acceptance.

Two fixture defects were found before completing that run:

- m0-t423-s5-expanded-contracts failed in native-console-host-test streams:
  its requested child cwd was cache/tests, which did not exist. The runner
  now creates that empty build-only prerequisite. No product change.
- m0-t423-s5-expanded-contracts-r2 passed the full default host sequence and
  control-input, then failed --completion. The fixture passed a zeroed view
  without opening CONIN$/CONOUT$, so presentation failed with invalid handle
  before reaching the intended cancelled transport. The observed target
  result 37 was already preserved correctly. Initialize/end the real view
  around each case so ERROR_OPERATION_ABORTED is tested at the intended edge;
  do not loosen the assertion or alter production error order.

Both failures and console transcripts remain under O:/winnt/logs. The r3 run
uses the repaired checked-in test, rebuilt against unchanged production objects.
All seventeen cases in r3 passed. Reproducer:

```powershell
tests/component-integration/verify-frontend-transport-contracts.ps1 `
  -BuildRoot build/M0-T423/S1/restart-formal-x86 `
  -Observer build/M0-T423/S4/observer-modal.exe `
  -LogPrefix m0-t423-s5-expanded-contracts-r3 -ExpandedBackend
```

Each case has its own observer report and Console text under O:/winnt/logs;
expected exit and the named capability witness are both checked. Only two
fixture links and the changed host-test compilation were required. Production
binaries/guest/configuration remain at the S4 published baseline. This does
not complete S5: expanded mixed faults remain a separate gate.

The ordinary published-package A/B four-level integration also passed with
PackageRoot and ProcessPackageRoot O:/winnt, the same observer, Cases
frontend-chain-a,frontend-chain-b, OrdinaryFrontend, original G7.COM fixture
from build/M0-T423/S4 and prefix m0-t423-s5-ordinary-four-level. The summary
JSON records final results 1/23; real Console transcripts contain native
inner/parent-return markers and both guest MEM outputs. Each run observed
exactly one independent frontend (3840 and 23908 respectively), not launcher
presentation. The verifier also awaited its retirement. Unchanged guest
COMMAND determines its own return value; no new forced exit-code policy.

Next: reconcile and extend the mixed fault matrix against the existing S4
normal/launcher/frontend/worker cases. Specifically prove helper I/O failure
with live mixed execution and unrelated character-session survival; local
stalled-peer tests or the two normal twelve-target chains do not replace
those integration assertions. Do not begin Window work or close S5 early.

## Expanded mixed-fault investigation

Extend the existing frontend_loss_noio_test.c rather than invent a second
product mechanism. ExpandedFaults adds an independent native character session
to each normal/frontend/launcher/worker case and a fifth helper-loss case.
The second session has a distinct frontend and a real gated native target;
it must remain alive across the tested failure, return actual result 53, and
retire naturally. Helper loss occurs while a real native parent and its nested
DOS NOIO.COM task are live. The native target must survive and return 37;
guest file work must reach NIODONE. Helpers are not counted as target programs.

The first four cases passed in mixed-fault-r1. The helper assertion initially
expected DOS result 7 unconditionally and failed. Further observations r2-r4
showed NIODONE existed, native result was 37, and the guest/worker/launcher
remained alive. Crucially, worker-owned visible dialog #32770 reported the
system ERROR_NO_DATA text, "The pipe is being closed", with Terminate/Ignore.
This was not a silent wait or evidence that helper death had killed the DOS
task. It was original error interaction, which the initial test had not handled.

Source contract: mvdm/softpc.new/host/src/nt_error.c::DisplayErrorTerm formats
the OS error and calls host_error; host_error opens ErrorDialogBox with
RMB_ABORT|RMB_IGNORE. Its WM_COMMAND IDB_QUIT (nt_uis.h value 100) returns
RMB_ABORT; ErrorDialogBox then invokes original TerminateVDM for DOS. No new
automatic execution termination, retry, error suppression or success policy
is introduced. The test asserts the exact FormatMessage(ERROR_NO_DATA) text,
visible original button and still-live worker/launcher, then posts that
original Terminate action only to the exact test worker on its unswitched
private desktop. The resulting unfinished DOS task returns 1067, frontend
retires, and the independent session still returns 53. r5 proves this helper
negative contract. It is not a successful DOS exit-7 case.

All failed transcripts and diagnostic process/window snapshots remain under
O:/winnt/logs/m0-t423-s5-mixed-fault-r1 through r4. The final all-five run is
separately recorded below after completion. Production bytes are unchanged;
only the test fixture is compiled x86 /MT with user32.lib, under build/M0-T423/S5.

## P2 bounded conclusion

The final strengthened all-five run passed with an explicit worker-exit check
before any test cleanup, not just launcher/frontend retirement. Reproducer:
map the fresh build/M0-T423/S5/p candidate to R: temporarily, then run
tests/observation/verify-frontend-lifetime.ps1 with Observer
build/M0-T423/S4/observer-modal.exe, PackageRoot R:/, ProcessPackageRoot the
physical candidate, EvidenceRoot build/M0-T423/S5/mixed-fault-retirement,
LogPrefix m0-t423-s5-mixed-fault-retirement and ExpandedFaults. Remove the
temporary mapping afterward. Reports are O:/winnt/logs/<prefix>-{normal,
frontend,launcher,worker,helper}.txt and their actual Console transcripts.
The earlier mixed-fault-final run also passed all five; the retirement run is
the stronger final evidence. Test source SHA256 is
C5C47EEB1A3F0BB6CEBBF6232ED708C5DCE1700B4BD52D6483FA5BA46FF0FB4C;
NOIOLIFE.EXE SHA256 is
A6079D2B94B9BA6A88D07495180DEBC3890BD52E2FB675BA5B09A23B84300FF4.

All S5 checklist rows have the applicable inspected evidence. Component mocks
remain labelled; physical focus/clipping are owner-waived. Original fault
interaction is preserved rather than suppressed. No src/, original mirror,
guest, configuration or runtime binary changed in S5; O:/winnt retains the
verified S4 seven-file package, rechecked against the pinned manifest. The
historical startup-timeout debt remains explicitly unattributed, not repaired.
S5 can conclude after governance/commit/push; S6 owns the independent frontend's
display/window implementation, not an unresolved S5 product implementation.
