# M0 T318 S2 P75 — post-`cmdSetInfo` guest/device-bootstrap owner cohort

Date: 2026-08-31  
Status: static owner-boundary closure; no failure attribution or runtime-continuity claim

## Purpose

P73 proves only that the short-root failure is independent of the declared
ordinary-child input and that the original bootstrap reached COMMAND
`54h:05h`.  It deliberately does not interpret the unstable trailing console
capture as absence or presence of a later service.  This record closes the
complete source-defined cohort after that service, so recovery cannot turn
into a speculative `54:0C`, HIMEM, or `EXEC` patch.

## Reached original service

The guest source-built NTDOS mirror invokes `CMDSVC SVC_CMDSETINFO` in
`mvdm-guest/dos/v86/doskrnl/dos/msinit.asm`.  The selected original host path
is unchanged:

```text
guest DS:DX/BX/CX
  -> BIOS[54h] / nt_bop.c::MS_bop_4
  -> cmddisp.c::CmdDispatch(5)
  -> cmdmisc.c::cmdSetInfo
```

`cmdSetInfo` records the source `SCSINFO` location through the existing
session-owned numeric-location boundary.  It does not acquire an ordinary
child, launch COMMAND, preprocess CONFIG, or invoke an XMS service.

## Complete post-return cohort

On return, `msinit.asm` remains guest-owned.  Before its next explicit host
service it:

1. saves the BIOS exchange pointer and DOS data segment;
2. replaces the local DOS stack with `DOSDATA:dskstack`;
3. builds the original device/SFT state; and
4. initializes each source device through `charinit`.

`charinit` uses original `dos/dev.asm::DEVIOCALL2`: it copies each device
header's strategy and interrupt far addresses into `CALLDEVAD`, then performs
the original far calls.  This is a guest device/CPU/stack contract, not a
COMMAND or Base VDM host broker call.

After the device list is processed, the next explicit MVDM host crossing is
the original `SVC_DEMGETDPBLIST` in `msinit.asm`:

```text
NTDOS msinit / device strategy + interrupt calls
  -> 50h:48h SVC_DEMGETDPBLIST
  -> nt_bop.c::MS_bop_0
  -> demdisp.c::DemDispatch
  -> demgset.c::demGetDPBList
```

The later `54h:0Ch cmdGetConfigSys` crossing is in the distinct
`sysinit1.asm::ProcessConfig` cohort, after this device/DPB stage.  Its
64-byte CONFIG-root admission is already governed by P74.

## Existing compositions and non-attribution

- The selected formal CPU40 graph contains the original NTDOS image, full
  `demgset.c` owner body, `demdisp.c`, the original `nt_bop.c` selector, and
  the CCPU40/SAS machine path.
- The short-root P73 record contains stable bootstrap markers only through
  `50:11`, `50:3B`, `50:0F`, `50:1B`, and a partial `54:05` record.  P73
  explicitly states that trailing bytes do not establish whether a later BOP
  was reached.  This record therefore does **not** claim that `50:48` or
  `54:0C` was absent.
- XMS/HIMEM is not a missing provider in this interval.  P36--P37 select the
  original XMS package and its same-shaped session-memory backend; P49 maps
  the later CONFIG/HIMEM directive cohort.  Neither record justifies changing
  it to explain this unlocated access violation.
- The declared `/C VER` child is not consulted here; P73's no-child control
  already excludes it as the cause.

## Recovery disposition

The remaining candidate is one **guest CPU/device far-call and stack cohort**
between `54:05` and the later explicit DEM/COMMAND crossings.  Its next work
must audit original CPU40 far-call/return, `LDS`, segment/stack, device-header
and device strategy/interrupt semantics as one owner contract.  It must not
invent a new BOP provider, widen the CONFIG carrier, change HIMEM/XMS, or
reimplement DOS device logic in an adapter.

