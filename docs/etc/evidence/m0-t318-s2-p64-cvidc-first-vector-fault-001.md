# M0 T318 S2 P64 — C-VID first-vector fault localization

## Purpose

Locate the fixed-container `0xC0000005` without changing the selected media,
observer, command line, console ownership, or timeout.

## Evidence

The original `ntvdm.c` exception wrapper reaches
`VdmUnhandledExceptionFilter()` before terminating the VDM process.  A
declaration-only diagnostic seam records the received exception context before
that original termination order.  It reported:

```
code=0xc0000005 address=0x00000000 ip=0x00000000 return=0x0086e57b
base=0x00810000
```

The x86 linker map resolves the return RVA to
`softpc.new/base/video/ega_prts.c::init_vga_globals`, immediately after its
first `setVideov7_bank_vid_copy_off(0)` indirect call.  The selected original
`config()` can initialise EGA before its later `setup_vga_globals()` call, so
the selected original `Video` table had not yet been copied from C-VID's
original `C_Video` table.

## Recovery boundary

`MVDM-HOST-DIV-162` adds only a video-table binder after original `gfi_init()`
and before original `config()`.  It preserves the original order and does not
bind CCPU slots early.  The full CPU-vector binding remains at the existing
post-CPU-initialization site.

The changed x86 product then passed the first null vector call and exposed a
separate stack-overflow failure.  This proves the first null vector was real;
it is not an NTIO, NTDOS, EXEC, or guest-reachability result.
