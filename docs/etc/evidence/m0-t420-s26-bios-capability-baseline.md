# T420 S26 BIOS capability closure

## Scope and source shape

S26 closes the complete selected fourteen-body
`src/mvdm/softpc.new/base/bios` unit:
`bios.c`, `cmos_bis.c`, `cntlbop.c`, `emm_fncs.c`, `reset.c`, `rom_basc.c`,
`rtc_bios.c`, `slave_bs.c`, `tape_io.c`, `equipmnt.c`, `ill_bop.c`,
`build_id.c`, `virtual.c` and `mem_size.c`.

Normalized against the pinned OpenNT union, eleven bodies are exact:
`bios`, `cmos_bis`, `cntlbop`, `emm_fncs`, `rom_basc`, `rtc_bios`,
`slave_bs`, `equipmnt`, `ill_bop`, `build_id` and `mem_size`.
The remaining three retain only previously declared finite boundaries:

| Body | Retained delta | Reason and owner |
| --- | --- | --- |
| `reset.c` | CPU40 direct EIP continuations | Two DOSX continuation targets are ordinary guest code rather than BOP addresses; this is the selected CPU40 carrier, not a BIOS replacement. |
| `tape_io.c` | Original `xms.h` declaration spelling | Preserves the original XMS-mediated A20 transition used by `INT 15h` move-mode service. |
| `virtual.c` | `ClearInstanceDataMarking` declaration | Header availability only; the original virtual-state body is retained. |

No BIOS product source changed in S26.  The new test fixture and the
row-wrap-safe regression assertion are test-only.  No firmware image, guest
media, CPU30 monitor, new device provider or adapter-owned BIOS policy was
added.

## Whole-unit disposition

| Family / selected owners | Disposition and evidence |
| --- | --- |
| Reset and BDA/IVT publication: `reset`, `cmos_bis` | Real guest witness verifies nonzero ROM vectors for INT 11h/12h/15h before invoking them.  Normal worker launch is the initial reset lifecycle; its destructive soft-reset path is not injected into a healthy worker. |
| Equipment and conventional-memory services: `equipmnt`, `mem_size`, `bios` | Real `INT 11h` and `INT 12h` values equal BDA `40:10` and `40:13`, proving the original ROM BOP stubs and selected C owners. |
| INT 15 normal/failure service: `tape_io`, `bios` | Real AH=80h, 85h and C0h return original success; AH=24h returns original CF=1/AH=86h failure.  AH=87h/89h are protected-mode ownership in S38/S39, not duplicated here. |
| Build and control BOPs: `build_id`, `cntlbop` | Real BOP 21 returns `Base$`, nonzero build ID and AL=0; real generic BOP FF function zero returns original `ERR_NO_FUNCTION` AX=1. |
| RTC: `rtc_bios`, `cmos_bis` | S24's real CMOS register and periodic interrupt witness already proves the selected provider.  Its BOP 70 dispatch is the same original BIOS vector owner; S26 adds no second RTC. |
| EMS: `emm_fncs`, `bios` | The selected x86 binary contains BOP 66/67/68 and original EMM sources.  Current COMMAND has PIF EMS size zero; original `config.c` makes `C_LIM_SIZE` zero, so `InitSpcEmm` deliberately does not install EMM.  This is a source-proven profile-null, not a missing implementation.  A diagnostic attempt to call the remaining default INT 67h vector returned `42h`; it was removed from the pass fixture because it is not an EMM endpoint. |
| Error/device endpoints: `rom_basc`, `ill_bop`, `slave_bs` | Original error, illegal-BOP and unused/slave-IRQ handlers are intentionally destructive/diagnostic or hardware-driven.  No normal COMMAND/MEM/EDIT caller reaches them.  They are source-proven failure endpoints; injecting them would corrupt the worker and is not a success test. |
| Virtual-instance state: `virtual` | Under `NTVDM`, BIOS BOP 25 is selected as `illegal_bop` rather than `VDD_Func`; current normal guest profile has no caller.  VDD ownership remains S43. |

## Real guest witness

`tests/observation/bios_capability.asm`, built by
`tools/build/Build-T420S26BiosGuestTest.ps1`, is a test-only COM image.  It
uses the original ROM vectors/BOP bytes and does not install a handler or
modify media.  Through the deployed formal package it produced:

```text
S26_RESET_VECTOR_OK S26_EQUIPMENT_OK S26_MEMORY_SIZE_OK S26_CASSETTE_OK
S26_CASSETTE_ERROR_OK S26_BUILD_ID_OK S26_CONTROL_BOP_OK
S26_EMM_PROFILE_NULL S26_BIOS_OK
```

The transcript is `O:\winnt\logs\m0-t420-s26-formal-bios.raw`.  The
ConPTY observer's `--bios-capability` path gates only this visible guest
marker and absence of command-resolution failure.

## Formal build and regression

The fresh graph `build/M0-T420/S26/formal-x86-r1` completed all 526 x86
product units and linked `run16.exe`, `basesrv.exe`, `ntvdm.exe`, `dtmgr.exe`
and `VDMREDIR.dll`; its `VdmTib` ownership check passed.  The five artifacts
were copied to `O:\winnt` and their pre/post-copy hashes are recorded in
`O:\winnt\logs\m0-t420-s26-formal-artifacts.json`.

The established transcript-gated matrix subsequently passed all 17 cases:
direct and interactive COMMAND, nested COMMAND, MEM/repeated MEM, direct
MEM, COMMAND `/c`, guest exit seven, native stdout/stderr/EOF and EDIT.
The machine-readable report is
`O:\winnt\logs\m0-t420-s26-formal-command-r4-summary.json`.

During this run a 53-column Console split an expected English diagnostic over
two physical observer rows.  `Verify-CommandExitStatus.ps1` now removes row
labels and row separators solely for assertion matching; it does not alter
the captured Console text or product behavior.  The full matrix passed after
that test-harness correction.

S26 therefore closes every selected BIOS body as a real safe guest pass, a
source-proven current-profile null, or an intentional original error endpoint.
