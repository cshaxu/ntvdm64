# M0 T324 S1 — Redirector source-unit disposition ledger

## Question

Which original VDMREDIR units may participate in a bounded local Redirector
recovery, and which require a different owner or remain source-shaped
unavailable?

## Inputs and procedure

- Reviewed every source selected by `src/mvdm-host/vdmredir/sources`, plus the
  original COMMAND caller `dos/command/cmdredir.c`.
- Compared each source unit's original outbound calls with the existing
  `adapter-mvdm-host-out/redir`, SoftPC identity/mapping, session, and selected
  `opennt-host/netapi/netlib/ntstatus.c` source surfaces.
- Classified a unit only by its original control/data boundary. A public Win32
  function is not itself a claim that its asynchronous guest continuation is
  complete.

## Unit ledger

| Unit | Original role and reached boundary | Final S1 disposition | Required later owner / constraint |
| --- | --- | --- | --- |
| `vrdisp.c` | INT 2F Redirector table and register dispatch; uses bounded guest-address conversion. | Preserve original dispatcher; same-shaped SoftPC guest-span binding. | Local dispatch cohort may enable only handlers with an established completion contract. |
| `vrinit.c` | DLL initialization, VDD user hook, async completion queue and hardware interrupt routing. | Preserve original lifecycle; no local hook-success substitute. | VDD and SoftPC/ICA device-event owner. |
| `vrdll.c` | DLL entry, initialization/uninitialization and process configuration. | Preserve original lifecycle skeleton. | Module/VDD lifecycle disposition before runtime enable. |
| `vrmisc.c` | Unsupported result and termination cleanup. | Direct original helper candidate. | Must share the original named-pipe/mailslot cleanup ownership. |
| `vrputil.c` | DOS/network path and Win32-error conversion helpers. | Direct original helper candidate. | Bounded caller spans; no path-policy rewrite. |
| `vrmslot.c` | Local mailslot create/read/write/delete and DOS 16-bit handle table. | Public-Win32 plus existing same-shaped host-resource mapping. | Existing `ADAPTER-REDIR-002`; no second 16-bit handle allocator. |
| `vrnmpipe.c` | Named-pipe operations, worker threads and asynchronous request/completion records. | Public-Win32 local transport candidate; original worker/control flow remains. | Session-owned async records plus original SoftPC completion/interrupt contract. |
| `vrnetapi.c` | NetAPI/RAP operations and NTSTATUS-to-NetAPI status conversion. | `NetpNtStatusToApiStatus` is the newly mirrored original dependency; `XsNet*` remains unavailable. | No RAP/LM-session emulation or invented remote success. |
| `vrremote.c` | RAP request/response packing and SMB transaction conversion. | Retain source/failure evidence only. | Redirector remote/RAP owner; dependent on unavailable `RxpTransactSmb`. |
| `vrnetb.c` | INT 5C NetBIOS, post callback and async completion. | Retain source/failure evidence only. | NetBIOS device/network plus SoftPC interrupt owner. |
| `vrdlc5c.c` | DLC API load, adapter and event-worker control. | Retain source/failure evidence only. | DLC device/VDD owner; no dynamic-DLC success substitute. |
| `vrdlcpst.c` | DLC receive/post queue and hardware-interrupt delivery. | Retain source/failure evidence only. | DLC device plus SoftPC/ICA owner. |
| `vrdlcbuf.c` | DLC buffer-pool helpers. | Package-internal source retained, not independently enabled. | Enables only with the DLC owner package. |
| `vrdlcdbg.c` | DLC diagnostics. | Source-only diagnostic support. | DLC owner and optional debugger activation. |
| `vrdebug.c` | Redirector diagnostics and debugger commands. | Source-only diagnostic support. | Debugger owner; no runtime activation implied. |
| `cmdredir.c` | COMMAND standard-stream redirection and pipe workers. | Original source with registered session identity/thread binding. | `MVDM-HOST-DIV-020` and `-120`; this is a COMMAND caller, not a replacement Redirector provider. |
| `opennt-host/netapi/netlib/ntstatus.c` | `NetpNtStatusToApiStatus` and reverse status conversion. | Complete byte-identical original mirror. | Declaration/build binding only; it admits no NetAPI/RAP provider. |

## Observations

- The two existing Redirector identity divergences are narrow and source-shaped:
  `ADAPTER-REDIR-001` resolves original `HANDLE_FROM_WORDS` values through the
  session host-resource mapping, and `ADAPTER-REDIR-002` replaces the original
  private mailslot bitmap with that same mapping. Neither adds a provider.
- `vrnmpipe.c` is not a simple public-API wrapper. Its original asynchronous
  worker records eventually route through `VrQueueCompletionHandler`,
  `VrRaiseInterrupt` and the SoftPC/ICA lifecycle. Enabling just the pipe API
  would falsely claim guest completion.
- The selected `ntstatus.c` remains a pure algorithm/declaration dependency;
  its historical headers and RTL conversion call require a later formal
  declaration/binding review before it becomes a selected object.

## Interpretation and follow-up

S1 establishes one finite local cohort: dispatcher/utilities, COMMAND
redirection identities, mailslots, named-pipe control flow and status
conversion. It also establishes the exact exclusions: VDD/DLC/NetBIOS,
SoftPC interrupt completion and remote/RAP branches. The next admitted S may
recover the complete local cohort only after it supplies one source-shaped
asynchronous completion contract; it may not select a trace leaf or claim a
remote backend.
