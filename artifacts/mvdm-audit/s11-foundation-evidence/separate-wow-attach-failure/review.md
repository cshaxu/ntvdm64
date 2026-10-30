# Separate WOW forced provider-attach failure

An isolated native host, itself launched through the product Run16, called
`CreateProcessA` on the hash-verified prebuilt `IDLPROBE.EXE` with
`CREATE_SEPARATE_WOW_VDM`. The returned process handle named the staged
`run16.exe`. A diagnostic-only WOW32 forces the original small-heap failure
branch and writes an attach marker. The actual worker trace reports
`load-failed`. Both the current launcher and its earlier baseline returned
255 through the process handle; neither claimed successful startup. The
observer exited zero after checking that result. Per-run package manifests,
native identity, marker and trace are retained beside this review. The guest
was copied unchanged; the diagnostic provider was not published.

The different code from the direct shared-WOW failure (1114) is not evidence
of truncating a Win32 exit code. Source inspection shows that the separate
WOW service receipt is backed by worker-process completion, which copies the
worker's exit code. Original `nt_reset.c` initializes `VdmExitCode` to
`0xFF` and uses it for process exit; `wait_wow_startup` preserves a nonzero
completion instead of synthesizing 1114. This is consistent with the
observation, although this probe does not independently capture the worker's
exit code or prove every possible failure follows that route.

The baseline and current launchers both use the startup-only route generated
by Hook; this is not an independent test of `--wait` for separate WOW. The
first run incorrectly expected 1114 and failed the test assertion while
recording the same 255 and load-failed marker. The corrected check was
repeated against both package identities. The retained temporary witness is
provided for inspection, not as a general regression runner. It relies on
the identified prebuilt input and temporary diagnostic packages under build.
