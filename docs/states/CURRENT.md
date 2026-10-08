# Project Status

## Current Work

**No active M/T/S packet.**

## Latest closure

M0 T437 closed with portable installed-Windows-3.1 launch/path tools, S2
single-entrypoint cleanup, and S3 AMD64 PIF/hash utilities. The original Setup
protected-mode transition and the separate Win1.01 CMD orchestration migration
remain deferred; no active packet may absorb either without admission. See the
[T437 closure](../history/m0-t437-installed-win31-portable-launch-closure.md).

## Current Technical Baseline

The current ten-image release remains unchanged. T436 supplied approved,
identity-bound `KRNL386` and `WIN386` candidates plus released `MOUSE31.DRV`.
This task consumes those assets only for an existing, recoverable installed
Windows 3.1 tree; it does not publish a host release merely for tooling work.

| Architecture | Images |
| --- | --- |
| AMD64 | `run16.exe`, `ntsrv.exe`, `ntcon.exe`, `ntvwm.exe`, `ntmon.exe`, `nthook64.dll` |
| I386 | `ntvdm.exe`, `WOW32.DLL`, `VDMREDIR.DLL`, `nthook32.dll` |

## Recent M0 Closures

| Task | Outcome |
| --- | --- |
| T436 | Limited Windows 3.1 driver/setup delivery; ordinary Setup transition deferred. [Closure](../history/m0-t436-win31-setup-limited-closure.md) |
| T437 | Installed-tree portable launch and path repair. [Closure](../history/m0-t437-installed-win31-portable-launch-closure.md) |

## Recent Governance

T436 is closed at `7bd752b53`; T437 is closed after its portable installed-tree
tool delivery. Its ordinary-Setup transition recovery remains an unadmitted
queue-tail candidate.
