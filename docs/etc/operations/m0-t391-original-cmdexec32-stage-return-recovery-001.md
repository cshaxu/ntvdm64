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

### S4 — Formal link and local lifecycle proof

Build the affected formal CPU40/x86 libraries and final product.  Add focused
tests for request validation, worker start, pending/re-entry, cancellation,
completion and the `exit 37` result without treating a direct host launch as
guest evidence.

### S5 — One fixed-container observation

Use the established console-owning container once to observe the frozen
DOS-initiated workload and its original visible completion/return boundary.
If it stops earlier, record the first complete source owner cohort rather than
patching a trace-selected leaf.

## Completion

T391 can close only when the original command chain is formally linked and a
frozen DOS-initiated native-child workload proves the stage-to-return result,
or when a source-owned boundary is demonstrated with the complete owner,
original contract and a separately queued recovery package.  A successful
app-side `CreateProcess` alone never satisfies this plan.
