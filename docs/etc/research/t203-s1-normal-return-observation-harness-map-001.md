# T203 S1 normal-return observation harness map

## Question

Does an existing product or fixture entry provide a qualified way to observe
normal `54:0B` return without changing guest control flow?

## Existing entries

| Entry | What it proves | Why it cannot prove normal return |
| --- | --- | --- |
| `bx_ntvdm_engine_run_v1` | Product engine can install a copied composition, arm a minimal mantle machine, execute under a finite watchdog, and clean up. | It maps only generic controlled stop and budget to public terminal kinds; no COMMAND result crosses into it. |
| `tests/bx-mantle/bx_ntvdm_engine_direct_composition_v1_test.c` | The engine can execute the current source closure twice and preserve reset/cleanup. | It enables `bx_ntvdm_terminal_observation_v1`, which records only an already accepted STOP, and asserts that a `54:01` STOP occurs. It is explicitly not a normal-return path. |
| `tests/bx-vdm/t198_s23_native_ntio_boundary_bridge.c` | A historical diagnostic bridge can record preselected BOP state and a generic-fault snapshot. | Its fallback converts the first unhandled generic `#UD` into `STOP`; its final assertion requires that diagnostic event. It changes the observation control flow. |
| `bx_ntvdm_vdm_generic_ud_bridge_v1.c` | Product BOP composition routes bound provider outcomes without test bridge state. | It has no passive event/result recorder and therefore cannot distinguish an observed `54:0B` resume from other execution. |

## Required qualified harness boundary

The normal-return observer must use the same copied engine request,
composition installation, mantle machine lifecycle and CPU5 execution path as
`bx_ntvdm_engine_run_v1`.  It may add a **default-off, test-only copied
observation** after product generic-bridge dispatch has returned, with these
constraints:

- record at most one bounded exception window and typed outcome;
- record only an already present `C4 C4 54 0B` event and its existing resume
  or stop disposition;
- never read guest memory, parse CMDINFO, select a provider, inspect a BOP
  other than the fixed observation predicate, mutate CPU state, alter result
  data, change a timer, or turn an unhandled generic fault into a stop;
- leave product composition default-off and retain the native watchdog; and
- report normal return only if the recorded event is accepted by the existing
  COMMAND session and resumes at `fault_rip + 4` with its existing result.

The listener is not a terminal-result transport.  It would establish only the
precondition for a later, separately admitted engine/CLI copied result ABI.

## Current boundary result

The successor [passive observation record](t203-s1-passive-normal-return-observation-001.md)
now supplies the fixed-width, default-off recorder and proves its positive and
negative predicate behavior.  Its fresh source-built finite-run execution did
not reach `54:0B` before the existing `54:01` declared-plan terminal.

This does **not** make the finite-run fixture an engine-qualified harness:
it does not enter through `bx_ntvdm_engine_run_v1`'s copied request,
installation and cleanup path.  A future engine-level normal-lifecycle run is
still required before terminal-result transport can be admitted.  A different
existing controlled terminal or budget is evidence, not permission to add a
BOP handler or alter the bridge.

## Disposition

This map is an admission design only.  It adds no source, no Bochs intrusion,
no provider, no runtime trace, and no terminal-result ABI.
