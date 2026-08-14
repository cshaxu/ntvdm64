# M0 T199 S21 C1 A20 Selector-Blind Capability Admission

## Result

The adopted native owner already exists: `bx_pc_system_c::set_enable_a20` and
`get_enable_a20` in `src/bx-mantle/pc_system.cc`. The setter changes the
native A20 mask and calls `MemoryMappingChanged` on a transition, which flushes
the native CPU TLB. `bx_ntvdm_minimal_machine_c::initialize` establishes A20
enabled after CPU initialization and before reset.

No bx-core change is required. A new mantle wrapper is nevertheless a direct
change below the adopted/mantle boundary and is registered as BX-MANTLE-078
before implementation.

## Selected ABI

One C++ mantle-private fixed request/result pair will carry only:

- request version;
- operation `set` or `query`;
- one Boolean requested value for `set`; and
- one Boolean returned native value plus success/rejection status.

It will reject an unknown operation, invalid version, a request before a
minimal-machine initialization, and a request after cleanup. It holds no
adapter state, BOP selector, XMS/DPMI service, guest pointer, allocator,
interrupt, firmware, or host capability. A positive test toggles the native
value `enabled -> disabled -> enabled`; a negative test proves rejected
requests leave the native value unchanged.

## Consequence

S22 may implement this wrapper and fixture only. It may not attach it to an
XMS/DPMI BOP provider; that needs a later full XMS package admission after C2
through C6 are separately closed.
