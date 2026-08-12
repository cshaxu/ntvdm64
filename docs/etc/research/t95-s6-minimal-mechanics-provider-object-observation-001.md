# T95 S6 minimal-mechanics provider object observation 001

## Question

Can the smallest currently selected original Bochs mechanism objects provide
the CPU island's non-CRT requirements without importing the Bochs product
shell?

## Inputs and procedure

The fresh CPU3/MSVC x86 root
`artifacts/analysis/t95-s6-msvc-x86-minimal-mechanics-objects-001-20260811-001`
contains exactly these ten compile inputs:

1. `bx_ntvdm_minimal_machine.cc`
2. `bx_ntvdm_minimal_sim.cc`
3. `memory/memory.cc`
4. `memory/misc_mem.cc`
5. `iodev/minimal_port_space.cc`
6. `iodev/devices.cc`
7. `pc_system.cc`
8. `logio.cc`
9. `gui/siminterface.cc`
10. `gui/paramtree.cc`

Each source received one BuildTools x86 `/nologo /c /Gy /MT` invocation with
the accepted CPU3 projection. All ten objects were produced with exit code
zero and only the known `FMT_LL` warning. A single `dumpbin /symbols` pass
then examined them together with the frozen 93-object CPU island. No source
or configuration change, library, link, executable or run occurred.

The mechanism symbol dump hash is
`E517B1E732E391B01983B6684156DB749D5D4032FC0E907C485DAA7D1939BA0C`;
the result and object-hash inventories are respectively
`0BB581D01288492CE19E9DBDD83997AA9E6B9DDDE5D30D9C44626CFE72506463` and
`6610B323B098FF648A7071A10E2E8A9CA5F7689017253CB12315AE5B6021380D`.

## Positive result

The ten mechanism objects provide all 28 non-CRT identities required by the
CPU island: its original logging methods, SIM/parameter constructors, four
machine globals, memory helpers, PC reset/countdown, and I/O `inp/outp`.
The CPU's direct deficit consequently falls to the normal MSVC CRT surface
only. This proves provider availability, not link closure.

## Retention result

Across the 93 CPU and 10 mechanism objects, 139 unresolved identities remain
after same-set providers are eliminated. The machine-readable union ledger is
`logs/union-unresolved.json`, SHA-256
`5808E58EF9F8E9D162199C5D5328BA97BDB7C9664BAB1ADCE84B3A147B72EA9D`.
The non-CRT product-shell roots are source-object-specific:

| Object | Retained product owners | Consequence |
| --- | --- | --- |
| `iodev/devices.cc` | plugin initialization, CMOS/DMA/floppy/hard-drive/keyboard/PIC/PIT/VGA entries, virtual/slowdown timers, GUI global | Full device assembly is retained by the global/port implementation object. |
| `gui/siminterface.cc` | real-SIM deleting destructor, Bochs configuration read/write/start functions, NIC/USB option parsing, optional plugin loading, GUI/timer globals | The original SIM construction path retains configuration/product lifecycle. |
| `pc_system.cc` | GUI global and cleanup | Product shutdown is retained with the PC-system lifecycle object. |
| `logio.cc` | `bx_atexit` | Original fatal path retains product exit ownership. |

The remaining unresolved identities also include normal MSVC CRT support and
the corresponding virtual deleting destructors. `main.cc` is absent and must
remain absent: adding it would merely hide the product-shell roots behind the
historical startup composition.

## Decision

The 103-object candidate (93 CPU + 10 mechanisms) is **not admissible for a
link observation**. Its positive provider result must not be mistaken for a
minimal-machine closure. There is no safe next “one more helper” move:
the retained paths are class/vtable/lifecycle ownership, not isolated missing
functions.

Any future change must be a separately designed, cohesive Bochs-internal
product-shell extraction that treats SIM, device-manager global/lifecycle,
PC-system shutdown and logging shutdown together. It must first prove a
stable original ownership boundary and be evaluated against the no-rewrite
rule. Adapter/OpenNT code cannot supply any of these symbols. Until such a
design exists, no link, device enablement, `main.cc`, GUI/configuration/plugin
provider, adapter or OpenNT addition is authorized.
