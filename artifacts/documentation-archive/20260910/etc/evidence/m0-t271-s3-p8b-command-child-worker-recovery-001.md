# M0 T271 S3 P8b — COMMAND host-child worker recovery

P8a established the host-owned child record and process capability. P8b moves
the rest of its mechanical lifecycle into the same `opennt-host` capability:

- manual completion-event creation and opaque publication;
- thread creation and opaque worker-token publication;
- nonblocking completion poll, worker/event release and re-entry predicate;
- Job cancellation, worker wait and owned-token retirement on dispose.

The sole COMMAND-side thread procedure is now a source-body callback: it
binds neutral copied session input, supplies copied command/environment text
to the original `cmdCreateProcess`, then unbinds. It receives an explicit
session context, so `opennt-host` neither knows BOP nor depends on a
selector/service. `cmdexec.c` still determines when that callback is started,
when a pending result is returned and when its original post-worker sequence
resumes.

The direct formal fixture
`build/M0-T271-S3/r003/bin/t236-s1-command-local-child-fixture.exe` exited
zero outside the sandbox after the extraction. It covers direct and COMSPEC
children, pending/re-entry, completion consumption, opaque stream endpoints,
pipe I/O, invalid stream failure, cancellation and teardown. The P8a-recorded
native machine PENDING prerequisite remains separate and is not relabeled by
this packet.
