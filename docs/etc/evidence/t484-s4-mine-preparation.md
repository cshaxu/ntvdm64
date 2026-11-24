# T484 S4 P31: WINMINE preparation

## Scope and ownership

Complete verify-wow-mine-gameplay preparation using existing CMD producers,
prepare-test-inputs and physical-stage TestFiles. All sibling variants now
prepare their canonical names before staging; evidence/report copies remain
PowerShell assertions. Existing prebuilt hash and diagnostic-provider guards
remain. No product implementation, original guest or release image changed.

## Test review and corrections

Commit c158c342d removed the callback-frame text reporter. Two remaining
assertions matched its unavailable before/after text. Remove them rather than
reintroducing unused product instrumentation. Guest CDL2 destruction, window
validity, error/count records, exception non-continuation, exactly-once task
retirement and survivor gameplay remain required. Retired callback chronology
is not claimed as covered by the remaining assertions.

The initial inherited-environment run exited before a game window. Normal
Userenv launches reached the game but timed out waiting for patterned cells.
The unchanged HEAD script reproduced that failure under the same environment.
Failure captures showed clipped content. Moving the owned test window to the
desktop origin restored the board and flag checks. F2 then reproduced clipping:
actual GetWindowRect reported left=-79,top=-48. The probe now moves a window
back into view when it becomes negative before pixel sampling. No pixel
criterion is relaxed: reset must reproduce the initial cell hash exactly.
Failure captures are retained for board-ready and reset failures.

## Actual evidence

Root: build/t484-s4-mine-preparation.

- PID7484: cached full build and11 raw/22 published hash checks pass; inherited
  environment test fails before the game window (probe4).
- PID33296: native Userenv test reaches window, board timeout (probe5).
- PID14016: pre-change script reproduces probe5 in the same user environment.
- PID18904: failure screenshot confirms clipping.
- PID31516 and25132: initial positioning fixes board/flag; reset clipping is
  preserved as probe9 and a screenshot, not counted as successful.
- PID32844,2026-11-24T20:31:37Z-20:33:22Z,exit0: final native probe rebuild
  and four real private-desktop cases pass: ordinary gameplay, startup-return,
  normal retiring sibling and exception sibling. Both sibling cases require
  the surviving game to remain interactive and finish normally. Reset returns
  cell hash c924d93b after the recorded reposition; final captures are retained.

Final source changes after the cached host build concern only test preparation
and the native test probe, which was rebuilt for the final cases. The unchanged
11-image product and22 dual-publication hashes are verified; no fresh product
copy or Full test run is claimed. Diagnostic-only Shell/startup-failure and
sealed CBT variants retain their provider guards and share the same preparation;
this batch does not claim their runtime injection behavior was reaccepted.

## Accounting and follow-up

This caller's preparation is delivered. Frozen census347/355 disposed,8 pending;
five overlapping supplemental chains remain. S4 and final S5 acceptance stay
open. The earlier inherited-environment failure remains a known boundary,
not a product repair attributed to this migration.
