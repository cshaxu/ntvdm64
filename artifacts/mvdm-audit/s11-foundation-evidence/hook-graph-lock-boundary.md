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
