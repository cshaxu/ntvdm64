# adapter-softpc

Bochs-backed recovery of reached historical SoftPC, CCPU, SAS and VDM-facing
mechanical interfaces. A recovered interface retains the original name,
parameters, calling convention and observable mechanical semantics while using
bounded `bx-mantle` operations.

It is selector/service blind: it does not own BOP, DOS, WOW, OpenNT host policy
or Win32 capability. It is the only OpenNT-facing component allowed to request
Bochs mechanics. It owns the one mapping-manager implementation and its three
session instances (`guest_memory`, `host_handle`, `session_data`).

The future static library name is `adapter-softpc`; its public headers expose
only declared same-shaped SoftPC/CCPU/SAS interfaces.
