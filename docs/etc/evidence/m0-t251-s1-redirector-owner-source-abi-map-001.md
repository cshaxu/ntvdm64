# M0 T251 S1 — Redirector owner/source/ABI map

## Result

The repository contains the complete **guest-side** Redirector client and
the public/private ABI declarations for the historical `VDMREDIR` component,
but it does **not** contain the `VDMREDIR` provider translation units,
including `VrDispatch`.  The OpenNT host entry in
`softpc.new/host/src/nt_bop.c` dynamically loads `VDMREDIR`, resolves nine
exports, reads the service byte with `Sim32GetVDMPointer`, and then calls the
DLL.  It is a historical product shell, not a composable bx-vdm dispatcher.

The current `bx-vdm` DEM mirrors retain explicit `ERROR_CALL_NOT_IMPLEMENTED`
named-pipe fallbacks (`LoadVdmRedir`, `Vr*NamedPipe`); they are intentional
unavailable behavior, not Redirector implementation.  T251 must replace them
only as part of a complete selected owner profile.

## Inventory and owner groups

All service numbers are defined contiguously by
`src/opennt/base/mvdm/inc/rdrsvc.h`; its `MAX_REDIR_SVC` is `0x31`.
The tracker already has one row for each entry below.  “Source body” means an
actual VDMREDIR provider body in this repository, not a prototype/header or
the 16-bit guest caller.

| Services | Original guest client / direct ABI owner | Historical provider owner | Source body in tree | Current bx-vdm route / required future owner |
| --- | --- | --- | --- | --- |
| `00` initialize, `01` uninitialize | `redir.asm` / `resident.asm`; `VrInitialize`, `VrUninitialize`, `VrInitialized` | `VDMREDIR` initialization and per-VDM state | No | Current no-provider behavior.  A session-owned Redirector lifecycle is the prerequisite for every other group. |
| `02..08` named-pipe query/state/peek/transact/call/wait | `namepipe.asm`; packed `PIPEINFO`, `AVAILDATA`, 16:16 buffers | `VrGetNamedPipe*`, `VrPeekNamedPipe`, `VrTransactNamedPipe`, `VrCallNamedPipe`, `VrWaitNamedPipe` | No | Existing DEM stream tokens are only local handles; a future Redirector named-pipe profile must bridge them through opaque IDs and checked spans. |
| `09..0F` mailslot operations and reset | `mailslot.asm` / `resident.asm`; `VR_MAILSLOT_INFO`, 16:16 buffer and PDB ownership | `Vr{Delete,GetInfo,Make,Peek,Read,Write,Terminate}Mailslot` | No | No route.  Requires a session-owned mailslot table and original-shaped PDB cleanup, not a raw `HANDLE` ABI. |
| `10..1A` remote/local LAN Manager APIs | `netapis.asm`; descriptor/selector conversion | `Vr{NetTransact,NetRemote,NetNullTransact,NetServerEnum,NetUse*,NetWksta*,NetMessageBufferSend}` | No | No route.  Requires a declared NetAPI capability/profile and 16:16 descriptor conversion; cannot be treated as host filesystem access. |
| `1B..1F`, `22`, `2B..2E` identity, assignment, service APIs | `resident.asm` / `netapis.asm`; strings and DOS assignment tables | `VrGet*Name`, `VrNetGetDCName`, `Vr{Return,Set}AssignMode`, `Vr{GetAssignListEntry,DefineMacro,BreakMacro,NetServiceControl}` | No | No route.  These are not safe “always succeed” metadata calls; their source-derived behavior follows the selected network/session profile. |
| `20..21`, `23..24` pipe-handle and asynchronous pipe APIs | `namepipe.asm`; `DOS_ASYNC_NAMED_PIPE_INFO`, `OVERLAPPED`, ANR and semaphore layouts | `VrNetHandle*`, `VrReadWriteAsyncNmPipe` plus completion queue | No | Existing local COMMAND child streams do not supply historical async completion.  Needs a bx-vdm session completion queue, checked guest writes and opaque tokens. |
| `25..27`, `2F..31` NetBIOS/DLC and interrupt acknowledgement | `dpmi/dxnetbio.asm`, `int5c.asm`; DLC window / post routine ABI | `VrCheckPmNetbiosAnr`, interrupt queue, DLC/NetBIOS provider | No | Not a named-pipe extension.  Requires an explicit network-device/interrupt capability and is not selectable in S1. |
| `28` VDM window initialization | `rdrsvc.h`; `VDM_LOAD_INFO`, `VDM_REDIR_DOS_WINDOW` | VDMREDIR/DLC window producer | No | Depends on the NetBIOS/DLC group and a declared machine/interrupt bridge; it must not be placed in bx-mantle as DOS/Redirector semantics. |
| `29..2A` return/set mode | `resident.asm`; Redirector pause/continue state | VDMREDIR session dispatcher | No | Must share the future session lifecycle/state owner with `00/01`, not become a standalone adapter flag. |

## Historical composition boundary

`nt_bop.c:MS_bop_7` keeps a process-global three-state DLL load attempt,
uses `SafeLoadLibrary("VDMREDIR")`, gets `VrDispatch` and eight auxiliary
exports with `GetProcAddress`, accesses guest memory through historical
CCPU/SAS primitives, then increments IP itself.  Direct reuse fails all of
the current composition requirements:

1. it imports the absent DLL/provider instead of a source body;
2. it embeds CCPU/SAS guest-pointer and register ownership;
3. it owns global loader state rather than the single bx-vdm session; and
4. it bypasses the existing typed mechanical BOP ingress/resume boundary.

Therefore the first usable recovery rung is not direct compilation of
`nt_bop.c`.  S2 must select the smallest source-derived session/provider seam
that preserves each guest caller's ABI, service ordering and error result;
the selected seam may use public Win32 APIs internally but must never expose
their `HANDLE`, callback or thread identity to guest memory.

## Direct cross-owner dependencies

| Dependency | Current state | T251 consequence |
| --- | --- | --- |
| `50:47/50:48` DEM pipe EOF | Imported DEM source exists; its current `Vr*` helpers decline. | Redirector owns remote/named-pipe completion; T251 must replace no-op helpers only together with a complete pipe lifecycle. |
| `54:06/08/0A/0B` COMMAND streams/lifecycle | Local session-owned child record and opaque streams exist. | Reuse the record as the local stream owner; do not create a second child broker. |
| opaque handle manager | Present for DEM/COMMAND host handles. | Redirector needs a distinct typed token kind/table, not historical split words or raw host handles. |
| NetBIOS/DLC/VDM window | No provider body or approved machine capability. | Remains an explicit profile exclusion until a whole device/interrupt package is admitted. |
| bx-core / bx-mantle | Selector-blind mechanical execution only. | No Redirector selector, service, pipe, network or DOS semantic enters either layer. |

## S1 disposition

S1 is complete as a source/ABI map: every `57:00..31` entry is covered by one
of the owner groups above, and every group has a source-body availability
result.  The next task is S2: declare one bounded Redirector capability
profile and record the four-rung disposition per owner group before changing
any current no-op shim.
