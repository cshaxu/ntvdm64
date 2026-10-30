# Native-return substage diagnostic

An isolated diagnostic WOW32 provider split the existing short-circuit native
return publication into the same ordered operations: window/payload publish,
borrowed-client graph publish, and menu refresh. Only on a failure it invokes
the existing opt-in retirement trace with the failing substage and preserved
LastError. The diagnostic source and its linked DLL/map hashes are retained;
the runtime source was restored immediately after compiling the diagnostic,
and the cached non-diagnostic WOW32 was subsequently rebuilt. The diagnostic
package is not a release candidate.

With existing window/callback tracing enabled, the isolated package completed
five and eight SOL Options open/close/recovery rounds in two runs; both
launchers exited normally. No diagnostic failure marker was emitted. The
captured package manifests, output, window traces and input hashes are in
`run-01` and `run-02`. This is a non-reproduction, not evidence that the
unmodified baseline's separately observed `publish-native-return` 1400 is
fixed, that any substage is safe to ignore, or that tracing cannot perturb the
failure. The exact failing HWND and substage remain unknown. Guest binaries
were unchanged; no Full product gate was run.

A further bounded external-window control completed five interactions. The
probe created 150 of its own top-level native windows after round one,
confirmed all 150 in the desktop child/next chain before round two, and
destroyed them before round three. All five dialogs recovered and the launcher
completed; no substage failure marker appeared (`external-01`). This checks
the added/removed-window scenario in one ordered run, not which external
nodes entered every guest projection, all concurrent destruction timings, or
the root cause of the separately observed 1400. It does not make the original
non-pass a pass or establish a performance improvement.

A subsequent uninterrupted 16-round run of the same isolated diagnostic
package completed every dialog and launcher exit (`run-03`); no substage
failure marker appeared. The failure remains intermittent and unlocated;
the run does not prove the diagnostic and uninstrumented candidates have
identical timing or that further repetitions would never fail.
