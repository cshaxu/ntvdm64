# M0 T381 S1 — Original `cmdExec32` stage-to-return ledger

## Rebased source path

The selected current product retains this complete original owner path:

1. Immutable guest COMMAND parses the initial declared DOS command.  For a
   non-DOS image, original NTDOS invokes `54:07`; original
   `mvdm-host/dos/command/cmdexec.c:cmdCheckBinary` creates the original
   `COMMAND.COM /z` re-entry command and carries the DOS parameter tail.
2. Immutable guest `command/init.asm:CheckZSwitch` reaches original `54:08`
   with the command tail at `DS:SI`, environment at `ES:0`, and standard
   handles at `SS:BP`.
3. Original `cmdExec` captures precisely those inputs, then original
   `cmdExec32` blocks events, begins the Base VDM pending interval, creates
   original `cmdCreateProcess`, and issues `RETURN_ON_NO_COMMAND`.
4. Original `cmdCreateProcess` increments the re-entry count, performs the
   original environment conversion and source-ordered standard-handle and
   `CreateProcess` calls, resumes/waits for the child, saves its exit code,
   decrements re-entry and exits the worker.
5. Original BaseClient retry then returns to `cmdExec32`; only that original
   caller releases the event block and returns `AL=dwExitCode32` to guest
   COMMAND.  `54:0B` is separate DOS-child return ownership and is excluded.

## Current binding audit

The formal CPU40/x86 manifest selects the original `cmdexec.c`, `cmdenv.c`,
Base VDM client/local source, native-child snapshot, cdecl worker bridge and
child-only process binding.  The active source changes are the four already
registered boundary divergences:

- `MVDM-HOST-DIV-196`: session snapshot replaces worker-lifetime raw guest
  aliases while retaining the original command, environment and handle data
  flow;
- `MVDM-HOST-DIV-109`: original void/cdecl worker receives only a Win32
  thread-entry ABI bridge;
- `MVDM-HOST-DIV-149`: original `SetStdHandle`/`CreateProcess` ordering binds
  child-local endpoints rather than mutating the app process streams; and
- `MVDM-HOST-DIV-197`: local Base VDM retains the original pending,
  increment/decrement and retry interval without CSRSS wait-handle transport.

No app launcher, alternate executor, BOP leaf provider or guest-media change
is in this path.  Worker thread binding is also source-shaped: the thread
bridge binds the creating session, and the registered Base VDM thread hook
binds the same local Base VDM record before `cmdCreateProcess` calls the
original `GetNextVDMCommand(INCREMENT_REENTER_COUNT)`.

## T380 runtime correlation

The fixed product reached `54:07`, successfully DEM-opened
`C:\WINDOWS\SYSTEM32\CMD.EXE`, and entered `54:08` stage 0.  Its console
contained the public Windows command interpreter banner and prompt, which is
strong evidence that a command interpreter was created.  It did not produce
the selected `/c exit 37` completion, a `54:08` return, `54:0A`, `54:0B`, or
an exit-result marker before timeout.

That evidence does **not** justify a trace-local fix.  It instead shows that
the complete input-and-return cohort has not been proved end-to-end: the
original guest `/z` tail, `cmdExec` snapshot, worker `CreateProcess` payload,
Base VDM retry and final original return must be treated as one contract.

## Four-rung disposition and S2 cohort

1. **Direct original reuse:** selected for guest `/z`, `cmdexec.c`,
   `cmdenv.c` and Base VDM client control flow.
2. **Same-shaped bindings:** selected only for the four recorded NT4-to-modern
   mechanics above; no additional provider has been authorized.
3. **External intrusion:** rejected; no CPU, guest or third-party source
   change is needed to inspect or prove this cohort.
4. **New behavior:** rejected; neither a launcher nor a synthetic child
   result can replace the original route.

S2 therefore owns a complete source-level payload/re-entry proof and any
necessary repair of the existing four binding families as one cohort.  It must
exercise original `cmdExec` through worker completion, verify exact command
and argument preservation plus `AL=37`, and cover failure/cancellation and
standard-handle isolation before another guest observation is eligible.
