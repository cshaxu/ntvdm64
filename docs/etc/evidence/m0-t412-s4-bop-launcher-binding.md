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

### Direct interactive COMMAND: original PIF predecessor

A bounded no-source-change A/B established that the direct first shell needs
the original PIF selection before `config.c` asks its first
`GetNextVDMCommand(ASKING_FOR_PIF)`. `run16.exe COMMAND.COM` resolves the
package-root `O:\ntvdm64\COMMAND.COM`; original `BaseCheckVDM` searches for a
same-root `COMMAND.PIF`. A temporary, byte-identical sidecar copied from the
retained `profiles\pure-dos\pure-dos.pif` removed the former visible `File not
found` result. The sidecar was deleted in a `finally` block after observation;
neither source nor guest media changed. The bounded run still did not reach
the source-owned buffered-console-input boundary, so this proves only the PIF
predecessor, not prompt or keyboard success. The result is retained at
`O:\ntvdm64\logs\m0-t412-s4-command-pif-sidecar-r2`.

An earlier `system32\COMMAND.PIF` A/B did not change the result, as expected:
that is not the application path selected by the positional package-root
launch. Its log is retained at
`O:\ntvdm64\logs\m0-t412-s4-command-pif-sidecar-r1`.

Fresh bounded three-program runs of both `run16.exe COMMAND.COM` and
`run16.exe O:\ntvdm64\system32\COMMAND.COM /p` without the PIF sidecar
displayed `File not found` and never reached the buffered-console-input
boundary. A trace-only `COMMAND.COM /c MEM.EXE` control reached `Check`,
reservation, worker prepare and worker connection, but not its first
`GetNextVDMCommand`.

The existing startup traces without the selected PIF show repeated successful original `DEM` opens of
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
repair must bind the retained PIF through `BaseCheckVDM` before its original
capture/dispatch—not by modifying a returned message—and then trace the
remaining prompt handoff while preserving that original BaseSrv lifecycle.

### PIF-sidecar RPC trace

The fresh, rebuilt three-program set was deployed together and observed with
the same temporary package-root sidecar.  Its BaseSrv trace records successful
launcher `Check`/reservation/prepare, worker connection, and exactly two
successful original `GetNextVDMCommand` requests.  The unchanged COMMAND
observer records the first request as `state=0005` (PIF plus DOS request) and
the second as `state=0200` with success (the source-defined
`STARTUP_INFO_RETURNED` result).  This proves that the current RPC route
delivers both the selected PIF and the first normal DOS command record; it is
not a missing-PIF or missing-command failure.

After that second result, the guest repeatedly opens and reads
`system32\COMMAND.COM` but does not reach its buffered-console-input
boundary or write Console text within the bounded observation.  The existing
Console presentation observer produced no invalidation record, consistent with
the guest not yet emitting a text update; it does not establish a presentation
failure.  The temporary PIF was removed after the run and the orphaned broker
and worker processes created by the observer timeout were explicitly cleaned
up.  Logs are retained at
`O:\ntvdm64\logs\m0-t412-s4-command-pif-rpc-trace-r3` and
`O:\ntvdm64\logs\m0-t412-s4-command-pif-console-trace-r4`.

## Limit

This establishes the host-side replacement boundary, copied stream route and
an end-to-end staged `BOP 54:08`/DOS-parent return. Guest command redirection
remains a separate unavailable Redirector boundary.

## Follow-up

Run the staged three-program product through an interactive COMMAND session,
record the default-off original COMMAND phase trace, and verify BOP dispatch,
child completion and DOS-parent return.  Test direct CLI redirection separately
from guest COMMAND redirection; they have different owners.
