# r20 `DemSetDTALocation` Contract

## Source closure

The NTDOS caller at `src/opennt/base/mvdm/dos/v86/doskrnl/dos/msinit.asm`
uses `SVC_DEMSETDTALOCATION` after placing the three direct locations in
DS:AX, DS:DX and DS:CX and a `DOSWOWDATA` base in DS:SI. The historical owner
at `src/opennt/base/mvdm/dos/dem/demgset.c` registers these locations and
loads `pSFTHead` from the low word of `DOSWOWDATA.lpSftAddr`. The packed record
definition in `src/opennt/base/mvdm/inc/doswow.h` places that field at byte 32.

## Adapter contract

`src/bx-ntvdm-adapter/bx_ntvdm_dem_dta_service.c` accepts only real-mode #UD
bytes `C4 C4 50 1B`. It admits exactly one ordinary-RAM read: two bytes at
physical `DS:(SI+32)`, rejecting offset wrap and addresses outside 1 MiB. The
completion validates the same opaque action, derives the four physical guest
locators, retains them only in adapter runtime state, and produces generic
`RIP+4` with no register, flag, guest-memory, device, CLI, filesystem, or
Bochs-service semantic result.

## Evidence

The focused C11 test `bx-ntvdm-dem-dta-service-test` verifies exact matching,
read address/length, little-endian SFT decoding, `RIP+4`, and rejection of a
different service byte or 16-bit offset wrap. The r20 UCRT container was
incrementally linked as `ntdos64-observation-fixture.exe` (not full
`bochs.exe`). Its three-second source-built NTIO trace records:

```text
CS=9346:EIP=7484  C4 C4 50 1B  -> accepted resume next=7488
CS=9346:EIP=7490  C4 C4 50 32  -> native pass-through
```

This closes only the registration mechanics. It does not admit later use of
the locators, DOS file operations, host filesystem behavior, or runnable DOS.
