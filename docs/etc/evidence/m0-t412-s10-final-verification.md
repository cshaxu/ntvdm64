# T412 S10 final broker verification and accounting

## Inputs and scope

Owner subsequently directs a bounded S10 handoff and admits S11 for the
EDIT-to-MEM Console regression. The [handoff](m0-t412-s11-console-handoff.md)
preserves later adverse evidence and exact unchanged P3 products. This is not
full T412 acceptance; the earlier Console matrix did not prove narrow-Terminal
rendering correctness. No S10 product patch remains uncommitted.

Owner admits final closure, build/test/commit/push, then wait. S9 predecessor
is df2467de0; whole-T input is 08b33351b1c341bb433f27ab517ee9a4e0bc3e0c.
Owner confirms current DOSX/HIMEM/MSCDEX startup loading, not complete device
or WRITE acceptance. Final build root is build/M0-T412/S10/final, MSVC x86
/MT, original CCPU40. Runtime package is `O:\winnt` and all observations are
under `O:\winnt\logs`. At P1/P2 APP_VERSION was 0.0.412 and protocol was 2.
The owner-authorized P3 below advances the protocol to 3 without changing the
application version. Historical P1/P2 inputs and findings are retained.

## Audit repairs

### Follow-up admission: process loss (P3)

After P2 the owner authorizes fail-stop containment: startup may start a
missing broker; a running task never reconnects/replays. Broker death must
terminate connected workers and launchers. Protocol 3 adds a wait-only typed
broker-process capability; APP_VERSION stays 0.0.412. Fresh build root:
build/M0-T412/S10/process-loss, MSVC x86 /MT CCPU40. P3 verification is
recorded below; preceding protocol-2 observations remain historical inputs.

Recovery ladder: original srvvdm.c BaseSrvCleanupVDMResources,
BaseSrvExitVDMWorker and UPDATE_VDM_UNDO_CREATION remain composed and own
record cleanup. Original BaseClientDisconnectRoutine's CSR process notification
is unavailable in standalone RPC; existing process-watch/rundown mechanics
are reused. No mirror/overlay change is needed. Newly authored broker-death
containment is the explicit owner product exception: an authenticated wait-only
process attachment plus local wait/abort, not an NT4 kernel/CSR substitute.
The earlier rungs do not supply independent basesrv.exe crash containment:
NT4's service is hosted by the excluded system CSR shell. This seam remains
transport lifetime only and must not acquire guest state or command policy.
Launcher death preserves an already claimed VDM, matching original ownership;
unclaimed launch resources use original UndoCreation and finite reservation
release. Negative verification must include all three process deaths and
unrelated-worker isolation, alongside normal COMMAND/EDIT and version tests.

The startup Job is an additional explicit product mechanic, not an OpenNT
algorithm: PROC_THREAD_ATTRIBUTE_JOB_LIST installs it atomically during
CreateProcess. Only the launcher owns its non-inherited kill-on-close handle.
After authenticated Prepare the limit is disabled and the handle closed;
normal resident/nested worker lifetime is unaffected. Before then launcher
death cannot leave an unknown suspended worker. Runtime fail-stop is armed
only after Resume in run16 (immediately after Connect in ntvdm), so startup
RPC failure retains the opportunity to roll back its child and original record.

Two failures found during this follow-up were fixed rather than counted as
passing: worker death originally woke its parent but original missing-record
exit lookup returned zero; the service now marks an unsignalled original
notification event as failed before original cleanup, and returns 1067.
The first abandoned-launch test also exposed an omitted request-thread binding
at UndoCreation; it now uses the same thread/registry/resource binding as Update.
Neither original mirror nor private overlay is modified by P3.

### P3 identified products and evidence

Final products are copied from build/M0-T412/S10/process-loss to `O:\winnt`.
The build-only startup-barrier executables are never published there.

| Product | Final SHA-256 |
| --- | --- |
| run16.exe | 20709EC0F1DB68D465EB994C3D14E134618A2CB289A64B2B4080FCA6E80F6C49 |
| basesrv.exe | 2152439F487D305E77A1AFD5DCC7D7D4F0079AD48E3BBDF398749CC4AD30D8B2 |
| ntvdm.exe | 3344EA956EF4EA19E53966190C922A78A61263D408B4190EF53A57304A1EAE50 |

Observed P3 rows (all reports under `O:\winnt\logs`):

- m0-t412-s10-p3-product-summary.json: all 17 original Console cases pass,
  including nested COMMAND/MEM, EDIT return, streams/EOF and exact exit codes.
- m0-t412-s10-p3-broker-results.txt: two admitted COMMAND workers and launchers
  exit on broker death; the second launcher explicitly returns 1722; new MEM
  starts a fresh broker and returns zero. No task replay.
- m0-t412-s10-p3-worker-results.txt: killing one worker fails its waiting
  launcher; the unrelated worker completes normally and broker survives.
- m0-t412-s10-p3-launcher-results.txt: claimed worker survives launcher death
  until normal guest exit/test cleanup; the other command completes normally.
- m0-t412-s10-startup-loss-c-results.txt: build-only barriers before and after
  Prepare prove launcher death leaves no suspended child; broker death before
  Prepare also fails and reclaims the child. Trace records successful original
  launcher-abandon cleanup. This is controlled cut-point evidence, distinct
  from unmodified-product runtime rows.
- m0-t412-s10-p3-reservation.txt and m0-t412-s10-p3-service-reservation.txt:
  claimed-worker preservation, unclaimed rollback and original DOS/WOW service
  lifecycle pass. A reused worker killed with process code zero still returns
  ERROR_PROCESS_ABORTED to its unfinished task, not false success.
- m0-t412-s10-p3-owner-lifecycle.json, p3-classifier.json and p3-membership.json:
  original source identity, service waits/resources, image classification and
  same/different Console boundary pass on final formal libraries.
- m0-t412-s10-p3-service/: actual RPC, typed resources, rundown, forged/stale
  identity refusal and wait-only BrokerProcess attachment pass; attempted
  termination through that wait-only handle is denied.
- m0-t412-s10-p3-version/: all five negative peers against both run16 and
  ntvdm return 1306 before task admission; no retry on version mismatch.
- m0-t412-s10-p3-lifetime-results.txt: concurrent startup/singleton, worker
  cleanup, empty grace cancellation, empty broker exit/restart and real shared
  WOW PIF acquisition pass. WRITE still times out after acquisition and is not
  claimed as application acceptance; its original provider recovery stays queued.
- m0-t412-s10-p3-drain-results.txt: controlled empty callback wins its race;
  startup connects to a new broker and MEM returns zero. This startup retry
  is not runtime task replay. Final product hashes remain unchanged.

The final product matrix and focused fault tests pass. Documentation and
relative-link gates pass. P3 is the implementation delivery for owner testing;
no next T is admitted, and owner real-package confirmation is still pending.

P3 source delta from 99e86538e (excluding README/tests/tools/docs): +275/-20,
net +255 lines. Mirror and overlay delta is zero. These are registered lifetime
bindings and explicit failure policy, not falsely claimed recovered original
code or a footprint reduction. The updated whole-T report
m0-t412-s10-p3-accounting.json records +8533/-2071, net +6462 across 98 source
paths; the paired mirror-distance results and 29-line private overlay are
unchanged from P1. Imported original source is not counted as autonomous code.

### P1 repairs (retained history)

1. The old original-lifecycle driver did not link the now-selected original
   RTL environment owner. Its Check/Get test buffers predated added stream
   fields, and its media paths still named the retired runtime directory.
   Use the final formal archives, current structure capacities and `O:\winnt`;
   retain short-buffer, narrowing, malformed payload and cleanup assertions.
2. This exposed uninitialized reserved receipt fields in the generic Check
   scalar encoder. Zero-initialize its value before filling numeric fields.
   The production Check command encoder already initializes its own complete
   prefix; it does not call this generic Check branch. This is a real dormant
   contract defect, not evidence of the earlier startup failure. Get/Update
   and original BaseSrv policy are unchanged.
3. A controlled empty-timer barrier reproduced launcher exit 1306: UNKNOWN_IF
   during endpoint drain was wrongly labelled incompatible by S9. The finite
   RPC client now queries interface identities on the same authenticated
   binding. A confirmed different major/minor remains immediate rejection;
   an absent/drained interface returns unavailable for the existing bounded
   startup retry. No command is accepted before the normal two-sided identity
   handshake. Actual old-interface and corrupt-response negatives still pass.
4. The S5 BaseSrvDOSWorkerWaitPending extension was not original OpenNT and
   exceeded the mirror-body rule. Its body is relocated unchanged into
   opennt-host-overlay/base/win32/server/dos_worker_wait.inc, included only
   inside its mirror function and library. OPENNT-HOST-031 records the retained
   boundary. No caller invokes an overlay API and no new service policy is
   introduced. This is relocation, not algorithm deletion or original recovery.

The source-first ladder is unchanged: original BaseClient/BaseSrv bodies own
policy; the unavailable NT4 CSR/process transport is bound by authenticated
public local RPC/resources; the retained wait observation stays private to the
record owner. Neither a replacement CSR subsystem nor a second queue is added.
The existing wait query is retained because VDM_READY alone does not prove
that interactive COMMAND is awaiting GetNextVDMCommand. The fixture verifies
both no-wait refusal and actual-wait command delivery. Its eventual removal
requires a source-proven delivery contract, not a pool or worker timer.

## P1 whole-T physical source accounting

Run tools/audit/Measure-BrokerClosure.mjs. Output is
`O:\winnt\logs\m0-t412-s10-accounting.json`. Counts are physical text lines,
including comments/declarations but excluding Markdown, tests, build scripts
and generated objects from this table. Changes outside the broker slice,
including the already-authorized physical-mapping and CPU prerequisite edits
within this revision interval, remain visible rather than silently subtracted.

| Source group | Changed files | Added | Deleted | Net |
| --- | ---: | ---: | ---: | ---: |
| adapter-mvdm-host-out | 19 | 354 | 1228 | -874 |
| adapter-opennt-host | 36 | 3453 | 0 | +3453 |
| app | 11 | 1130 | 774 | +356 |
| broker | 16 | 687 | 0 | +687 |
| mvdm-host | 8 | 73 | 67 | +6 |
| opennt-host | 7 | 2552 | 2 | +2550 |
| opennt-host-overlay | 1 | 29 | 0 | +29 |
| Total | 98 | 8278 | 2071 | +6207 |

The T is an original-owner/cross-process restoration, **not a net source-size
reduction**. Added mirror source includes restored original bodies; adapter
counts include headers and finite transports, not 3453 lines of new guest
algorithms. Conversely, moving code into a mirror does not make it original.
The earlier [S6 deletion ledger](m0-t412-s6-removal-ledger.md) is a different,
overlapping denominator: retired implementation plus obsolete tests/build
wiring and README, 3282 deleted / 57 added, net -3225. Do not add the totals.
The five removed BaseVDM implementation/header files total 1082 deleted lines;
old app entry/declaration/shell files total 774 deleted lines in this interval.

For the eight changed existing MVDM mirror files, distance from the pinned
OpenNT files is **749 added / 139 omitted -> 752 added / 136 omitted**:
888 changed lines at both endpoints, not a net diff reduction. cmddisp removes
18 added diagnostic lines; cmdexec removes 16 net added lines; nt_reset reduces
distance by 19. Entry/diagnostic/prerequisite corrections offset these savings.
For the five existing changed non-MVDM mirror files, distance is
260 added / 9579 omitted -> 624 added / 7709 omitted, down 1506 lines overall.
Most of that gain is restoring previously cropped BaseClient source, not
deleting autonomous algorithms. Newly imported csrutil/exports subsets are
excluded from this paired denominator but included in the source-growth table.
MVDM overlays are unchanged; the new 29-line OpenNT overlay is the relocated
wait query. No claim of zero autonomous implementation is made.

Retain the original owner algorithms, bounded CSR marshalling, typed resource
receipts, authenticated process/Console identity, launcher composition and
worker-local pre-init. The retired local BaseVDM queue, its client retry policy,
second worker entry and old app shell are absent from the final graph. Retained
trace hooks provide evidence only and are not counted as functional recovery.

## P1/P2 verification ledger

The following final same-source checks pass:

- Fresh 482-step formal three-program build and VdmTib 4208-byte no-overlap
  gate, followed by incremental relinks for the recorded fixes.
- Verify-BasesrvProduct: actual RPC registration, malformed/app/protocol
  identity, resource receipts, generation/role negatives, child reservation,
  rundown and reconnect. Logs: m0-t412-s10-service-final.
- Verify-ProductVersions: five negative peers against both run16 and ntvdm,
  including legacy RPC major and corrupt successful replies, all reject 1306
  without task admission. Logs: m0-t412-s10-version-final.
- Verify-BrokerOriginalLifecycle: actual final owner archives; command/PIF/
  directory capacities, original retry ordering, streams, allocation failures,
  WOW scope/generation, cleanup, registered-process lifetime and thread binding.
  Logs: m0-t412-s10-lifecycle-final.json.
- Verify-BrokerConsoleMembership and both formal reservation executables:
  same/different Console, wrong identity/generation, DOS/WOW completion and
  actual worker-exit cleanup. Logs: m0-t412-s10-membership-final.json and
  m0-t412-s10-final-basesrv-*-test.txt.
- Verify-BrokerOriginalClassifier: original bodies and final archive ownership,
  real MEM/COMMAND/EDIT/WRITE classification, PE, DLL/suffix/missing-file cases.
  Log: m0-t412-s10-classifier.json. Immutable COMMAND image/source proof passes.
- Verify-CommandExitStatus: 17 real Console cases, including three nested MEM
  depths, EDIT return, original exit conventions, stdout/stderr emitted bytes,
  native EOF/result 37, direct DOS/native result 7. All pass in
  m0-t412-s10-acceptance-final-summary.json.
- Verify-BrokerFinalLifecycle: concurrent launch/singleton, abrupt worker
  cleanup, grace-period arrival, actual 60-second empty stop, restart/first-VDM,
  and real WOW acquisition pass (m0-t412-s10-lifetime-final-results.txt).
- Build-BrokerCallbackBarrier plus Verify-BrokerFinalLifecycle -BarrierBroker:
  the real timer callback is held with test-only events in a build-only broker;
  the old callback wins, launcher retries a fresh broker, MEM returns zero.
  Logs: m0-t412-s10-barrier-final-results.txt. No test binary replaces the broker.

All logs above are below `O:\winnt\logs`. Source recovery maps D01 command
selection, D02 PIF, D03 first-VDM, D04 directories, D05 completion, D06 reentry
and D07 client capture/retry to original srvvdm.c/vdm.c bodies. D10 classifier
remains original GetBinaryTypeW/BaseIsDosApplication. Modern resources and
transport are the retained finite boundaries, not alternate policy owners.

### P1/P2 blocking finding: loss of an admitted broker

The planned S5 abnormal-broker-loss requirement was not discharged by the
older S5 closure or by the positive rows above. S10 now includes its failing
regression rather than dropping it. An observer-only named-event gate holds
the first guest keystroke; the controller observes successful DOS command
delivery (state 0200), stops only its owned broker, then releases ordinary
`EXIT` keyboard input. The Console displays EXIT, but run16 times out at
20 seconds. Logs: m0-t412-s10-loss-gated.trace and
m0-t412-s10-loss-gated-broker-loss.txt. This proves a missing bounded outcome,
not that guest termination itself completed or that CCPU is faulty.

The earlier ungated immediate-EXIT trial returned zero because normal task
completion beat the broker kill; it is not broker-loss acceptance. The earlier
VER/EXIT trial also timed out but mixes in a new shell-out request, so the gated
EXIT-only case is the selected failure evidence. No arbitrary watchdog,
worker reaper, task replay or original guest change is introduced to hide it.

**S10 and T412 remained open at P2.** That delivery completed the verified repairs and
accounting, not T closure. Follow-up must define and prove loss handling for
already-admitted parent/worker waits, without replaying an indeterminate task
or modifying original COMMAND/CCPU policy. The next queued T is not admitted.

### P1/P2 published formal products (superseded by P3)

| Product | SHA-256 |
| --- | --- |
| run16.exe | A4327C2B14D9606C451F4E2A64093220C4B7E05BA72ACD8304BD1172E92FA989 |
| basesrv.exe | 4479626FC6DCE8252988DA7489A44E200DA8723F810FABB3C2848DC1446825B8 |
| ntvdm.exe | 76233D1FC9F3012AF8EF6F86A93BBF1052B54E209615F7A7C27EE565FA9B6D99 |

Artifact correction: rebuilding the observer through the all-target wrapper
also relinked ntvdm.exe without recompiling its product objects. The earlier
same-source version/lifecycle runs used worker SHA-256
329FF716EDD942F9B8CB9C84F7B4116F418027AB1363D9F666C18EB40A96534A.
The final table names the actually published last link; its exact 17-case
Console repetition is m0-t412-s10-published-summary.json. These identities are
not interchangeable: earlier lifecycle evidence is same-source, not the same
EXE hash. The outstanding full closure still requires the final artifact
matrix after the broker-loss repair. Run16 and basesrv hashes did not change.

The current launcher also rejects the old formal S8 worker with 1306 and
releases its reservation (m0-t412-s10-old-worker-summary.json). The final
worker above was restored in a finally block. No guest media changed and all
test-owned processes and temporary drive mappings are cleaned after testing.

The initial negative-peer/normal-test overlap correctly returned a version
error and is not a product regression result. Tests now run serially against
the shared endpoint. The observer's intentionally limited scan-code table
could not transmit quotes or drive-path punctuation. Native stream/EOF checks
now use fixed build-only CMD fixtures passed through original COMMAND /c,
not synthetic punctuation keystrokes. The temporary observer scan-code
extension was withdrawn after its modifier limitation was observed; product
keyboard code is unchanged. Both native stream markers and EOF/result 37
then pass. No failed or timed-out attempt is counted as a pass.

Full WRITE is not a broker exit gate: the recorded real worker reaches
successful original shared-WOW PIF acquisition (state 0102), then the workload
times out at the 20-second observation bound. Original WOW completion/cleanup
is tested separately with the source-owned reservation fixture. This is not
proof of successful W32Init, WOWEXEC, USER or WRITE execution. Existing queued
WOW/debugger and WOW16 proposals retain those scopes; the broker does not
substitute a success result. Guest COMMAND redirection and MEM's anomalous
largest-block field retain their existing external-owner limitations.
