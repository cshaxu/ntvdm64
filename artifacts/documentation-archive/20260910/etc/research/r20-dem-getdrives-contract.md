# r20 `SVC_DEMGETDRIVES` closure contract

## Reached boundary

The r20 source-built NTIO/NTDOS run reaches `C4 C4 50 0F` at copied real-mode
state `CS:IP=9346:7439` (linear `0x9a099`). `DOSSVC.INC` assigns `0Fh` to
`SVC_DEMGETDRIVES`; `demdisp.c` dispatches it to `demGetDrives`.

This is the immediate predecessor of the earlier `0000:0A84` #UD delivery.
That later address is native exception delivery after adapter pass-through,
not a source guest call site.

## Source contract

`dos/dem/demgset.c:334-430` defines the operation:

1. Probe A: and B: using the host OEM drive-type routine; normalize missing and
   remote drives to `DRIVE_UNKNOWN` and keep the two types in
   `PhysicalDriveTypes[0..1]`.
2. Derive host-only `IsAPresent` and `IsBPresent` flags.
3. Start `nDrives` at two, then enumerate C: through Z: using the physical
   drive-type helper. Count a contiguous prefix of removable, fixed, CD-ROM or
   RAM-disk drive types; store every observed type in the 26-entry inventory.
4. Return `AX=nDrives` and clear CF. NTDOS (`dos/msinit.asm`) consumes AL as
   `numio` immediately after the service.

The operation is not a Bochs hardware query and must not enable, attach or
infer a floppy, IDE, CD-ROM, BIOS or filesystem device in the Bochs machine.
The original inventory is host/service-plane state, not guest memory.

## Adapter boundary decision

The future adapter capability receives the immutable, CLI/BYOB-admitted
`host_drive_inventory.types` declaration. It defines all 26 drive-type slots and
the exposed contiguous count; it must not enumerate ambient Windows volumes at
run time. The capability may retain that declaration in adapter session state,
publish the derived A/B flags only to later source-closed adapter services, and
apply the exact AX/CF result through a separately admitted generic CPU-result
record.

This declaration deliberately has no guest path, host handle, directory,
file-system traversal, Bochs object, device binding or raw memory pointer.
The initial contained profile may represent only the source baseline of two
drive slots, but it must state the A/B types explicitly. A later service that
opens, searches or reads a drive requires its own artifact-root/namespace and
guest-buffer contract.

## Implemented contained slice

The profile/CLI parser copies the required 26-slot declaration into the
adapter session.  `bx_ntvdm_dem_drive_service_v1_dispatch` consumes that
immutable inventory only for the exact real-mode `C4 C4 50 0F` boundary.  It
computes the contiguous count, returns `AX=nDrives`, clears CF, and resumes at
`RIP+4`.  CPU-result v2 is the generic, CF-only mechanical result transport;
Bochs still contains no selector, service, inventory or device branch.

The r20 private observation fixture was incrementally rebuilt and its bounded
trace records acceptance at `9346:7439` with `next=743d`; its next unhandled
boundary is `9346:7484`, `C4 C4 50 1B`.  This is runtime evidence for this
slice only, not filesystem or DOS-runtime completion.

## Remaining blockers

- No later DEM consumer of `PhysicalDriveTypes`, `IsAPresent` or `IsBPresent`
  has been closed for the contained profile.
- `SVC_DEMSETDTALOCATION` (`50:1B`) publishes four host-side addresses based
  on guest DS:AX/DX/CX/SI and therefore requires a separately reviewed
  guest-pointer and DOS/WOW-data ownership contract.

Returning only `AL=2` remains rejected: it would omit the defined AH and CF
effects and conceal the profile-owned inventory dependency.

## Evidence

- `src/opennt/base/mvdm/inc/DOSSVC.INC`
- `src/opennt/base/mvdm/dos/dem/demdisp.c`
- `src/opennt/base/mvdm/dos/dem/demgset.c`
- `src/opennt/base/mvdm/dos/v86/doskrnl/dos/msinit.asm`
- `docs/etc/research/r20-real-ntio-next-boundary-triage.md`
