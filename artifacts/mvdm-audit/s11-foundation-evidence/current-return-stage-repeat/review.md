# Bounded native-return diagnostic repeat

The existing failure-stage diagnostic provider, built from the current
`wow_user_task_lifecycle.c`, completed 24 SOL Options opens, closes and
parent recoveries in one visible-desktop run. The owned launcher exited zero;
no diagnostic publication failure or native-return 1400 appeared in the
trace. The package manifest, probe and observer output, and trace hash are
retained here; the full trace remains in the task build directory.

This result does not identify the earlier failure or prove that it was fixed.
One process and one schedule are not exhaustive, and instrumentation may
alter timing. The diagnostic package was not published; no guest was changed.

A separate old-input control reused the nine non-WOW32 component inputs from
the historical SOL non-pass and a failure-only diagnostic WOW32. It also
completed 24 rounds with launcher exit zero and no failure file. Its inputs,
summary and hashes are in [old-input-control.json](old-input-control.json);
full output remains in `build/t473-s11/sol-old-provider-triage`. The WOW32
provider differs from the historical uninstrumented image, so this is not an
exact binary replay. An earlier injected window failure validated that
logger's window-stage path; borrow and menu negative controls were not
established. This control therefore cannot exclude the historical failure or
establish its cause, and does not justify further repetition on its own.
