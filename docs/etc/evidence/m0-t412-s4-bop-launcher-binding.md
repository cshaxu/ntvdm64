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

The staged product now provides that observation. `COMMAND.COM /c run16.exe
MEM.EXE` records outer-worker `reenter-inc`, a nested public
`run16 -> basesrv -> ntvdm` Check/reserve/prepare/Get sequence, nested child
exit zero, outer `reenter-dec`, and outer parent exit-code/disconnect. The
original native-child reporter retains the copied tail and zero child result.
The log is `O:\ntvdm64\logs\m0-t412-s4-bop-native-child-r2`.

The negative staged regression used `COMMAND.COM /c run16.exe
NOT-A-REAL-S4-PROGRAM.EXE`. The original native-child reporter recorded phase
1 value `2` (file not found), while the outer worker still recorded
`reenter-inc`, `reenter-dec` and normal parent exit cleanup. This proves the
nonzero child result reaches the COMMAND-side completion path without
stranding the worker. The log is
`O:\ntvdm64\logs\m0-t412-s4-bop-nonzero-r1`.

### Owner-visible EDIT click

The owner launched the staged three-program `run16.exe EDIT.COM` session in a
normal Console and verified that a mouse click succeeded.  The associated
startup trace is `O:\ntvdm64\logs\m0-t412-s4-visible-edit-r1`; it records the
expected `run16 -> basesrv -> ntvdm` reservation and worker-command sequence.
The click result is owner-visible acceptance, not an inference from that trace.
It deliberately does not claim keyboard input, mouse movement, button-release
pairing, or welcome-dialog handling: those remain separate S4 interaction
checks.

### Automated Console mouse delivery

The existing console-owning, non-debug observer was run without product code
changes against `O:\ntvdm64\run16.exe EDIT.COM`.  With only its default-off
presentation trace enabled, the observer reported
`console-mouse-mode-observed=yes`, input mode `0x000001b8`, and
`console-mouse-input=delivered`.  It wrote the ordinary public Console move,
left-down, drag and left-up records to `CONIN$`; the trace records the
selected original mouse queue (`stage=2`) and callback return (`stage=7`) for
each.  The result and presentation trace are retained at
`O:\ntvdm64\logs\m0-t412-s4-observer-edit-r2\result.txt` and
`O:\ntvdm64\logs\m0-t412-s4-observer-edit-r2\presentation.txt`.

This is an automated delivery proof for the three-program Console-to-SoftPC
input path.  Together with the owner-visible click above, it supports click
usability; it does not substitute a separate keyboard or all-editor-behavior
acceptance run.

### Direct interactive COMMAND remains open

Fresh bounded three-program runs of both `run16.exe COMMAND.COM` and
`run16.exe O:\ntvdm64\system32\COMMAND.COM /p` displayed `File not found`
and never reached the source-owned buffered-console-input boundary.  The
path spelling and `/p` form therefore are not the cause.  A trace-only
`COMMAND.COM /c MEM.EXE` control reached `Check`, reservation, worker prepare
and worker connection, but not its first `GetNextVDMCommand`.

The existing startup traces show repeated successful original `DEM` opens of
the same `COMMAND.COM` path after `CONFIG-DONE`: the queued initial
`BaseCheckVDM` record asks the configured default `COMMAND /p` to EXEC another
`COMMAND.COM`, rather than registering a worker with no initial task.  That
is a direct-interactive-entry topology gap, not evidence against Console
keyboard delivery or the already-proved BOP child path.  Logs are retained at
`O:\ntvdm64\logs\m0-t412-s4-observer-command-p-r1` and
`O:\ntvdm64\logs\m0-t412-s4-command-c-trace-r2`.

The original `BaseSrvCheckDOS` itself creates the first Console's DOS record,
copies the `BaseCheckVDM` payload into it and marks it
`VDM_TO_TAKE_A_COMMAND`.  Therefore a new no-initial-record registration mode
would be an invented policy and is not accepted as a workaround.  The next
repair investigation must compare the actual first `VDMINFO` fields
(`AppName`, `CmdLine`, PIF, directory, environment and stream state) with the
retained original self-`COMMAND.COM` execution route, then repair the first
divergent field while preserving that original BaseSrv lifecycle.

## Limit

This establishes the host-side replacement boundary, copied stream route and
an end-to-end staged `BOP 54:08`/DOS-parent return. Guest command redirection
remains a separate unavailable Redirector boundary.

## Follow-up

Run the staged three-program product through an interactive COMMAND session,
record the default-off original COMMAND phase trace, and verify BOP dispatch,
child completion and DOS-parent return.  Test direct CLI redirection separately
from guest COMMAND redirection; they have different owners.
