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

## Selected-profile capability closure

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

## S24 guest device witness

`tests/observation/system_capability.asm` is a test-only DOS COM probe, built
by `tools/build/Build-T420S24SystemGuestTest.ps1`. It uses guest BIOS ticks,
CMOS/PIC/DMA port I/O and the selected ROM mapping; it installs no replacement
device and changes no guest media. A bounded normal CPL3 instruction workload
separates the two tick reads. HLT is intentionally not used: S21 establishes
that CPL3 HLT is a timeout boundary in this selected CCPU40 profile.

On 2026-09-18 the public Console route ran `SYSTST.COM` through a temporary
short DOS drive mapped to `O:\winnt\tests\m0-t420-s24-r5`. The raw guest transcript at
`O:\winnt\logs\m0-t420-s24-system-r5.raw` contains:

```text
S24_TIMER_OK S24_CMOS_OK S24_PIC_OK S24_DMA_PORT_OK S24_ROM_READ_OK S24_SYSTEM_OK
```

The refined r9 guest run additionally exercised the original RTC periodic
provider: it republishes the existing register-A rate, enables PIE through
the original register-B path, waits under normal CPL3 execution, acknowledges
register C and restores B. Its raw transcript contains
`S24_RTC_PERIODIC_OK`. The initial requirement that C_IRQF still be visible
at the guest read was withdrawn: C_PF is present, while C_IRQF may already be
consumed by the original RTC IRQ/EOI acknowledgment route. Treating that
normal consumption as a device failure was an invalid assertion.

`at_dma.c` and its `gfi_sflp.c` caller are selected in the formal graph, but
the current runtime profile provides no floppy image or A: attachment. The
only source-shaped transfer route is original floppy I/O through
`gfi_sflp -> dma_request -> do_transfer`; there is no DOS DMA-copy API to
invent. DMA transfer is therefore profile-null in this package, while the
guest fixture records the existing controller port provider. A future floppy
media admission must validate it through an original INT 13h read.

The remaining selected `illegalp`, `unexp_nt` and `dummy_nt` bodies are not
ordinary DOS services: the original BIOS BOP table selects them only for
illegal/unused BOP or unexpected-interrupt endpoints. `illegal_op_int` keeps
the original fault-address/report-and-skip sequence; `unexpected_int` records
the cause in the BDA and masks/EOIs an unexpected hardware line; `dummy_int`
is intentionally empty. No selected normal COMMAND/MEM/EDIT caller targets
these slots. They are source-proven failure endpoints, not safe runtime
fixtures: injecting an illegal BOP into a healthy worker would deliberately
corrupt its normal fault/lifecycle state and cannot demonstrate a successful
device capability.

The same deployment passed all six Console geometry/mouse/resize cases and
five short-window `EDIT -> MEM` repetitions through
`tools/audit/VerifyCvidIntegrated.ps1` (runner log:
`O:\winnt\logs\m0-t420-s24-command-regression.runner.txt`).

The regular Ninja launcher was unable to progress past `.ninja_lock` before
spawning `cl.exe` or `link.exe`. This is a host launcher fault, not accepted as
a build result. On 2026-09-18, the five link commands emitted by the unchanged
formal `build.ninja` product graph were instead invoked verbatim in the x86 VS
environment. `run16.exe`, `basesrv.exe`, `ntvdm.exe`, `dtmgr.exe` and
`VDMREDIR.dll` all linked successfully; the existing VdmTib ownership check
also passed (`mvdm_vdm_tib.obj`, 4208 bytes, no overlap). The exact resulting
five artifacts were copied to `O:\winnt`; SHA-256 was compared before and after
deployment.

The deployed formal package then ran two transcript-gated regressions. The
S24 witness output at `O:\winnt\logs\m0-t420-s24-formal-system.raw` is:

```text
S24_TIMER_OK S24_CMOS_OK S24_RTC_PERIODIC_OK S24_PIC_OK S24_DMA_PORT_OK S24_ROM_READ_OK S24_SYSTEM_OK
```

The direct interactive `COMMAND -> MEM -> EDIT -> MEM` observer completed with
exit zero and wrote `O:\winnt\logs\m0-t420-s24-formal-command-edit-mem.raw`.
Its acceptance is not that host status alone: the raw DOS transcript contains
both `655360 bytes total conventional memory` MEM reports, the MS-DOS Editor
startup/exit sequence, and no `Bad command or filename` string. This is the
same guest-text gate required of all S24 product regressions.

This completes the selected profile. RTC alarm is not a separate selected
provider from the proven original RTC periodic route; DMA transfer has the
source-proven no-media disposition above; and illegal/unexpected endpoints are
source-proven non-normal failure handlers rather than a safe success-path
probe. No adapter-owned timer, PIC, CMOS, DMA or ROM substitute was added.
