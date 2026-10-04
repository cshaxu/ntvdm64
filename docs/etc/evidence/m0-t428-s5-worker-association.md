# T428 S5 worker association consolidation

## Boundary and implementation

Baseline e33f90807; previous published S4 886f13758. APP0.0.427/RPC38/I/O25
is unchanged. The initial implementation increment is e81e9fc9f. The final
paired cancellation increment and owner's GUI policy clarification are
recorded below; both coherent packages passed their own publication gates.

| Mechanism / provenance | Production owner and change | Retained independent boundary |
| --- | --- | --- |
| Project root-worker association | Existing NTSRV WORKER_WATCH is the persistent authority. Removed connection native_root and watch frontend_associated; registry binding, lifecycle, selection, parent resume and management read the same root generation. | Original DOS/WOW record execution and cleanup stay in their mirrors. |
| Authenticated initial association | Connect consumes the exact prepared route grant before Console-context admission. Canceled grants cannot fall through to a replacement root. | Console membership proves admission only; it is not a task registry. |
| Native control registration | native_frontend_registered records the pre-text/subsequent registration phase, not a second root identity. | Existing worker-owned stop/closed events and duplicate-registration checks remain. |
| Shared GUI carrier selection | Unbound GUI carriers may reuse across caller Console identities; text-bound carriers remain limited to their authenticated live root. | Default shared WOW/GUI carrier behavior is not a new exclusive GUI launch option. |
| Management projection | One outer kind initialization and explicit original/native record reader. | Native GUI and original DOS/WOW task records have different source owners; no scheduler or observed records are introduced. |
| True native Console close | Existing local callback recognizes disappearance of its captured HWND identity after last-member FreeConsole as successful closure. | Worker-base acknowledgment still requires real local success; errors and live-client closure are not waived. |

The last-member close probe initially failed with ERROR_INVALID_HANDLE because
the window disappeared between IsWindow and its owner-identity check. The
correction also covers disappearance during PostMessage. It does not interpret
an arbitrary live window or a timeout as successful close, kill GUI descendants,
or change native presentation polling. Original mirrors are unchanged.

## Verification and retained failures

All run paths below are under build/M0-T428/S5. Incremental x86 /MT CCPU40
uses the validated build/M0-T427/S2/r001 MSVC14.43/SDK22621 graph.

- r001 build.cmd: affected EXEs/service/shutdown fixtures linked. Subsequent
  `ninja -C build/M0-T427/S2/r001 ntvwm-close-test.exe VDMREDIR.dll` rebuilt
  the native close probe and affected DLL. The DLL hash changed after relink;
  final runtime validation therefore uses a new coherent r015 package, not
  r008's previously tested DLL.
- r010 `verify-service-fixtures.ps1`: 25 production archive cases passed,
  4087ms, including paired DOS/native existing-watch root binding/loss,
  authentication, selection, parent restoration and I/O barriers.
- r009 `Invoke-ProductVerification.ps1 -Suite Product`: earlier r008 candidate
  Console17/Window17/retained WOW passed (223780ms). These results alone do
  not certify the changed r015 package.
- r011 management.ps1: earlier r008 paired frontend loss/receipt1067, selected
  native close and independent session exit23, unexpected worker failure and
  explicit frontend close passed.
- r012 GUI-only management failed with timeout1460; retained as a failed
  attempt, not a passing retry. A supplemental actual empty Console probe
  then demonstrated the last-member close race. This proves an adjacent
  concrete bug, not every possible cause of the earlier timeout.
- `ntvwm-close-test.exe` after correction: both actual hidden Console cases
  passed, with a live text client receiving CTRL_CLOSE and with no other
  Console member. Neither case relies on a replacement provider.
- r016 `verify-native-gui-routing.ps1 -MonitorRpc` against r015, with temporary
  Z: mapping: all six passed. Repeated shared GUI launch now asserts the same
  actual worker PID; actual GUI-only close retains the independent GUI until
  its test-controlled exit37. Z: was removed in finally.
- r017 final r015 `Invoke-ProductVerification.ps1 -Suite Product` passed:
  Console17 69599ms, Window17 76365ms, retained WOW 66889ms, total217851ms.
  This manifest, not the earlier candidate, is the publication authority.
- r018 final service25 passed, 4229ms. The added production-service assertions
  verify that borrowed and nested native requests cannot retire the root,
  while its creating launcher may retire a self-created final-empty Console.
- `worker-shutdown-test.exe`: 37 assertions passed, handles before91/after91.

## Source-owned differences and current policy coverage

The first increment e81e9fc9f's `service_clear_frontend()` uses one route-list rundown. Its retained
native branch deletes the route because the native Direct record owns request
cancellation; the undelivered DOS-startup tombstone keeps the original pending
worker identity so Connect observes cancellation rather than adopting a new
root. These are different accepted/pending record contracts, not duplicated
root registries. This delivery does not blindly merge those operations.

### Subsequent paired cancellation repair

The first increment's type-based root-loss deletion was re-examined rather
than accepted as final. New `--route-cancellation` fixture against that
production library failed: `FAIL cancellation kind=0 phase=1 line=277`.
It found that a fully closed DOS lease remained as a pending marker, while an
undelivered native lease was discarded. The two outcomes depend on transport
phase, not worker kind.

The production rundown now deletes a delivered lease or one with both closure
acknowledgments; it retains a null-root cancellation marker for undelivered
DOS and native leases alike. Existing worker-process rundown/pruning owns
marker removal; no new list, scheduler, observer or timer is added. Launcher
abandonment still invokes its source-owned DOS/native command cancellation.
That command contract is not confused with the common root-loss I/O grant.

r020 `verify-service-fixtures.ps1` passed all 26 cases, 4154ms, with all
previous assertions retained. The new case covers both kinds and both pending
and fully closed phases, denies use after cancellation, and ensures a live
prepared-process marker is not pruned. It seeds only service-private route
identities; it does not claim guest execution or replace the production provider.
The source change rebuilds the affected x86 closure. Final r021 coherent
eight-file package passed r022 Product: Console17 62983ms, Window17 75922ms,
retained WOW 65991ms, preparation1894ms, total210617ms. r022's manifest is
the final publication authority, not a mixture with earlier runs.

The additional `--prepared-native-root-loss` case drives actual reservation,
PrepareWorker, RequestFrontend and Connect in the production service archive.
Its owned child remains suspended: after root loss, the exact child receives
an already signaled shutdown instruction and cannot fall back to the old
Console identity. Actual child death and asynchronous watch cleanup must both
complete before IsEmpty/Stop can pass. It does not claim guest execution or
RPC root-admission coverage from its trusted private root seed.

Retained test-development failures: r023 initially failed the final IsEmpty
check by assuming process death meant asynchronous cleanup had finished.
r024's callback lacked WINAPI and failed compilation; a mistakenly continued
stale-fixture invocation is not a valid result. r025 then failed because the
fixture attempted ConfigureEmptyNotify after registration; that API requires
an empty service. The final fixture configures its cleanup event before
Connect and waits for the actual callback, without Sleep or weaker assertions.
r026 passed all 27 service cases in4397ms with previous assertions retained.

r027 passed all six actual GUI routing/residency cases against r021, including
same-worker reuse and GUI target survival after carrier management close.
r028 passed paired DOS/native frontend loss/receipt1067, actual Console close,
independent session input/exit23, worker death with surviving target, and
explicit frontend close. All real-package runs were serial; Z: was removed.

Original cmdmisc.c DosSessionId/CloseOnExit and srvvdm.c separate-WOW handling
remain unchanged. Current run16 default Win16 classification is
BINARY_TYPE_WIN16; native GUI likewise uses the shared carrier. No separate-GUI
CLI option or new native scheduling/lifetime policy was invented. Exclusive
native text final-empty retirement and shared borrowed/nested protection are
tested; a newly exposed exclusive GUI launch is not claimed.

## Delivery and closure

Final-package r019 management.ps1 passed paired DOS/native frontend loss with
direct receipt1067, actual selected-worker Console close, independent session
input/exit23, worker death with surviving target, and explicit frontend close.
Only test-owned cleanup was used; it does not prove normal idle retirement.

r013 publish.ps1 verifies r017's eight hashes and saves coherent S4 recovery,
then publishes the r015 eight-file package to O:/winnt/system32. Guest media,
NTVDM.REG and configuration are untouched. Publication passed. r014 deployed
Console/Window empty/native-zero/MEM/EDIT smoke and all eight hashes passed.
Actual diff, ownership, source/ABI, lock and handle review, documentation
governance, relative links and whitespace checks passed. Original mirror diff
is empty. The reviewed implementation P is recorded in Git; unrelated planning
changes remain excluded.

Owner explicitly confirms “不新增独占 GUI 选项”: default native GUI carriers
remain shared/resident like shared WOW. Native exclusive text follows the
existing self-created Console/CloseOnExit boundary. The plan is corrected,
not expanded with an unrequested option or a new lifetime policy.

r029 publishes the final r021 package after validating r022's eight hashes,
preserving the previous e81e9fc9f/r015 package in r029/recovery. Guest media,
NTVDM.REG and configuration remain untouched. Publication passed. r030
published Console/Window empty/native-zero/MEM/EDIT smoke and all eight hashes
passed. Final source/ownership/lock/handle and diff review passed; the only
production change in this increment is phase-based route cancellation.
No src/mvdm or src/opennt-host file changed. Documentation governance/links
and whitespace are checked before forming the final reviewed P.

S5 closes only after those final gates and push. S6 retains naming and final
duplicate/caller audit; this is not whole-T closure. Physical
RDP/focus remains owner-waived; WOW retains its existing frontier contract
rather than a new full-usability claim. Side-session proposal and queue edits
remain separate.
