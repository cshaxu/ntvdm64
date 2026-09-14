# T412 S4 BOP launcher binding

## Question

When original COMMAND handles guest `BOP 54:08`, can its retained
`COMSPEC /c` native-child path re-enter the three-program product through the
public `run16` boundary rather than treating the worker as a second launcher?

## Inputs

- OpenNT `cmdexec.c` retains ownership of guest capture, COMSPEC command-tail
  construction, native-child wait, exit-code return and reentry calls.
- `run16` is the admitted public executable owner of image resolution,
  BaseSrv startup and worker reservation/registration.
- `basesrv` is a broker only: it does not classify an image.

## Recovery

`command_process_compat.c` recognizes only the exact COMSPEC `/c` shape that
the unchanged original COMMAND worker has already constructed.  It replaces
the historical system-VDM spawn with the sibling `run16.exe` path and forwards
the copied tail unchanged.  It does not parse the tail, classify an image, or
carry guest memory, a mapping token or a HANDLE across the seam.  All other
child launches retain the original public `CreateProcessA` route.

## Verification

### Procedure and observations

The focused x86 fixture derives its command line from the real `COMSPEC`, uses
the exact `/c` form, and places a copy of itself at the sibling `run16.exe`
path.  The child marker `COMMAND-CHILD-STREAM` returned through the explicitly
supplied standard-output pipe; the parent reported:

```text
PASS: child-only COMMAND standard-stream binding
```

The x86 three-program product also relinked `ntvdm.exe` with the changed
adapter binding.  The S4 BaseSrv reentry proof remains valid: increment and
decrement calls use the original BaseClient/BaseSrv route over authenticated
IPC.

### Interpretation and confidence

High confidence for the finite host boundary: the fixture proves the exact
recognition form, sibling-launcher selection, copied tail and standard-stream
binding.  Medium confidence for full guest behavior until a COMMAND session
reaches the original BOP and records its normal parent return.

## Limit

This establishes the host-side replacement boundary and copied stream route.
It is not yet an end-to-end guest observation of `BOP 54:08`, guest command
redirection, or DOS parent return.  Those are still required S4 acceptance
tests with COMMAND/MEM/EDIT.

## Follow-up

Run the staged three-program product through an interactive COMMAND session,
record the default-off original COMMAND phase trace, and verify BOP dispatch,
child completion and DOS-parent return.  Test direct CLI redirection separately
from guest COMMAND redirection; they have different owners.
