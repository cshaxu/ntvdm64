# WOW task terminal observer: thread binding chain

The current selected worker's `obj/host/nt_thred.obj` SHA-256 is
`713243EDFC4A6CBF945CBAD1FF0B188811603151564A1872F3DB3E0546B57EF3`.
Its actual Ninja compile command forces `thread_start_compat.h` with
`OPENNT_HOST_CREATE_THREAD_ABI`; `dumpbin /symbols` identifies the unresolved
`_opennt_create_winapi_thread_named` reference. The linked worker cache input
SHA-256 is `181232BF842294B776E6677C6FFEB80E123A2B23ECFDCF4F43AFF9C69E3241D9`.
This establishes which adapter the selected original host thread object calls;
the linker and source observations alone do not assert every live thread took
this entry.

The relevant chain is `wkman.c` W32Thread creation -> `host_CreateThread` ->
the selected named WinAPI adapter -> session-aware thread thunk. The thunk
binds the creator's captured session before entering the original thread body
and uses terminal session cleanup in its `finally`. The worker attaches the
WOW session binding before admitting this work; the session hook binds a
thread-local USER runtime for each session-bound thread. A failed session
binding prevents entry to the original body.

For a successfully initialized task, `wow_user_task_lifecycle_init` assigns
its native thread and runtime context before registering a thread-specific
`WH_CALLWNDPROCRET` observer, and does so before original task admission.
Failed initialization removes installed hooks and context. Normal native
retirement refuses live owned windows, unhooks the terminal observer before
freeing task state, and runtime unbind invokes retirement before clearing the
thread-local binding. The observer retains the exact window generation and
checks the native owner thread before calling terminal cleanup.

These facts support the assumption that an ordinarily initialized task's
thread-specific observer has a session/runtime binding while installed. They
are not a proof about every foreign native callback, abnormal termination,
hook reentry, error path, or the specific historical SOL 1400. The controlled
[published-window scheduling witness](terminal-schedule-formal/review.md)
uses a session-bound resource thread, not a second real Win16 task. No product,
guest or installed image was changed by this audit.
