# Original hook callback and graph publication lock boundary

Source inspection identified a second entry to the shared graph publisher
that needs a directed concurrency witness. In `WU32StdHookProc`,
`wow_task_callback_enter` acquires the task's execution scope and returns
after releasing the runtime exclusive lock. `wow_user_borrow_enter` only
establishes a thread-local borrow scope. The hook then invokes
`wow_user_window_refresh_payload`; its `refresh_graph` implementation scans
the task process object table and captures required window roots without an
explicit runtime-lock acquisition in that function. Creation enrollment is
a separate call and its own lock must not be assumed to span the later graph
refresh. The original hook is dispatched only after that publication.

By contrast, native-call return enters the runtime owner before publishing;
the terminal owner now also acquires that lock around table retirement. The
existing forced terminal-schedule test holds that lock around its graph call;
the lightweight hook fixture stubs both scheduler admission and graph
publication. Neither is evidence that this actual hook entry is protected
against a concurrent resource thread. Task execution ownership and the data
lock are distinct, and the already observed terminal resource thread does not
necessarily hold a task execution scope.

This is a source-level synchronization candidate, not proof that a real hook
callback and terminal observer overlapped, nor proof that it caused the
historical 1400. Establish the actual lock state at graph entry and force a
published-window retirement at that point with the real page-domain and
registered hook path. Check same-thread reentry, exception/failed publication,
callback completion and lock restoration; review native calls and finalizer
callouts before changing lock scope. Do not hold a lock across a guest/native
callout merely to suppress the failure. No runtime or guest change was made.

## Controlled task-callback overlap

The selected-object `-CallbackGraphOverlap` fixture now runs the real task
callback scope and graph publisher against a published CCPU window. A bound
resource thread pauses after acquiring the runtime lock and before acquiring
the publication lock; the callback thread, whose callback admission has
already released the runtime lock, successfully completes
`wow_user_window_refresh_payload` while the other thread owns it. The fixture
then releases the resource thread and both retire/restore normally. Output:
`WOW_CALLBACK_GRAPH_OVERLAP errors=0 graph_succeeded=1 other_thread_held=1`.
This demonstrates that the graph publisher does not enforce the proposed data
lock on this route. It does not call the actual hook dispatcher or force a
window mutation during capture; it is not a reproduced 1400 or proof of the
historical failure. The temporary terminal-provider hook is test-only.

## Published candidate

The project-owned graph entry now acquires the existing runtime lock only when
the active page domain has a bound runtime and the caller does not already hold
it. Its `finally` restores the caller's lock ownership and last error; the
existing inactive-domain and invalid-owner paths remain in the original graph
publisher. This is not a guest or mirrored MVDM change. The controlled test
now gates the competing resource thread while it holds that lock and proves
the actual graph entry waits, acquires it and restores the callback's unlocked
state. Its first run exposed an incorrect test assumption that a still-live
native window could not be re-observed after guest ownership retirement; the
corrected test checks the guest owner entry instead and passed.

Selected `graph-snapshot`, `native-creation -CreationUnwind`, `native-nested`
and the corrected `-CallbackGraphOverlap` fixtures passed. A coherent staged
candidate also passed the real shared-worker idle/dialog witness. Outputs are
under `build/t473-s11/callback-graph-overlap-r3`, `graph-owner-graph-r1`,
`graph-owner-creation-r1`, `graph-owner-native-nested-r1` and
`graph-owner-shared-idle-dialog`. This is not full product validation or proof
of the historical 1400 cause. The coherent candidate passed Full and was
published; see its [review](graph-owner-candidate/review.md).
