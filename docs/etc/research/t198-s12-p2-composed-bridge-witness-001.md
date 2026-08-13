# T198 S12 P2 Composed Bridge Witness 001

## Question

Can the alternative generic-UD bridge symbol become the single adapter
composition entrypoint while preserving default decline before explicit session
binding?

## Procedure And Observation

The alternative i686 bridge now delegates only to
`bx_ntvdm_boot_namespace_composition_v1_handle`. The focused source-built i686
composition executable was rebuilt and exited `0`: its unbound generic bridge
call declines, while its bound COMMAND, DEM read/completion, direct DEM and
executor-failure cases traverse the same public bridge symbol. After unbind it
declines again.

`Test-VdmGenericUdBridgeBoundary.ps1` passed. Its updated source scan requires
the single adapter-session delegate and rejects the historical runtime,
host-session, callback, guest-memory helper, multi/bulk transaction and Bochs
product dependencies from the generic bridge source/header.

## Interpretation And Limit

The core-facing symbol carries only opaque event/outcome records. It contains
no selector dispatch and no provider implementation; both remain behind the
adapter-owned bound session. This is still an i686 fixture closure, not an
MSVC/x86 CPU5 `UD2` execution witness. The latter must explicitly include the
initialized provider object set and retain the existing no-core-patch rule.
