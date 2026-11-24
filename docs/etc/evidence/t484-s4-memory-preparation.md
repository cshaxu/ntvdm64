# T484 S4 memory-transition preparation

## Change and scope

P24 routes S6PROBE.COM aliasing and the two task-cleanup children through the
existing CMD input/stage owners. Build before staging, so assembly failure does
not allocate a runtime. Remove the redundant copied EMS builder executable.
The short prepare-ems-test.cmd accepts only a marked immediate O:/tmp stage,
copies the already staged probe and invokes the existing native EMS builder.
It preserves E30.COM/PIF/NT/AUTO.NT placement and close-on-exit behavior.
No new PIF algorithm, interpreter dependency or original guest change is added.

Existing runtime assertions, observer deadlines, guest markers, product/media
hash recording and failed-run evidence retention remain. The EMS case also
checks non-stage rejection and duplicate preparation rejection; hashes prove
all four outputs remain unchanged after the rejected duplicate call.

## Verification

Owned Userenv process15764,2026-11-24T19:06:49Z to19:10:02Z, exit0, runs
build/t484-s4-memory/run.ps1. Cached full build and all six real private-desktop
cases pass: interrupt16, interrupt32, code32, task-cleanup, ems-call and
xms-failure. The latter asserts forced relocation/data release, not arbitrary
allocation failures. Each case requires its exact success markers, absence of
failure markers, normal guest/outer COMMAND exit and unchanged product inputs.
EMS rejection contracts execute in the checked-in integration runner; an
additional call against O:/winnt returns2 without preparing files there.

All six stage.json paths were rechecked absent after success. All11 raw host
images and22 dual-published hashes match the existing delivered package.
Publication reuse is justified by identity; this is not a new Full result.
No source under src/, release image or assets/winnt.zip changes.
Parser, documentation/tracker and diff gates precede delivery.

## Progress

Frozen census remains343/355 disposed,12 pending. Supplemental preparation
chains decrease9 to8. Video7 preparation remains outstanding; this record
does not claim that adjacent chain complete. S4 and final S5 remain open.
