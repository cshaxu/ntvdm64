# Failure-only native-return diagnostic

The isolated diagnostic splits native-return publication into the same
short-circuit order and writes the failed substage through the existing trace
writer only after a failure. Its dedicated path is controlled by
`MVDM_WOW_PUBLISH_FAILURE_TRACE_PATH`; regular window/callback traces were
disabled during the run. The source and linked image/map identities are
retained. This was a diagnostic provider, not a production change: source was
restored, the non-diagnostic cached WOW32 rebuilt, and Ninja reported no work.

One isolated 16-round SOL Options run completed every open, close, parent
recovery and launcher exit. No failure file appeared. This is a
non-reproduction, not a repair of the uninstrumented baseline's observed 1400.
The failure-only logger and environment delivery have not yet been verified
by an injected substage failure, so absence of the file cannot independently
establish that a failing substage would have been reported. Package manifest,
probe output, observer receipt and build logs are retained. No guest or Full
product acceptance is implied.

A separate negative-control provider injected `ERROR_INVALID_WINDOW_HANDLE`
before the first window-publication result when a test-only environment
variable was set. Its isolated run did not open SOL as expected; the probe
returned 22 and the failure-only file recorded `diag-publish-window` and
status 1400 from the worker. This verifies the path, environment propagation
and window-substage logger for that controlled failure. The injection
occurred during startup, so it is not a reproduction of the intermittent
dialog failure; borrow and menu substage injections remain unverified. The
negative-control source, image identities and logs are in `negative-window`.
The injected source was removed and the non-diagnostic cached WOW32 rebuilt;
the negative provider was never published.

A subsequent 24-round failure-only run completed all dialogs and launcher
exit without a failure marker (`run-03`); this does not invalidate the prior
unmodified baseline failure. An initial 32-round request did not start the
product: the runner's computed observer budget was 650 seconds, exceeding
the observer's 600-second maximum. The test runner now rejects this parameter
combination before creating a staging directory; a negative preflight check
passed. This is a test-parameter correction, not a product failure or fix.
