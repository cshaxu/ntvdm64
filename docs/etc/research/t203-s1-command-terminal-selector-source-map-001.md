# T203 S1 COMMAND normal terminal selector source map

## Question

Which callable COMMAND BOP represents normal DOS program return, and what
existing engine/CLI terminal path may legitimately observe it?

## Inputs and procedure

Read the original COMMAND dispatcher and handlers, then compare the current
typed session route and its tests:

- `src/opennt/base/mvdm/dos/command/cmddisp.c` and `cmdsvc.h`;
- `src/opennt/base/mvdm/dos/command/cmdexec.c:cmdReturnExitCode`;
- `src/bx-vdm/bx_ntvdm_cmd_get_next_service.c`; and
- `src/bx-vdm/bx_ntvdm_command_execution_lifecycle_v1.c`.

## Observation

`54:0B` is the callable `cmdReturnExitCode` service.  It receives the DOS
exit code in `DX`, calls the historical command broker, returns `CF=1` only
when a subsequent DOS command exists, and otherwise returns `CF=0` with the
terminal low result byte.

`54:11` is **not** a callable service.  `SVC_CMDLASTSVC` is the dispatcher
sentinel just beyond the implemented `54:00..10` table.  The current ingress
correctly rejects it before a COMMAND facade/provider is selected.  An
observation fixture searching for a normal `54:11` cannot prove normal DOS
return and must not be used to justify an engine result ABI.

The current product's `bx_ntvdm_cmd_return_exit_code_v1_dispatch` already
matches only instruction bytes `C4 C4 54 0B`, records the copied low `DX`
value in package state, and resumes.  That package-local value is not yet an
engine-to-CLI terminal result and cannot be promoted merely because a unit
test exercises it.

## Consequence

T203/S1's runtime observation target is corrected to a normal `54:0B`
terminal branch with no declared next DOS slot.  It still must prove the
following distinct chain before any ABI implementation is admitted:

```text
guest COMMAND 54:0B -> bx-vdm copied package result -> engine terminal
state -> CLI copied presentation
```

The current fixture may instead remain at the DEM frontier.  That is a valid
non-reach result and does not permit selector substitution or synthetic
normal-return injection.

## Confidence and follow-up

High: the selector boundary is an original dispatcher fact and agrees with
the current byte-locked provider/test surface.  S1 next runs one fresh,
source-built bounded observation after this correction, then decides whether
the complete chain is reachable and has a safe fixed-width owner.
