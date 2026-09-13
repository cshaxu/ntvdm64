# M0 T407 S10 — DOSX IDT Lifecycle Recovery

## Decision

The prior CPU40 IDT carrier was not source-correct. It inferred an IDT base
from the first `53:00` table-address BOP and retained that value forever.
That BOP exposes DOSX's temporary low-memory construction buffer, which
original `MoveDscrTables` copies to XMS and then frees. The retained address
could therefore become stale and the protected interrupt path rewrote IDTR on
every dispatch.

S10 removes that latch. At `53:01`, CPU40 now resolves the final `SEL_IDT`
descriptor (selector `88h` for the selected `IFDEF WOW` profile) from the
already published GDT shadow and installs its base and limit once. It does not
rewrite IDTR during each protected interrupt.

## Original Ordering Proof

| Original operation | Source | Consequence retained here |
| --- | --- | --- |
| The temporary block lays out IDT immediately before GDT. | `src/mvdm-host/dpmi/486/dxboot.asm`, `CBIDTOFF`/`CBGDTOFF` definitions. | This explains why the old subtraction located only the temporary buffer. |
| `PassTableAddress` publishes the temporary GDT segment. | `dxboot.asm`, `AllocateExtMem`. | `53:00` is not final-IDTR publication. |
| `MoveDscrTables` copies IDT/GDT to XMS and frees the low-memory copy. | `dxboot.asm`, `MoveDscrTables`. | A process-lifetime `53:00` address is invalid. |
| `InitIntrDscrTable` sets `bpIDT` and calls `NSetSegmentDscr(SEL_IDT, ...)`. | `dxboot.asm`, `InitIntrDscrTable`. | The published `SEL_IDT` descriptor is the final base/limit authority. |
| `WOW_x86` omits direct `LIDT` and enters through `DPMISwitchToProtectedMode`. | `src/mvdm-host/dpmi/486/dxutil.asm`. | CPU40 installs that final descriptor at the equivalent `53:01` boundary. |

`SEL_IDT` is `88h` in the original WOW profile at
`src/mvdm-host/dpmi/pmdefs.inc`. The normal descriptor-publish path already
copies TI=0 entries to the CPU40 GDT shadow in `dpmi32/dpmiselr.c`; no new
guest table or IDT gates are invented.

## Code Disposition

| Surface | Before | After |
| --- | --- | --- |
| `53:00` | Derived and latched a presumed IDT base. | Performs only its original table-address and descriptor-shadow work. |
| `53:01` | Installed the latched construction-buffer base. | Resolves the final published `SEL_IDT` descriptor and installs it once. |
| Protected interrupt dispatcher | Reinstalled the latched IDTR on every entry. | Leaves CPU architectural IDTR state intact. |
| Worker lifecycle | Static latch had no reset mechanism. | No IDT latch exists. A fresh worker with no published descriptor fails the transition before PE, so it cannot inherit a stale IDTR. |

This is a source-shaped CPU40 binding, not a claim that the original x86
kernel's VDM context machinery is composable in user mode. The original
kernel performs the actual x86 task/context work; the selected CCPU worker
uses the same final DOSX descriptor as its architectural input.

## Verification

| Check | Result |
| --- | --- |
| Focused fixture | `cpu40-descriptor-domain-fixture.exe` exited `0`: no descriptor fails, a valid published `SEL_IDT` resolves, a later publication replaces it (no first-address latch), and a short descriptor fails. |
| Fresh formal x86 build | `build/M0-T407/S10/r001-idt-descriptor-recovery`, Node `v22.22.1`. |
| Product link | `original-softpc-process.exe` linked. |
| Staged executable | `O:\ntvdm64\ntvdm32.exe`, 3,233,792 bytes, SHA-256 `6d4a4c725d42b3c88c1db2d8ddd398630f39834ac5701da3bc29bd75fec5243e`. |
| Runtime regression | `MEM.EXE` and `COMMAND.COM /c exit` each exited `0` under a 30-second bounded `Start-Process` run. |
| WRITE frontier | `system32\WRITE.EXE` exited `72`; stdout/stderr were empty. This is a recorded frontier, not a WOW acceptance claim. |

Runtime logs are under `O:\ntvdm64\logs\m0-t407-s10-r001`. Build products
remain below the declared `build/` root.

## Remaining Limitation

This closes only the stale-IDTR lifecycle defect. It does not prove that the
selected DOSX binary reaches and publishes `SEL_IDT` in the current standalone
runtime, nor does it recreate NT kernel trap/locked-stack behavior. The next
T407 package must instrument the non-fast BOP/descriptor publication sequence
and compare its observed `53:00/01/02/11` order with the original profile
before treating the WRITE exit as a DPMI semantic failure.
