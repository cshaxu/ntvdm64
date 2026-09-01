# M0 T339 closure — Minimal vertical-slice integration

## Result

T339 closes with a narrow, source-defined transfer rather than a DOS execution
completion claim. S1 completed the direct original `50:3D -> demExitVDM`
predecessor map. S2 compiled the passive discriminator and consumed its one
unchanged short-root observation. That observation reached original COMMAND
`54:05` but not `50:3D`, so it selected no `demExitVDM` repair.

The source continuation after `54:05` is now explicit:

1. NTDOS `msinit.asm::DosInit` invokes original `CMDSVC SVC_CMDSETINFO` with
   the `SCSINFO`, DOS-binary and floppy-access locations.
2. Original `cmdmisc.c::cmdSetInfo` records those locations and returns.
3. NTDOS establishes the initial device-init IRET vector and calls
   `charinit` on the original BIOS-supplied resident device list.
4. `charinit` invokes original `DEVIOCALL2`, which calls each device header's
   strategy and interrupt entries before NTDOS can reach its next
   `SVC_DEMGETDPBLIST` (`50:46`) boundary.
5. The selected resident list begins in original `msbio1.asm` with the
   `CON`, `AUX`, `PRN`, `CLOCK$`, COM and LPT headers, all using the original
   BIOS `strategy` and device-interrupt entry points.

Thus the first unobserved post-`54:05` owner is the complete resident
DOS-BIOS device-initialization contract. It crosses guest device request
packets, BIOS/firmware code, CPU40 instruction execution and machine
equipment interfaces. It is neither a BOP leaf nor a missing `config.nt`,
`HIMEM.SYS`, command-root, BaseSrv, drive-policy or DEM/COMMAND provider.

## Evidence

- [S1 original guest-predecessor contract](../etc/evidence/m0-t339-s1-original-guest-predecessor-contract-001.md)
- [S2 passive terminal observation](../etc/evidence/m0-t339-s2-passive-terminal-discriminator-001.md)
- [T339 plan](../etc/operations/m0-t339-minimal-vertical-slice-integration-closure-plan-001.md)

The T339 S2 source locations are:

- `mvdm-guest/dos/v86/doskrnl/dos/msinit.asm:350,453,524,788-804`
- `mvdm-guest/dos/v86/doskrnl/dos/dev.asm:518-540`
- `mvdm-guest/dos/v86/doskrnl/bios/msinit.asm:202-205`
- `mvdm-guest/dos/v86/doskrnl/bios/msbio1.asm:76-90,220-244`
- `mvdm-host/dos/command/cmdmisc.c:663-677`

## No selected repair

T339 did not change guest media, configuration, drive policy, BOP routing,
firmware/device behavior, CPU semantics, or the original COMMAND/DEM service
ordering. The passive ingress record is retained as registered
`MVDM-HOST-DIV-164`; it has no business routing or guest-state effect.

The next package is a complete source/ABI/failure review of the resident
device-initialization contract. It must establish all original device headers,
strategy/interrupt paths and machine dependencies before it can select any
recovery. It may not infer a device repair from T339's timeout.
