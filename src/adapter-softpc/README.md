# adapter-softpc

Bochs-backed recovery of reached historical SoftPC, CCPU, SAS and VDM-facing
mechanical interfaces. A recovered interface retains the original name,
parameters, calling convention and observable mechanical semantics while using
bounded `bx-mantle` operations.

It is selector/service blind: it does not own BOP, DOS, WOW, OpenNT host policy
or Win32 capability. It is the only OpenNT-facing component allowed to request
Bochs mechanics. It owns the one mapping-manager implementation. `session`
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
| `SOFTPC-DIV-003` | OpenNT providers obtain CCPU/SAS register, pointer and handle state from the historical product host. | Raw pointers and product-global state cannot cross the modern machine boundary. | Preserve source-facing helpers through one call-scoped copied frame, checked mapping and opaque handle record; the neutral CCPU/SAS alias surface does not identify a BOP service. | `opennt_dem_ccpu_sas_facade.c:1`, `opennt_dem_ccpu_sas_facade.h:162`, `opennt_ccpu_sas_facade.h` |
