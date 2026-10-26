# M0 T469 S2 — COMM source disposition

## Correction search

The selected `wucomm.c` has the same SHA-256 as the local OpenNT,
OpenNT-4.5 and opennt-src-2 copies. The Windows 2000 and XP SP1 copies differ
as whole files but retain the same `FreeCommSupportResources` first-match
`break`. A full SoftPC source search finds no corresponding COMM owner or
correction. Thus no existing original correction can replace the current body.

## Operation and close boundary

The current original owner is complete for its intended Win16 contract:
`WU32OpenComm` builds the COM/LPT port object and partial-construction unwind;
state/read/write/error/event APIs operate on that owner; `WU32CloseComm` and
task loss enter the same `DeletePortTabEntry` resource path. The only real
external endpoint remains COM3 without CTS/DSR, whose original transmit
timeout is already recorded in the device-boundary witness. No project
adapter/provider or alternate cleanup owner was found.

The original close code sets `fClose`, clears the native mask and signals its
events, then waits up to 1.5 seconds for the modem thread and five seconds for
the writer. These waits are not newly introduced product retry/deadline logic;
they are part of every inspected original host revision. The writer path also
has an original timeout continuation that releases the port state after the
wait. This is a host-code risk to be retained as an original limitation, not
silently reimplemented.

## Decision

No production diff is admitted. The multi-port task-loss and close-timeout
edges are explicit inherited host limitations because no matching correction
exists. The next subtask owns a coherent real guest normal/negative/lifecycle
test group for the retained owner, and must not convert the existing mock or
controller loopback into external-device success.
