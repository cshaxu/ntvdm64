# M0 T423 S16 — zero-delay input handoff

## Question and inputs

Why does the supplementary `dos-native-typeahead` case intermittently turn
the last `exit` into `eexit`? Inputs are the accepted S14 baseline, published
S15 eight-file package, `Verify-CommandExitStatus.ps1`, its unchanged observer
binary, original COMMAND/CMD/MEM media, and private-desktop Console/Window.

## First investigation P

The observer writes one paired key make/break per character, 100 ms apart,
with no *extra line* delay in this case. Window injection uses the public
Window key route; Console injection uses `WriteConsoleInputA`. Both route
through NTKVM before the selected worker consumes copied input.

- Published Window run `t423-s15-published-typeahead-r1` completed with two
  MEM outputs and exit 1. An identical `r2` timed out with real captured
  `WINNT>eexit`; this is not a marker-only test error.
- Console comparison `t423-s16-console-typeahead-r1` actually completed with
  exit 1 and two MEM outputs. Its line-02 snapshot contained the genuine
  native CMD version line, but the final 25-row DOS screen no longer did.
  The verifier checked only the final Console snapshot, falsely reporting
  that the native target never started.
- The existing contiguous-overlap merger reconstructs this run with one CMD
  version line and exactly two MEM outputs. Its test now includes the
  buffer-shrink case, and the verifier invokes it for this Console case as
  well as the established Window cases. Re-run
  `t423-s16-console-typeahead-r2` passed without relaxing the execution,
  output-count, bad-command, or exit-code assertions.

Thus one failure was definitely an observer/verifier false negative; the
Window `eexit` timeout remains a separate real input-order fault.

## Published-package counterchecks

The same unchanged ordinary eight-file package reproduced the defect after
P1, with actual final DOS text rather than only a timeout:

| Route / log prefix | Result | Final command |
| --- | --- | --- |
| Window, `t423-s16-window-repro-1` | timeout | `xexit` |
| Console, `t423-s16-console-repeat-1` | timeout | `eexit` |
| Window, short build-root alias `t423-s16-window-shortroot-control` | timeout | `xexit` |
| Pure DOS nested `t423-s16-dos-only-typeahead` | pass | intact `exit` |

Both Console and Window failures occurred after two real MEM outputs, so a
Window-only key mapping defect and a final-screen marker false negative are
insufficient explanations. The short-root control used the byte-identical
published NTKVM and still failed; the package path is not the cause. The
observer calls its scripted input writer once and submits one make/break pair
per character. The current evidence therefore establishes a real shared
handoff/consumption instability, but not yet its exact producer.

An isolated x86 `frontend-video-observer.exe` was built under the S15 formal
cache and run from a copied package in `build/M0-T423/S16` via a temporary
short drive alias. Its three Window runs passed. A reduced handoff-only trace
variant also passed twice; the experimental test-only change was removed
because logging changed timing and did not capture a failing run. Neither
observed pass is accepted as a product repair, and the production package
was never replaced. Static review narrows the next witness to NTKVM's copied
input queue, NTCON's hidden-Console `return_unused_input`, and the original
DOS history/reentry consumer. No code owner among those three has yet been
proved to duplicate or lose a particular key.

Next: obtain low-perturbation key-by-key ownership evidence across both
handoff directions, then patch only the proven boundary and repeat real
zero-delay routes plus the full production regression gate. Do not add sleeps
or weaken assertions.
