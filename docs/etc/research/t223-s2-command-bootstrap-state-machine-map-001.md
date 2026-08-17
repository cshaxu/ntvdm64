# T223 S2 COMMAND Bootstrap State-Machine Map

## Question

Which `cmdGetNextCmd` state transitions belong to the admitted CLI COMMAND
bootstrap/environment/config package, and what source-shaped disposition is
required where the original NT host composition cannot be reused?

## Source and current inputs

- Original owner: `src/opennt/base/mvdm/dos/command/cmdmisc.c`,
  `cmdGetNextCmd`; `cmdenv.c`, `cmdGetInitEnvironment`; and `cmdsvc.h`.
- Current recovered state: `bx_ntvdm_cmd_get_next_state_v1`,
  `bx_ntvdm_cmd_comspec_bootstrap_v1`, and
  `bx_ntvdm_command_package_session_v1`.

## State map

| Original transition | CLI-recoverable contract | Current status | Required disposition |
| --- | --- | --- | --- |
| First `54:0F` then first `54:01` | copied environment bytes and CMDINFO guest segments; no retained guest pointer | partially represented by COMSPEC stages and `delivered==0` | preserve ordering and capacity retry |
| Environment too small | `AX=required bytes`, `CF=1`, no delivery | represented | regression must prove retry then delivery |
| Command delivery | checked writes of UCOMBUF, executable path, CMDINFO fields, and registered SCS bytes | partially represented | audit every written field and no registration write before `54:05` |
| Returned child / repeat call | original uses BASE, redirection, event thread and environment merge | not composable as-is | later launch/lifecycle component; bootstrap may not invent it |
| No next command | original terminates VDM after `GetNextVDMCommand` fails | CLI has bounded declared plan | controlled terminal result only after package route, with recordable reason |
| WOW, PIF, console, redirection | historical VDD/BASE/console integration | outside T223 | deferred or explicit unavailable by their owner package |

## Consequence

T223 must not claim a general `cmdGetNextCmd` replacement.  Its closure is the
contained first-command contract plus explicit capacity retry and bounded
terminal behavior under Direct/Readonly.  Child return, BASE command supply,
redirection, console and WOW remain separately owned; none may silently be
simulated by the bootstrap state.

## Next implementation unit

Add one focused package regression that drives `54:02 -> 54:0F -> 54:05 ->
54:01`, first through insufficient environment capacity and then successful
delivery, under both Direct and Readonly contexts.  It must also prove that
the command provider publishes no host fallback or raw guest pointer.
