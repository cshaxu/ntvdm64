# Shared WOW worker: second-task startup failure

With Winmine already running, a diagnostic provider forces the second
allocation in original `xxxInitTask` to fail for the independently prebuilt,
hash-identified `GENPROBE.EXE`. The same real task/thread reaches seven
original initialization failures and registered cleanup; its native thread
exits with code 1. With the corrected launcher, the second task's `--wait`
returns 1114, while the first task remains alive and passes flag, reset, open
and close checks. The recorded runtime image hashes and provider identity are
retained alongside the trace and probe output. The diagnostic WOW32 is not
publishable.

The same injected failure with the previous launcher returned zero. The
updated formal probe expects 1114 and rejected that baseline with exit 17;
the earlier comparison with an old expectation independently recorded zero
and successful first-task survival. This is the existing-worker branch of the
same launcher correction as the [new-worker host-attach failure](../wow-host-attach-failure/review.md).

`verify-wow-mine-gameplay.ps1` now accepts an explicit, SHA-256-checked
prebuilt startup probe and checks the startup failure exit code instead of
incorrectly accepting zero. The corrected formal test and an ordinary
`-StartupOnly` Winmine control passed. No guest binary was built or modified.
The injected original InitTask failure does not cover every possible second
task failure or prove the independent intermittent SOL 1400 is fixed.
