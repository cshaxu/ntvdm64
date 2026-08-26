# M0 T271 S3 P8a — COMMAND host-child capability recovery

## Scope

OpenNT `base/mvdm/dos/command/cmdexec.c` owns the historical sequence:
prepare standard handles, create suspended child, attach it, resume/wait,
obtain exit code and complete the pending COMMAND continuation. That sequence
remains in the imported source body.

P8a moves only the modern host implementation behind the reached seam to
`opennt-host/opennt_host_child_runtime.{h,c}`:

- fixed-width copied command/environment and opaque stream/job/event IDs;
- ordered `STARTUPINFO` endpoint lookup;
- ANSI `CreateProcessA` with the original source argument order;
- Job-object ownership and cancellation observation; and
- completion event plus child result/state recording.

The record has no guest pointer, BOP selector/service, CPU state or raw host
handle. `opennt-bop` now holds that declared host record and retains only
one-line wrappers at the unchanged original `cmdexec.c` call sites. The
generation counter is preserved before each record clear, matching the old
separate session counter and preventing stale pending re-entry reuse.

## Divergence

`HOST-DIV-031` records the required departure: original NTVDM temporarily
changed process-global standard handles and used product lifecycle state. The
modern capability maps the same ordered endpoints only to the child and keeps
native handles behind the existing opaque manager. It does not change the
OpenNT provider's ordering, arguments or caller-visible failure flow.

## Verification

The refreshed formal graph `build/M0-T271-S3/r003` compiled the new
`opennt-host` source and linked both lifecycle targets. The direct
`t236-s1-command-local-child-fixture.exe` exited zero outside the sandbox,
covering direct child, COMSPEC child, invalid stream token, pipe endpoint,
double completion, cancellation, re-entry generation and host standard-handle
isolation.

`t236-s2-native-cmdexec32-fixture.exe` returned 2 before the worker path:
its first machine slice did not produce `PENDING`. P8a code runs only after
that transition, so this is recorded as a native machine/dispatch prerequisite
and is not used as either pass evidence or a P8a regression claim.
