# WOW32 host attach failure and Win16 launcher completion

The selected original `W32DllInitialize` returns FALSE if `CreateSmallHeap`
fails. A test-only copy of that selected WOW32 object forces that exact branch;
the other selected provider objects and the guest remain unchanged. In isolated
packages, the original `nt_bop.c` trace reached `load-failed`. On the baseline
launcher, default Win16 startup returned `ERROR_DLL_INIT_FAILED` (1114), but
`--wait` returned zero for the same failure. The formal regression rejects
that baseline; both per-mode reports, traces and manifests are retained here.

The root cause was the launcher's split: default Win16 startup called
`wait_wow_startup`, while `--wait` bypassed startup confirmation and consumed
the original parent completion value, which was zero in this case. Both the
new-worker and existing-worker paths now check the same service-owned startup
receipt first. Default returns on successful startup; `--wait` continues to
the original task completion event only after startup succeeds. Startup failure
retains the existing `ERROR_DLL_INIT_FAILED` translation; no guest or mirror
source changed.

With the corrected launcher, the formal forced-failure test passed both modes
with exit 1114 and the original `load-failed` trace. A second diagnostic build
also writes a test-only marker from the actual forced initializer branch; the
marker and `load-failed` were observed for both modes. Its baseline negative
control reached the same marker but returned zero for `--wait`, so the formal
test rejected it. Separately, normal SOL
`--wait` dialog/exit, default Winmine startup before window close, and a
shared-worker sibling task with normal exit and surviving Winmine passed.
An isolated service-death test with an admitted WOW task also returned the
existing Run16 error 6 and verified carrier cleanup; it makes no claim about
Console restoration.
The builder and test runner are in `tests/observation`; the complete production
package Full gate passed all 16 recorded gates with the matching production
candidate; `timings.json` and `runtime-manifest.json` identify this run. The
ten-component candidate was then published to `assets/release` and
`O:/winnt/system32`; `publication-result.json` records all 20 target hashes
matching. The injected DLL is diagnostic-only and was never published.

This demonstrates one actual host-loader failure under forced small-heap
failure, not all WOW32 attach, notification or unload failures, and does not
diagnose the independent intermittent SOL 1400. The current target policy and
normal guest lifecycle continue to apply.
