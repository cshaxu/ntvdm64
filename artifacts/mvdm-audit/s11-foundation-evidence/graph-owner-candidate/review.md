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

The subsequent selected-object owner-restoration witness passed graph success
and invalid-payload failure with both held and unheld caller locks. It checks
the exact error and caller lock state, along with the forced competing
retirement. The production source was unchanged after publication. Its
output and exact test/source hashes are `owner-restoration.txt` and
`owner-restoration-hashes.csv`. This does not inject an exception within every
native observation or test all reentrant finalizers.

The later `-HookGraphOverlap` witness runs the actual `WU32StdHookProc`
wrapper and original `WowOriginalStdHookProc` against the real task and CCPU
graph. Test-only synchronization pauses after callback admission, makes a
bound resource thread hold the runtime lock, and verifies the graph waits for
that lock and the callback restores its unlocked state. The test uses an
unused original hook slot and negative hook code, so no guest hook procedure is
invoked. The existing direct-callback overlap control also passed. The first
attempt incorrectly selected original slot zero, which is a permanently used
sentinel requiring a real PTD, and exited with access violation; slot one fixed
the fixture setup. This is not a product failure or proof of the historical
1400. `hook-overlap.txt` and `hook-overlap-hashes.csv` bind the passing run.

An additional selected-object hook-overlap run injected a controlled exception
inside the actual graph owner's `__try`, after lock admission. Both a caller
that initially held the lock and one that did not caught the injected exception
and retained their original lock state; the hook/retirement overlap also
passed. See `owner-exception.txt` and `owner-exception-hashes.csv`. This tests
the owner's unwind boundary, not an actual native USER or guest exception.

An unchanged prebuilt Win16 guest additionally exercised a real CBT hook,
including nested creation, veto, later recovery and unhook on the published
input set; see [guest witness](../graph-owner-real-hook/review.md). It does not
force the concurrent retirement interleaving.
