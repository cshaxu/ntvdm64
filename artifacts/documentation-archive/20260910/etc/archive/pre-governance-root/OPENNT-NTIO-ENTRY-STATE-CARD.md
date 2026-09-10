# OpenNT NTIO entry-state card

## Scope

This card records the entry evidence for the staged OpenNT NTIO image used by
the fixed research profile.  It does not claim that a retail NTIO file, an
arbitrary BYOB file, or this compatibility-built image has booted.

## Image evidence

The isolated `ntio-stage-v6` build manifest records a headerless `NTIO.SYS`
with a load segment of `0070h`.  Its 33,760-byte output has SHA-256
`9BCD709E962DBD897DA062C5287B02403B1470C78763B245197866870E03B43F`.

This is an archived staging artifact, not the current fixed profile. Its
former `0x83E0/08AEh` layout is superseded for profile use by
`NTIO-IMAGE-LAYOUT-PROVENANCE-CORRECTION.md`.

The first bytes are:

```text
E9 6D 03 00 00 EA FD 02 70 00
```

`E9 036D` at offset `0000h` transfers to offset `0370h`.  The staged map
locates public `init` at `0000:0370`, and `msbio1.asm` places the entry jump at
the beginning of the image.  Thus the profile can prove this control-flow
relation:

```text
load image at physical (0070h << 4)
start at 0070:0000
first near jump reaches 0070:0370 (init)
```

Starting directly at `0070:0370` is equivalent only for this verified image
and only after an explicit profile check.  The normal profile entry remains
the image origin so that its initial jump remains guest code, not a wrapper
substitution.

## Register inputs at `init`

`base/mvdm/dos/v86/doskrnl/bios/msinit.asm` documents the boot-loader inputs
at public `init`:

| Register | Meaning |
| --- | --- |
| `DL` | INT 13h drive number that supplied the boot image |
| `CH` | boot medium byte |
| `BX` | first data-sector number (low part) |
| `AX` | first data-sector number (high part) |
| `DI` | sectors-per-FAT for the boot medium |

The procedure explicitly describes all other entry register contents as
unknown.  It immediately disables interrupts, clears `DS`, saves selected IVT
entries, installs `INT 15h` and `INT 19h`, then sets `SS:SP` to `0000:0700`.
The wrapper must therefore not infer a caller-provided initial stack, data
segment, or interrupt-enabled state as part of the contract.

## Directly observed early machine writes

Before the DOS-load service, `init` reads or writes the following machine
areas:

- IVT at physical `00000h` through at least the vectors it saves and replaces;
- low-memory words at `0500h` and `lstdrv`;
- BDA byte `0040:0096` for extended-keyboard detection;
- its own loaded image/data segment; and
- the service call `SVC SVC_DEMLOADDOS`, whose exact bytes are `C4 C4 50 11`.

This establishes that a valid execution experiment requires writable low RAM,
an initialized or deliberately specified IVT/BDA policy, and a profile-supplied
boot-media state.  A file-only `NTIO.SYS` planner cannot invent those values.

## Private entry trace (2026-08-06)

The staged source-built image was copied into a private, zero-filled one MiB
SAS allocation at physical `00700h`.  A self-authored CCPU adapter entered it
at `0070:0000` with the documented boot-register inputs set to zero and a
finite instruction budget.  The first bounded run reached the historical
`SVC_DEMLOADDOS` encoding and stopped through a process-local `BIOS[0x50]`
table entry:

```text
ntio budget-stop=0000:0000 bop-count=1 bop=50 bop-stop=1 \
  bop-next=0070:0478 di=08AE service=11
```

The result proves only this narrow execution fact: the copied image can reach
its request for selector `0x50` under the staged CCPU/SAS adapter.  CCPU has
already consumed the three-byte BOP when it calls `BIOS[0x50]`; the handler
observed the following byte as service `11h`, exactly
`SVC_DEMLOADDOS`.  It also observed `DI=08AEh`, confirming the historical
loader's requested private destination is physical `08AE0h`.  The stop
occurs before the service byte is consumed, so the printed `0000:0000` budget
location is not an NTIO entry or post-service register assertion.  No DEM
loader was implemented, no `NTDOS.SYS` bytes were loaded, and no BIOS, IVT,
BDA, disk, keyboard, timer, or interrupt contract was thereby validated.  The
probe executable SHA-256 is
`F09199CFEDD45EC378B94C52034385A814489DBEEA26430E0A6B17F530C5D89C`.

## Private NTIO-to-NTDOS execution ladder (2026-08-06)

A later bounded probe uses the same private staged NTIO image and a private
copy of the self-authored-copyright staged `NTDOS.SYS` image.  It is not a
distribution artifact and does not modify either input.  Its profile contains
only the following source-verified effects:

1. on `BOP_DOS/SVC_DEMLOADDOS` (`50h/11h`), preflight the full NTDOS range,
   copy all `27,954` bytes to the requested `DI:0000` (`08AE:0000` in this
   trace), then consume only the minor service byte;
2. on BIOS BOP `12h`, return the explicitly selected `640 KiB` conventional
   memory value and mirror it in BDA word `0040:0013`;
3. on `BOP_DOS/SVC_ISDEBUG` (`50h/3Bh`), return non-debug `AL=0`;
4. on `BOP_DOS/SVC_DEMGETDRIVES` (`50h/0Fh`), return the profile's two empty
   logical drive slots with clear carry, without scanning or exposing host
   drives; and
5. on `BOP_DOS/SVC_DEMSETDTALOCATION` (`50h/1Bh`), checked-convert and retain
   the guest DTA/PDB/extended-error/WOW-data addresses without dereferencing
   them or implementing filesystem operations.
6. on `BOP_DOS/SVC_DEMSETHARDERRORINFO` (`50h/32h`), checked-convert and
   retain the hard-error packet and device-chain addresses; and
7. on `BOP_CMD/SVC_CMDSETINFO` (`54h/05h`), checked-convert and retain the
   command SCS-info, DOS-binary marker, and file-access addresses.
8. on `BOP_DOS/SVC_DEMGETDPBLIST` (`50h/46h`), validate the guest list
   destination and return a source-consistent empty list for this explicit
   no-mounted-physical-media profile.

The bounded run reached each of those services and then safely stopped at the
next unimplemented service:

```text
ntio bop-count=7 bop=50 bop-next=9386:75D6 di=00EB wire=50/46 \
  demload=6/1/0 int12=1 isdebug=1 drives=1 dta=1/0 harderr=1/0 cmd=1/0 dpb=1/0 \
  image=27954 first=E9 last=FE budget=1@0000:0086 \
  bytes=86930001 stack=00A7:00A8 ret=9386:2906
```

`50h/46h` is `SVC_DEMGETDPBLIST`.  The `dpb=1/0` result confirms that the
source-consistent empty list was accepted and execution continued for a
65,536-instruction internal budget without another BOP.  The copied NTDOS
first and final bytes (`E9` and `FE`) match the private source image.  The v15
probe SHA-256 is `F647D7C847F0C6BB15630714DDAB5BC6BDD72319D8E72BC061DF44E3E01DFDA9`.

`SVC_DEMGETDPBLIST` is the first startup request that cannot truthfully be
implemented as merely a checked pointer handoff or scalar profile response.
Its historical contract writes drive parameter blocks at guest `ES:BP` and
advances `BP` over the emitted list.  Before this project implements it, the
runtime must define a concrete media profile: which logical slots have media,
whether each is image-backed or directory-backed, its geometry/attributes,
and the failure behavior for missing or read-only media.  Returning a count of
two earlier does not by itself establish those semantics.

The post-DPB budget stop is also a concrete prepared-entry limitation.  It
occurred at `0000:0086`, with a guest stack value that contains a return-like
`9386:2906` pair.  The four bytes at the execution location are nonzero, so
this evidence does **not** prove that execution fell through an all-zero IVT.
It does prove that a merely zero-filled low-memory baseline plus the services
above is insufficient to complete this initialization path.  The next machine
work must derive and provide the required BIOS/IVT/BDA and interrupt/timer
state rather than guessing a service response.

### Source-backed scaffold-IVT experiment (2026-08-06)

The next bounded probe applies the keyboard/video-independent initial bytes
from OpenNT `base/mvdm/v86/scaffold/i386/fakeinit.c`, rather than inventing an
IVT policy.  Before entering the private NTIO image it verifies these effects:

- IVT vectors `00h` through `1Dh` initially target the default `F000:0100`
  `C4 C4 5F CF` unimplemented-interrupt stub;
- `INT 11h`, `INT 12h`, and `INT 19h` are replaced with the scaffold's
  equipment, memory-size, and warm-boot entry points;
- BDA equipment word is `0004h` and conventional-memory word is `0280h`;
- the scaffold's reset-vector and `F000:FF53` IRET bytes are present.

The adapter keeps this memory writable, because the same scaffold has an empty
`rom_init`; this is an initial-state experiment, not a claim of a ROM mapping
or a complete PC/AT BIOS.  With the same private source-built images and all
earlier bounded BOP services, it produced:

```text
ntio scaffold=1 bop-count=8 bop=FFFFFFFF bop-next=F000:0103 di=002C \
  wire=5F/CF demload=6/1/0 int12=1 isdebug=1 drives=1 dta=1/0 \
  harderr=1/0 cmd=1/0 dpb=1/0 image=27954 first=E9 last=FE budget=0@0000:0000
```

The `F000:0103` state is after CCPU consumed `C4 C4 5F`; the adapter's
initialized default BOP disposition then safely stopped the run.  Therefore
the IVT is now demonstrably in the live control path, and the next missing
piece is to identify the specific guest interrupt and implement only its
source-defined service contract.  This output does not establish keyboard,
video, disk, timer, PIC, or any generic interrupt-delivery support.  Probe
`ccpu-ntio-demload-probe-v16.exe` SHA-256 is
`40D64A21566757AA508E77713FD151BEDBD92231733C98EF4EBB39A07CDFD996`.

This is a controlled transition and early initialization trace, not a DOS
boot.  In particular it proves neither a usable DOS namespace, a mounted
medium, interrupt delivery, BIOS ROM, keyboard/video, time, XMS/DPMI, program
launch, nor PE handoff.

## Remaining execution prerequisites

The following remain unproven and must be supplied by explicit configuration
and trace evidence before `nvtdm` may execute this profile:

1. boot-medium values for `DL`, `CH`, `BX`, `AX`, and `DI`;
2. initial IVT and BDA contents, including INT 13h/15h/19h behavior;
3. ROM/video, keyboard, timer, PIC, disk, and memory topology policy;
4. a repeated private-image transition mechanism that traps the live
   `SVC_DEMLOADDOS` location and validates `DI << 4` before loading `NTDOS.SYS`;
5. an NTDOS preflight that validates the full image range before any guest
   memory write; and
6. bounded execution and redacted trace rules for a user-selected profile.

The current `--plan-bootstrap` command ends before all six items.  It is a
read-only profile check and rewrite plan, not an NTIO entry invocation.
