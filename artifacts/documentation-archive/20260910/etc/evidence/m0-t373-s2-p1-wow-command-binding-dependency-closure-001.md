# M0 T373 S2 P1 — Original WOW Command Binding Dependency Closure

## Result

The S1-selected `WK32WowGetNextVdmCommand` body is recoverable as an original
mirror true subset, but it has three mandatory adjacent original contracts. It
must not be activated as a free-standing `GetNextVDMCommand` wrapper or called
directly from app/an adapter.

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
3. Original SoftPC `MS_bop_1` remains the only selected ingress. It dynamically
   loads `WOW32`, resolves `W32Init`, `W32Dispatch`, `GetCommHandle`,
   `GetCommShadowMSR` and `W32HungAppNotifyThread`, invokes `W32Init` once,
   then invokes `W32Dispatch` under the original CPU40
   `GLOBAL_SimulateContext` preservation sequence. The selected cohort must
   therefore be a source-shaped `WOW32.dll` sidecar with those exports, not a
   static replacement route.

The smallest complete S2 source cohort is therefore:

* original `wkman.c:WK32DosWowInit` and
  `wkman.c:WK32WowGetNextVdmCommand`, as two explicitly registered true
  subsets of the one original owner file;
* the original `nt_bop.c:MS_bop_1` dynamic-load/resolve/init/dispatch
  contract, plus a selected source-shaped `WOW32.dll` export cohort;
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
| `MS_bop_1` / `SafeLoadLibrary("WOW32")` | Historical SoftPC owns WOW provider admission, export lookup, one-time initialization and CPU40 dispatch-context preservation. | Retain the original dynamic-DLL boundary. Build and stage a selected source-shaped `WOW32.dll`; neither app nor an adapter may call `WK32WowGetNextVdmCommand` directly. |
| `W32Init` | The original full initializer reaches private USER/GDI, shared task-list, registry and hung-app product shells before dispatch. | Select only the exact original initialization needed by the admitted non-GUI subset, with every unavailable private product-shell edge retaining a same-shaped explicit failure. Do not make a replacement W32 provider. |
| `W32Dispatch` | The original dispatcher reads the current VDM frame, establishes current task state, dispatches by original call ID, writes AX:DX and restores VDMSTACK. | Preserve original dispatcher ordering in the selected DLL cohort. The only admitted pointer divergence is a bounded mapping-manager lease at raw VDM-span access; task/USER/GDI thunk families remain unavailable until their owner cohort is admitted. |

## Why the adjacent ordinal is required

`WK32DosWowInit` receives `WOWDOSWOWINIT16.lpDosWowData`, which is the
original WOW kernel handoff of DOS locations.  Historical code converts those
values to durable process pointers.  The already selected COMMAND/DEM/SoftPC
adapter instead records the same SCS location as numeric real-mode data and
writes `SCS_ToSync` through a short session lease.  Reusing that adapter is
both the smallest divergence and the correct shared owner; a WOW-specific
second DOS-data map would violate the project's single mapping-manager rule.

## Original SoftPC dynamic ingress correction

The prior P1 wording described the command body and its adjacent data contract
but did not record the earlier original host ingress. `nt_bop.c:MS_bop_1` is
already selected into the CPU40 product and does not statically bind a command
dispatcher: it loads `WOW32.dll`, resolves five exports, calls `W32Init`, then
dispatches through `W32Dispatch`. In the CPU40 branch it also preserves and
restores `GLOBAL_SimulateContext` and aborts the current fragment on a detected
stack switch. A direct static call would bypass this source-owned lifecycle and
would be a new parallel provider.

Accordingly, P2 starts with an export/dependency selection for an original
source-shaped `WOW32.dll`. It may not claim that the full historical
`wow32/sources` DLL is modern-linkable: its private USER/GDI, shared-memory,
task-thread, registry and hung-application shells are reviewed one by one.
The DLL selection must preserve `MS_bop_1`'s names, lookup failures, one-time
initialization and CPU40 call order. A currently unavailable selected export
returns its exact source-shaped failure through that boundary; it is never
silently replaced by a direct Base VDM call.

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
the app's copied WOW declaration, the selected `WOW32.dll` export cohort and
their focused local proof. It must compile the mirror and `mvdm-host-overlay`
as one owner DLL/library selection; no adapter, app or fixture may call an
overlay-private symbol directly. The proof begins at `MS_bop_1`'s dynamic
boundary, not with a direct call to a WOW command body.
