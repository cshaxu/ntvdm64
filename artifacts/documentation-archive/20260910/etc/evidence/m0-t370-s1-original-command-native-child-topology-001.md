# M0 T370 S1 — Original COMMAND native-child topology

## Question

What is the complete immediate original-owner path for COMMAND services
`54:08`, `54:0A`, and `54:0B`; which current bindings preserve it; and what
smallest whole cohort must be recovered before a native-child observation is
meaningful?

## Inputs and method

- Mirrored `src/mvdm-host/dos/command/{cmdexec,cmdmisc,cmddisp,cmdenv}.c`
  and `cmd.h`.
- The byte-identical OpenNT, OpenNT-4.5 and OpenNT-src-2 historical
  `cmdexec.c` candidates. Their three historical SHA-256 identities agree:
  `98a941095d47ae73ef9731d389a433203c7184b0a016b62e24b202bb89f829d0`.
- Current Base VDM adapter, SoftPC command guest-state/identity adapters, and
  the Win32 thread/process compatibility adapters.
- T369's formal CPU40/x86 final-input proof, which includes the original
  COMMAND archive and the three relevant binding archives.

The mirror has SHA-256
`2859f98ea6c2c94ed0ce98fa613e3ffc4654499a3164968b0d373095d43bbd3c`.
Compared with the historical file it has 35 added and 135 removed lines.
All reached changes are existing registered `MVDM-HOST-DIV-108`, `-109`,
`-111`, `-112`, and `-149` boundary changes; they do not replace the original
COMMAND worker/control-flow body.

## Original owner topology

1. `cmddisp.c` maps `SVC_CMDEXEC` (`54:08`),
   `SVC_EXECCOMSPEC32` (`54:0A`) and `SVC_RETURNEXITCODE` (`54:0B`) directly
   to original `cmdExec`, `cmdExecComspec32` and `cmdReturnExitCode`.
2. `cmdExec` obtains the command tail and ANSI environment from guest memory;
   `cmdExecComspec32` obtains `COMSPEC` and the guest environment. Both call
   original `cmdExec32`.
3. `cmdExec32` saves the two source values, blocks the original event thread,
   starts original `cmdCreateProcess`, then calls `GetNextVDMCommand` with
   `NO_PARENT_TO_WAKE | RETURN_ON_NO_COMMAND`. A returned command sets CF and
   `IsRepeatCall`; no next command resumes events and returns `dwExitCode32`
   in AL.
4. `cmdCreateProcess` increments Base VDM re-entry, reads the original packed
   `STD_HANDLES` guest record, transforms the original environment through
   `cmdXformEnvironment`, calls `CreateProcess` suspended, resumes/waits,
   records the child exit code, decrements re-entry, then exits its worker.
5. `cmdReturnExitCode` is the distinct DOS-child return route. It updates
   current directories, runs original Redirector completion ordering, makes
   the same `RETURN_ON_NO_COMMAND` query and either requests a new DOS command
   or resumes events. It does not consume native-child worker output.

## Current binding disposition

- The original C `void` worker body is compiled. `thread_start_compat` only
  bridges its historical cdecl entry to Win32's `WINAPI` callback and binds the
  owner session to the worker. This is an appropriate same-shaped seam.
- `command_process_compat` preserves the original
  `SetStdHandle`/`CreateProcess` ordering but captures the three endpoints in
  thread-local child `STARTUPINFOA` state, so it does not mutate the CLI's
  global standard handles. It uses public `CreateProcessA`.
- The command standard-handle fields resolve through the session-owned host
  resource mapping-manager instance. Guest register fields contain only a
  32-bit opaque identity, never a native `HANDLE`.
- `cmdXformEnvironment` remains the original COMMAND algorithm; it is not
  replaced by a new environment policy.
- `nt_block_event_thread`, `nt_resume_event_thread` and
  `nt_std_handle_notification` remain original SoftPC host calls. In
  particular, `nt_std_handle_notification` retains its original local
  `stdoutRedirected` update; X86GFX side effects remain under their original
  owner.

## Immediate gap and selected S2 cohort

The current one-session Base VDM binding correctly maintains the original
increment/decrement re-entry count, but it does not yet make that transition
the completion condition of a pending `RETURN_ON_NO_COMMAND` request. Its
present busy-record branch returns an immediate no-command result. That can
let `cmdExec32` consume an uninitialised/previous `dwExitCode32` while its
original worker still runs.

The historical globals `pCommand32` and `pEnv32` also receive direct guest
aliases before the detached worker starts. A guest-memory lease may not cross
that asynchronous boundary. These are one joint lifecycle defect, not two
leaf repairs.

**S2 selection:** recover the complete local native-child cohort together:

- take checked, bounded snapshots of original command/environment and packed
  standard-stream input before the worker begins;
- preserve the original `cmdExec32 → CreateThread → cmdCreateProcess →
  CreateProcess → wait → decrement → RETURN_ON_NO_COMMAND` ordering, while
  making the private snapshot worker-owned;
- extend only the source-shaped Base VDM adapter so a pending local
  `RETURN_ON_NO_COMMAND` request observes the matching re-entry decrement or
  a next command, without a synthetic guest command or CSR/CSRSS recreation;
- retain original CF/AL, `IsRepeatCall`, event block/resume, cleanup and
  child-only standard-stream behavior; and
- test cancellation/failure/completion and stale/invalid guest locations.

Redirector pipe completion, WOW broker work, multi-VDM CSR coordination and
DOS PSP/parent return are not immediate requirements of this local cohort and
remain later owner packages.

## Recovery ladder

1. **Original reuse:** selected `cmdexec.c`, `cmdmisc.c`, `cmddisp.c` and
   `cmdenv.c` are direct formal inputs; their function names, call order,
   `VDMINFO` states and CF/AL directions are retained.
2. **Smallest same-shaped seams:** existing Base VDM, session mapping,
   thread-start and child-process adapters carry only unavailable NT4
   transport, worker ABI, host identity and process-global stream mechanics.
3. **External intrusion:** none.
4. **New behavior:** none is selected. The S2 wait/snapshot state belongs to
   the existing Base VDM/session binding and must be source-shaped rather than
   a new COMMAND dispatcher.

## Result

S1 closes with a complete immediate topology and one bounded S2 cohort. This
is static/source evidence only; it does not prove that a native child has yet
been reached by the fixed product container.
