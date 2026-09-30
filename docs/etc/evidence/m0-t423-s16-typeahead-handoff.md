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
Window `eexit` timeout remains a separate real input-order fault. Static
review points to the handoff among NTKVM's copied input queue, NTCON's hidden
Console `return_unused_input`, and DOS reactivation. None is yet proved to be
the producer of the extra `e`; no production fix is claimed by this P.

Next: obtain key-by-key ownership evidence across both handoff directions,
then patch only the proven boundary and repeat real zero-delay routes plus the
full production regression gate. Do not add sleeps or weaken assertions.
