# T95 S7 BOP 5F Bochs Owner Map 001

## Reached contract

`dos/v86/doskrnl/bios/spckbd.asm:2059-2110` names BOP `5F` directly: it sends
"interesting addresses to softpc C BIOS."  It constructs `kio_table`, places
its resident-code segment in DS, table offset in SI, an iret-BOP table offset
in DI, its entry size in CX, and `VERSIONID = BEEFh` in AX.  The immediately
following `JC isk_int9` selects the x86 vector path.

The historical handler `MS_bop_F` invokes `kb_setup_vectors`, releases
historical ICA interrupt delay, and sets CF when `MONITOR` is defined
(`softpc.new/host/src/nt_bop.c:727-764`).  In this NTVDM source mode, CF=1 is
therefore the guest-visible completion condition for the reached x86 branch.

## Field and owner mapping

| Historical data/effect | Historical consumer | Bochs replacement owner | First-profile disposition |
| --- | --- | --- | --- |
| `DS:SI` kio table: key maps, C-BIOS callback addresses, idle counters | SoftPC `kb_setup_vectors` / C keyboard, video, timer helpers | No adapter consumer. Bochs's native 8042 model owns keyboard hardware; guest NTIO remains owner of its own resident tables. | Deliberately not decoded or copied. It would reintroduce SoftPC C-BIOS semantics into the adapter. |
| `DI/CX` iret-BOP table | SoftPC monitor iret hook machinery | No equivalent admitted: Bochs CPU interrupt delivery owns its own mechanics. | Unavailable/denied for first CLI profile; no WOW/monitor iret-hook feature follows. |
| `DelayIrqLine=0` and `ica_restart_interrupts` | SoftPC host ICA, which had blocked IRQs before `spckbd` loaded | Bochs native PIC/PIT/8042 lifecycle, initialized before the guest plan: `main.cc:1369`, `iodev/devices.cc:189-208`, `keyboard.cc` ports/IRQ1/IRQ12, and `pit.cc` IRQ0. | Required machine composition predicate: retain native PIC/PIT/keyboard owners; do not call SoftPC ICA or inject an interrupt from adapter. |
| Carry flag | Guest `JC isk_int9` after BOP | Adapter fixed-width CPU result v2 | Resume at BOP+3 with CF=1, preserving all GPR/segment/memory state. |

## Decision

Admit one exact real-mode `C4 C4 5F` endpoint only when AX is `BEEFh`.
It expresses the observed x86 completion: resume after three bytes and set
CF.  It does not parse `DS:SI`, inspect `DI/CX`, read guest RAM, call a Bochs
device, add a host event/input path, alter PIC/PIT/keyboard state, or claim
that historical monitor/WOW iret hooks exist.

This is not a replacement SoftPC BIOS.  It replaces the small guest-visible
completion of a removed SoftPC-to-host wiring step, while the machine side is
owned by already initialized native Bochs devices.  Any future minimal Bochs
composition must prove those three native owners remain present before this
endpoint is enabled.  If it does not, `5F` must stay pass-through rather than
being used to mask a missing device.

## Verification

Use a focused C11 test for exact selector, real mode, AX version, BOP+3,
CF=1 and empty GPR delta; reject wrong AX, mode, selector and overflow.  Then
run one fresh MSVC root/trace only after its source closure records both this
adapter endpoint and the diagnostic listener change.  A repeated `5A` wait is
expected evidence of absent CLI input, not authorization to add synthetic
keyboard input.
