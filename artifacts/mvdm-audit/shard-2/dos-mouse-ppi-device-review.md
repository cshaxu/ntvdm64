# Shard 2: DOS bus-mouse and PPI device owners

Two pending source files match both original trees. The observer and JSON record exact current compile edges, normalized header dependencies and worker map symbols. Both are **partial** candidates; identity and linkage do not establish functional parity.

| Entry | Source and current build evidence | Bounded recovery work |
| --- | --- | --- |
| `softpc.new/base/keymouse/mouse.c` | `mouse_init` defines input/output port handlers and connects the declared mouse port range. The current object is compiled into the keymouse library; captured worker map names `mouse_init`, `mouse_inb` and `mouse_outb`. | Verify that the intended reset path calls registration under the selected profile, then exercise port reads/writes, IRQ delivery/acknowledgment and reset/reinitialization with a guest/device witness. This legacy bus-mouse adapter is not automatically the same path as the host cursor, PS/2 mouse, or Windows 3.1 Video7 rendering. |
| `softpc.new/base/keymouse/ppi.c` | `ppi_init` defines its port handlers, connects the source-specified odd ports and resets `ppi_register`. The current object is compiled into the keymouse library; the map names its init/in/out functions. | Verify reset/reinit, port state and reads/writes in the selected machine configuration, including interactions with keyboard/speaker paths, before asserting behavior. |

Current `reset.c` contains calls to both init functions, but source references and mapped functions do not prove that a particular worker executes that reset branch or that I/O dispatch reaches the correct callback. `mouse.h` and `ppi.h` are confirmed in their corresponding object's Ninja dependency lists after resolving paths from the worker build directory. Other port providers, interleavings, errors and lifecycle remain untested. Do not infer that matching these two original source files proves complete DOS or Win16 mouse behavior.

Replay `python artifacts/mvdm-audit/shard-2/dos-mouse-ppi-device-observer.py` and compare its JSON; graph/map hashes describe the captured configuration. No product, guest, installation, shared ledger or build tree was modified.
