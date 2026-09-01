# M0 T351 S1 — COMMAND/NTDOS bootstrap owner admission

## Question

Does the timeout after observed original `54:05` select a COMMAND leaf repair,
or does it leave a complete original owner package that can be recovered
without inventing a DOS/CPU path?

## Source walk

The selected OpenNT baseline and current mirrors establish this route:

```text
NTDOS msinit.asm: CMDSVC SVC_CMDSETINFO
  -> SoftPC nt_bop.c::MS_bop_4
     sas_load(CS:IP service); CmdDispatch(service); IP += 1
  -> COMMAND cmddisp.c::CmdDispatch(0x05)
  -> COMMAND cmdmisc.c::cmdSetInfo
  -> CPU40/SAS executes following original guest instructions
  -> NTDOS msinit.asm saves BIOS exchange state and changes to dskstack
```

`cmdSetInfo` is not a command broker, disk, console, process or graphics
operation. In the original it retains four process aliases obtained from
`GetVDMAddr`: the SCS structure, `SCS_ToSync`, `SCS_Is_Dos_Binary` and
`SCS_FDACCESS`. Its later original consumers are COMMAND, DEM and selected
SoftPC device sources. The complete current owner disposition is in the
[S1 ledger](../operations/m0-t351-s1-command-ntdos-bootstrap-owner-ledger.tsv).

The immediate post-BOP guest sequence is byte/source-owned NTDOS code. It does
not call a host BOP before popping saved registers, copying the BIOS exchange
pointer and establishing DOSDATA/`dskstack`. A host-side rewrite would violate
guest ownership and the recovered source contract.

## Current binding review

The current `cmdSetInfo` retains original input register positions and later
SCS field semantics, but delegates durable location retention to
`mvdm_command_guest_state_set_scs_scalars`. The facade stores numeric 16:16
locations and checks the bound session and guest-memory lease on each later
short read/write. This remains necessary on x86: project policy forbids native
pointer identity pass-through and a pointer cannot survive a session epoch.

The outer original entry bridge binds the selected session and begins the
CPU40 SAS-backed guest-memory lease before invoking original `ntvdm.c`; it ends
that lease only after the original entry returns or the existing controlled
termination escape fires. The registration call therefore has the lease that
its same-shaped replacement requires. No second mapper, CPU loop, BOP
dispatcher or synthetic guest state is present.

## Disposition

S1 rejects a `54:05` leaf repair. S2 owns the complete original SCS scalar
lifecycle: registration, reached COMMAND/DEM/selected SoftPC consumers, and
outer CPU40/session lease lifetime. It may remove only a divergence that can
safely revert to original source.

WOW's `DOSWOWDATA.lpSCS_ToSync` alias is not reached by this CPU40 DOS
bootstrap path and remains a future WOW owner boundary. Graphics remains the
subsequent queued package.
