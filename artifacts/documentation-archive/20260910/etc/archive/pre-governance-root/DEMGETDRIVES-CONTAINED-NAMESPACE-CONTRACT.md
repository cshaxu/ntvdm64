# DEMGETDRIVES Contained Namespace Contract

Status: M19 source evidence and neutral contract, 2026-08-08.

## Historical Contract

`dossvc.h` assigns `SVC_DEMGETDRIVES = 0x0f`. `demdisp.c` maps it to
`demGetDrives`; `dos/msinit.asm` immediately stores returned `AL` in NTDOS
`numio`.

The historical service has no guest inputs. On its successful return it sets
`AX` to `nDrives` and clears carry. Its C implementation also mutates
host-side per-session globals:

```text
PhysicalDriveTypes[0..25]
IsAPresent / IsBPresent
nDrives
```

The implementation probes A and B using `GetDriveTypeOem`, then opens every
remaining ambient DOS root through NT native path conversion, `NtOpenFile`,
and `NtQueryVolumeInformationFile`. It classifies removable, fixed, CD-ROM,
and RAM disk devices; remote, subst, and unknown roots become unknown. It
counts only the contiguous valid run beginning at C. The drive-type array is
later consumed by DEM disk/DPB code.

This is historical Windows host policy, not guest ISA behavior. It does not
access guest ports, ATA/FDC registers, CMOS, PIC, DMA, or BIOS `INT 13h`.

## Admissible Modern Meaning

The profile/mantle layer needs an immutable per-session logical namespace
record prepared before the service boundary:

```text
drive letter [A..Z]
  present: yes/no
  class: unknown | removable | fixed | cdrom | ramdisk
  root/backend identity: profile-selected only
  boot eligibility and later DPB/media descriptor: deferred companion facts
```

For the historical `DEMGETDRIVES` projection, the record must validate these
invariants before guest execution resumes:

1. At most 26 named letter slots, with no ambient discovery source.
2. `AL` is the historical contiguous-count projection, not merely the total
   number of mounted roots.
3. A/B presence and every type answer derive from the same immutable record,
   so later DPB/DASD consumers cannot observe a different topology.
4. Later boot/DPB data is coherent already, or the session stops before the
   first guest mutation that relies on it.
5. A malformed profile yields a bounded terminal diagnostic, never an
   arbitrary successful service return.

The profile owns letter allocation, mounts, root containment, and drive class.
Generic core may expose only generic stopped-transition/atomic-resume
mechanics; it must not define DOS letters, counts, or historical classes. The
host adapter may open an explicitly selected root only when actual file I/O is
needed. `DEMGETDRIVES` itself must not scan the host.

## Immediate Follow-On

`DosInit` next reaches `SVC_DEMGETDPBLIST`; that source path determines which
profile facts become guest-visible DPB memory. No local replacement SVC handler
is introduced here.
