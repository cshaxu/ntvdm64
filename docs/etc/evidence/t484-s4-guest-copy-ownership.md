# T484 S4 P33: guest-copy ownership and task-name cleanup

## Owner request and audit scope

While consolidating guest installation copying, the owner requested that the
historical .t479-owner protocol and similar current mechanisms be audited and
regularized together. Scanned tools, test and src, including uppercase CMD/BAT,
for historical task-number file suffixes and ownership producers/consumers.
Historical evidence and named regression checkpoints are not mutable ownership
protocols and are retained for traceability.

The live findings were the guest marker, a local identity-test directory without
matching cleanup, and two .T438 temporary suffixes. The host-package marker
.ntvdm64-test-stage.json is distinct: it carries package identities/hashes,
whereas a guest directory records its owning evidence directory. Do not merge
these different contracts into a second generic lifetime state machine.

## Changes

- create-guest-probe.cmd owns fresh directory creation and the stable
  .ntvdm64-guest-owner marker. The default destination is an immediate O:/tmp
  child; the explicit local-control mode uses the user's LocalAppData/Temp.
  Owners must exist beneath repository build. Existing destinations are refused.
- copy-guest-probe.cmd composes that creator with bounded robocopy. Both Win31
  and Win95 helpers use it and retain existing source-hash evidence and failure
  cleanup. Source markers cannot confer ownership on a new copy. The sole old
  marker reference is an exclusion during source copying, never an accepted
  cleanup marker or a new producer.
- Remove-OwnedGuestProbeCopy retains wrong-owner, linked-path, Git-content,
  active-process and final ownership checks. Its explicit local-control option
  uses the known local-app-data directory, independent of later TEMP changes.
  verify-win31-identity now closes its formerly leaked auxiliary directory.
- The cleanup test uses the shared creator and verifies the local lifecycle
  after TEMP changes. Expected negative native exits no longer leak as a stale
  final failure code after all assertions pass.
- GRP uses .grp-tmp, and win31-path uses .path-repair.tmp, replacing task labels.
  Their existing replacement/backup algorithms are otherwise unchanged.

## Actual evidence

Root: build/t484-s4-guest-copy.

- Initial copy PID33100: cached full build and11 raw/22 published host checks
  pass;224 Win31 and1166 Win95 files match source hashes, which remain unchanged.
  Existing destinations are refused. Negative testing confirms invalid owner
  creates nothing and a locked source returns failure with an owned partial copy
  that the existing cleanup removes.
- Final marker/copy PID32200,21:00:07Z-21:00:37Z,exit0, repeats both real copies
  under the new protocol. All owned O:/tmp copy directories are removed.
- PID15544: final cached full host/addon build and14-addon publication pass.
  All cleanup assertions pass, but its first wrapper reports failure because
  an expected negative case left LASTEXITCODE set. This is recorded, not erased.
- PID17804,21:01:00Z-21:01:04Z,exit0: corrected test returns success after all
  assertions, including C-local cleanup, wrong-owner/link/Git/live-process
  rejection, nested ownership and failed Win31/Win95 preparation cleanup.
- grp-write-result.json proves an actual CHANGED=1 relocation, changed group
  bytes, valid parsed output and no .grp-tmp residue. The preceding CHANGED=0
  run is not evidence for the write path.
- path-repair-result.txt records actual APPLY.CMD success on an owned Win31
  copy, including INI and group rewrites and no remaining workfiles. That copy
  is removed through the shared owner checks; installed O:/w31 is not modified.

Only GRP.EXE and addon-manifest.json change in assets/release. The11 host images
retain all22 published hashes. The addon publication uses the current CMD
pipeline; original guest binaries and winnt.zip are unchanged. No Full or full
Win31/Win95 interactive acceptance is claimed for this preparation cleanup.

## Remaining work

The shared copy step is delivered, but INI/PIF/hive preparation in its callers
is still pending. Census remains348/355 disposed,7 pending and five overlapping
supplemental chains. This cleanup does not count any parent caller complete.
Current-state history is compacted into its indexed delivery table without
discarding the earlier failure records. S4 and S5 remain open.
