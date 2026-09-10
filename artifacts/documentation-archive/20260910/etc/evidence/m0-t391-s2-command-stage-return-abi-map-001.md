# M0 T391 S2 — COMMAND Stage-to-Return Source/ABI Map

## Scope and method

This is a source-only rebaseline of the selected native-child path.  It
compares the selected OpenNT `base/mvdm/dos/command/cmdexec.c` with the
`mvdm-host` mirror and follows every changed binding into its present owner.
It does not claim a guest workload has run.

The selected workload for the later S4/S5 evidence remains `cmd.exe /c exit
37`, requested from the DOS COMMAND path.  A direct app-side `CreateProcess`
is explicitly outside this map and cannot satisfy it.

## Original chain and current disposition

1. **`54:08` / `cmdExec` — adapted, source-shaped.** `cmddisp.c` retains
   `cmdExec` at the original service-table slot. `cmdExec` remains the
   original owner of binary selection and native-child entry. Its original
   raw `GetVDMAddr(DS:SI)`, `GetVDMAddr(ES:0)` and `GetVDMAddr(SS:BP)` aliases
   are replaced by `mvdm_command_native_child_capture_guest`. The adapter
   copies the bounded command, DOS MULTI_SZ environment and three 32-bit
   standard-handle identities through session mapping leases. It preserves the
   original CR-to-NUL guest write and rejects unmappable/unterminated values
   as a COMMAND failure. This is `MVDM-HOST-DIV-196`, not a command parser.

2. **`cmdExec32` worker admission — adapted, source-shaped.** The original
   block-event, shell-count, `fBlock`, thread-start and
   `RETURN_ON_NO_COMMAND` sequence is retained. `base_vdm_local_native_child_begin`
   records the original narrow launch-pending interval; duplicate launch is
   rejected with the existing carry-clear/`ERROR_BUSY` path. The local
   BaseClient counterpart retains the source `STATUS_PENDING → wait →
   ASKING_FOR_SECOND_TIME` retry order without retaining a caller `VDMINFO`
   pointer. This is `MVDM-HOST-DIV-197` plus the bounded BaseSrv local slice.

3. **`cmdCreateProcess` thread ABI — adapted, source-shaped.** The original
   void/cdecl worker body, increment-before-launch, conversion, suspended
   `CreateProcess`, resume/wait, exit-code capture, decrement and `ExitThread`
   order remain in `cmdexec.c`. `opennt_create_void_cdecl_thread_named` is
   solely a cdecl-to-WINAPI thread entry bridge which binds the existing
   session to the new host thread. This is `MVDM-HOST-DIV-109`; it does not
   introduce a second COMMAND scheduler.

4. **Standard streams — adapted, source-shaped.** Original 32-bit
   `STD_HANDLES` values are resolved by the session-owned opaque mapping
   manager, never treated as native pointers. `opennt_command_set_std_handle`
   keeps the original call sequence in a thread-local carrier, while
   `opennt_command_create_process_a` passes the values only in that child’s
   `STARTUPINFOA`. It never replaces app process-wide standard streams. This
   is `MVDM-HOST-DIV-108` and `MVDM-HOST-DIV-149`.

5. **Completion/re-entry — adapted, source-shaped.** The worker calls
   `mvdm_command_native_child_finish` before original
   `DECREMENT_REENTER_COUNT`; that adapter releases only its copied worker
   inputs. The original caller’s `GetNextVDMCommand(RETURN_ON_NO_COMMAND)`
   then determines whether it sets `CF`/`IsRepeatCall` for a next command or
   returns `AL = dwExitCode32` and resumes event/standard-stream notification.
   `cmdReturnExitCode` retains its separate DOS-child service path and its
   redirector record resolution; DOS EXEC/PSP parent restoration is not
   claimed here.

## Required S3 decisions

- Verify that every `mvdm_command_native_child_*` call has a bound active
  session on both original entry forms: `cmdExec` guest tail and
  `cmdExecComspec32` host command.
- Verify the BaseVDM pending state is released after both worker failure and
  worker completion, without manufacturing an extra command record.
- Verify child-local standard stream carriage is valid for the selected
  inherited console endpoints and rejects invalid opaque identities.
- Verify `cmdXformEnvironment` reaches the already selected original
  `BaseCreateVDMEnvironment`-shaped environment path rather than a host
  environment shortcut.

## Explicit later boundaries

- DOS `EXEC`/PSP parent restoration and ordinary DOS-child `54:0B` semantics;
- Redirector remote/named-pipe completion;
- CSRSS/BaseSrv multi-process broker and external console-record ownership;
- WOW command records and callback/re-entry; and
- console-input product behavior beyond the already proved first-shell DOS
  CON path.

These are not failures of the selected `cmdExec32` worker chain and must not
be silently supplied by a launcher or guest modification.

## S2 verification

- The formal CPU40/x86 graph at
  `build/M0-T389/S6/formal-x86-r2` was invoked through its checked-in
  `run-ninja-parallel.cmd` wrapper, which initializes the pinned VS 2022
  x86 environment once and then runs Ninja. `original-mvdm-command.lib`
  rebuilt all twelve original COMMAND translation units, including
  `cmdexec.c`, and `lib.exe` produced the archive successfully.
- `build/M0-T317/S3/x86/command_process_compat_fixture.exe` passed its
  child-only standard-stream binding contract.
- `build/M0-T302/S2/x86/base_vdm_local_fixture.exe` passed its local Base VDM
  broker contract.

The archive warnings are retained as visible historical-source diagnostics;
this packet neither suppresses them nor treats their absence as proof of
runtime behavior. These checks prove only S2’s source/ABI/build-entry
baseline. They do not run `54:08` from the product guest or prove native child
completion.
