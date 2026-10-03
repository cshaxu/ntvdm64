# T424 S13 final source and delivery audit

## Question, inputs and procedure

Owner authorizes all remaining stages after S11 and requires a stop before
T closure. Input is S12 46abdb554, its source freeze/publication/recovery
manifests and final regression reports, plus the accepted pre-T424 baseline
f64559086. S13 performs a read-only production audit and documentation repair;
no executable source, wire contract, imported library or guest changes.
Outputs are build/M0-T424/S13/r001; final-audit.ps1/result retain exact checks.

Audit the actual source/build providers, not spelling alone. Compare every
frozen S12 input and every published/staged/cache product. Scan source/tests/
tools and generated graph for old project identities, classify raw historical
hits and review original mirror diff. Review service creation, completion,
retirement, frontend ownership and both worker I/O contracts against their
stage ledgers. Run documentation governance, links and actual Git diff review.

## Completed sequence and retained evidence

| Stage | Delivered boundary and evidence |
| --- | --- |
| S1 | [Referent/source audit](m0-t424-s1-name-referent-audit.md), original Console exclusions, no production rename. |
| S2 | [Native worker/Win32Record naming](m0-t424-s2-native-worker-identity.md), intermediate identity without new policy. |
| S3 | [Broker retirement repair](m0-t424-s3-abnormal-exit-relaunch.md), later refined by S4's bounded grace. |
| S4 | [Broker-centered launch/control](m0-t424-s4-broker-centered-launch-control.md), no launcher/frontend direct control edge. |
| S5 | [Shared control/ownership cleanup](m0-t424-s5-shared-control-cleanup.md), completion/return authority retained. |
| S6 | [NTVWM naming](m0-t424-s6-native-worker-name.md), original names/provenance preserved. |
| S7 | [Common protocols/providers](m0-t424-s7-common-service-separation.md), control RPC versus I/O pipe, no service policy in common. |
| S8 | [Service-private provenance/split](m0-t424-s8-ntsrv-service-separation.md), one state/lock/provider, original DOS/WOW owners unchanged. |
| S9 | [Native GUI routing](m0-t424-s9-native-gui-routing.md), launcher classification before admission, GUI startup/default and explicit wait. |
| S10 | [Notification/coordinate repair](m0-t424-s10-frontend-notification.md), exact current-state return/relaunch. |
| S11 | [NTCON frontend identity](m0-t424-s11-frontend-name.md), eight-file recovered publication and zero obsolete frontend providers. |
| S12 | [Unified logical surface](m0-t424-s12-logical-surface.md), atomic publication, upper-left projection and symmetric handoff. |
| S13 | This audit, documentation correction, unchanged production identities and clean delivery; not T acceptance. |

## Ownership and semantic conclusions

- run16 main creates only a missing NTSRV during bootstrap, classifies/submits
  targets and waits on service receipts when required. frontend_scope holds
  startup-only diagnostic target references but does not use them as native
  completion or Console-return authority. GUI default returns startup success;
  explicit wait preserves real result. No new CLI delimiter requirement.
- NTSRV creates/authenticates frontend and workers, owns registration, task
  delivery/results and orderly retirement. Service-private seven-unit split
  shares its one state/lock, not a second registry. Original DOS/WOW policies
  remain in opennt-host srvvdm. Win32Record is one direct request; no Observed
  descendants, task graph or NTMON process enumeration was introduced.
- NTVDM original execution/block/resume/completion stays in MVDM/OpenNT.
  NTVWM owns its real hidden Console, target creation/wait and actual exit
  report. NTSRV delivers both results to run16; it does not fabricate a Win32
  result from a worker's successful exit. Backend-specific operations remain
  explicit, not a generic DOS/native scheduler in worker-base.
- NTCON alone owns visible Console/Window, display, logical cells/projection
  and input. It owns no execution target or hidden Console backend. Its
  direct worker pipe is I/O only: frames, input, title/geometry and final-state
  barriers. Common protocol/client/transport providers are single-selected;
  common has no reverse dependency on worker-base or EXE-private code.
- NTMON consumes NTSRV projection/control only. kind DOS=0, Win16=1, Win32=2
  and actual registered direct records retain their accepted meaning. Monitor
  UNBOUND enhancements belong to the queued candidate, not this T.
- Existing ntsrv/transport/worker_spawn startup Job is retained and disarmed
  before Resume after admission. It is rollback for an unhanded-off worker,
  not target lifetime pairing, descendant observation or a new runtime policy.
  This audit did not add a Job/helper/process or move original algorithms.

## Findings and repairs

Current design docs mixed delivered T424 common/S8 work with stale candidate
and declaration-only interface language; run16 README still claimed local GUI
creation. Corrected to the actual single-provider graph and service route.
NTSRV README repeated its entire private-service paragraph; removed the
duplicate. MVDM README's current prose retained the old worker spelling;
changed only that documentation referent. T423 source/evidence filenames stay
historical, not renamed or treated as current executable aliases.

Architecture now distinguishes delivered S12 storage from planned work and
labels old T423 ConPTY/frontend-owned-backend descriptions historical. It
records no-frontends/no-workers/no-admissions service grace accurately and
identifies broker-owned startup rollback rather than launcher-owned worker
creation. Current eight-file packaging/artifact/logging documentation matches
the admitted build-root and logs2 exceptions. Existing restrictions are not
expanded into new capabilities. Unexplained management-close failure is
explicit debt; existing 30ms sampling remains intentionally unchanged.

## Actual checks and result

```text
pwsh -NoProfile -File build/M0-T424/S13/r001/final-audit.ps1
git grep -I -n -i ntkvm -- src tests tools
git grep -I -n -i ntw32 -- src tests tools
git diff f64559086 --numstat -- src/mvdm src/opennt-host src/opennt-abi
pwsh -NoProfile -File tools/governance/Verify-DocumentationGovernance.ps1
pwsh -NoProfile -File tools/governance/Test-DocumentationRelativeLinks.ps1
git diff --check
```

final-audit.txt passes 246 frozen inputs: the only changed inputs are the
three documented source READMEs, not source/test/build implementations.
All 47 frozen imported-library-path inputs are unchanged. Every eight-file
published/staged/cache identity is identical to tested S12, x86 machine014c;
all eight prior S11 recovery hashes match. Protocol/RPC34 and I/O22 match.
No obsolete NTKVM source/test/tool hit; exactly two NTW32 hits are historical
T423 evidence filenames. Retired interface contains one README move marker
and no production files; generated graph has no retired providers. Initial
audit incorrectly classified this marker directory as a live alias, failed,
then was corrected to assert its exact one-file historical-only content.
No result from that failed audit is counted as a pass.

Entire T424 original mirror/opennt-abi delta contains only src/mvdm/README.md;
zero original executable algorithm/ABI/library/guest changes. No Z: remains.
S13 documentation-only changes reuse exact S12 x86/MIDL, focused, 34-route
Console/Window, strict DIR, EDIT/current-cell return, lifecycle/isolation,
five mismatch, GUI and WOW frontier evidence. A fresh runtime rebuild/test
is not claimed or needed for documentation; source/product hashes prove the
unchanged tested code. Governance, relative links and diff must pass before
the containing reviewed commit/push; CURRENT owns final clean state.

## Remaining owner gate and confidence

No host scrollback guarantee or manual desktop/RDP acceptance is invented.
SOL/WRITE remain their prior memory-dialog frontier, not usability passes.
One management-close probe failure remains unattributed despite four unchanged
production-source passes; preserve failure diagnostics and exact replay record.
This is not a proven production repair or an original-guest exemption.
T424 stays open. All admitted S stages are delivered; no next T is admitted.
Stop for owner validation before writing a T closure/acceptance record.
