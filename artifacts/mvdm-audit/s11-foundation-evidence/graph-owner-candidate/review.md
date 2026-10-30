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

A further [shared-worker witness](../graph-owner-shared-cbt/review.md) ran the
same real CBT guest alongside a second active Win16 task, whose idle callback
continued after the hook task retired. It does not deterministically force
their window operations to overlap.

The [Winmine/CBT shared-worker witness](../graph-owner-mine-cbt/review.md)
establishes that a real game window remains live while a real guest CBT hook
performs its nested creation/veto/unhook sequence; gameplay still works after
the hook task retires. It does not force the original race.

The adjacent [writer lock audit](../graph-writer-lock-audit/review.md) records
the directly identified creation, rollback, adoption, retirement and menu
publication paths, along with their bounded selected-object evidence.

The [prebuilt DPMI task-cleanup witness](../graph-owner-dpmi-cleanup/review.md)
also passed against these release inputs, including four child exits with live
allocations and restored aggregate XMS capacity. Failure paths remain distinct.

## Graph refresh immediately before competing retirement

The terminal-schedule fixture now places a second gate immediately before the
resource thread's runtime-lock acquisition. In the ordinary and callback
variants, the main thread still holds the runtime lock, releases that gate,
waits until the resource thread reaches the acquisition boundary, and executes
an actual graph refresh before releasing its lock. The test asserts that the
published root is still present and no retirement mutation has run. The hook
variant retains its original callback-admission ordering, then checks the same
graph-before-retirement state; it does not claim that the competitor had
passed its gate during that preliminary refresh.

The final fixture passed the plain terminal, callback-overlap and original
hook-overlap verifier modes, including the existing owner/exception checks.
All three produced `WOW_TERMINAL_GRAPH_BEFORE_RETIRE errors=0 root_live=1
no_mutation=1`. Fixture EXE SHA-256 values are respectively
`96788BE38CE30B671309EF97586D9336781EB675FA5E689639C980EB11247157`,
`A4AA716DE4F52BF52198984AB2452542D589732E5F908473D39931BE9F7953B1`,
`15638835A49051B16E83EA130DFD49C6C7A514F56A53495E3DC0FBACD821D68F`.
The test header SHA-256 is
`0996BF84B66AD12802F11E4659A911877C3018CE80E23CDD6438A85A7E78C213`;
the verifier SHA-256 is
`FD9697934AA25E96B807CDDDE74CB216D241BFC1B27EB09ED8437BDB8AE0528E`.
Outputs are in `build/t473-s11/terminal-graph-{plain,callback-final,hook-fixed}-20261030`.

An initial ordinary-sandbox run exited before creating its test desktop; it
is not counted. A first hook variant was interrupted after its test-only gate
was released before callback admission, which could deadlock against the
paused resource thread. The corrected hook variant keeps the prior ordering.
No product or guest code changed. The gate proves the competitor is poised at
lock acquisition, not that the OS has already parked it on that lock; this
is selected-object evidence, not a real Win16 forced race or attribution of
the historical SOL failure.
