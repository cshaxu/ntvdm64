# T225 S23 P2 — Native PIC Lifecycle and Link-Closure Audit

## Question

Can the reached 8259 PIC be composed as a small native `bx-mantle` machine
facility under the formal MSVC x64 `/MT` graph, without reintroducing the
Bochs product shell, plugin loader, or an adapter-owned interrupt model?

## Inputs

- `refs/bochs/iodev/pic.cc` and `pic.h`, whose SHA-256 identities are fixed by
  [S23 P1](t225-s23-int06-native-pic-source-map-001.md).
- Current `src/bx-mantle/minimal_port_space.cc`,
  `bx_ntvdm_minimal_machine.cc`, `bx_ntvdm_minimal_sim.cc`, and
  `pc_system.cc`.
- Formal include roots and CPU5/P, MSVC x64 `/MT` configuration from
  `tools/build/t225-s7-full-module-manifest.json`.

## Procedure and observations

1. A fresh compile probe with the formal forced configuration and `/DWIN32`
   reached the native PIC source. The earlier `unistd.h` result was therefore
   a probe-command omission, not a required product dependency.
2. The first real compile stop was `instrument.h`; the formal manifest already
   owns `refs/bochs/instrument/stubs`. Adding only that existing include root
   produced `build/M0-T225-S23/pic-compile-probe-r4/pic.obj` successfully with
   MSVC x64 `/MT`. No source was changed.
3. Object-symbol inspection shows the PIC needs only existing machine globals
   and ordinary Bochs facilities: `SIM`, `bx_pc_system`, `bx_devices`,
   `pluginlog`, logging, and the four port-registration/unregistration methods. The only
   product-shell-specific unresolved symbol is `pluginRegisterDeviceDevmodel`,
   reached solely from `libpic_LTX_plugin_init`.
4. `bx_pic_c::init()` uses the existing mantle port registry to register exactly
   eight byte handlers: reads and writes at `20h`, `21h`, `A0h`, and `A1h`.
   It initializes original master/slave state. The body does not invoke a
   plugin loader, timer, firmware, GUI, or other device.
5. Current mantle has the needed empty-port registry and default handlers, but not the ordinary `bx_devices_c::register_io_read_handler`, `register_io_write_handler`, `unregister_io_read_handler`, or `unregister_io_write_handler` bodies. Its cleanup intentionally rejects any non-default port mapping. Thus those four original methods from `refs/bochs/iodev/devices.cc:376-686` are the smallest required source-first addition for a repeatable machine lifecycle.
6. `bx_pic_c::~bx_pic_c()` removes `"pic"` from the finite SIM root. This root
   exists in the current mantle. `register_state()` is not called, so no PIC
   state node is created; the missing-name removal is the only destructor
   dependency to verify in the focused lifecycle test. It is not evidence for
   adding Bochs state persistence or a product configuration tree.

## Disposition

| Candidate path | Disposition | Reason |
| --- | --- | --- |
| `libpic_LTX_plugin_init` / `pluginRegisterDeviceDevmodel` | Reject | Plugin discovery and device registration are product-shell facilities; the mantle owns a fixed compile-time PIC binding. |
| `bx_pic_c` constructor, `init`, IRQ, IAC and byte-port semantics | Adopt unchanged | This is the native 8259 implementation required by the reached owner package. |
| `bx_pic_c::register_state` and debugger registration | Do not call | Save-state/debugger product support is not required by the reached path. |
| Current finite SIM, logging, PC system, default port-space | Retain | They already satisfy the compiled PIC's non-plugin machine requirements. |
| Original port registration/unregistration bodies | Recover into mantle unchanged in behavior | They connect native PIC ports and restore the established default-port lifecycle before existing cleanup. |
| Selector `06h`, OpenNT handler order, BOP/provider behavior | Exclude | These remain bx-vdm/OpenNT responsibilities and must not enter the PIC or bx-core. |

## Minimum P3 implementation boundary

P3 implementation note: the formal link demonstrated that the original plugin wrapper itself is emitted even when never called. CPU5 also selects BX_USE_PIC_SMF=1, so the original translation-unit `thePic` singleton must be bound before init(). The admitted mantle-private create/destroy helpers replace only that wrapper; they are not a plugin API or an adapter seam.

P3 may do only the following:

1. Preserve the native PIC source identity in `src/bx-mantle` and register any
   necessary external-source movement/exception before changing it.
2. Add the four original registration/unregistration method bodies to the existing mantle port-space source, retaining their default-handler restoration behavior.
3. Add a fixed mantle PIC lifecycle: call the mantle-private native PIC create helper, replace the stub
   `pluginPicDevice`, call `init()` after `init_empty_port_space()`, then
   unregister its eight handlers, restore the stub pointer, and destroy it
   before `cleanup_empty_port_space()`.
4. Add focused mechanics tests proving PIC masks/commands/EOI use native port
   dispatch and lifecycle cleanup returns to the all-default state.

P3 must not invoke the plugin entry, call `register_state`, add a device other
than the PIC, change CPU selector semantics, or write guest bytes. The original
OpenNT `illegal_op_int`/`unexpected_int` source-derived x64 seam is a separate
post-P3 decision; this audit does not claim it is implemented.

## Confidence and follow-up

High confidence that the PIC compilation and ordinary machine dependency
closure are finite: the source compiles under the exact product architecture
and the remaining lifecycle gap is four directly identified original port registration/unregistration methods. Medium confidence in destructor cleanup until P3's
focused test runs. P3 is therefore the next admissible implementation part.
