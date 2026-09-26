# S2 Console boundary ledger

## Question and baseline

Move user-facing DOS Console I/O to run16 without replacing original guest
devices, execution or re-entry. Baseline c39b9ca0c; selected x86 CCPU40.
This initial source audit is not production or runtime closure.

## Procedure and findings

Search ntvdm-exe and mvdm/softpc.new/host/src for Console read/write,
scroll and buffer operations; inspect original caller bodies before choosing
transport. Paths below are repository-relative.

| Family | Current producer/consumer | Required disposition and proof |
| --- | --- | --- |
| Input | nt_event.c::nt_event_loop waits for Console input, suspend and prepend events, reads one record without blocking, then applies nt_rdp_normalize_key and key history. | Keep normalization and guest dispatch in worker; replace host-read/wait with transported records plus cancellation. Test ordering, prepend, suspend, key releases and queue exhaustion. |
| Stream output | nt_graph.c::host_stream_io_update calls WriteConsoleA with explicit byte count. | Preserve byte/codepage and ordering in run16; periodic cell snapshots cannot replace scrollback or final output drain. Test MEM and long output. |
| Cell output/readback | nt_fulsc.c uses WriteConsoleOutput and ReadConsoleOutput around guest text transitions. | Preserve rectangles, attributes and readback; audit selected branches before choosing local frame state versus frontend reply. Test EDIT then MEM and native return. |
| Scroll | nt_graph.c calls ScrollConsoleScreenBuffer. | Ordered frontend operation, not latest-frame-only delivery; retain fill and cursor rules. Test small viewport and scrollback. |
| Resize | ntvdm-exe/win32/console_compat.c::console_resize_grid preserves rows across modern Console reflow. | Move host grid mechanics to frontend while retaining existing source-derived ordering. |
| Native transition | dos/command/cmdexec.c::cmdCreateProcess and cmdExec32 retain original execution/completion. | Establish existing suspend/resume points and output barrier before wiring handoff. Process existence alone cannot determine input ownership. |

The native-child binding in ntvdm-exe/command/source copies bounded guest
command/environment/stream inputs into session-owned state. It is not a user
Console reader; its guest leases must not move into run16 merely for UI ownership.

## Recovery and remaining work

Keep original MVDM event/video/COMMAND bodies selected. Console server/CSR is
a stopping boundary, not a new import. The owner-required frontend split admits
a minimal copied I/O protocol; exact authentication, layout, capacity, failure
and teardown contracts must be reviewed before implementation.

The reference branch contains worker-owned presentation/console_route,
graphics_api, graphics_buffer and graphics_console. These are comparison
candidates, not approved imports; do not copy their worker-owned frontend
composition wholesale. No library import in S2; S4 uses latest nxvm libraries.

- [ ] Complete selected caller/handle/mode/flush/title/cursor inventory.
- [ ] Prove native yield/resume and nested launcher association.
- [ ] Register finite protocol, authentication and disconnect behavior.
- [ ] Implement/test complete input/output/handoff production paths.
- [ ] Pass S2 regression/negative/lifecycle gates and publish coherent package.

No production file, binary, guest or runtime configuration changed in this
audit. S1 build-only waiver does not apply to S2.
