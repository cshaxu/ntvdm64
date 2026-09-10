# Historical Command Object Boundary Audit

Status: M34 audit, 2026-08-08.  This record audits the preserved NT4 command
library as a default-disabled historical input.  It neither links a runtime
nor supplies a command service replacement.

## Reproducible Compile Gate

The exact original descriptor is
`src/opennt/base/mvdm/dos/command/sources`.  It declares the `command` library
with `WIN_32` and `NTVDM`, and exactly these eleven translation units:

```text
cmd.c cmddata.c cmddisp.c cmdexec.c cmdexit.c cmdmisc.c
cmdpif.c cmdredir.c cmdconf.c cmdkeyb.c cmdenv.c
```

`src/opennt/CMakeLists.txt` preserves that same set in
`ntdos64-opennt-command-objects`, an `OBJECT EXCLUDE_FROM_ALL` target.  On
2026-08-08 the fixed x86 clang-cl configuration rebuilt all eleven objects:

```text
cmake --build artifacts/build/current/opennt-r5-clang-x86 \
  --target ntdos64-opennt-command-objects
```

The target completed successfully.  The target-local forced include,
`local/include/opennt_host_native_io_compat.h`, is declaration-only historical
native-API compatibility; it contributes no command, memory, filesystem,
console, process, or VDM operation.  The build emits modern SDK annotation and
deprecated-CRT warnings only.  No command source overlay or handler substitute
was required for this rebuild.

## Dispatch Ownership Cannot Be Trimmed

`cmddisp.c:15-32` exports the original `CmdDispatch` table.  Its seventeen
entries point to every command selector from `CMDEXITVDM` (0) through
`GETSTARTINFO` (16).  In particular, a link that retains `CmdDispatch` retains
references to native-exec, redirection, PIF, save-world, keyboard, and command
handlers as well as the first-cut services.

Therefore a smaller, replacement table is prohibited.  A future executable
must do one of the following, with the former preferred:

1. link the complete original library and satisfy its original provider
   boundaries, while admitting only the observed first-cut dynamic paths; or
2. demonstrate an original build/link mechanism that preserves the whole table
   while discarding unreachable handler sections without changing selector
   semantics.

It may not define a new `CmdDispatch`, remove table entries, or call a selected
handler as an alternative service dispatcher.

## Source Ownership Map

| Source unit | Historical owner in the table/path | First-cut status |
| --- | --- | --- |
| `cmddisp.c`, `cmddata.c` | selector table and command global state | retain unchanged |
| `cmdmisc.c` | next command, set-info, COMSPEC, current directory, console acknowledgement | required, but its raw guest pointers and BASE/session calls need later narrow bridge admission |
| `cmdconf.c` | CONFIG/AUTOEXEC service | required; historical temporary-file/PIF behavior is not admitted |
| `cmdenv.c` | initial environment | required; ambient environment enumeration/mutation is not admitted |
| `cmdkeyb.c` | keyboard-layout selector | required only for fixed `DX=0` no-install response |
| `cmdexec.c` | return exit code plus native-image/exec services | completion branch is required; binary classification, CreateProcess, shell-out/reentry are deferred |
| `cmdexit.c` | VDM termination request | required, mapped to bounded runner stop only at the original lifecycle seam |
| `cmdredir.c` | raw standard-handle/redirection services | deferred until a tokenized stream bridge has evidence |
| `cmdpif.c` | PIF file/configuration selection | deferred; no PIF discovery or host title policy in first profile |
| `cmd.c` | historical system-directory initialization | not a first-cut initialization owner; ambient system-directory discovery is excluded |

## Object-Level Import Classification

The rebuilt COFF objects were inspected with `llvm-nm --undefined-only`.  The
following groups are material; ordinary CRT and documented Win32 import-library
symbols are not a claim that their historical policy is admitted.

| Import/caller group | Original owner | Classification |
| --- | --- | --- |
| `get*/set*`, `setCF`, `GetVDMAddr` in `cmdmisc`, `cmdenv`, `cmdconf`, `cmdexec`, `cmdredir` | monitor guest-register/memory ABI | **future backend/mantle bridge**. M33 forbids exporting a raw pointer; it requires copied checked records and atomic multi-span publication. |
| `GetNextVDMCommand`, `SetVDMCurrentDirectories`, `GetEnvironmentVariableOem`, `SetEnvironmentVariableOem`, `GetDriveTypeOem` | historical Base/CSR and OEM process/path facade | **narrow host seam, not currently implementable by pass-through**. The first profile replaces its input only with immutable runner session data; it may not consult ambient process state. |
| `TerminateVDM`, `VdmExitCode`, `nt_block_event_thread`, `nt_resume_event_thread`, `nt_init_event_thread`, `cmdPushExitInConsoleBuffer`, `CntrlHandlerState` | historical event/console lifecycle | **deferred product policy**. First cut needs only bounded stop and terminal acknowledgement; it does not start a historical event thread or mutate console title/buffer state. |
| `GetPIFData`, `GetPIFConfigFiles`, `pfdata`, `cmdCheckForPIF` | original PIF host group | **deferred PIF policy**. Not a substitute for profile configuration/AUTOEXEC records. |
| `cmdCheckStandardHandles`, `cmdCheckCopyForRedirection`, `CreateFile`, `CreateThread`, `SetStdHandle` | historical redirection/pipe system | **deferred stream boundary**. No raw host handle can cross the 16/32-bit DOS transport. |
| `CreateProcess`, `GetBinaryType`, registry queries, `cmdExec32`, `cmdExecComspec32` | Windows executable handoff | **deferred native execution feature**. It is distinct from a DOS `COMMAND.COM /C` smoke test. |
| `demCloseAllPSPRecords`, `demGetPhysicalDriveType` | original DEM DOS process/device state | **retain original owner**. A contained namespace may answer only through DEM's proven future seam; it may not invent a physical-drive result. |
| `host_lpt_flush_initialize` | original printer host group | **not bootstrap-required**. Admission waits for a live printing/flush caller. |

## Consequences

1. The historical command library is compile-closed but not runtime-closed.
   Its original table is a required part of the eventual link graph, not a
   small first-cut library that can be copied into the runner.
2. M33 is a hard prerequisite for the address-bearing first-cut functions;
   direct `GetVDMAddr` pointer borrowing is not an external adapter ABI.
3. M31/M29 and this audit establish a common pattern: preserve the historical
   function and dispatch owner, but replace an obsolete host operation only at
   its own narrow data/host boundary after a backend contract exists.
4. The x86 CCPU generated-ABI block remains independent.  This audit does not
   make the historical command objects executable without a selected backend.

## Admission Result

M34 is complete.  The next design task is synthesis of the neutral core/mantle
delta evidenced by M29 (DEM read), M31 (NTDOS load), M33 (command records),
and this object audit.  It must identify the smallest shared backend contract
without referring to `NTIO.SYS`, `COMMAND.COM`, BOP selectors, SVC numbers, or
DOS packed structures.
