# r20 `DemSetHardErrorInfo` Contract

## Source closure

NTDOS invokes `SVC_DEMSETHARDERRORINFO` at
`src/opennt/base/mvdm/dos/v86/doskrnl/dos/msinit.asm`: DS:DX names `vheVDM`
and DS:BX names the DOS device chain. The service number is `0x32` in
`src/opennt/base/mvdm/inc/dossvc.h`. The DEM dispatcher maps it to
`demSetHardErrorInfo`, whose entire implementation in
`src/opennt/base/mvdm/dos/dem/demerror.c` stores the two translated addresses
and returns without guest output or host I/O.

## Adapter contract

`bx_ntvdm_dem_hard_error_service.c` accepts only real-mode #UD bytes
`C4 C4 50 32`. It derives two physical real-mode locators from DS:DX and
DS:BX, rejects addresses at or beyond the 1 MiB aperture, retains the result
inside adapter runtime state, and returns generic `RIP+4`. It does not read or
write guest memory, interpret either structure, expose a host pointer, begin a
hard-error policy, or add a Bochs change.

## Evidence and explicit non-claim

The focused C11 service test verifies exact byte matching, both locators,
`RIP+4`, and rejection of a different service byte. The r20 incremental
`ntdos64-observation-fixture.exe` trace records:

```text
CS=9346:EIP=7490  C4 C4 50 32  -> accepted resume next=7494
CS=9346:EIP=749d  C4 C4 54 05  -> native pass-through
```

This does not implement hard-error handling, device-chain traversal, a DOS
filesystem, a command processor, or an interactive NTVDM session.
