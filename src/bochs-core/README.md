# bochs-core

Adopted Bochs 2.6 core: CPU/decode, memory, exceptions and admitted native
machine mechanics.

It may depend only on adopted Bochs/internal machine inputs, except for the
fixed copied-record declaration admitted by `BX-UD-001`. That declaration
does not grant core any OpenNT, DOS, VDM, WOW, BOP selector/service,
SoftPC/CCPU or Win32 policy knowledge.

## Formal input closure

T266/S2 imports the exact eleven Bochs 2.6 header/configuration inputs reached
by the current formal Ninja dependency database: `bochs.h`, `config.h`,
`osdep.h`, `plugin.h`, `extplugin.h`, `param_names.h`, `bx_debug/debug.h`,
`gui/gui.h`, `gui/siminterface.h`, `instrument/stubs/instrument.h`, and
`iodev/iodev.h`. They are byte-identical source mirrors, not divergences or
new semantics; their source/destination hashes are recorded in T266/S2
evidence. The imported `config.h` is the pinned formal CPU5 input consumed by
the build projection, not a new Bochs product configuration.
## Local intrusion register

Every modification to adopted Bochs source has one exception identifier.  The
table is the component-local index; the linked central record retains the
upstream identity, need, exact procedure, boundary and verification evidence.
No new core modification is admitted without both records.

| Exception | Adopted upstream surface | Narrow purpose |
| --- | --- | --- |
| `BX-MEM-024` | `memory/memory.h`, `memory/misc_mem.cc` | SIM-free reset-window RAM initialization. |
| `BX-UD-001` | `cpu/exception.cc`, `cpu/bx_ntvdm_exception_intercept.h` | Default-off generic copied-state `#UD` hand-off. |
| `BX-ABI-004`, `BX-ABI-005`, `BX-ABI-006`, `BX-ABI-008`, `BX-ABI-015`, `BX-ABI-018`, `BX-ABI-019`, `BX-ABI-036`, `BX-CORE-084` | `cpu/exception.cc` and its fixed copied-record headers | Selector-blind typed mechanical request/result transport only. |
| `BX-MEM-007`, `BX-MEM-010`, `BX-MEM-011`, `BX-MEM-012`, `BX-MEM-013`, `BX-MEM-020` | Bochs memory access boundary | Checked, copied ordinary-RAM mechanics with no host pointer export. |
| `BX-ABI-022`, `BX-ABI-027`, `BX-TRACE-017`, `BX-TRACE-035` | CPU observation/opt-in boundary | Default-off diagnostic and bounded observation controls. |
| `BX-MACH-023`, `BX-IO-025`, `BX-IO-029`, `BX-IO-030`, `BX-MACH-026`, `BX-MACH-009`, `BX-EXEC-014`, `BX-EXEC-016`, `BX-EXEC-032`, `BX-EXEC-034`, `BX-ABI-053` | Native reset/machine/port composition | Minimal Bochs-native composition; no BOP or OpenNT semantics. |
| `BX-BUILD-002`, `BX-ABI-028` | Adopted build/source compatibility edge | Toolchain lexical compatibility only. |

The canonical detailed register is
[`adapter-external-intrusion-exceptions.md`](../../docs/etc/research/adapter-external-intrusion-exceptions.md).
The S3 rename of `native_port_is_registered` is a mantle-local neutralization
of a project-specific name; its declaration remains in the adopted `iodev.h`
surface and is covered by `BX-IO-025`.

## Per-file mirror divergence index

This index is deliberately file-granular: the local `DIVERGENCE(<ID>)` marker
in each named adopted file identifies the precise retained change group. The
central register supplies the original admission evidence; this table does not
authorize additional Bochs semantics.

| Local ID | Original surface and retained minimal change | File |
| --- | --- | --- |
| `BX-CORE-DIV-001` | Real/V86 segment-limit compatibility profile guards and its state/entry propagation. | `cpu/access.cc`, `cpu/access32.cc`, `cpu/cpu.cc`, `cpu/cpu.h`, `cpu/ctrl_xfer16.cc`, `cpu/ctrl_xfer32.cc`, `cpu/exception.cc`, `cpu/icache.cc`, `cpu/init.cc`, `cpu/io.cc`, `cpu/stack.cc`, `cpu/string.cc` |
| `BX-CORE-DIV-002` | Default-off copied segment-access observation; its gate/no-op macro scaffolding is private-overlay code and the mirror retains only the native fault call sites. | `cpu/access32.cc`, `../bochs-core-overlay/cpu/observation_gates.h` |
| `BX-CORE-DIV-003` | Default-off copied instruction, software-interrupt and physical-write observations; shared gate/no-op scaffolding is private-overlay code. | `cpu/cpu.cc`, `cpu/cpu.h`, `cpu/instr.h`, `cpu/soft_int.cc`, `memory/memory.cc`, `../bochs-core-overlay/cpu/observation_gates.h` |
| `BX-CORE-DIV-004` | Default-off copied interrupt-return observation; its shared gate/no-op scaffolding is private-overlay code. | `cpu/ctrl_xfer16.cc`, `cpu/ctrl_xfer32.cc`, `../bochs-core-overlay/cpu/observation_gates.h` |
| `BX-UD-001`, `BX-UD-002` | `exception.cc` retains one selector-blind private-overlay member call; copied record construction, validation and result application live in the private overlay. | `cpu/exception.cc`, `cpu/cpu.{h,cc}` |
| `BX-EXEC-016` | One constructor call and typed declarations only; private overlay owns real-mode profile initialization, entry setters and fetch invalidation. | `cpu/init.cc`, `cpu/cpu.h`, `../bochs-core-overlay/cpu/realmode_profile.cc` |
| `BX-BUILD-002` | Re-rooted FPU include spelling only; no declaration or behavior change. | `cpu/i387.h` |
| `BX-BUILD-002` | MSVC token-separation spelling for the original `FMT_LL` hex format; no format or runtime behavior change. | `gui/paramtree.cc` |
| `BX-MACH-023` | True-subset PC-system mirror omits the original full product-shell `Reset` and `exit` methods; the bounded Bochs-only lifecycle is assembled outside the mirror. | `pc_system.cc` |
| `BX-MACH-024` | One-call PIC teardown boundary delegates finite port unregistration to the private `bochs-core-overlay`; no non-core component imports the overlay. | `iodev/pic.h`, `iodev/pic.cc` |
| `BX-MACH-025` | True-subset PIC destructor omits removal of a product-tree node that this finite composition never creates. | `iodev/pic.cc` |
| `BX-MACH-026` | True-subset PIC mirror omits unreachable plugin-registration entry points; minimal Bochs assembly owns the one fixed PIC instance. | `iodev/pic.cc` |
| `BX-MACH-027` | The same-shaped original keyboard plugin lifecycle retains only a minimal create/destroy delegation; both non-product lifecycle bodies are private to the overlay because the original plugin/SIM/GUI product shell is not composed. | `iodev/keyboard.{cc,h}`, `../bochs-core-overlay/iodev/keyboard_lifecycle.cc` |
| `BX-MEM-007`, `BX-MEM-010`, `BX-MEM-020`, `BX-CORE-DIV-006` | Checked ordinary/backing-RAM copy/read/preflight declarations; private overlay owns all bodies. | `memory/memory.h`, `../bochs-core-overlay/memory/checked_ram.cc` |
| `BX-MEM-024` | SIM-free reset-window memory declaration and minimal-machine friendship; private overlay owns the allocation body. | `memory/memory.h`, `../bochs-core-overlay/memory/minimal_memory.cc` |

`cpu/bx_ntvdm_exception_intercept.h` was a project-authored, zero-consumer
legacy declaration and is intentionally deleted by T262/S2 rather than being
treated as a mirror exception.

The static library name is `bochs-core` and its public include surface is
only the explicitly declared Bochs mechanical API consumed by `adapter-bochs`.
