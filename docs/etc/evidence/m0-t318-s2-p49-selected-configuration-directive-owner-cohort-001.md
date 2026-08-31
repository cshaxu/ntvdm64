# M0 T318 S2 P49 — Selected configuration-directive owner cohort

## Fixed input

The frozen `runtime-r33-config-system-media` container selects the byte-identical
original `config.nt` directives:

```
dos=high, umb
device=%SystemRoot%\system32\himem.sys
files=20
```

This record audits the complete original owner cohort selected by those three
directives before another runtime observation.  It does not add a BOP handler,
guest loader, memory allocator, or device emulation substitute.

## Original DOS configuration path

`mvdm-guest/dos/v86/doskrnl/bios/sysconf.asm` is the original guest parser and
owner:

- `ProcDOS` records `HIGH` in `runhigh` and `UMB` in `DevUMB`; when a device
  is loaded it calls original `AllocUMB`, and subsequently calls original
  `TryToMovDOSHi` when `runhigh` is selected.
- `InitAllocUMB` requires the original `IsXMSLoaded` check, obtains the XMS
  entry through `INT 2Fh, AX=4310h`, and creates the original DOS UMB arena.
  This is guest DOS/SoftPC memory semantics, not a host pointer or mapping
  token operation.
- The `FILES=20` parser writes the original guest `files` value after the
  original `SVC_DEMWOWFILES` call.  The reached host owner remains
  `mvdm-host/dos/dem/demdisp.c::demWOWFiles`; no app or adapter route is
  substituted.
- The configuration pass also issues the original `SVC_CMDINITCONSOLE` from
  guest configuration code.  Its reached original owner is
  `mvdm-host/dos/command/cmddisp.c::cmdInitConsole`, already in the selected
  COMMAND package.  This is distinct from the fixed host console-owning
  observation container.

## HIMEM and XMS boundary

The staged `HIMEM.SYS` is the original guest driver built from
`mvdm-guest/dos/v86/dev/himem/{himem,himem1,himem2,himem4,himem5}.asm`.
Its original `INT 2Fh` installation/entry contract exposes the guest XMS
control entry.  XMS allocation, move, A20 and UMB requests use the original
`XMSSVC` service table rather than a new BOP family.

The selected host owner path is likewise original:

```
softpc.new/host/src/nt_msscs.c::XMSInit
  -> xms.486/xms.c::XMSInit
  -> xms.486/xmsdisp.c::{xmsMoveBlock, xmsAllocBlock,
                         xmsRequestUMB, xmsReleaseUMB}
  -> xms.486/xmsumb.c
```

The formal x86 Ninja graph contains `nt_msscs.c`, `xmsdisp.c`, `xmsumb.c`,
`original-mvdm-xms.lib`, and the selected
`adapter-mvdm-host-out/softpc/mvdm_xms_memory.c` backend.  The latter is the
already registered session guest-memory mapping boundary: host-native identity
is never passed to the original XMS body as a truncated pointer.  It does not
replace the guest's contiguous XMS/UMB numeric semantics.

## Country and media disposition

The generated original `COUNTRY=` line remains guest `sysconf.asm` parsing and
`setdoscountryinfo` work using the staged byte-identical `COUNTRY.SYS`.
`P48` already established its selected media placement.  `MSCDEXNT.EXE`
remains absent from the approved source-built media set and is an explicit
AUTOEXEC guest-artifact transfer; it is not implicated by the three selected
CONFIG directives and receives no fabricated replacement here.

## Result and next boundary

The selected CONFIG cohort is source-shaped and formally composed.  No local
host/guest ownership gap is found in `DOS=HIGH,UMB`, `DEVICE=HIMEM.SYS`,
`FILES=20`, `COUNTRY.SYS`, or their reached DEM/COMMAND/XMS dispatch owners.
No product source changed, so the frozen runtime container is not rerun.

The next source-first audit boundary is the original host display/console
initialization reached between `host_start_cpu` and the still-unobserved NTIO
guest marker.  A newly linked product may receive exactly one fixed-container
observation only after that complete host owner cohort is reviewed or repaired.
