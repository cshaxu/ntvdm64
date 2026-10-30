# Split native-return diagnostic

A current-source diagnostic-only DLL splits the return window projection,
returned-menu refresh, borrowed graph, and process-wide menu refresh failure
markers. The first two new markers record the target HWND or returned HMENU
respectively. These are observations, not window-generation identity; the
window target is not necessarily the root that failed. The patch and input
hashes are retained. Original short-circuit order and error-preserving tracing
are preserved; no guest or released component was changed.

A fresh isolated package completed eight SOL Options open/close/recovery
rounds, exit 0, without a failure marker. This checks normal-path operation of
the instrumented package only. It did not trigger either new marker, prove
failure-path attribution, reproduce the historical 1400, or establish a fix.
Instrumentation can affect timing. This is not product Full acceptance.
