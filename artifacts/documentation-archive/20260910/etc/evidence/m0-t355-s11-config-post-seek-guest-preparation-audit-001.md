# M0 T355 S11 — CONFIG post-seek guest preparation audit

## Scope

S11 follows the successful S10 EOF seek through original guest-only CONFIG
preparation.  It is a source audit; it adds no observer, build, runtime run,
or source behaviour change.

## Original control flow

After `doconf` stores the successful EOF position in `count`, it:

1. stores that value in `config_size`;
2. calls `pararound`, an original local helper which rounds the byte count to
   paragraphs and returns without an external service;
3. subtracts the rounded allocation and header from `confbot`;
4. calls original `sysinit1.asm::tempcds`;
5. only after `tempcds` returns, changes `DS:ES` to the new `confbot` and
   invokes the read.

`tempcds` is mostly guest-local allocation and CDS initialization.  Its first
non-local operation is deliberate: while initializing each drive's temporary
CDS, it invokes INT 21h `AX=4408h` (is drive removable) until it finds a fixed
drive.  Original `ioctl.asm::ioctl_removable_media` routes that request via
`HRDSVC SVC_DEMIOCTL`.  Its selected host provider is
`dos/dem/demioctl.c::demIOCTL → demIoctlChangeable`, which preserves the
original drive-type/`AX`/carry result contract through public drive queries.

## Result

The prior interval is not wholly guest-local: `tempcds` contains the unique
immediate host-service boundary before read.  The next bounded seam is the
original `SVC_DEMIOCTL` removable-media request.  It must be observed as a
whole existing IOCTL owner path; no CONFIG parser, memory allocation, read or
drive policy repair is selected by S11.
