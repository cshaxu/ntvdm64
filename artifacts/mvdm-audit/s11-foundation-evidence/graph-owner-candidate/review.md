# Graph publication ownership candidate

The project-owned graph publisher now acquires the existing runtime exclusive
lock around a complete graph refresh, unless its caller already holds it. This
closes the demonstrated gap between callback-side graph capture and terminal
table retirement. The `finally` restores the entry lock state and last error;
inactive domains and absent bindings retain the existing validation path.
There is no guest or mirrored MVDM source change. Native identity sampling
remains a snapshot, and existing original finalizer/task scopes retain their
own callout handling.

The pre-change controlled fixture observed graph publication succeed while a
resource thread held the runtime lock. The post-change version uses the real
task callback scope and real CCPU graph: a test-only gate holds the competing
runtime lock, the publisher requests it, then acquires it after retirement
progresses; the callback's original unlocked state is restored. The normal
and forced failure history is documented in the adjacent
[boundary review](../hook-graph-lock-boundary.md). This witness does not call
the original hook dispatcher or prove the historical intermittent 1400 cause.

Selected graph snapshot, native creation/unwind, native nested callback and
forced overlap tests passed. The candidate's real shared-worker idle/dialog
test passed. The coherent ten-image package passed all 16 Full gates, four SOL
Options open/close cycles with normal launcher completion, and Winmine
flag/reset/open/close gameplay. Package and actual gate manifests, Full timing,
and focused output are retained here. The full immutable guest archive was
used in the staged test, not a repaired `O:/winnt` guest installation.

Publication copied the tested complete ten-component package to both
`assets/release` and `O:/winnt/system32` and verified all 20 file hashes.
This does not establish universal callback interleavings, remedy the 15 absent
installed guest files, or attribute the historical Winmine/SOL 1400.
