# DOSINIT Post-DPB Dependency Ledger

Status: M21 ordered static dependency record, 2026-08-08.

## Ordering Correction

`SVC_DEMGETDRIVES` is the first non-debug external request, but it is not the
only host-facing operation before the DPB step. The fixed `DosInit` order is:

```text
DEMGETDRIVES (0x0f)          -> logical namespace count/type state
DEMSYSTEMSYMBOLOP (0x45)     -> debug-only, excluded from bounded profile
DEMSETDTALOCATION (0x1b)     -> registers guest DTA/PDB/error pointers
DEMSETHARDERRORINFO (0x32)   -> registers guest hard-error/device pointers
CMDSETINFO (command svc 5)   -> registers command shared-state pointers
character-device initialization
DEMGETDPBLIST (0x46)         -> M20 guest DPB transaction
DOS-owned low-memory/IVT patching and PSP/arena setup
retf back into BIOS/sysinit
```

The three registration services are not drive, timer, interrupt-controller, or
controller emulation. They record addresses of NTDOS-owned guest structures
for the historical host. A future mantle profile may represent them as typed,
validated guest-address descriptors. It must not substitute them with generic
host-pointer access or expose the addresses outside the session.

## Post-DPB Result

After M20 the remaining `DosInit` code:

- patches DOS-owned IVT entries and internal vector tables;
- creates the initial PSP and conventional-memory arena;
- establishes console SFT/device references;
- initializes `sysinittable` pointers; and
- returns to the BIOS/sysinit caller with relocation and size information.

The only `INT 14h` in this region is under `DEBUG`; it is not part of the
selected bounded profile. XMS/A20 checks occur in `seg_reinit` only on an HMA
path and are likewise not evidence that the initial low-memory command cut
requires XMS or a hardware keyboard controller.

## Boundary Result

The next source-backed generic machine needs remain checked ordinary-RAM
transactions, constrained register patches, and correct DOS-owned IVT memory
semantics. There is still no basis to promote PC/AT FDC/HDC/CMOS devices into
core. The next execution-relevant cut is the BIOS/sysinit continuation after
`DosInit` returns, where configuration and command initialization choose the
first actual namespace/file/stream operations.
