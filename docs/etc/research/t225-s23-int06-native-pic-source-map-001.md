# T225 S23 P1 — INT 06 Handler and Native PIC Whole-Package Source Map

## Scope

This map covers one reached machine package only: the original SoftPC INT 06
handler pair and the minimum native Bochs PIC lifecycle it requires. It does
not authorize any DOS/DEM/COMMAND/debugger provider or a general device shell.

## Original OpenNT Contract

| Original unit | Required semantic operation | Required order/failure property |
| --- | --- | --- |
| `base/mvdm/dos/v86/doskrnl/bios/spckbd.asm:int06h_vector` | Enter `BOP 06h`, then execute `iret_com`. | The BOP operates on the already-pushed real-mode #UD frame; it is not the original faulting instruction. |
| `base/mvdm/softpc.new/base/system/illegalp.c:illegal_op_int` | Obtain `SS:ESP`, read saved IP:CS, report historical illegal-op condition, advance saved IP by one with `FFFFh` segment carry, call `unexpected_int`. | Frame mutation precedes `unexpected_int`; the following guest IRET consumes the altered saved frame. |
| `base/mvdm/softpc.new/base/system/unexp_nt.c:unexpected_int` | Read master PIC ISR; when hardware, read/mask master/slave PIC as needed, issue EOI, then write the interrupt cause to BDA `0x46B`. | It owns the PIC state policy and BDA cause write. It does not select the BOP or emulate guest IRET. |

The two original C translation units are an x86 clang-cl historical island.
They cannot be linked into the current MSVC x64 `/MT` process; the original
logic, order and failures therefore remain the mandatory source reference for
a smallest source-derived x64 rehost, not a copyable product object.

## Native Bochs PIC Candidate

The exact adopted input candidate is `refs/bochs/iodev/pic.cc` SHA-256
`BD6760AA5310CABFF4809751AF3FAB6A0A0A5AA006A2FA00202C9A4B5BB24831` and
`pic.h` SHA-256 `5CCFC3F101BE896C561861AD26DD0B0557C2A4CE9E8AF010E1DAB3C4E255C6D8`.
Its `init()` registers byte port handlers at `20h`, `21h`, `A0h`, and `A1h`,
initializes master/slave PIC state (masked initially), and supplies the normal
PIC acknowledgement/INTR implementation. These are exactly the ports touched
by original `unexpected_int` and the native FPU path already uses
`DEV_pic_raise_irq(13)`.

| Candidate dependency | Classification | S23 disposition |
| --- | --- | --- |
| `pic.cc` / `pic.h` 8259 state and byte-port handlers | Required native Bochs logic | Candidate for direct adoption into bx-mantle, with source identity preserved. |
| Existing bx-mantle empty port registry and `bx_devices.inp/outp` | Required existing mantle facility | Retain; it is the registrar/dispatcher, not a new host service. |
| `bx_pc_system::IAC`, CPU `DEV_pic_iac`, `DEV_pic_raise_irq` | Required existing bx-core/bx-mantle machine connection | Retain and test through the native PIC pointer; no selector data crosses it. |
| `pluginPicDevice` pointer | Required finite device binding | Mantle must set/restore this pointer during its own lifecycle; it is not an adapter capability. |
| `SIM` state registration and debugger device records | Default Bochs product support, not required by the reached handler | Explicitly reject from the minimal lifecycle; no `register_state`, debugger record, plugin loader or full configuration shell is admitted. |
| Other devices (PIT, keyboard, CMOS, DMA, VGA, PCI, storage, network, sound) | Not shown necessary by this handler path | Explicitly reject. |
| Empty port-space `FFh` reads/ignored writes | Existing fallback only | Retain for unmapped ports but reject as a substitute for PIC ports. |

## Architecture Boundary

`bx-core` continues to emit only the fixed generic #UD event and apply typed
resume/stop results. It receives no selector, OpenNT or SoftPC knowledge.
`bx-vdm` alone recognizes that the copied event is machine selector `06h` and
chooses the machine-plane route. `bx-mantle` owns native PIC construction,
port binding, saved-frame ordinary-RAM access and a fixed-width mechanical
result. `src/opennt` remains the semantic owner/reference of the handler
algorithm; no historical x86 object enters the product link.

The old `src/bx-vdm/bx_ntvdm_machine_composition_v2.c` is therefore not an
accepted implementation: it combines selector routing with global callback
emulation and assumes an x86 handler object. It is a migration input only.

## Four-Rung Recovery Ledger

| Rung | Finding | Disposition |
| --- | --- | --- |
| Original source reuse | `illegalp.c`/`unexp_nt.c` source and exact operation ordering exist, but their translation units are only proven in an x86 clang-cl island. | Retain as mandatory reference; direct product composition rejected by architecture/ABI. |
| Smallest shim | A typed mantle call can expose only ordinary-RAM byte operations, native port dispatch and a diagnostic result to bx-vdm selection. | Candidate, but must be defined and tested before use. |
| External-code intrusion | Direct adoption of the reached Bochs PIC source into bx-mantle may be necessary. It requires prior exception-registry identity and a finite lifecycle; no bx-core semantic patch is implied. | Pending source-motion/build audit. |
| New behavior | An x64 source-derived rehost of only `illegal_op_int`/`unexpected_int` may be necessary because the original source island cannot compose. | Pending exact behavior test; no free-form replacement or invented PIC response is permitted. |

## Next S23 Work

P2 must audit the minimum Bochs PIC constructor/destructor and macro/link
closure, then decide whether direct source motion or a narrow mantle wrapper
can retain the exact PIC code without SIM/product-shell dependencies. No
handler, PIC source, manifest, runtime path or native trace changes are made
by P1.