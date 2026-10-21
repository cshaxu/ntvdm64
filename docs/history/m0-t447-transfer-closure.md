# M0 T447 — owner-directed transfer closure

## Disposition

Owner explicitly directs closure of T447, placing all unfinished WOW32 work
into a successor candidate and then placing NTSRV lifecycle unification ahead
of it. This is administrative scope-transfer closure, not full functional
acceptance of WOW32 or a claim that the CAF defect is universally repaired.
The reviewed checkpoint preserves pending production/test changes and the
coherent published package; remaining failures are carried, not waived as passes.

## Complete ownership handoff

1. [NTSRV lifecycle unification](../proposals/proposal-ntsrv-lifecycle-authority-001.md)
   owns all outstanding managed-process control, final request completion,
   GUI/CUI separation, ten-second deadlines, service-loss cleanup and CAF
   Console return work, including the full latest source-audit findings.
2. [WOW32 continuation](../proposals/proposal-wow32-execution-continuation-001.md)
   owns every other unfinished T447 obligation: full M1--M6/C/W inventory,
   execution/control/dialog/menu dependencies, individual task termination,
   application/DOS gates, source minimization, inherited risks and final review.

The [working plan](../etc/operations/t447-wow-execution-plan.md) and its linked
ledgers remain the complete detailed transfer inventory. No row is silently
dropped because historical sections used different S numbers. The remaining
queue retains its prior relative order after these two packages.

## Checkpoint evidence and limitations

Final repeat on 2026-10-21 UTC: closure-build/result.json passes thirteen
images and 1376 unchanged intermediates. The generator refreshed NTVDM's
generated configuration, so the earlier package identity was rejected and
the final complete package retested. closure-tests/results.json has seven
zero exits; closure-service.out reports all 33 cases; closure-mine.result
passes Off/Win16/All; closure-caf.out passes actual internal CMD and direct
NTMON deletion plus visible-buffer echo before Enter. Background build PID
13324 and runtime PID 32376 completed. These repeat results do not erase the
installed/guest limitations below. Final publication is recorded separately
in closure-publication.json and the committed release-manifest.json.

The stray generated console_terminal_observer.obj was moved from repository
root to build/t447-s4-p4/recovered-root-console-terminal-observer.obj;
SHA-256 848B9937883624FDA7E7BB206B03F979CFE36016F38BA6C12A22953CC294F5AF.
It remains recoverable, not a source deletion.

Base a9bc337fd plus the reviewed P4 repair: service draining transaction,
separate preparation/final return, terminal presentation freeze, independent
restoration attempts, uniform launcher finish and expanded lifecycle tests.
This is partial hardening; the newly approved central authority contract is
not implemented by this checkpoint.

Full-build r4 passed all thirteen images and retained 1376 intermediates on its
second pass. Seven focused lifecycle cases and service33 passed. WINMINE Off,
Win16 and All gameplay/normal exit passed. Staged actual conhost internal CMD
and direct NTMON self-delete passed pre-Enter echo; twelve immediate native
interactive relaunches passed. Installed direct NTMON self-delete passed.
Installed CAF natural CMD exit passed immediate echo with NTCON still alive.

Installed nested CMD self-delete did not reach the confirmation/deletion step;
it is not an echo-after-delete pass. COMMAND failed before first prompt in both
candidate and prior installed baseline. Strict DIR/full DOS groups therefore
do not pass. WRITE failed startup with illegal instruction in both old and new
packages, without proof of a shared cause. SOL lacked sufficient test canvas.
An earlier observer batch with leaked empty environment flags is invalid and
excluded. No claim that all product tests passed or that remaining defects are
all guest-originated is made. Exact reports are retained in the
[CAF evidence](../etc/evidence/m0-t447-s4-p4-ntcon-caf-close.md).

All thirteen built images were published together to assets/release and
O:/winnt/system32 with matching SHA-256, recoverable prior sets and no guest,
configuration or assets/winnt.zip change. OpenNT mirror diff is unchanged.
One nxvm header timeout change is explicitly registered, with upstream/local
hashes retained. Final verification and Git delivery are recorded by the
closure commit and its accompanying evidence; subsequent admission is separate.

Raw fault dumps and package rollback copies remain temporarily under
build/t447-s4-p4 for the lifecycle successor, not reusable caches. Stable
.ninja/.ccache remain untouched. No prune, guest rebuild or wholesale runtime
copy was performed.
