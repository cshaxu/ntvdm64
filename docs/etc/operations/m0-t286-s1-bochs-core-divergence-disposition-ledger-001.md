# M0 T286 S1 — Bochs-core divergence disposition ledger

## Scope and method

This is the complete T286/S1 denominator. It compares the current
`src/bochs-core` path set with the pinned
`O:\repos.external\bochs-2.6-compat\bochs-2.6` baseline, excluding
component READMEs and source paths not selected for the adopted core. It also
counts every non-README source/header body under `bochs-core-overlay`.

The audit is source and boundary evidence only. It does not use `src.old`,
enable a BOP, or alter a CPU/device contract.

## Baseline

- 29 adopted upstream-relative files have non-identical content.
- Their aggregate baseline is 431 inserted and 353 deleted lines relative to
  the pinned upstream files. This count includes source-root include changes
  and true-subset crops, not only executable behavior.
- 15 private overlay source/header files total 1,164 physical lines.
- There are no production source files under `bochs-core` that lack an
  upstream-relative counterpart. `README.md` is component provenance, not
  adopted Bochs source.
- No source outside `bochs-core` includes an overlay header. The mirror itself
  is the only overlay consumer, as required.

## Complete disposition ledger

| Group | Current paths / symbols | Baseline change | Consumer and private-state fact | Disposition | Successor |
| --- | --- | --- | --- | --- | --- |
| B1 build-root includes | `bochs.h`, `cpu/i387.h`, `gui/siminterface.h`, `gui/paramtree.cc` | 21 added / 10 deleted | Required solely to use adopted-core headers with the current component root and MSVC. No runtime consumer. | retain as same-shaped binding-only changes | S6 remeasure/register check |
| B2 CPU5 host-width configuration | `config.h` | 14 / 2 | `adapter-bochs` admits CPU5/P-MMX guest configuration while host builds remain x86/x64. | retain; confirm generated-profile minimum | S2 configuration/lifecycle review |
| C1 real/V86 segment-limit profile state | `cpu/cpu.h`, `cpu/init.cc`, `cpu/cpu.cc`, `cpu/access.cc`, `cpu/access32.cc`, `cpu/ctrl_xfer16.cc`, `cpu/ctrl_xfer32.cc`, `cpu/exception.cc`, `cpu/icache.cc`, `cpu/io.cc`, `cpu/stack.cc`, `cpu/string.cc`; overlay `cpu/realmode_profile.cc` | 218 / 60 | The CPU paths alone can apply this compatibility predicate; `adapter-bochs` only selects it through a typed call. Profile state needs private CPU state. | required-private; reduce duplicate predicates only if one core-local helper preserves all instruction groups | S4 CPU profile review |
| C2 fixed-width #UD callback | `cpu/cpu.h`, `cpu/cpu.cc`, `cpu/exception.cc`; overlay `cpu/opaque_callback_private.h`, `cpu/opaque_callback.cc`, `cpu/exception_hook.cc` | 100 / 6; 252 overlay lines | `adapter-bochs` binds one opaque callback; only the CPU owns exception delivery and copied register application. No selector resides here. | required-private; retain only the single exception call and opaque registration boundary | S4 CPU callback review |
| C3 segment-access observer | `cpu/access32.cc`; overlay `cpu/segment_access_hook.cc`, `cpu/observation_gates.h` | included in C1 source paths; 94 overlay lines shared | No current product consumer is named; the callback is diagnostic-only and default-off. | optional-remove | S4 observer removal |
| C4 instruction/software-interrupt/physical-write observers | `cpu/cpu.cc`, `cpu/cpu.h`, `cpu/instr.h`, `cpu/soft_int.cc`, `memory/memory.cc`; overlay `cpu/instruction_history_hook.cc`, `cpu/software_interrupt_hook.cc`, `cpu/physical_write_hook.cc`, shared `cpu/observation_gates.h` | 109 / 6; 142 overlay lines including shared gates | No current product consumer is named; copied observations do not participate in BOP ingress or bounded execution. | optional-remove | S4 observer removal |
| C5 interrupt-return observer | `cpu/ctrl_xfer16.cc`, `cpu/ctrl_xfer32.cc`; overlay `cpu/interrupt_return_hook.cc`, shared `cpu/observation_gates.h` | 24 / 16; 97 overlay lines including shared gates | No current product consumer is named; it is diagnostic-only. | optional-remove | S4 observer removal |
| M1 SIM-free memory/bootstrap | `memory/memory.h`, `memory/misc_mem.cc`; overlay `memory/minimal_memory.cc` | 39 / 1; 36 overlay lines | Original memory setup is SIM/product-shell coupled. The finite machine requires an explicit private member initializer. | required-private; externalize only size/config selection | S5 memory review |
| M2 checked ordinary/backing RAM | `memory/memory.h`; overlay `memory/checked_ram.cc` | included in M1; 153 overlay lines | Checked copied RAM uses private memory tables; `adapter-bochs` must not expose a Bochs object. | required-private | S5 memory review |
| D1 minimal port-space lifetime | `iodev/devices.cc`, `iodev/iodev.h`; overlay `iodev/minimal_port_space.cc` | 17 / 162; 396 overlay lines | Original device init drags SIM/plugins/product devices. Arrays/default handlers require private `bx_devices_c` state. | required-private; review upstream no-product reuse and remove duplicate assembly | S3 device review |
| D2 PIC teardown | `iodev/pic.cc`, `iodev/pic.h`; overlay `iodev/pic_lifecycle.cc` | 23 / 3; 22 overlay lines | The finite native PIC assembly is selected by `adapter-bochs`, but private port registration teardown stays with the mirror. | required-private | S3 device review |
| D3 headless keyboard lifecycle | `iodev/keyboard.cc`, `iodev/keyboard.h`; overlay `iodev/keyboard_lifecycle.cc` | 72 / 82; 40 overlay lines | Current no-GUI machine still needs original 8042-facing private device state; plugin/GUI portions are cropped. | required-private; re-evaluate a smaller upstream lifecycle | S3 device review |
| D4 finite PC product-shell crop | `pc_system.cc` | 3 / 31 | `adapter-bochs/minimal_product_shell.cc` supplies finite Reset/exit behavior. The mirror crop avoids full GUI/device reset sequencing. | external-composition candidate; verify whether this can become a pure subset with no changed core body | S2 lifecycle review |

## Resulting implementation order

1. S2 considers B2 and D4 only: configuration/product-shell decisions that
   may belong in `adapter-bochs` without private state.
2. S3 resolves D1--D3 as a single machine-device lifecycle group.
3. S4 first deletes C3--C5 unless a currently admitted product contract is
   discovered; it then retains/minimizes C1--C2 as the only CPU-private group.
4. S5 resolves M1--M2.
5. S6 remeasures the exact file/line/overlay totals and verifies both host
   architectures.

No group transfers BOP, OpenNT, DOS, VDM, WOW or Win32 semantics into a
Bochs component. The only proposed deletions are default-off diagnostics with
no current named product consumer.
