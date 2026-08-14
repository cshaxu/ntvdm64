# T200 S5 — COMMAND return/next-command lifecycle

## Result

The contained COMMAND lifecycle now preserves OpenNT `cmdReturnExitCode`'s
decision shape.  `54:0B` (not `54:11`) is dispatched through the COMMAND
session.  After a delivered command it resumes with CF set when an immutable
declared next slot exists; when the plan is exhausted it resumes with CF clear
and low AL copied from the DOS return code.  No host console, process, queue,
or Bochs behaviour is introduced.

The plan's optional second slot remains the CLI/profile-declared `QUIT.COM`;
this change does not invent one for a single-slot plan.  CMDINFO retains its
separate input/output contract.

## Verification

`artifacts/build/t200-s5-command-lifecycle-r2/` is a fresh MSVC x64 `/MT`
boot-namespace fixture run.  It includes the focused CMDINFO lifecycle test:
two-slot return observes CF re-entry, single-slot return observes CF clear and
terminal AL, wrong service is rejected, and duplicate return is rejected.

## Follow-up

A later packet may admit one native trace to observe the repaired lifecycle;
it is not part of this source-built package regression.
