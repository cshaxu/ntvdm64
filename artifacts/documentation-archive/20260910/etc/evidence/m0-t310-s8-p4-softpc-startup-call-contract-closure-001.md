# M0 T310 S8 P4 SoftPC startup call-contract closure

## Scope

The reviewed original startup calls are:

- `base/support/main.c` → `host_start_cpu` in `host/src/nt_cpu.c`;
- `base/support/main.c` → `InitialiseDosEmulation` in `host/src/nt_msscs.c`;
- `base/support/main.c` → `mouse_driver_initialisation` in
  `base/keymouse/mouse_io.c`.

## Source result

All three providers are selected original MVDM sources. `host_start_cpu` and
the mouse initializer are `void`; `InitialiseDosEmulation` has several
original status-return statements and therefore has the native `int` result
contract even though its historical definition omitted the type.

`CPU_40_STYLE` hid the old `host_start_cpu` declaration even while retaining
the call. No selected common header carries the other two declarations. The
support source now contains only the three exact provider declarations and the
provider definition spells its existing `int` result. Startup ordering and all
provider bodies stay original.

## Validation

On each architecture, the formal Ninja graph cleaned
`obj/support/main.obj` and `obj/host/nt_msscs.obj`, then rebuilt:

- `original-softpc-support.lib`;
- `original-softpc-host-roots.lib`.

Both x64 and x86 reached their archive steps with zero errors and without the
three reviewed C4013 calls. Other visible diagnostics, including unreviewed
LIM/PIF and keymouse/XMS contracts, remain compiler-visible and are outside
this narrow closure.

## Boundary

This restores calls to existing original providers; it introduces no machine
adapter, mapping-manager operation, guest behavior, or warning policy change.
