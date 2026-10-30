# Published-window retirement negative control

The [formal positive witness](../terminal-schedule-formal/review.md) was rerun
with a disposable test-only copy of the selected terminal provider. In that
copy, both runtime-lock acquisitions in `wow_window_words_terminal` were
disabled while the test's event hooks and all other selected providers were
retained. The fixture exited 1 and the runner rejected it: the marker reports
`errors=1 first_phase=1 held=0 unheld=2 retired=1`. Thus the witness detects
the targeted missing lock instead of passing solely because retirement ends.
The exact source and executable hashes and output are recorded here; temporary
inputs and compile output remain under `build/t473-s11/terminal-schedule-negative-run/`
and `build/t473-s11/terminal-schedule-negative.c`.

This is an intentionally invalid synthetic comparison, not a historical product
binary or a reproduced SOL failure. The separate current-production-source
positive run passed; neither run proves all guest interleavings. No product or
guest image was changed.
