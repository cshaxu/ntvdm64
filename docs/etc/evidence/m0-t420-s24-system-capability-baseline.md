# T420 S24 selected system capability baseline

## Frozen selected unit

The original `softpc.new/base/system/sources` manifest selects eleven bodies:
`at_dma`, `cmosnt`, `ica`, `idetect`, `illegalp`, `qevnt`, `rom`, `timer`,
`timestrb`, `dummy_nt` and `unexp_nt`.  The current formal graph compiles all
eleven into `original-softpc-system.lib`, which is linked by `ntvdm.exe`.

Normalized against the pinned OpenNT files, nine selected bodies are exact:
`at_dma`, `cmosnt`, `ica`, `illegalp`, `qevnt`, `timer`, `timestrb`,
`dummy_nt` and `unexp_nt`.  `idetect.c` changes only the recomposed-tree
`vdm.h` include spelling.  `rom.c` has `+7/-1` normalized lines: the declared
standalone selection retains the unchanged original generic `rom_init()` body
instead of the historical kernel-VDM pre-resident-ROM expectation.  It is an
original-body selection, not an adapter loader.

## Current evidence is not capability closure

S4 proves fresh x86 linkage, original system package composition and a
CCPU HALT/RESET fixture.  Deployed COMMAND/MEM/EDIT and Console geometry
regressions prove that ordinary worker operation did not regress.  They do
not by themselves prove real selected timer/PIC delivery, CMOS/RTC port
state, DMA transfer semantics, ROM protection, idle detection, or every
device-failure transition.

No current focused S24 guest or linked fixture was found for those families.
Therefore the following is a work ledger, not a pass claim:

| Family | Original selected owners | Reached boundary / required proof |
| --- | --- | --- |
| Timer, strobe and quick events | `timer`, `timestrb`, `qevnt` | Original `timer_init` enters the selected worker-local `nt_timer` provider; prove tick delivery, quick-event calibration/restart and normal/abnormal stop without a second timer owner. |
| PIC / IRQ | `ica` | Original port state and interrupt acknowledgement route require positive IRQ/EOI plus masked/cancelled behavior.  `nt_eoi` supplies the finite Win32 lock boundary only. |
| CMOS / RTC | `cmosnt` | Original CMOS ports, time conversion, periodic/alarm state and RTC IRQ route require real port reads/writes and error/clear behavior. |
| DMA | `at_dma` | Original DMA ports, channel state and SAS copy route require a valid transfer, ROM-boundary rejection and release/cleanup witness. |
| ROM | `rom` | The selected original generic initialization requires resource lookup, SAS copy and protection proof, including missing-resource disposition. |
| Idle / illegal / unsupported | `idetect`, `illegalp`, `dummy_nt`, `unexp_nt` | Prove selected idle/illegal paths retain original failure/return behavior and do not turn diagnostic observation into a replacement policy. |

## Immediate design constraint

S24 will add only test-owned guest/linked probes and minimal verification
bindings needed to observe the existing original paths.  It must not add a
timer, PIC, CMOS, DMA or ROM reimplementation to an adapter.  Any required
worker-local Win32 wait/thread binding remains owned by the existing host
lifecycle package and is verified as a finite provider, never duplicated.
