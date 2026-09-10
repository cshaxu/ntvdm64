# NTDOS Post-Load First-Instruction Dependency Ledger

Status: M18 static source/artifact evidence, 2026-08-08.

## Fixed Artifact

```text
NTDOS.SYS length: 27858 bytes
SHA-256: 957662320654AD5251C3A8B228A5DADEC28AA65DDDBCBA38C3658A6E7F93BC84
map: artifacts/toolchain-runs/ntdos-tools16-historical-v1/.../NTDOS.MAP
```

The file begins `E9 A9 51`. `mshead.asm` documents that this header is the
initialization entry and that its word at offset three carries the nonzero
ORG/paragraph adjustment used by BIOS. The map identifies `DOSINIT` as a
separate internal routine; a file's first byte is therefore not by itself the
runtime entry state of initialized DOS.

## Handoff Proven By Source

After `SVC_DEMLOADDOS` returns, NTIO continues into `sysinit1.asm`. The
historical flow is:

```text
NTIO calls original demLoadDos at live DI:0000
  -> NTIO sysinit copies the NTDOS image to a temporary high-memory location
  -> reads header word [loaded DOS + 3] and adjusts current_dos_location
  -> relocates BIOS code and invokes its segment reinitializer
  -> prepares ES:DI = BIOS communication block
  -> prepares DS:SI = device list
  -> sets DX = conventional-memory size
  -> calls the temporary NTDOS DOSINIT entry
```

`sysinit1.asm` explicitly states that `DOSINIT` relocates DOS data into low
memory and builds core DOS data structures. This means an external profile
must not treat NTDOS as a flat image that begins normal operation immediately
at `DI:0000`.

## Resident Device-Chain Contract

The device-list input is now source-identified, rather than a generic
prepared-entry placeholder. `doskrnl/bios/msinit.asm:202-205` assigns the
`sysinit1.asm` `device_list` far pointer to `res_dev_list` in the resident BIOS
code segment. `doskrnl/bios/msbio1.asm:74-90` defines that list in the same
historical image. Its first node is the `CON` character device, followed by
`AUX`, `PRN`, `CLOCK$`, `COM1` and the serial/parallel chain, ending at `COM4`.

The source-built `NTIO.SYS` artifact is 33,792 bytes with SHA-256
`CFC8BE16576BB6ACD16BB8FC9B2D9A080F544BBFDD2D2D2DF07ED908B3AB4937`.
Its matching `ntio.map` locates `res_dev_list` at `0000:0024` and `strategy`
at `0000:01ec`. The bytes at file offset `0x0024` independently decode the
first `CON` header as:

```text
next      0070:0036
attribute 8013h
strategy  0070:01ech
interrupt 0070:01f7h
name      "CON     "
```

This is the exact resident chain data installed by the source-defined NTIO
image loaded at `0070:0000`; it is not an invented profile record. The pointer
values remain location-dependent and must be derived again if a different
validated NTIO build/profile is selected.

`doskrnl/bios/sysinit1.asm:958-981` then loads that original far pointer into
`DS:SI`, loads `BiosComBlock` into `ES:DI`, loads `memory_size` into `DX`, and
performs the temporary `DOSINIT` call. These are the actual entry registers;
they are not independently selectable wrapper defaults.

After the demonstrated `CMDSETINFO` return, `doskrnl/dos/msinit.asm:788-804`
uses that first device header in `charinit`: it creates a DOS-owned device-init
request packet and calls original `DEVIOCALL2`. `doskrnl/dos/dev.asm:525-542`
shows that `DEVIOCALL2` calls the header's strategy address and then its
interrupt address. Thus the next continuation is a guest-to-guest call into
the original resident BIOS device driver, not a DEM result, host capability,
or candidate modern console shim.

The immediate acceptance evidence is therefore a source-derived prepared
entry that preserves the BIOS-resident device-chain bytes and the `DS:SI`,
`ES:DI`, and `DX` handoff as one transaction. It is forbidden to substitute a
fixed synthetic device header, fake `CON` strategy/interrupt target, or
prewritten request success while claiming this historical path.

## First Source-Visible External Dependency

`dos/msinit.asm` begins `DosInit` by preserving the BIOS-provided `DX`,
`DS:SI`, and `ES:DI`, relocating its DOS data, and consuming the BIOS
communication flag. Its first non-debug external service is:

```text
SVC SVC_DEMGETDRIVES  ; service 0x0f
AL <- number of logical drives
```

The original `demGetDrives` implementation is in `dos/dem/demgset.c`. Its
comment states that it is called by NTDOS `msinit` to obtain `numio` and that
it initializes a physical-drive-type list. The historical implementation
enumerates ambient host drive state; that policy is not admissible for a
contained modern runner.

## Boundary Result

The next required capability is a profile-owned, session-scoped logical-drive
enumeration result. It needs at minimum:

- a validated DOS-visible drive count returned in `AL`;
- a matching bounded drive-type/media description for the immediately
  following original initialization path;
- explicit profile/mount configuration rather than ambient host drive scans;
- a terminal, redacted failure for an impossible profile rather than a
  fabricated success or a PC/AT controller emulation shortcut.

It does **not** yet prove that CMOS, RTC, FDC, ATA/HDC, DMA, or a physical disk
controller belongs in generic core. The historical first dependency is a
logical namespace/service result, and controller extraction remains deferred
until a second real consumer and a PC/AT-neutral contract exist.

## Next Evidence Cut

Trace `demGetDrives` and the following NTDOS initialization requests to define
the smallest contained drive namespace record: number, letters, drive type,
boot selection, DPB list, error behavior, and whether the original service
mutates guest memory. No new service handler is admitted by this record.
