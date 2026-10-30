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

A controlled fixture extracted the two instrumented functions from the exact
diagnostic source and supplied Windows/service seams. It passed normal return,
window refresh failure 1400, returned-menu failure 1410, borrowed-graph failure
5023, process-menu failure 5, and invalid payload count. The checks cover
short-circuit order, first stage label and error, final last-error, and the
pending-publication flag. The first harness run exited 21 because its assertion
mistook the additional outer payload marker for the first marker; the corrected
assertion checks the first marker and passed. This fixture verifies the
instrumentation with mocked service boundaries, not a live 1400 or object
generation. The diagnostic remains absent from the released product.
