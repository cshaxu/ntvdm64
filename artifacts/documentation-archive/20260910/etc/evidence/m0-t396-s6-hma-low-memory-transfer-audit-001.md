# M0 T396 S6 — HMA low-memory child-transfer audit 001

## Question

After the repaired original WOW16 `KRNL386.EXE` reaches NTDOS `$Exec`, what source-owned mechanism transfers to the child, and is the observed `FFFF:FFFF` target attributable to the original A20/stack profile or to a separate current CPU compatibility divergence?

## Scope and method

This is an audit, not a repair. It examines the original `disa20_xfer` low stub and its reached CCPU/SAS/XMS machinery. `disa20_iret` is explicitly excluded: it is a different interrupt-return route beginning after the `disa20_xfer` body. No guest source, guest media, BOP result, CPU opcode, or loader behavior changed.

## Original mechanical ledger

`src/mvdm-guest/dos/v86/inc/lmstub.asm` defines `disa20_xfer` at map offset `0794:10F5`. Its ordered contract is:

1. Call `XMMDisableA20`, which invokes `XMMcontrol` with `XMM_LOCAL_DISABLE_A20` (`AH=06h`).
2. Issue `SVC_DEMENTRYDOSAPP` (`50:36`) while the DOS stack is still active.
3. Clear DOS interrupt state and `InDos`.
4. Load child `SS:SP` from `AX:DI`, enable interrupts, and push `DS:SI`.
5. Establish `ES`/`DS` from `DX`, restore `AX` from `BX`, then far-return to the pushed child entry.

The established trace reaches this exact sequence: `50:36`, `00A7:1105 mov ss,ax`, `1107 mov sp,di`, `1109 sti`, `110A push ds`, `110B push si`, and its final `retf` at `00A7:1113`. The latter is an instruction location within `disa20_xfer`; it is not the symbol `disa20_iret`.

## Current provider ledger

| Group | Current source-shaped route | Audit result |
| --- | --- | --- |
| A20 ownership | `XMMDisableA20` → original XMS local-disable service → `xmsEnableA20Wrapping` → `sas_enable_20_bit_wrapping` → CCPU `SasWrapMask=0x000fffff` | Reached owner is original XMS/SAS. The target child-stack address `0xA006C` is below 1 MiB, so this mask cannot by itself turn that stack word into `FFFF:FFFF`. |
| Real-mode segment load | `c_seg.c::load_stack_seg` records `SS=selector`, `base=selector << 4` in real mode | The source route is intact for `SS=9FFF`, hence base `0x9FFF0`. |
| Stack pushes | `c_stack.c::spush16` → virtual write → `phy_w16` | The two far-return words should occupy physical `0xA006C`/`0xA006E` for the traced `SP=0x007C`; this is normal RAM, not an A20 wrap address. |
| Physical store | `ccpusas4.c::phy_w16` masks with `SasWrapMask`, verifies SAS type and calls `write_word` for RAM | Existing selected-image SAS observation is attached to `c_sas_store[w|dw]`, not `phy_w16`; it cannot prove these stack writes. |
| Control fetch | CCPU real-mode far return reads two words at child `SS:SP` and transfers to them | Current trace reads `FFFF:FFFF`, establishing that the failure is already present at the transfer fetch boundary. |

## Bounded runtime evidence and limitation

The earlier formal short-root run used the current staged product and media (`ntvdm32.exe` SHA-256 `f0fe06be5068052428a2451f8a50940668decd3a688a8fc685dd415da0026a44`) and recorded the complete route above in `build/M0-T396/S5-stage/wow-fixexe-short.events.txt`.

S6 rebuilt the Console-owning observer after correcting it to retain caller-supplied BOP/SAS report paths instead of overwriting them with defaults. Two 15-second, same-product short-root attempts (`hma-bop-observation.txt` and `hma-bop-without-observation.txt`) timed out with the same loaded-module shape but produced no caller-selected BOP or SAS report. That is an observation-container discrepancy, not proof that the original route did not execute; the identity-matched earlier trace remains the positive route evidence. The selected SAS hook also cannot see `phy_w16` stack stores by design.

## Result and next owner boundary

S6 rules out malformed WOW16 MZ layout and simple A20 address wrapping as the explanation. The first unresolved contract is the source-shaped pair of `phy_w16` stack writes and the immediately following far-return fetch. A subsequent implementation packet must add a finite, default-off probe at that exact physical-store/fetch boundary, preserve all CPU semantics, and compare the requested address, masked address, RAM classification, stored words, and fetched far target before proposing any compatibility repair.
