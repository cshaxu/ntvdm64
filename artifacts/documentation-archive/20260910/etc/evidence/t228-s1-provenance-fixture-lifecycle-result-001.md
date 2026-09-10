# T228 S1 P27 — Explicit-Provenance Fixture Lifecycle Result

## Result

P27 closes the fixture-only lifecycle defect found while preparing P26.  The
change is limited to
`tests/bx-mantle/t228_s1_budget_terminal_position_fixture.cc`: its final
synthetic post-reset CS-transition wrap block is retained for scalar history,
but is not run when explicit provenance is compiled.  The synthetic records
would otherwise invoke the established ordinary-RAM provenance reader after
`bx_ntvdm_machine_stage_v1_reset`, when no machine is active.

No production source, Bochs exception, public ABI, host capability, guest
input, CPU behavior, BOP route, or runtime behavior changed.  This does not
define inactive-memory semantics.  The independent P1 fixture remains the
positive explicit-provenance test and exercises the ordinary-RAM copy while a
minimal machine is active.

## Verification

| Graph | Target | Result |
| --- | --- | --- |
| `build/M0-T228-S1/p26-endinit-lodcom-provenance-20260818a` | `bin/t228-s1-budget-terminal-position-fixture.exe` | Rebuilt after the test edit; exit `0` with explicit history and provenance enabled. |
| `build/M0-T228-S1/formal-ninja-20260818b` | `bin/t228-s1-budget-terminal-position-fixture.exe` | Rebuilt and ran; exit `0` in the default/scalar configuration. |

The default configuration still runs the synthetic wrap assertions.  The
explicit-provenance configuration now tests only active-machine provenance
through P1, and no longer makes an invalid post-reset read.

## Boundary

P27 only restores validity of P26's pre-existing focused verification.  It
selects no guest repair and makes no claim about guest execution, COMMAND,
NTDOS, BOP, providers, or Direct/Readonly behavior.