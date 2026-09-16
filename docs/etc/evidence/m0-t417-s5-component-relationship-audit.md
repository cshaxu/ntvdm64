# M0 T417 S5 — component relationship audit

## Scope and method

This is the required non-mutating final phase of T417.  It audits the current
formal x86 graph at `build/M0-T417/S4/formal-x86-003`, the selected source
headers and all direct source references below `src/app`, `src/broker`,
`src/session`, `src/adapter-opennt-host` and `src/adapter-mvdm-host-out`.
It does **not** infer an active provider merely because an archive is linked.
In particular, the audit checked direct callers of the legacy `broker_*`
exports and found definitions only: there is no production source caller.

The strict mirror rule already adopted during T417 applies to every proposed
change: neither `src/mvdm` nor `src/opennt-host` may receive a new file.  An
existing mirror may take only a minimal original-owner-local divergence.  A
modern mechanism stays in a named adapter/ABI component.

## Actual product composition

| Product / direction | Caller → callee | Process boundary | State and cleanup owner | Original owner |
| --- | --- | --- | --- | --- |
| `run16.exe` | `run16_entry.c` → BaseClient classifier, `BaseCheckVDM`, configuration and broker RPC client | launcher → `basesrv.exe`; launcher creates `ntvdm.exe` suspended | run16 owns discovery, its environment projection, suspended-child rollback and parent wait; BaseSrv owns published record | BaseClient / `BaseCheckVDM` |
| `basesrv.exe` | `basesrv_entry.c` RPC handlers → `OpenNtBaseService*` | RPC server; receives authenticated launcher/worker calls | BaseSrv service owns Console-record selection, record lock/queue, reservations and worker cleanup; app entry owns endpoint lifetime and the empty-service timer | `srvvdm.c` BaseSrv policy |
| `ntvdm.exe` | original `ntvdm.c`/SoftPC → BaseClient RPC, session and external bindings | worker → `basesrv.exe` | worker owns its process-local capture heap, session, SoftPC thread binding and final process exit; service owns command reply and parent completion semantics | original MVDM/SoftPC worker plus BaseClient |
| Command path | original BaseClient caller → `adapter-opennt-host` RPC codec → `basesrv_entry` → original `srvvdm.c` owner | worker/launcher ↔ broker | copied RPC buffers are adapter-owned only until call return; BaseSrv records and events remain server-owned | BaseClient/BaseSrv |
| Machine path | original SoftPC/MVDM caller → `adapter-mvdm-host-out` family → modern Win32/session facility | in worker only | session owns per-worker lifecycle, guest-memory lease and teardown registration; a family owns only its finite external handle/translation | selected MVDM/SoftPC caller |

The formal graph confirms these link directions: `run16.exe` links
`opennt-base-client.lib`, `opennt-base-bindings.lib` and
`broker-transport.lib`; `basesrv.exe` links the server equivalent; `ntvdm.exe`
links original MVDM/SoftPC archives first, then the named adapter archives,
`session.lib`, `broker.lib` and `broker-transport.lib`.  This link presence is
not a license for either adapter to own original policy.

## Component ledger

| Component / current size | Public or cross-component surface | Correct present boundary | Audit finding and disposition |
| --- | --- | --- | --- |
| `app` — 8 source/header files, 1,316 lines | executable entry, sibling/package paths, Console query, shared version | product-only process creation, singleton startup and endpoint hosting | Keep. `run16` is the CreateProcess CLI realization and `basesrv_entry` is the process host.  Neither may absorb image classification, record selection or worker semantics from OpenNT.  The current entry comments that call this integration “WIP” are stale documentation, not an alternate provider. |
| `broker` — 22 files, 1,072 lines | RPC IDL, authentication, connection receipt/delivery/payload/stream mechanics; legacy in-process wire API | cross-process transport and only transport | Split disposition. `rpc_security`, `console_membership`, `vdm_receipt`, `vdm_delivery`, `vdm_payload`, `vdm_message` and `service.idl` are required transport mechanics.  `broker.c/.h`, `wire.c/.h`, and `base_vdm_record.c/.h` are a 387-line legacy local state plane with no direct production source caller.  It is linked as `broker.lib` but should be proved removable in a separate package, never repurposed as a second BaseSrv. |
| `session` — 4 files, 786 lines | worker lifecycle, thread bind, termination escape, guest-memory lease, immutable media roots, presentation snapshots | worker-local neutral state | Keep the lifecycle/lease core.  Two fields cross a family boundary: `mvdm_command_native_child` is private state of the BaseSrv command adapter, and the text-plane/sink is presentation state used by the Win32 Console adapter.  They are not safe deletion candidates: the first has registered teardown; the latter backs current EDIT/Console rendering.  Extract only behind private typed extension accessors after regression proof; do not make `session` an alternate BaseSrv or UI provider. |
| `adapter-opennt-host/basesrv` — 36 files, 3,538 lines | BaseClient capture/config/classifier/RPC and BaseSrv request, process, resource, wait and stream bindings | finite modern CSR/RPC/handle binding around selected original callers | Keep as the named non-MVDM OpenNT adapter.  It is too broad as one physical family, but its original-shaped policy must not move to `app` or `broker`.  Partition by caller direction: BaseClient process-local bindings; BaseSrv server-local bindings; and RPC/handle transport facades.  The last group may consume broker transport interfaces but must not duplicate record state. |
| `adapter-mvdm-host-out/basesrv` — 6 files, 870 lines | image classification, guest command state, native-child capture | MVDM command-to-host finite bridge | Keep temporarily, but make native-child state a private extension rather than an untyped `session` field.  Image discovery must remain run16/app policy before `BaseCheckVDM`; this family must not grow a second classifier. |
| `adapter-mvdm-host-out/softpc` — 33 files, 4,748 lines | firmware, A20, guest memory, mappings, execution, termination, event thread, descriptors, C-VID binder | CCPU40/SAS/SoftPC external ABI and worker-local services | Keep as a named external binding family.  The C-VID binder belongs here because it is a modern ABI composition mechanism, not an OpenNT source body.  No new mirror file is allowed.  Future work may delete only a wrapper for which the selected original symbol is already linkable (as T417 did for the redundant SAS wrapper). |
| `adapter-mvdm-host-out/win32` — 30 files, 3,023 lines | private Win32/NT call shapes, Console, CRT, process and WOW UI seams | modern public Win32 translation | Keep as a finite API family.  Console text projection is a current functional dependency, so it cannot move into MVDM.  Divide it internally into Console/presentation, NT wait/thread, and CRT/process source-owned slices; no generic “compat” bucket may acquire policy. |
| `adapter-mvdm-host-out/monitor` — 10 files, 342 lines | VDM TIB/context, idle, VINT and printer bridge | unavailable kernel-monitor/user-mode boundary | Keep.  Its printer source is a modern host bridge and was correctly moved out of the old overlay.  It must not become a V86 monitor substitute. |
| `adapter-mvdm-host-out/redir` — 10 files, 393 lines | Redirector async, guest copy, handles and command redirection | file/guest copying external boundary | Keep and test separately.  The known CLI `>` limitation remains Redirector recovery work; no command parser may move into run16. |
| `adapter-mvdm-host-out/wow` — 7 files, 617 lines | callback frame lease and private USER compatibility | unavailable WOW/USER external interface | Keep as an explicit limited boundary.  It must not claim a private USER/CSRSS implementation. |
| `adapter-mvdm-host-out/vdd` — 3 files, 60 lines | VDD configuration/callback shape | VDD host registration boundary | Keep; already narrow. |
| `adapter-mvdm-host-out/debugger` — 5 files, 275 lines | debug state/export shapes | debugger external interface | Keep; it has no current license to own worker scheduling. |

## Confirmed consolidation targets

1. **Retire the dead legacy local broker plane.**  The six files named above
   are 387 source/header lines.  Their absence of a source caller is strong
   evidence, but the formal link list still names `broker.lib`; the next
   package must remove it from every product and fixture graph, rebuild and
   exercise real COMMAND/MEM/EDIT plus broker death before deleting it.  This
   is the only currently quantified net source-deletion candidate.
2. **Make ownership boundaries explicit without changing semantics.**  Split
   `adapter-opennt-host/basesrv` build groups by BaseClient, BaseSrv and RPC
   transport facade; split the Win32 family by Console, wait/thread and
   process/CRT.  This is principally reorganization, not a claimed line
   reduction.  The public headers and error/cleanup behaviour must remain
   source-shaped.
3. **Remove session's untyped adapter slot only after proving its lifecycle.**
   Move `mvdm_command_native_child` into a private BaseSrv-adapter extension
   object associated with the current worker session.  Preserve teardown,
   capture, standard-handle and nested COMMAND tests.  This reduces leaked
   cross-family representation, but is not authorized as a blind textual
   move.
4. **Do not relocate current functional adapters into a mirror.**  C-VID
   binding, Console projection, broker RPC, guest-memory publication and VDD
   attachment are modern-system mechanisms with no exact directly composable
   original body.  Moving them to `mvdm`/`opennt-host` would violate the
   strict mirror rule and obscure, rather than reduce, divergence.

## Migration order, risks and verification

| Proposed group | Safe destination / estimated movement | Compatibility risk | Required proof |
| --- | --- | --- | --- |
| B1 legacy broker removal | delete 6 files / 387 lines and unlink `broker.lib` if a fresh map proves no retained export | hidden link-time or fixture dependency | full x86 graph, link-map export check, broker death/pending-command, COMMAND/MEM/EDIT and nested COMMAND |
| B2 adapter-opennt partition | subdirectories/libraries inside `adapter-opennt-host`; no mirror move | capture-buffer, request TLS, handle ownership or status translation regression | BaseClient/BaseSrv lifecycle fixtures, RPC version mismatch, worker disconnect and map ownership check |
| B3 session extension extraction | a private typed BaseSrv-adapter extension; expected net reduction is unknown | native-child teardown, async guest pointer capture, standard-handle lifetime | nested COMMAND, native-child report/capture and session dispose stress |
| B4 Win32/SoftPC family tightening | existing named adapter families only; no required source move yet | Console resize/mouse/text-plane and CCPU mapping regression | EDIT keyboard/mouse, EDIT-exit→MEM, ConPTY/RDP resize, CCPU/C-VID fixtures |

The source audit therefore finds no justification to add a file to either
mirror and no justification to merge a modern adapter body into an original
file.  It identifies one high-confidence deletion cohort (387 legacy broker
lines) and two source-shape-preserving boundary cleanups.  The accompanying
candidate proposal owns implementation; this audit alone changes no runtime
semantics.
