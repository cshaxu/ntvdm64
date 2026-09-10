# M0 T199 S11 Shared XMS/DPMI Machine Capability Plan

## Single-owner plan

The minimum machine already owns native reset, one-MiB RAM, empty port space,
CPU initialization, and startup A20 enablement. It does not yet expose runtime
machine services. XMS and DPMI therefore share the following future capability
packages; none may be implemented in bx-vdm.

| Capability package | bx-core owner | bx-mantle responsibility | bx-vdm responsibility | Consumers |
| --- | --- | --- | --- | --- |
| C1 A20 state | Bochs A20/wrap semantics | Fixed copied set/query ABI and lifecycle validation | Translate `52:00` source contract only after C1 exists | XMS, DPMI mode transitions |
| C2 extended-memory aperture | Bochs RAM mapping/ordinary RAM checks | Fixed allocation/handle/copy/query ABI; no host pointer | Translate XMS/DPMI copied structures and register errors | XMS blocks, DPMI memory |
| C3 UMB map | Native memory map | Reserve/request/release lifecycle over a real map | Translate XMS UMB errors only | XMS UMB, DOS bridge |
| C4 protected-mode state | CPU modes, descriptors, debug registers | Typed selector/mode/debug request ABI | DPMI source-shaped selection only | DPMI descriptors/debug |
| C5 interrupt/fault delivery | CPU exception, PIC, firmware vectors | Typed interrupt/fault lifecycle ABI | No delivery/emulation; only result translation | DPMI interrupts, XMS INT15 |
| C6 DOS transition/session | Guest control transfer and native memory | Explicit composition lifetime after C1-C5 | Copied DPMI/DOS request ownership | DPMI INT21/session |

The existing ordinary-RAM mechanical action remains C0, useful for bounded
guest copies but insufficient for C1-C6. `52:00..0B` and `53:00..18` remain
deferred except `53:12`, which stays explicitly unavailable due to VCD/VDD
host integration. No XMS/DPMI facade is admitted before a corresponding
capability is source-built and boundary-tested.

## Ordering

1. C1 and C2 prove native state mutation/query without BOP vocabulary.
2. C3 and C5 prove real map/interrupt preconditions, not synthetic devices.
3. C4 proves protected-mode/descriptors over C1-C2.
4. C6 is last because it combines guest control flow with the prior native
   capabilities.
5. Only then can one XMS package facade and one DPMI package facade be
   implemented against the same ABIs.

This avoids a second allocator, CPU-mode manager, or interrupt engine in
bx-vdm, and avoids importing SoftPC into bx-core/bx-mantle.
