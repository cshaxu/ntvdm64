# M0 T391 — Original COMMAND cmdExec32 Stage-to-Return Recovery

## Purpose

Recover the original OpenNT COMMAND native-child lifecycle as one owner
package: BOP `54:08` enters `cmdExec`, reaches the original `cmdExec32` and
`cmdCreateProcess` worker path, returns through the Base VDM command/control
boundary, and exposes the original completion result.  The frozen first
workload is the existing source-shaped native child, `cmd.exe /c exit 37`.
The child must be requested by the DOS/COMMAND path; an app-owned launcher is
not evidence for this package.

T391/S1 proved that the default DOSX path is instead a combined DPMI/WOW
bootstrap requiring `SYSTEM.INI` and `KRNL?86.EXE`.  Under the established
pure-DOS and DOS-initiated-native-child priority, this packet therefore
continues with COMMAND rather than treating DOSX as a generic extender.

## Source and boundary

- Preserve the original bodies and order in `mvdm-host/dos/command`, in
  particular `cmdexec.c`, `cmdmisc.c`, `cmdenv.c` and their original command
  dispatcher callers.
- Existing Base VDM, Win32, session and SoftPC bindings may provide only
  same-shaped inputs, wait/completion, handles and machine transitions.
- `COMMAND.COM` guest code and its media are immutable.  No app-owned command
  parser, synthetic completion record or direct host launch is allowed.
- This packet excludes WOW bootstrap, Redirector/remote pipes, CSRSS/BaseSrv
  multi-process brokering, DOS EXEC/PSP-parent restoration and complete
  console injection.  Each remains an explicit later owner boundary.

## Work sequence

### S2 — Source/ABI rebaseline

Re-read every original stage from `54:08` through `cmdExec`, `cmdExec32`,
`cmdCreateProcess`, Base VDM re-entry and `cmdReturnExitCode`.  Produce one
source/ABI map that identifies each historical import, existing same-shaped
binding, result/CF contract, worker ownership and stop condition.  Do not run
an unfrozen product trace in this stage.

### S3 — Original stage-to-return recovery

Replace any remaining shortcut on the selected chain with the smallest
source-shaped binding.  Preserve original failure and completion ordering;
the session worker owns only the modern public-Win32 replacement for the NT4
worker/broker seam.

### S4 — No-argument COMMAND startup diagnosis and recovery

Reproduce the current no-argument `ntvdm64.exe` stop in the established
console-owning, non-debug container. Trace only far enough to name the first
complete source owner boundary: the original empty launch declaration, Base
VDM `GetNextVDMCommand` no-command disposition, COMMAND first/second-shell
transition, or a SoftPC/CPU40 owner. Recover the selected source-shaped
empty-launch behavior only when its complete owner package is bounded and
already in this packet's source/adapter perimeter. Do not change guest
`COMMAND.COM`, invent an app command parser or make an empty invocation mean
an implicit host command.

### S5 — Original PermCom initial-record recovery

All product observations and user-testable product staging for this packet use
the single short runtime root `O:\ntvdm64`.  Formal build directories remain
build-only inputs; they are never `SystemRoot`/runtime containers.  This keeps
the source-shaped DOS path contract below its fixed-size limits and prevents a
diagnostic or nested build path from becoming a false COMMAND failure.

The first `COMMAND.COM` is the original permanent shell (PermCom), not a
one-shot `/C` shell. Recover the BaseVDM startup record as one original
contract:

```
app declaration -> BaseVDM AppName/CmdLine record -> PermCom BOP 54:01
    -> DOS EXEC target -> target-owned Console behavior -> next BaseVDM request
```

`AppName` must name the target image and `CmdLine` must contain only its
argument tail with the original CR/LF termination. Explicit `command.com` is
therefore an ordinary second guest child, never a special app route or a `/C`
wrapper. A bare invocation may use a bounded child `/C` tail solely to end the
otherwise commandless session after PermCom's next original request. No guest
change, app command parser, keyboard change, synthetic BOP or direct
guest-memory write is allowed.

**S5 closure:** the record/EXEC path is proven in
[the S5 evidence](../evidence/m0-t391-s5-permcom-initial-record-recovery-001.md).
The sole product container is `O:\ntvdm64`; a nested build runtime path is
not a valid product observation.

### S6 — Formal link and local non-zero-exit lifecycle proof

Build the affected formal CPU40/x86 libraries and final product.  Add focused
tests for request validation, worker start, pending/re-entry, cancellation,
completion and the `exit 37` result without treating a direct host launch as
guest evidence.

### S7 — One fixed-container observation

Use the established console-owning container once to observe the frozen
DOS-initiated workload and its original visible completion/return boundary.
If it stops earlier, record the first complete source owner cohort rather than
patching a trace-selected leaf.

## Completion

T391 can close only when its no-argument launch has a source-shaped terminal
disposition, its explicit target has the original PermCom/AppName/CmdLine
shape, the original command chain is formally linked, and a frozen
DOS-initiated native-child workload proves the stage-to-return result. A
successful app-side `CreateProcess` alone never satisfies this plan. If a
stage stops at a complete owner outside this packet, the evidence must identify
and queue that owner before T391 can close.
