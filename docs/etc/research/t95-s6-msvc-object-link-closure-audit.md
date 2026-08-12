# T95 S6 MSVC source-level object/link closure audit

## Scope and method

This is a read-only audit beginning with syntax-003's exact fixture and 15
candidate translation units. Searches used `rg` over those source files, their
declaring headers, and `cpu/fetchdecode.cc`/`ia_opcodes.h`; no compiler,
archiver, linker, project, or configuration command was invoked. Historical
VS2008 metadata and full `bochs.exe` aggregates are not inputs to this ledger.

The read-only searches were:

```powershell
rg -n "bx_ntvdm_minimal_machine|bx_ntvdm_minimal_sim|init_memory_without_sim|init_empty_port_space" src/bochs
rg -n "init_FetchDecodeTables|BxOpcodesTable|ia_opcodes|execute[0-9A-Za-z_]*" src/bochs/cpu
rg -n "DEV_cmos_get_reg|DEV_vga_mem_(read|write)|DEV_vga_refresh" src/bochs
rg -n "bx_pc_system|bx_user_quit|pluginlog|bx_cpu|bx_mem|bx_devices" src/bochs
rg -n "BX_CPU_C::(initialize|reset)|BX_MEM_C::|bx_devices_c::" src/bochs
```

Classification means source-level reachability only: **required** is needed by
the admitted minimal composition call sequence; **default-not-required** is
compiled source not reached by that sequence; **rejected** is a denied product
aggregate; **unresolved** has no source-only proof that the object/linker can
discard it.

## Candidate ledger

| Candidate | Direct defining role / external source-level edges | Classification |
| --- | --- | --- |
| `fixture_syntax.cc` | Declares only `bx_ntvdm_minimal_machine_c*`; type from `bx_ntvdm_minimal_machine.h`; no construction/call. | default-not-required fixture object |
| `bx_ntvdm_minimal_machine.cc` | Defines `pluginlog`, `bx_pc_system`, `bx_dbg`, `bx_cpu`, `bx_mem`, `bx_user_quit`; calls minimal SIM, private memory/I/O helpers, `BX_CPU_C::initialize/reset`. | required composition object |
| `bx_ntvdm_minimal_sim.cc` | Calls original `bx_init_siminterface()` in `gui/siminterface.cc`; creates original parameter types defined in `gui/paramtree.cc`. | required |
| `gui/siminterface.cc` | Defines SIM singleton/interface; its full object also contains debugger/UI callbacks (`bx_dbg_*`) and device refresh calls. | required SIM definition; unproven retained debug/device edges |
| `gui/paramtree.cc` | Defines parameter/list/bool classes used by minimal SIM; full object also exposes generic serialization/UI paths. | required parameter definitions; retained-object edges unresolved |
| `logio.cc` | Defines logging types accessed by `SAFE_GET_IOFUNC/SAFE_GET_GENLOG`; references `bx_pc_system`/`bx_user_quit` defined by composition. | required logging definitions |
| `pc_system.cc` | Defines PC-system methods/global type behavior; minimal composition owns global but does not call `initialize/Reset/timers`. `Reset`/timer paths are product defaults. | constructor/global support required; timer/reset paths default-not-required and retention unresolved |
| `memory/memory.cc` | Defines physical memory access methods used by CPU/memory machinery; depends on `bx_mem`, handler tables from `misc_mem.cc`. | required candidate, exact retained method subset unresolved |
| `memory/misc_mem.cc` | Defines constructor, registered no-SIM init/cleanup and block/handler storage; debug helpers reference `DEV_vga_mem_read/write`. | required helper/storage; VGA debug edges default-not-required but retention unresolved |
| `iodev/devices.cc` | Defines global `bx_devices` and registered empty port-space helpers; full `init/reset/timer` paths call CMOS/timer/device code. | required global/helpers; full device paths explicitly rejected and retention unresolved |
| `cpu/init.cc` | Defines constructor, `initialize`, `reset`; `initialize` calls `init_FetchDecodeTables` and `init_SMRAM`. CPU3 eliminates CPUID/MSR/WX branches by config. | required; pulls fetch/decode and SMM candidates |
| `cpu/fetchdecode.cc` | Defines decode tables and `init_FetchDecodeTables`; includes `ia_opcodes.h` to populate opcode metadata and function-pointer entries. | required candidate; complete handler-object closure unresolved |
| `cpu/icache.cc` | Defines instruction-cache/fetch helpers referenced by decoder/CPU execution machinery. | required candidate; exact retained subset unresolved |
| `cpu/smm.cc` | Defines `init_SMRAM` and SMM helpers called by `initialize`; SMRAM is disabled by no-SIM memory state. | required initialization definition; SMM execution paths default-not-required and retention unresolved |
| `cpu/io.cc` | Defines CPU port-I/O instruction handlers reaching `bx_pc_system.inp/outp` and default port arrays. | required decoder-handler candidate; execution handler closure unresolved |
| `cpu/exception.cc` | Defines CPU interrupt/exception delivery; default-off adapter seam excludes adapter headers. Triple-fault hard-reset branch still names `DEV_cmos_get_reg`. | required candidate by declared list; CMOS edge unresolved/rejected absent retention proof |

## Smallest possible set and blockers

The smallest **candidate** set is the 15 listed imported translation units plus
the optional fixture object; it is not a smallest linkable set. The
composition object owns the selected global definitions, so `main.cc` is
explicitly rejected. Product `config.cc`, GUI/text config, plugins, firmware
loading, full `iodev` aggregate, CMOS/timer/device objects, adapter/OpenNT,
and the VS2008 `bochs.exe` project are explicitly rejected.

Three exact source-level edges prevent a buildability claim:

1. `cpu/init.cc:224` unconditionally calls
   `BX_CPU_C::init_FetchDecodeTables`; `cpu/fetchdecode.cc:2034-2075` consumes
   `ia_opcodes.h` metadata and writes `BxOpcodesTable[*].execute*`. The table
   has function-pointer execution entries, so the defining instruction-handler
   objects cannot be derived from the 15 candidates without a retained-symbol
   proof. This is the primary unresolved edge.
2. Candidate full objects contain denied-path external references:
   `cpu/exception.cc:1204` → `DEV_cmos_get_reg`; `memory/misc_mem.cc:601,669`
   → VGA memory functions; `iodev/devices.cc:234-269,257` → CMOS/timer/device
   initialization. Source alone does not prove COMDAT/dead-strip elimination
   for those uncalled members.
3. `gui/siminterface.cc` and `pc_system.cc` contain UI/debug/timer callbacks
   outside the minimal lifecycle. Their object-level retention likewise cannot
   be decided without map/undefined-symbol evidence.

The required next action is therefore not an object recipe: it is coordinator
review of a controlled `/Gy` + link-retention proof admission that first names
how each unresolved edge is observed. No object list, library, or executable
is asserted buildable here.

## Non-claims

No `cl`, `lib`, `link`, `/c`, `/Fo`, `/Zs`, configure, VS project, source copy,
or Bochs/adapter/OpenNT mutation occurred. This audit neither enables devices
nor changes the accepted reset-to-bounded-stop scope.
