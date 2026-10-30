# Native-return error discrimination

The retained `publish-native-return` failure reports error 1400 but not which
publication substage or HWND failed. `publish_guest_return` short-circuits in
order through native window/payload refresh (and returned-menu refresh where
applicable), borrowed client graph, then all menu client views. The latter
independently calls `GetWindowInfo` for enrolled local windows. Consequently
the error alone cannot choose among those owners. Treating every 1400 as an
external-window race, swallowing it, or changing required-root behavior is
not supported by this evidence.

This statement applies to the uninstrumented failure, not to every 1400
observation. The earlier [two-task ownership diagnostic](../../s11-dialog-performance/two-task-ownership-nonpass/)
captured a concrete 1400 interleaving: one thread copied a mandatory root while
owning the USER execution lock, and another retired that exact generation
without that lock before capture. The table and page-domain slot were empty at
failure. That proves a synchronization gap in that diagnostic candidate; it
does not prove the later uninstrumented SOL/Winmine failure shared the same
cause or that the current candidate eliminates every such interleaving. The
current terminal changes acquire runtime ownership for retirement, but release
it around original finalizers; their full concurrency proof remains open.

Current-source owner review: `wow_user_runtime_unbind` calls the registered
`retire_current_thread` before clearing thread-local runtime binding; that
retirement refuses a task with a surviving owned window. The terminal observer
also checks the native thread identity before invoking terminal retirement.
These are useful owner invariants, not proof that all paths have a bound
runtime: `wow_window_words_terminal` has metadata-only and pending-collection
entrances, and conditionally acquires the runtime lock only when a binding is
present. Furthermore, `retire_current_thread` releases the runtime lock
before scanning the table for remaining windows. Whether any relevant writer
can actually interleave there remains unproved; this is an audit question,
not a reproduced additional race. Any fix must preserve the original finalizer
callout protocol and cannot merely treat a dead mandatory root as success.

The unlocked scan has a concrete possible writer family worth distinguishing:
`wow_window_procedure_adopt_native` can select another live task by the native
window's thread ID and publish ownership while holding the runtime lock; it
rejects an owner already marked `WOW_TASK_TIF_INCLEANUP`. Terminal preparation
sets that flag under the lock before destroying windows. The nonterminal
unbind path does not itself set that flag before the unlocked scan. This
source relationship alone does not prove a real native window survives to be
adopted at that moment or that the subsequent `retire_task` permits an orphan.
Moving only the scan under the lock would still leave a gap before `retire_task`
reacquires it, so it is not a demonstrated complete correction. A proper
decision needs the actual original cleanup/retirement ordering and a controlled
two-task witness, including ownership and failure semantics, before changing
the protocol.

There is a separate ambiguity: an error value of 1400 in a general window
trace is not necessarily a failure of that window operation. The selected
`native-nested` fixture explicitly checks a dead native target and receiver
exit where 1400 is the native call's preserved error while execution ownership
is restored. Its [output](../../s11-dialog-performance/idle-shared-regression/selected-lifecycles/native-nested.stderr.txt)
is a controlled positive error-propagation test, not the historical failure.
Only a checked failure return from a specific publication operation, with its
captured error and relevant exact-generation identity, can attribute a new
occurrence. Capture must preserve existing return and error semantics.

The current [graph snapshot test](graph-snapshot/) covers a dead required
root, later withdrawal, and slot reuse, but does not inject a dead enrolled
window specifically between the graph transaction and menu-client refresh.
The source shows that distinct path could return the same error; this is not
proof it occurred in Winmine. A useful next controlled test must distinguish
those two entrances and observe ownership and cleanup on each, rather than
turning either error into success. No runtime or guest bytes changed here.

A controlled selected-object negative now creates and destroys a native window, then deliberately leaves its identity in an isolated test object table before calling actual menu-client refresh. That distinct entrance fails with 1400, and the existing graph-root negative independently passes in the same run. The marker is required by the graph-snapshot verifier. [Results and hashes](menu-dead-enrollment/) bind the fixture; the synthetic stale enrollment does not establish that production creates it, nor attribute the historical Winmine failure.
