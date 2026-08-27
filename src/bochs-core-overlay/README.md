# bochs-core-overlay

Private implementation fragments for narrowly registered Bochs-core mirror
divergences.  This component has no public ABI and may be called only by its
native mirror component, `bochs-core`; no adapter, host, BOP, session or app
source may include or link to it directly.

## Exception index

| ID | Original purpose | Divergence reason and implementation | Files |
| --- | --- | --- | --- |
| `BX-MACH-023` | Bochs `Reset` and `exit` normally orchestrate the full device and GUI product shell. | The adopted finite machine must retain only CPU reset/A20 and timer-baseline semantics; the private replacement method definitions require `bx_pc_system_c` and CPU globals and therefore cannot live in `adapter-bochs`. | `pc_system_lifecycle.cc`, `../bochs-core/pc_system.cc` |
| `BX-MACH-024` | Bochs PIC teardown is normally owned by the complete product/device shell. | The finite empty-port-space unregister body is private here; `bochs-core/iodev/pic.cc` retains only the one-call member boundary. | `iodev/pic_lifecycle.cc`, `../bochs-core/iodev/pic.{cc,h}` |
| `BX-MACH-027` | Bochs keyboard initialization assumes plugin, SIM, GUI, mouse and paste product facilities. | The non-product creation and teardown bodies live here; original `keyboard.cc` retains only same-shaped plugin entry delegations and is the sole overlay caller. | `iodev/keyboard_lifecycle.cc`, `../bochs-core/iodev/keyboard.{cc,h}` |
| `BX-IO-025`, `BX-IO-029`, `BX-IO-030`, `BX-MACH-065`, `BX-MACH-082` | `devices.cc` normally composes a full Bochs device/product shell. | The original port lifetime/default-handler/dispatch bodies and new bounded no-device lifecycle are private here; `iodev` retains the smallest declared machine boundary. | `iodev/minimal_port_space.cc`, `../bochs-core/iodev/{devices.cc,iodev.h}` |
| `BX-UD-001`, `BX-UD-002` | Bochs normally delivers `#UD` through its native exception path only. | The admitted fixed-width copied-event/result mechanism is private here. `exception.cc` makes one selector-blind member call; private `opaque_callback.cc` owns registration state. App composes the opaque callback; no adapter is included by `bochs-core`. | `cpu/exception_hook.cc`, `cpu/opaque_callback.{cc,private.h}`, `../bochs-core/cpu/{cpu.cc,exception.cc}` |
| `BX-MEM-024`, `BX-CORE-DIV-006` | Bochs normally initializes RAM through SIM and exposes no checked copied-RAM helper. | The SIM-free initializer and checked backing/ordinary RAM copy bodies are private member definitions here; the mirror retains only declarations. | `memory/minimal_memory.cc`, `memory/checked_ram.cc`, `../bochs-core/memory/{memory.h,misc_mem.cc}` |
| `BX-EXEC-016` | Bochs has no externally assembled real/V86 profile entry transaction. | Profile-state initialization and real-mode state/entry bodies are private member definitions here; core keeps declarations and one constructor call. | `cpu/realmode_profile.cc`, `../bochs-core/cpu/{cpu.h,init.cc}` |
| `BX-DPMI-MECH-001` | Bochs normally enters protected handlers only through decoded guest instructions and native exception paths. | A recovered historical host call must atomically restore a same-privilege protected CPU state after all selector/descriptor checks have passed. | This private member body validates copied selectors and EIP before altering any architectural field, then restores data segments, SS, CS, GPRs, flags and EIP. It owns no DPMI/BOP/VDM semantics. | `cpu/protected_transition.cc`, `../bochs-core/cpu/cpu.h` |
