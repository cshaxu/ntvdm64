# M0 T427 system-root and application-search isolation closure

Owner accepts T427 on 2026-10-04: “本t可以收口了。” No next task is
admitted. Acceptance follows delivered S1-S5 evidence, not automated tests alone.

## Delivered scope

- S1: [source/selected-image audit](../etc/evidence/m0-t427-s1-root-search-audit.md).
- S2, 7d1bd3452: [own-image root and internal bindings](../etc/evidence/m0-t427-s2-own-image-root-bindings.md).
- S3, e33e8090c: [application-search isolation](../etc/evidence/m0-t427-s3-application-search-isolation.md).
- S4, d0c2fc1d0: [guest directory/layout integration](../etc/evidence/m0-t427-s4-guest-root-layout.md), including owner-admitted NTMON cleanup.
- S5, 62a71fd90; pushed registration cc245ca95:
  [system32 host co-location](../etc/evidence/m0-t427-s5-system32-host-colocation.md).

Each process derives the product Windows root from its own loaded EXE's
system32 parent. Internal executables, providers and original runtime media
use explicit package-relative paths. Applications use ordinary CWD/PATH and
explicit paths, without implicit package-first priority. Native Windows APIs
retain host meaning; original Win16 directory roles remain distinct. CLI,
guest EXEC and RPC acceptance are unchanged; no new root-validation RPC or
guest patch is added.

## Verification and publication

Delivered set: build/M0-T427/S5/r001/runtime, published in O:/winnt/system32:
run16.exe, ntsrv.exe, ntcon.exe, ntvdm.exe, ntvwm.exe, ntmon.exe, WOW32.DLL
and VDMREDIR.DLL. APP0.0.427/RPC38/I/O25; MSVC14.43/SDK22621/x86 /MT CCPU40.

S5/r012 passes Console17, Window17 and retained three WOW frontiers
(217386 ms). Focused root/resource/native-environment/Win16-directory/search/
handoff/staging negatives are recorded in S5 evidence. S5/r011 retains
recoverable publication and eight hashes; r013 passes eight published smoke
cases and checks all eight hashes plus 46 preserved media/config/user files.
Failed attempts remain disclosed, not passes. All S deliveries are pushed.

This documentation-only closure changes no production input, running process
or deployment and requires no rebuild/runtime rerun. Other sessions' queue/
proposal changes remain untouched and outside this delivery.

## Extra applications and retained limits

Owner identifies WINMINE/SOL/WRITE copied from Windows 3.1 as extra Win16
applications and reports placing them at O:/winnt. Same-name NT4 setup entries
do not prove these particular binaries were NT4-supplied runtime files or
require system32 placement. They remain user applications selected through
CWD/PATH or explicit paths. This closure neither moves nor replaces them.
Sealed WOW results retain tested inputs; they are not a new run after the
owner's relocation.

Root SYSTEM.INI and beside-worker NTVDM.REG ownership remain. Physical RDP/
focus, host scrollback and broader SOL/WRITE usability are not newly proved;
WOW frontiers are not full application acceptance. Known long-path, original
guest and external-provider debt remains in [TODO](../states/TODO.md).
These disclosed limits do not reopen the accepted root/search scope.

Remaining unadmitted candidates retain [Queue](../states/QUEUE.md) order.
Wait for owner direction before allocating the next numeric task.
