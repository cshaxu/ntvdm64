# M0 T291 S4 P4 — WOW task lifecycle limit

## Source boundary

The original WOW32 package uses `CURRENTPTD()` per task and reaches far more
than the callback fields: task links, Win16 task data, native threads,
window/GDI handles, process pointers and compatibility state. Fast WOW also
uses `CurrentMonitorTeb` and x86 assembler monitor save/restore logic.

## Current implementation

`mvdm_wow_task_frame.c` deliberately projects only the fields reached by the
non-fast `CallBack16` foundation: `vpStack`, `vpCBStack`, callback-init flag
and `htask16`. It is thread-local, requires an active bound session, rejects
duplicate binding and underflow, and carries no raw native task/TEB pointers.
The callback transaction uses this projection only for its source-shaped stack
selection and restoration.

## Disposition

This is correct for one bound thread and one callback foundation, but is not a
complete original TD/TEB lifecycle. A worker, a second WOW task, fast WOW,
loaded callback stream, `CurrentMonitorTeb` or WOWEXEC notification requires
the source order and ownership of the appropriate WOW/monitor/broker package.
S5 is the earliest possible admission point; it must not turn the projection
into a process-global task record.

## Verification

The existing focused task-frame and stack-simulation Ninja fixtures must pass
on both x86 and x64 before S4 closes. They prove bind/reject, callback-stack
selection/restoration and the bounded real-mode `SETVDMSTACK → host_simulate`
interval only.
