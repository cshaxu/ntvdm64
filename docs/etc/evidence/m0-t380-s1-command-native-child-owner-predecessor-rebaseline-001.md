# M0 T380 S1 — COMMAND native-child owner and predecessor rebaseline

## Original route

The complete original route remains in the selected COMMAND mirror:

1. `cmddisp.c` maps `54:07` to `cmdCheckBinary`, `54:08` to `cmdExec`,
   `54:0A` to `cmdExecComspec32`, and `54:0B` to `cmdReturnExitCode`.
2. Original guest NTDOS `msproc.asm` calls `54:07` for an unknown executable.
   For a non-DOS binary, original `cmdCheckBinary` rewrites the supplied guest
   EXEC request to `COMMAND.COM /z <target>`; guest COMMAND `init.asm`
   performs original `54:08` from its `/z` path.
3. Original `cmdExec` snapshots guest command/environment/standard-stream
   inputs and calls `cmdExec32`; `cmdExecComspec32` obtains host `COMSPEC`
   and follows the same worker route.
4. Original `cmdExec32` blocks events, starts the original C worker
   `cmdCreateProcess`, then requests `NO_PARENT_TO_WAKE | RETURN_ON_NO_COMMAND`.
   The worker increments re-entry, transforms its original environment,
   creates/resumes/waits for the child, saves its exit result, decrements
   re-entry, and exits. `54:0B` remains the separate original DOS-child
   return path.

## Current source/binding state

T370's original `cmdexec.c` worker and `cmdenv.c` environment transformation
remain selected formal inputs. The only reached divergences are registered
boundary replacements:

- `MVDM-HOST-DIV-196`: bounded session snapshot before the asynchronous worker
  replaces unsafe durable `GetVDMAddr` aliases while retaining CR-to-NUL input
  mutation and original worker control flow.
- `MVDM-HOST-DIV-197`: the local Base VDM record waits for the original
  re-entry interval rather than returning an immediate no-command result.
- `MVDM-HOST-DIV-109`: the C `void` worker receives a cdecl-to-WINAPI thread
  bridge only; its original body and call order remain intact.
- `MVDM-HOST-DIV-149`: original `SetStdHandle`/`CreateProcess` ordering is
  bound to child-local `STARTUPINFOA` endpoints so the CLI's own streams are
  not mutated.

The matching adapter sources are already in the formal graph:
`mvdm_command_native_child.c`, `base_vdm_local.c`,
`thread_start_compat.c`, and `command_process_compat.c`. They carry only
checked session state, opaque stream identities, C-call ABI adaptation and
public `CreateProcessA`; they are not another COMMAND dispatcher or launcher.

## Workload and immediate predecessor

The selected S2/S4 workload is an original guest COMMAND request for the
public host executable `C:\Windows\System32\cmd.exe /c exit 37`. It uses the
existing guest `$Exec → 54:07 → COMMAND /z → 54:08` route; it neither adds a
guest program nor calls host process APIs from `app`. `cmdCheckBinary` is
already a selected original COMMAND body and its first-call rejection is
preserved. The native route occurs only after original `cmdGetNextCmd` has
completed the first Base VDM record and cleared `IsFirstCall`.

T370's sole `LOADFIX.COM` observation did not reach this route because it
stopped at a later empty original `54:01` continuation. T375--T379 prove that
the current package can reach COMMAND and open declared guest executables, but
they do not prove the non-DOS classification and `/z` route. The smallest
remaining cohort is therefore the existing whole `cmdCheckBinary` plus
`54:08 → cmdExec32 → cmdCreateProcess → RETURN_ON_NO_COMMAND` lifecycle,
not a new service leaf.

## Result

S1 closes with no new production code. S2 revalidates the already imported
whole lifecycle and its focused x86 proof against the current formal graph;
S3 then creates the selected final product. Only S4 may run the one public
native-child workload.
