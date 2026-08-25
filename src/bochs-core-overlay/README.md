# bochs-core-overlay

Private implementation fragments for narrowly registered Bochs-core mirror
divergences.  This component has no public ABI and may be called only by its
native mirror component, `bochs-core`; no adapter, host, BOP, session or app
source may include or link to it directly.

## Exception index

| ID | Original purpose | Divergence reason and implementation | Files |
| --- | --- | --- | --- |
| `BX-MACH-024` | Bochs PIC teardown is normally owned by the complete product/device shell. | The finite empty-port-space unregister body is private here; `bochs-core/iodev/pic.cc` retains only the one-call member boundary. | `iodev/pic_lifecycle.cc`, `../bochs-core/iodev/pic.{cc,h}` |
| `BX-MACH-027` | Bochs keyboard initialization assumes plugin, SIM, GUI, mouse and paste product facilities. | The existing headless controller method bodies live here; `bochs-core/iodev/keyboard_bridge.cc` is the only caller. | `iodev/keyboard_headless.cc`, `../bochs-core/iodev/keyboard.h`, `../bochs-core/iodev/keyboard_bridge.cc` |
