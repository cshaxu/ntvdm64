# M0 T373 S2 P1 — Original WOW Command Binding Dependency Closure

## Result

The S1-selected `WK32WowGetNextVdmCommand` body is recoverable as an original
mirror true subset, but it has two mandatory adjacent original contracts.  It
must not be activated as a free-standing `GetNextVDMCommand` wrapper.

1. `WK32DosWowInit` (WOW kernel ordinal 271) must first register the original
   `DOSWOWDATA`/SCS numeric locations with the existing
   `mvdm_command_guest_state` adapter.  That adapter already owns the
   mapping-manager-backed `SCS_ToSync` write required by the command-success
   path.
2. The selected WOW command body must acquire its `WOWGETNEXTVDMCOMMAND16`,
   `WOWINFO`, command, application, environment and current-directory guest
   spans under one short-lease group; it must release them after its original
   flush/free positions.  Neither `GETVDMPTR` aliases nor a host pointer may
   cross the Base VDM call.

The smallest complete S2 source cohort is therefore:

* original `wkman.c:WK32DosWowInit` and
  `wkman.c:WK32WowGetNextVdmCommand`, as two explicitly registered true
  subsets of the one original owner file;
* the existing Base VDM client/local/broker route, extended only to accept an
  app-declared copied WOW record;
* the existing `mvdm_command_guest_state` numeric scalar registry and its
  short guest-memory lease operations; and
* one private `adapter-mvdm-host-out/wow` lease group that has no provider,
  command-broker or NE-loader semantics.

## Direct dependency disposition

| Original dependency | Exact need | Selected resolution |
| --- | --- | --- |
| `GETARGPTR`, `GETVDMPTR`, `FLUSHVDMPTR`, `FREEVDMPTR` | Original body resolves five guest ranges and flushes the modified `WOWINFO`/command output. | Replace only the raw aliases with an adapter-owned scoped lease group.  The source body keeps its lookup, capacity, conversion, flush and free order. |
| `GetNextVDMCommand(VDMINFO *)` | Requests `ASKING_FOR_WOW_BINARY` and consumes exactly the WOW-owned copied record. | Existing original-name BaseClient facade plus its source-shaped Base VDM local record.  No separate command dispatcher. |
| `fSeparateWow` | Selects separate-WOW request and shared-WOWEXEC skip behavior. | Original `nt_reset.c` definition remains the selected carrier.  S2 permits shared WOW only; `ASKING_FOR_SEPWOW_BINARY` remains explicit unavailable. |
| `ExitVDM(WOWVDM, task)` | Original shared profile drops a returned `wowexec.exe` record and asks again. | Current Base VDM intentionally does not implement WOW exit or a second WOW record.  App rejects a `WOWEXEC.EXE` target for S2; no false cleanup loop is enabled. |
| `malloc_w` / `free_w` | Allocates the temporary ANSI/OEM environment buffer. | Reuse the original `wow32.c` helpers through a narrowly selected support object, or a same-shaped allocator binding if linker selection proves that body cannot be isolated.  No independent environment algorithm. |
| `ShowStartGlass` | Original non-essential UI timing presentation after successful record acquisition. | Existing same-shaped public-facing no-op preserves call order while general USER/GDI remains unselected. |
| `W32RefreshCurrentDirectories` | Sets per-drive host current-directory values and makes DOS current-directory state dirty. | Retain original per-drive environment loop.  Replace only `DosWowData.lpSCS_ToSync` raw store with `mvdm_command_guest_state_set_to_sync(0xff)`.  A missing numeric registration makes the selected caller fail rather than report success. |
| `OemToChar`, `CharToOem`, `_strupr` | Converts original returned command, app and environment data in-place. | Public/CRT x86 forms; preserve original conversion order and `windir` exception. |
| `ParseHotkeyReserved` / startup globals | Carries the original `STARTUPINFO` scalar values to later task initialization. | Retain the source-local scalar values and parser in the `wkman` subset.  S2 does not enable `WK32WOWInitTask`; S4 consumes them only if its task/NE cohort is admitted. |

## Why the adjacent ordinal is required

`WK32DosWowInit` receives `WOWDOSWOWINIT16.lpDosWowData`, which is the
original WOW kernel handoff of DOS locations.  Historical code converts those
values to durable process pointers.  The already selected COMMAND/DEM/SoftPC
adapter instead records the same SCS location as numeric real-mode data and
writes `SCS_ToSync` through a short session lease.  Reusing that adapter is
both the smallest divergence and the correct shared owner; a WOW-specific
second DOS-data map would violate the project's single mapping-manager rule.

## Explicit exclusions from S2

* `WK32WOWInitTask`, `W32Thread`, host task threads, USER `InitTask`, task
  lists, TLS/TEB product state and the `WOWEXEC` window remain S4/other WOW
  owner work.
* Fast WOW, `CurrentMonitorTeb`, `FastWOWCallbackCall`, CSR/CSRSS and kernel
  VDM stay exact-unavailable.
* General `wdos.c` directory synchronization and all USER/GDI APIs are not
  made prerequisites for a command record.  S2 only performs the original
  `SCS_ToSync` dirty-byte effect.
* S2 does not parse or load an NE image, create a host child, or run a product
  observation.

## Implementation gate

The next P may add the private lease group, the two registered source subsets,
the app's copied WOW declaration and their focused local proof.  It must
compile the mirror and `mvdm-host-overlay` as one owner library; no adapter,
app or fixture may call an overlay-private symbol directly.
