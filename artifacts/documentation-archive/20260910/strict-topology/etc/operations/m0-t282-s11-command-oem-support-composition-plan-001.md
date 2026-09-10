# M0 T282 S11 — COMMAND OEM support composition

Recover the original OEM environment/current-directory helper bodies required
by the next COMMAND environment group before altering any COMMAND mirror
caller. The target is the original `oemuni/process.c` and `oemuni/file.c`
paths reached by `cmdComSpec`, `cmdGetCurrentDir`, `cmdSetDirectories` and
the `cmdenv.c` environment algorithm.

## Objective

Establish whether the selected original helper bodies compose with the
existing same-shaped adapter-win32 RTL facade, then add only missing
source-shaped public-Win32 bindings needed for their reached code paths.

## Non-goals

No COMMAND mirror modification, BOP route, synthesized environment algorithm,
raw guest pointer, session policy change, CCPU executor, broker/WOW support or
Bochs change.

## Exit criteria

The reached original OEM helper set has a verified x86/x64 source-composition
result and every unresolved import has one exact adapter-win32 owner or a
source-proven later-owner disposition. Only then may the three S9 COMMAND
pointer call sites acquire their bounded leases.
