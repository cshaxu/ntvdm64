# adapter-softpc

Bochs-backed recovery of reached historical SoftPC, CCPU, SAS and VDM-facing
mechanical interfaces. A recovered interface retains the original name,
parameters, calling convention and observable mechanical semantics while using
the declared, fixed-width `adapter-bochs` mechanical facade.

It is selector/service blind: it does not own BOP, DOS, WOW, OpenNT host policy
or Win32 capability. It never imports a `bochs-core` object/header; it calls
only `adapter-bochs`' selector-blind mechanical facade. It owns the one mapping-manager
implementation. `session`
owns the identity and lifecycle of its three instances (`guest_memory`,
`host_handle`, `session_data`); this facade uses the applicable instance only
within its admitted synchronous call boundary.

The future static library name is `adapter-softpc`; its public headers expose
only declared same-shaped SoftPC/CCPU/SAS interfaces.

`include/` contains only the required production ABI header closure
used to preserve historical SoftPC/CCPU spelling at this boundary; its
`softpc.new/{base,host}/inc` hierarchy deliberately retains required relative
header relationships. Non-linkable historical overlays are evidence under
`docs/etc/legacy_code/adapter-softpc/`, not adapter production input.

## Local divergence register

| ID | Original definition purpose | Divergence reason | Replacement implementation | Production file(s) |
| --- | --- | --- | --- | --- |
| `SOFTPC-DIV-001` | Historical SAS/CPU macros use process-global names. | Independently imported owner modules collide on those names. | Private aliases retain the mirror spelling while binding to a call-local mechanical record. | `softpc_memory_size_shim.h:16` |
| `SOFTPC-DIV-002` | Historical CPU register macros use process-global names. | They cannot safely represent a bounded session call on modern hosts. | Private aliases bind to the synchronous copied-state record with no pointer retention. | `softpc_tape_io_shim.h:15` |
| `SOFTPC-DIV-003` | The reached keyboard handoff expects a mutable display-state source alongside CCPU/SAS accessors. | The BOP route must not own machine-facing state. | Keep the per-thread display-state value in the same SoftPC compatibility call boundary. | `spckbd_handoff_shim.{c,h}` |
| `SOFTPC-DIV-004` | `nt_lpt.c:host_lpt_flush_initialize` clears every `HOST_LPT::dos_opened` bit before COMMAND lifecycle transitions. | The minimal Bochs composition has no independently admitted LPT endpoint object. | Preserve the original global name and exact three-port `dos_opened = FALSE` operation over the existing selector-blind facade state; do not create, close or emulate a port. | `softpc_printer_openclose_shim.{c,h}` |
| `SOFTPC-DIV-005` | Historical CCPU/SAS macros read a globally available CPU register image and guest byte address space during an imported provider call. | The modern machine boundary permits only a borrowed fixed-width CPU/result record and checked callbacks; retaining the old global backing would couple provider code to a machine implementation. | A thread-scoped, selector-blind frame context keeps the original register/load/controlled-stop and bounded multisz-copy shape while updating only typed deltas and using checked callbacks. | `ccpu_frame_context.{c,h}` |
| `SOFTPC-DIV-006` | `xmsa20.c` retains the historical process-wide SAS pointer to HIMEM's A20 state byte. | A native guest pointer may not outlive a bounded source call or cross a component boundary. | The existing session `guest_memory` mapping-manager instance opens the XMS call epoch; the facade retains only the guest-linear address and acquires/releases a one-byte lease for validation and writeback. | `opennt_xms_softpc_facade.c` |
| `SOFTPC-DIV-006` | `xmsa20.c` retains the historical process-wide SAS pointer to HIMEM's A20 state byte. | A native guest pointer may not outlive a bounded source call or cross a component boundary. | The existing session `guest_memory` mapping-manager instance opens the XMS call epoch; the facade retains only the guest-linear address and acquires/releases a one-byte lease for validation and writeback. | `opennt_xms_softpc_facade.c` |
| `SOFTPC-DIV-006` | `xmsa20.c` retains the historical process-wide SAS pointer to HIMEM's A20 state byte. | A native guest pointer may not outlive a bounded source call or cross a component boundary. | The existing session `guest_memory` mapping-manager instance opens the XMS call epoch; the facade retains only the guest-linear address and acquires/releases a one-byte lease for validation and writeback. | `opennt_xms_softpc_facade.c` |
