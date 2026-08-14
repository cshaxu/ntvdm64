# T200 S8 — COMMAND exhausted-plan terminal

OpenNT `cmdGetNextCmd` calls `TerminateVDM()` when `GetNextVDMCommand` has no
command.  The contained CLI has no ambient command queue, so a declared plan
whose immutable slots are all delivered now returns the existing typed stop
from the already-routed `54:01` session path.  It creates no guest resume,
register/flags delta, fake command, host process, console, or Bochs change.

Fresh MSVC x64 `/MT` boot-namespace evidence is
`artifacts/build/t200-s8-command-exhausted-stop-r1/`; it verifies the stop and
the unchanged delivered count.
