# T200 S4 — COMMAND CMDINFO native observation

## Method

One and only one source-built CPU5 guest run was executed through the S3
fixture at `artifacts/build/t200-s4-cmdinfo-native-observation-r1/`.  Its S97
record reports `runs: 1` and the fixture's controlled exit 2.

## Observation

The `54:01` diagnostic line is:

```text
geometry=1 read=1 decoded=1 ds-dx=95ab:95cb env=0ce8:0100
cmd=95ab:931b:128 exe=95ab:9467:273 accepted=0 outcome=0
bootstrap=2 launch=1 drives=1 delivered=1 reg=1
```

Thus the historical CMDINFO geometry/capacities, environment lifecycle,
launch plan, drive state, registration, and ordinary-RAM read all hold.  The
current provider declines because `delivered == 1` while the CLI plan has one
slot; this is a COMMAND return/next-command lifecycle decision, not a Bochs,
memory, ingress, or CMDINFO defect.

The original `cmdReturnExitCode` in `cmdexec.c` is BOP `54:0B`: it asks the
historical command source for the next command and returns CF set only when
one exists, otherwise clears CF and returns the terminal result.  Current
`bx_ntvdm_cmd_return_exit_code_v1_dispatch` incorrectly matches byte `0x11`
rather than `0x0B`.  The mismatch and exhausted-plan policy share one owner
package and must be repaired together.

## Follow-up

S5 owns source/ABI mapping and implementation of the complete contained
`54:0B → next-command-or-terminal → 54:01` lifecycle.  No per-BOP patch or
new guest run is admitted by this observation alone.
