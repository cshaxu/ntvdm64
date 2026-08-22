# M0 T240 SoftPC BIOS Conventional-Memory Recovery Plan

## Task boundary

T240 is the first queue candidate admitted after the external-compatibility
roadmap split. It recovers one reached SoftPC BIOS component, not a general
machine profile: reset-owned conventional-memory data and the exact `BOP 12h`
query consumed by the source-built DOS bootstrap after `50:11`.

The working inventory is `BOP-DEPENDENCY-091`. This row is a machine/BIOS
dependency, not a new member of the canonical 203 host-service BOP inventory.

## Ordered S plan

| S | Scope | Required exit before the next S |
| --- | --- | --- |
| S1 | Audit `bios.c`, `mem_size.c`, `reset.c`, their memory macros, the guest caller, current mantle lifecycle, current adapter helper/ingress and formal manifest. Produce a four-rung source-recovery, reset/value and ABI ledger. | **Complete.** Exact original ownership and first-profile value provenance are reconciled; a minimal typed seam is specified; all unrelated current helper behavior is classified. |
| S2 | Bind and recover the source-shaped query through the existing selector-blind checked-RAM read action. `bx-mantle` sees only physical address/length; the BDA address and original read-to-AX meaning remain in `bx-vdm`. | **Implementation complete.** The direct `mem_size.c` mirror, its uniquely named SAS/AX shim and active generic-UD route pass mutation/negative local tests and the formal Ninja fixture. One bounded native observation remains the S2 integration exit. |

## Non-negotiable exclusions

- Do not retain the old adapter-local `BOP 12h`/`15h/AH=88h` coupled helper as product behavior merely because it has a fixture.
- Do not include keyboard/INT15, PIC/IRQ, FPU, CMOS, VGA, FDC/DMA, block media, generic BIOS ROM, UMB, DPMI, guest EXEC or COMMAND work.
- `bx-vdm` alone may recognize a BOP instruction window and the source-defined BDA address; `bx-mantle` receives only a selector-blind fixed-width checked RAM read.
- A native trace verifies the completed component; it cannot allocate a new service or device task.

## Required evidence record for S1

The S1 record must name the exact OpenNT and current-source paths, caller form,
historical data address/representation, reset writer, current machine/profile
source, result register and instruction advance, direct-link feasibility,
smallest required compatibility seam, rejected external intrusion and rejected
new implementation alternatives. It distinguishes the mutable BDA word used
by `mem_size.c` from the firmware/reset mechanism which seeds its initial
value, and records why the snapshot's `0x027f` is or is not the declared
first-profile seed rather than silently using `host_get_memory_size()`'s
historical default.
