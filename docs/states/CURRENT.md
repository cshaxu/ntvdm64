# Project Status

## Current Work

## Active Packet

No M/T/S packet is active. M0 T420 remains open between sequential package
units; a new S requires explicit admission before implementation.

## S1 Closure Record

[T420 S1 CCPU386 package audit](../etc/evidence/m0-t420-s1-ccpu386-package-audit.md)
was delivered at `2d128cbaa`. It freezes the complete CCPU manifest and its
C-VID state boundary; S2 owns that next package without reopening CCPU
semantics.

## S2 Closure Record

[T420 S2 C-VID package recovery](../etc/evidence/m0-t420-s2-cvidc-package-recovery.md)
closed at the T419-integrated formal product revision. It records all 39
selected C-VID sources, one `Gdp` allocation owner, the profile-null vector
dispositions, focused vector/map proof, the final four-executable x86 build,
and deployed `COMMAND/MEM/EDIT` regression. T420 may next admit S3 (`video`)
only on a new owner request.

## Current Technical Baseline

- Product package: Win32/x86 CCPU40 `run16.exe`, `basesrv.exe`, `ntvdm.exe`,
  and `dtmgr.exe`; latest selected artifacts are in `O:\winnt`.
- T419 is closed with its owner-accepted post-closure DTMgr record-projection
  correction: four EXE component directories are explicit, `dtmgr.exe`
  replaces `dtaskmgr.exe`, and the task display derives depth from original
  BaseSrv DOS records rather than a parallel stack.
- T420 S1 and S2 are delivered: S1 freezes the CCPU/C-VID boundary and S2
  closes its selected C-VID package. The next package, `video`, is not yet
  admitted.

## Prior Closure

[T419 Terminal task-manager closure](../history/m0-t419-terminal-task-manager-closure.md)
records the accepted task-manager delivery and its evidence boundary.

## Recent M0 Closures

| T419 | [Terminal task manager](../history/m0-t419-terminal-task-manager-closure.md) | Delivered `dtmgr.exe`, authenticated selected-worker management, compact first-option compatibility, and explicit executable component roots. |

## Recent Governance

No standalone governance delivery is active or pending.

## Next Work

The remaining candidate order is authoritative in [Queue](QUEUE.md). T420 S3
(`softpc.new/base/video`) is the next sequential candidate, not active work.
