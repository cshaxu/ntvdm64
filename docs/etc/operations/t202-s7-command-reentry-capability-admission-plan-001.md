# T202 S7 COMMAND host-child re-entry capability admission

## Scope

S7 treats `cmdExec32`, `cmdReturnExitCode`, `cmdGetNextCmd` and their caller
state as one historical lifecycle contract.  It determines whether a bounded
CLI-owned typed re-entry queue can preserve the source's “new DOS command”
branch without importing CCPU/SAS, VDM broker, event thread, VDD or raw
handles.

## Required source map

The admission record must cover all of the following together:

- `cmdExec32`'s increment/decrement re-entry count, `RETURN_ON_NO_COMMAND`,
  `CmdSize` and CF/AL branches;
- `cmdReturnExitCode`'s DOS exit and redirection-completion ordering;
- `cmdGetNextCmd`/CMDINFO registration and declared launch-plan ownership;
- lifecycle cancellation, child completion, queue exhaustion and teardown;
- the exact disposition of pipe/Redirector data, which remains outside this
  package; and
- direct, readonly, overlay and virtual profile outcomes.

## Admission bar

No code may enqueue a command until the record states a fixed-width request,
result and cancellation ABI, the one owner of each queue state, source-shaped
failure/terminal forms, and regressions for stale, duplicate, cross-session
and cancelled entries.  The queue may contain only copied declared DOS launch
records; it cannot store guest pointers, `HANDLE`s, callback addresses, C++
objects, Bochs state or ambient process resources.

If the source comparison proves that a profile cannot supply an equivalent
queue, it must retain the current typed terminal result.  S7 may not invent
an interactive session, pipe bridge or generic scheduler to make the branch
appear successful.
