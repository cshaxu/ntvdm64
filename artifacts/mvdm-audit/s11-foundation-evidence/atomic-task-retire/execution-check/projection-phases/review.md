# Modal projection phase and origin diagnostics

The first isolated diagnostic ran the integrated page-domain candidate with a
WOW32 timing wrapper around projection, native capture, chain normalization and
projection commit. Three SOL Options interactions passed. Probe clock markers
were matched against the first and last 64-call counter flush inside each
phase: the covered intervals show 128–192 projections per phase, and inclusive
projection measurements of 514–1,041 ms on opening and 527–743 ms on closing.
Commit measurements within those projections were 230–422 ms and 230–285 ms
respectively. Full phase wall times were 2.6–3.0 seconds, while the matched flush
intervals cover less than those whole phases. These values are not exclusive
CPU time: nested calls can double count, flushes omit partial intervals, and
QPC, file writes and diagnostic code affect timing. See `phase-analysis.json`,
the staged manifest and full probe output.

A separate instrumented build retained a thread-local immediate refresh origin
across nested refreshes, recording the return address every 64 projections with
the loaded module base. All three interactions passed. Among game-thread
samples matched to open/close phases, 13 resolved to `publish_guest_return`,
seven to the `wow_user_window_refresh_graph` wrapper and two to
`wow_user_window_refresh_message`; source map and per-sample data are under
`origin/`. These are sampled return addresses, not exact event frequencies;
optimized inlining can obscure the next caller. The first wrapper is the
native/scheduler return publication path, which also preserves returned window
and menu identities before guest continuation. Suppressing it without proving
the relevant lifetime, nesting and freshness contracts would be unsafe.

This narrows the observed repeated work to project-owned projection and its
publication entrances, rather than proving a fault in original CCPU logic. It
does not yet identify a safe point to omit a publication or claim a complete
latency breakdown. The diagnostic packages and original guest are not released.
