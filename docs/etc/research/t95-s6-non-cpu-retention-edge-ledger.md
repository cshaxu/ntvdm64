# T95 S6 non-CPU retention-edge ledger

## Scope and method

This is a source-level, read-only ledger from the fixed
`bx_ntvdm_minimal_machine_c::initialize()` sequence.  It does not select the
pending CPU profile and does not infer a call path beyond a named source call.
The sequence is: logging accessors, minimal SIM, no-SIM memory, empty port
space, then the original CPU `initialize()` and `reset(HARDWARE)`; cleanup only
reverses owned port-space then memory state.

Reproducible searches:

```powershell
rg -n -C 3 "bx_ntvdm_minimal_machine|bx_ntvdm_minimal_sim|init_memory_without_sim|init_empty_port_space|cleanup_(memory|empty_port_space)" refs/bochs
rg -n -C 2 "DEV_cmos_get_reg|DEV_vga_mem_(read|write)|DEV_vga_refresh|DEV_dma_raise_hlda|DEV_(timer|pit)|bx_pc_system\.(initialize|Reset|inp|outp)" refs/bochs
rg -n -C 2 "bx_init_siminterface|bx_list_c|bx_param_bool_c|SAFE_GET_(IOFUNC|GENLOG)|pluginlog|bx_user_quit|bx_dbg" refs/bochs/gui refs/bochs/logio.cc refs/bochs/pc_system.cc refs/bochs/bx_ntvdm_minimal_machine.cc refs/bochs/bx_ntvdm_minimal_sim.cc
rg -n -C 2 "BX_CPU_C::(initialize|reset)|init_SMRAM|init_FetchDecodeTables|cpu_loop|handleWaitForEvent" refs/bochs/cpu/init.cc refs/bochs/cpu/cpu.cc refs/bochs/cpu/event.cc refs/bochs/cpu/smm.cc
```

One initial search used Bash-style brace paths in PowerShell and was rejected
by the shell before `rg` began. It made no change; the explicit-path searches
above are the evidence used here.

## Edge ledger

| Edge/member | Imported owner/source family | Minimal sequence classification | Retention and ownership conclusion |
| --- | --- | --- | --- |
| `bx_ntvdm_minimal_sim_initialize` -> `bx_init_siminterface`, root `get_param(".")`, `bx_list_c`, `bx_param_bool_c` | `gui/siminterface.cc`, `gui/paramtree.cc` | directly reached | Required original Bochs core. The admitted helper creates only `cpu.reset_on_triple_fault=false`; generic configuration/UI methods in the same source objects are default-not-required but object retention is unresolved. Adapter ownership is rejected. |
| SIM callbacks `bx_real_sim_c::refresh_vga`, `handle_events`, debugger/configuration interfaces | `gui/siminterface.cc`; GUI/debug families | not called by the minimal helper | Default-not-required and product GUI/debug explicitly rejected. The containing SIM object is required, so future object retention must prove these members do not require GUI/VGA owners. They cannot move to the adapter. |
| `SAFE_GET_IOFUNC`, `SAFE_GET_GENLOG`; composition-owned `pluginlog` | `logio.cc`, composition globals | directly reached | Required original logging core. `logfunctions::ask` at `logio.cc:455` calls `DEV_vga_refresh` only under its ask/initialized-SIM path; that behavior is default-not-required, while its object retention remains unresolved. Adapter logging replacement is rejected. |
| Composition globals `bx_pc_system`, `bx_dbg`, `bx_user_quit`; `bx_pc_system_c` constructor | `pc_system.cc`, composition source | constructor directly reached by static global construction | Required original PC-system core only for constructor/global storage. The composition does not call `bx_pc_system.initialize` or `Reset`; those timer/reset lifecycle members are default-not-required and object-retention unresolved. |
| CPU I/O paths `bx_pc_system.inp/outp` -> `bx_devices.inp/outp` | `pc_system.cc`, `iodev/devices.cc` | not called by initialize/reset sequence | Default-not-required for the bounded no-loop fixture. The empty port arrays are nevertheless directly initialized below. This remains Bochs core, never adapter device policy. |
| `BX_MEM_C::init_memory_without_sim` and `cleanup_memory` | `memory/misc_mem.cc`, `memory/memory.h` | directly reached | Required original Bochs memory core: vector/ROM/bogus, block table, handler table, PCI/SMRAM false state, and owned cleanup. Original `init_memory` SIM/PCI/state registration is explicitly rejected. Other physical-memory member retention is unresolved. |
| `DEV_vga_mem_read/write` in `BX_MEM_C::dbg_fetch_mem` / `dbg_set_mem` | `memory/misc_mem.cc` -> VGA device family | not called by the helper | Default-not-required debug memory paths; VGA is explicitly rejected. Because they share an imported object with required allocation code, linker/member retention is unresolved. No adapter substitute is allowed. |
| `bx_devices.init_empty_port_space` / `cleanup_empty_port_space` | `iodev/devices.cc`, `iodev/iodev.h` | directly reached | Required original Bochs default-handler/sentinel/two-`PORTS`-array core and owned cleanup. `bx_devices_c::init`, plugin loads, CMOS/DMA/PIC/PIT/floppy/VGA, timers, and port 92 registration are explicitly rejected. |
| `DEV_cmos_get_reg` in `BX_CPU_C::exception` triple-fault hard-reset branch | `cpu/exception.cc` -> `plugin.h` CMOS macro -> iodev CMOS device | not reached by initialize/reset; false SIM parameter selects the original shutdown branch if a triple fault later occurs | Hard-reset/CMOS is explicitly rejected for this minimal profile. The original false branch is preserved; the source object still contains the hard-reset reference, therefore object retention is unresolved. CMOS semantics cannot be adapterized. |
| `DEV_dma_raise_hlda` in `cpu/event.cc` | CPU event source -> iodev DMA | only in wait/event/CPU-loop members, which composition forbids | Default-not-required; DMA is explicitly rejected. If `event.cc` becomes retained by CPU-family closure, member-level discard must be proved rather than assumed. |
| timer fields and `bx_pc_system.register_timer` in `bx_devices_c::init`; timer updates in PC system | `iodev/devices.cc`, `pc_system.cc`, timer family | not called by minimal helpers | Explicitly rejected product device/timer initialization. PC-system null-timer constructor state is inseparable from its original constructor and is required only as that state, not timer operation. Retention unresolved. |
| `DEV_vga_refresh` from SIM/log/debug and full GUI paths | SIM/log/debug -> VGA | not called by the declared minimal lifecycle | Explicitly rejected VGA/GUI operation; all are subject to object-retention proof. |

## Future grouping boundary

The only candidates for a future **stock Bochs core** group are the imported
SIM/parameter, log, PC-system, memory, and empty-port source families named
above, with their exact admission limits.  They remain Bochs-owned mechanics.
There is no eligible external-adapter implementation candidate in this ledger:
the adapter may later observe a typed result, but it cannot provide SIM,
logging, PC/timer, memory, I/O handlers, CMOS, DMA, or VGA semantics.

## Result and next gate

The direct minimal lifecycle has a finite non-CPU core: SIM/parameter,
logging, PC-system construction, no-SIM memory, and empty-port initialization.
All device/UI/debug/timer/DMA/VGA and CMOS-hard-reset behaviors are either
default-not-required or explicitly rejected.  The remaining unresolved issue
is object/member retention: source alone cannot prove that the required source
objects can be linked without their uncalled denied members.  The next action
is coordinator review of an evidence plan that separately names retention proof
for these owners; it must not choose the CPU profile or admit a build target.
