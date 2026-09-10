# T95 S7 DEMIOCTL 4408 Trace Audit

## Question

Which exact historical IOCTL subfunction is reached after the boot-drive
fallback, and does it require a device implementation?

## Evidence

Fresh r7 trace 008 logs `selector=50`, `service=21`, `ax=4408`, `bx=f002`.
OpenNT `dossvc.h` maps `0x21` to `SVC_DEMIOCTL`; `demioctl.c` dispatches on
AL. Therefore the reached input is DOS INT 21h AH=44h, AL=08h, BL=02h: the
changeable-media query for C:.

`ioctl.asm` sends AL 8/9 through `SVC_DEMIOCTL`; `demIoctlChangeable` queries
the drive type and returns AX=1 for a non-removable drive (AX=0 only for
removable media), then clears CF. The first profile's immutable inventory
already describes C as fixed. No parameter block, device request, port, ROM,
or host path is involved in this specific branch.

## Decision

The next candidate is a single adapter-owned `50:21` responder restricted to
AL=08, a profile-admitted nonzero BL letter, and the existing Win32 drive-type
numeric domain frozen in the profile. It returns only source-defined AX/CF and
resumes. All other IOCTL subfunctions pass through; AL=09, generic IOCTL,
media/state mutation, device/driver I/O and host rescans remain excluded.
