# T198 S26 P1 DEM Error-Lock Native 001

## Question

Can the exact source-built NTIO/NTDOS run resume original DEM `50:32` without
turning its registration into host hard-error policy?

## Source Contract

OpenNT `dossvc.h` identifies `32h` as `SVC_DEMSETHARDERRORINFO`;
`demdisp.c` routes it to `demSetHardErrorInfo`; and `demerror.c` stores only
`DS:DX` (`pHardErrPacket`) and `DS:BX` (`pDeviceChain`) through `GetVDMAddr`.
The guest caller in `msinit.asm` supplies exactly those two locations.

## Implementation And Verification

The existing `bx_ntvdm_dem_hard_error_service_v1` was placed behind a new
DEM error-lock plane.  The plane requires common ingress, original DEM
classification as `ERROR_LOCK`, service `32h`, and the normal deferred
provider disposition before storing its two fixed-width physical locations
and returning the existing typed `RIP+4` result.  It has no retry, dialog,
I/O, host handle, or Bochs behavior.

The complete MSVC x64 `/MT` composition closure and the exact CPU5
NTIO/NTDOS fixture both exit zero.  The native observation is:

```
t198-s23 status=4 observed-5011=1 observed-503b-resume=1 observed-dta-resume=1 observed-hard-error-resume=1 observed-stop=1 next=50:45 plane=0/0/1 error-lock=1 terminal=1:0000:0ad2
```

`error-lock=1` records the two-location registration.  The next unhandled
boundary is original DEM `50:45`; no meaning or provider has been inferred
for it yet.  The CPU prefetch diagnostic remains observational only.

## Limit

This is registration, not hard-error execution.  Any later retry or user/host
error policy needs separate original-source mapping and S admission.
