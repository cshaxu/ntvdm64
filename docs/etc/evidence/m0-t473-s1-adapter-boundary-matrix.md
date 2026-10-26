# M0 T473 S1 — non-original boundary matrix

## Purpose

The product link includes original owner packages plus a finite set of
project-owned carriers. This matrix is the duplicate/overlay side of the
whole-closure audit: a carrier is not treated as an original provider merely
because it sits beside one in the link line, and no original source is added
until the carrier's exact owner can be replaced without changing lifecycle or
resource authority.

The authoritative current inventory is the x86 generator's explicit source
arrays; it is reproduced here by library role rather than inferred from names.

| Link group | Current project-owned members | Allowed responsibility | S2 disposition |
| --- | --- | --- | --- |
| `softpc-bindings.lib` | `mvdm_softpc_{firmware,guest_memory,physical_mapping,guest_location,descriptor_fields,xms_memory,a20}.c`, `mvdm_shadow_registry.c` | Session-selected immutable media/root, bounded guest span/physical conversion, CPU40 resource projections, and host resource handles. | Prove one resource authority per field/handle; replace only a body for which a complete original owner and modern boundary are both known. |
| `softpc-bindings.lib` | `mvdm_softpc_{execution,termination,ccpu_wait,fast_bop,wow_page_domain,event_thread}.c`, `mvdm_standalone_worker.c` | Worker-local execution entry, original-call result transfer, lifecycle/teardown, and CPU40/WOW carrier setup. | Reconcile with selected original host/support/CCPU/DPMI call order; do not create a second scheduler or worker lifecycle. |
| `softpc-bindings.lib` | `mvdm_softpc_{presentation_font,text_video}.c`, `mvdm_keyboard_{history,reset}.c`, `mvdm_softpc_{mouse_input,mouse_bridge,mouse_guest}.c`, `ntvdm_dos_observer.c` | Frame/input conversion and explicitly default-off observations only. | Pair every bridge with the original device/interrupt owner and ensure observer removal cannot alter normal execution. |
| `mvdm-softpc-effective-address.lib`; `softpc-ccpu-vector-defaults.lib`; `softpc-activity-check.lib` | `mvdm_softpc_effective_address.c`, two selected `fmstubs` leaves | CPU40 address carrier and explicitly selected absent-provider defaults. | Keep as narrow original-shaped leaves until an original provider has a finite standalone closure; never inflate them into a general execution engine. |
| `softpc-win32-bindings.lib` | NT I/O, thread, Console, CRT, command-process and base-VDM environment facades | Modern USER/Console/thread/process ABI boundary only. | Map each body to the original caller and the one modern service it invokes; no control/menu/DOS policy may move here. |
| `worker-command-bindings.lib`; `wow-worker-bindings.lib`; `worker-shell.lib`; `session.lib` | COMMAND guest state/native child; WOW GDI/USER session; package layout; guest-memory lease/session | NTSRV/worker protocol, admitted guest-location leases and worker-local session state. | Verify identity, nested return, failure and teardown through the centralized lifecycle contract; not an original COMMAND/WOW replacement. |
| `monitor-bindings.lib`; `debugger-bindings.lib`; `vdd-bindings.lib`; `kernel-vdm-printer.lib` | VDM control/TIB, debugger prompt/register bridge, explicit unavailable VDD result, original printer subset | Finite monitor/debugger/VDD boundaries only. | A missing kernel/private subsystem stays an explicit failure/unavailable boundary. Restore an original provider only as a whole admitted package. |
| Redirector worker/DLL bindings | `mvdm_redirector_{worker_copy,async}.c` and VDMREDIR DLL bindings | Bounded guest-copy/async transport around original VDMREDIR. | Retain original DLL dispatch/teardown; audit copy ownership, cancellation and RAP rejection together with the VDMREDIR package. |

## Audit consequences

This is a closed *inventory*, not a clearance to retain all code. It fixes the
recovery search space at the explicit linker-member level: the S2 delta review
must assign every carrier to exactly one of (1) an original package owner that
can now replace it, (2) a necessary modern resource/ABI conversion, (3) a
selected original fragment constrained by a missing kernel/private provider,
or (4) removable diagnostic residue. A carrier may not be both a resource
projection and an independent behavioral owner.

The matrix also rejects two bad recovery moves: linking an unselected CPU30 or
kernel-VDM package merely to eliminate a carrier, and migrating an original
control/device algorithm into a binding library. Both would enlarge the
runtime authority surface rather than reduce it.
