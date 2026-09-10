# T95 S6 minimal Bochs-internal retention-isolation design

## Boundary rule

Every candidate remains inside Bochs. None may delegate SIM, logging, device,
PC-system, configuration, plugin, or CPU semantics to the adapter; none may
stub or silently replace a product behavior. This is design only.

## Decision matrix

| Root | Narrowest candidate and boundary | Effect / future exception | Risk, static proof, disposition |
| --- | --- | --- | --- |
| Full `bx_real_sim_c` vtable | Split original minimal-SIM construction/parameter operations from the product configuration virtual surface. Class is `gui/siminterface.cc:62-200`, construction `319-350`; minimal caller is `bx_ntvdm_minimal_sim.cc:11-24`. | Would remove rc/config/plugin/UI/`main.cc` virtual undefineds only if a Bochs-local minimal surface has no vtable references. High invasive class/lifecycle change; new exception required. | Prove original parameter lookup/list/bool behavior and zero config/plugin/main/GUI virtual references. No alternate minimal SIM class was found. Rejected pending a separate exception. |
| `devices.obj` full-PC members | Put registered empty-port init/cleanup into a dedicated Bochs-local source-object boundary. Full init is `iodev/devices.cc:103-271`; helper `273-354`; state/exit `356-390`; network/plugin `976-990`; caller `bx_ntvdm_minimal_machine.cc:52`. | Could remove plugin, device, timer and network undefineds now retained by common object. Medium source-object split; new exception required because BX-IO-025 covers only helper behavior. | Prove only arrays/sentinels/default handlers/cleanup and no init/exit/state/plugin/timer/network symbols. Risk: storage/lifetime duplication. Candidate only. |
| `bx_pc_system` global / GUI cleanup | Separate construction/required interrupt access from product `exit` lifecycle. Composition owns global at `bx_ntvdm_minimal_machine.cc:20-22`; constructor `pc_system.cc:48`; GUI cleanup `209-230`. | Could remove GUI cleanup only if exit is isolated from constructor/global retention. High lifecycle/object-layout change; new exception required. | Prove constructor and `set_INTR` but zero `exit`/GUI sections. No alternate PC-system class is evidenced. Rejected pending a source-backed boundary. |
| Logging accessor / `fatal -> bx_atexit` | Isolate original fatal/shutdown path from accessors needed by `SAFE_GET_GENLOG`. Root is `bx_ntvdm_minimal_machine.cc:41`; fatal `logio.cc:552-564`; owner `main.cc:1416`. | Could remove `bx_atexit` only if fatal is not retained with normal logging. Medium/high log-object split; new exception required. | Prove original accessors/ordinary logging and no fatal/`bx_atexit`. Error shutdown semantics are at risk. Remains rejected: current model is hard-incompatible and no narrow original split is proved. |

The inspected originals show no alternate minimal `bx_real_sim_c` or
`bx_pc_system_c`. Existing `init_empty_port_space` is a method boundary, not
an object-retention boundary. Adapter substitution, a synthetic class, or a
stub is forbidden.

## Decision

No candidate is selected. SIM/devices/PC candidates require new exception
records before source work; logging remains rejected until an original
accessor-only boundary is proven. A future admission must name one root only,
the original storage owner, the isolated section, and negative static checks
for every rejected undefined owner. No build, link, configuration, source, or
runtime action follows.
